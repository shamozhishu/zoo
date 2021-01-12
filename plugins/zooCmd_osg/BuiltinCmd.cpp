#include "BuiltinCmd.h"
#include "RenderDevice.h"
#include <zoo/Utils.h>
#include <zoo/Component.h>
#include <zooCmd/CmdManager.h>
#include "BuiltinEventHandler.h"
#include "WindowCaptureCallback.h"
#include <zooCmd_osg/OsgEarthContext.h>
#include <zooCmd_osg/OsgComponentImpls.h>

using namespace zooCmd;

ZOO_REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	_wndCaptureCB = new zooCmd_osg::WindowCaptureCallback(0, zooCmd_osg::WindowCaptureCallback::READ_PIXELS, zooCmd_osg::WindowCaptureCallback::END_FRAME, GL_BACK);
	_wndCaptureCB->setCaptureOperation(new zooCmd_osg::WriteToFile(ZOO_DATA_ROOT_DIR + "tmp/screenshot/screen_shot", "jpg"));
	return true;
}

void BuiltinCmd::help(CmdUsage* usage)
{
	usage->setDescription("内建命令: osg相关命令");
	usage->addCommandProcedureCall("home(int view_id)", "摄像机复位");
	usage->addCommandProcedureCall("focus(int ent_id, int ent_breed, int scene_id)", "将焦点设置到参数索引到的实体");
	usage->addCommandProcedureCall("screenshot(int view_id, int count)", "截屏: count是截屏帧数, count==0表示禁用截屏, count<0表示不限次数的截屏");
	usage->addCommandProcedureCall("posin(int view_id, int scene_id, bool show)", "显示当前光标在世界坐标系中和任意模型的交点位置(Ctrl+Left Mouse Button)");
	usage->addCommandProcedureCall("rotate(int view_id, int ent_id, int ent_breed, int scene_id)", "使用姿态球对模型进行旋转");
	usage->addCommandProcedureCall("exit()", "退出程序");
	usage->addCommandProcedureCall("bool(bool b) // bool& b", "测试布尔返回值");
	usage->addCommandProcedureCall("int(int i) // int& i", "测试整型数返回值");
	usage->addCommandProcedureCall("float(float f) // float& f", "测试单精度浮点数返回值");
	usage->addCommandProcedureCall("double(double d) // double& d", "测试双精度浮点数返回值");
	usage->addCommandProcedureCall("string(string str) // string& str", "测试字符串返回值");
	usage->addCommandProcedureCall("tips(string tip)", "测试提示信息");
}

void BuiltinCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	std::function<osgViewer::View*(int)> getView = [](int viewID) -> osgViewer::View*
	{
		osgViewer::View* pView = ServiceLocator<OsgDevice>::getService()->getView(viewID);
		if (!pView)
		{
			CmdManager::setTipMessage("视图[" + std::to_string(viewID) + "]不存在！");
			return nullptr;
		}

		return pView;
	};

	std::function<Entity*(int, int, int)> getEnt = [](int id, int breed, int sceneId) -> Entity*
	{
		Spawner* pSpawner = Spawner::find(sceneId);
		if (!pSpawner)
		{
			CmdManager::setTipMessage("场景[" + std::to_string(sceneId) + "]不存在！");
			return nullptr;
		}

		Entity* pEnt = pSpawner->gain(id, breed);
		if (!pEnt)
		{
			CmdManager::setTipMessage("实体[" + std::to_string(id) + ":" + std::to_string(breed) + "]不存在！");
			return nullptr;
		}

		return pEnt;
	};

	do
	{
		int viewid;
		if (cmdarg.read("home", viewid))
		{
			SignalTrigger::connect(subcmd, [viewid, getView]
			{
				osgViewer::View* pView = getView(viewid);
				if (pView)
					pView->getCameraManipulator()->home(2);
			});
			break;
		}

		int id, breed, sceneid;
		if (cmdarg.read("focus", id, breed, sceneid))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneid, getEnt]
			{
				Entity* pEnt = getEnt(id, breed, sceneid);
				if (!pEnt)
					return;

				if (pEnt->isSpawner())
				{
					pEnt->getComponentImpl<CameraImpl>()->_manipulatorMgr->home(2);
					return;
				}

				osg::Node* pFocusNode = pEnt->getComponentImpl<DoFImpl>()->_transWorld;
				pEnt->getSpawner()->getComponentImpl<CameraImpl>()->_manipulatorMgr->focus(pEnt->getSpawner()->getComponent<Camera>()->_manipulatorKey, pFocusNode);
			});
			break;
		}

		int count;
		if (cmdarg.read("screenshot", viewid, count))
		{
			SignalTrigger::connect(subcmd, [this, viewid, count, getView]
			{
				osgViewer::View* pView = getView(viewid);
				if (!pView)
					return;

				if (count == 0)
				{
					pView->getCamera()->setFinalDrawCallback(nullptr);
				}
				else
				{
					_wndCaptureCB->setFramesToCapture(count);
					pView->getCamera()->setFinalDrawCallback(_wndCaptureCB);
				}
			});
			break;
		}

		bool show;
		if (cmdarg.read("posin", viewid, sceneid, show))
		{
			SignalTrigger::connect(subcmd, [this, viewid, sceneid, show, getView]
			{
				osgViewer::View* pView = getView(viewid);
				if (!pView)
					return;

				OsgEarthContext* context = Spawner::find(sceneid) ? Spawner::find(sceneid)->getContext<OsgEarthContext>() : nullptr;
				if (!context)
				{
					CmdManager::setTipMessage("场景[" + std::to_string(sceneid) + "]不存在！");
					return;
				}

				if (show)
				{
					if (!_showCursorWPosHandler)
					{
						_showCursorWPosHandler = new ShowCursorWPosHandler(context);
						pView->addEventHandler(_showCursorWPosHandler.get());
					}
				}
				else
				{
					if (_showCursorWPosHandler)
					{
						pView->removeEventHandler(_showCursorWPosHandler.get());
						_showCursorWPosHandler = nullptr;
					}
				}
			});
			break;
		}

		if (cmdarg.read("rotate", viewid, id, breed, sceneid))
		{
			SignalTrigger::connect(subcmd, [id, breed, sceneid, viewid, getView, getEnt]
			{
				osgViewer::View* pView = getView(viewid);
				if (!pView)
					return;

				Entity* pEnt = getEnt(id, breed, sceneid);
				if (!pEnt)
					return;
			});
			break;
		}

		if (cmdarg.read("exit"))
		{
			SignalTrigger::connect(subcmd, []
			{
				ServiceLocator<OsgDevice>::getService()->as<RenderAdapter>()->setDone(true);
				exit(0);
			});
			break;
		}

		bool b;
		if (cmdarg.read("bool", b))
		{
			returnValue.setData("b", false);
			SignalTrigger::connect(subcmd, [b]
			{
				CmdManager::getSingleton().setReturnValue("b", b);
				zoo_info("return value is %s", b ? "true" : "false");
			});
			break;
		}

		int i;
		if (cmdarg.read("int", i))
		{
			returnValue.setData("i", 0);
			SignalTrigger::connect(subcmd, [i]
			{
				CmdManager::getSingleton().setReturnValue("i", i);
				zoo_info("return value is %d", i);
			});
			break;
		}

		float f;
		if (cmdarg.read("float", f))
		{
			returnValue.setData("f", 0.0f);
			SignalTrigger::connect(subcmd, [f]
			{
				CmdManager::getSingleton().setReturnValue("f", f);
				zoo_info("return value is %.3f", f);
			});
			break;
		}

		double d;
		if (cmdarg.read("double", d))
		{
			returnValue.setData("d", 0.0);
			SignalTrigger::connect(subcmd, [d]
			{
				CmdManager::getSingleton().setReturnValue("d", d);
				zoo_info("return value is %.6f", d);
			});
			break;
		}

		string str;
		if (cmdarg.read("string", str))
		{
			returnValue.setData("str", string(""));
			SignalTrigger::connect(subcmd, [str]
			{
				CmdManager::getSingleton().setReturnValue("str", string("\"") + str + "\"");
				zoo_info("return value is %s", (string("\"") + str + "\"").c_str());
			});
			break;
		}

		if (cmdarg.read("tips", str))
		{
			SignalTrigger::connect(subcmd, [str]
			{
				CmdManager::getSingleton().setTipMessage(str);
				zoo_info("tip message is %s", (string("\"") + str + "\"").c_str());
			});
			break;
		}
	} while (0);
}
