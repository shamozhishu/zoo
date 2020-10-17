#include "EarthCmd.h"
#include "EarthControls.h"
#include <zoo/Utils.h>
#include <zoo/Sigslot.h>
#include <zooCmd/CmdManager.h>
#include "LocateModelHandler.h"
#include "MeasureDistHandler.h"
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth;

ZOO_REGISTER(EarthCmd)
ZOO_REFLEX_IMPLEMENT(EarthCmd);

EarthCmd::EarthCmd()
	: _earthControls(nullptr)
	, _osgEarthContext(nullptr)
{
}

EarthCmd::~EarthCmd()
{
}

EarthControls* EarthCmd::getEarthControls()
{
	if (!_earthControls && _osgEarthContext)
		_earthControls = new EarthControls(_osgEarthContext);
	return _earthControls;
}

void EarthCmd::handle(const zooCmd::Event& evt)
{
	if (evt.getTopic() == zooCmd_osg::EVENT_RESET_OSGEARTH_CONTEXT)
	{
		SAFE_DELETE(_earthControls);
		Spawner* pSpawner = Spawner::find(evt.getProps().getData().to<int>());
		if (pSpawner)
		{
			_osgEarthContext = pSpawner->getContext<OsgEarthContext>();
			if (_osgEarthContext && _lonLatHandler)
			{	
				osgViewer::View* pView = _osgEarthContext->getEarthView();
				if (pView)
				{
					pView->removeEventHandler(_lonLatHandler.get());
					_lonLatHandler = nullptr;
				}
			}
		}
	}
}

void EarthCmd::help(CmdUsage* usage)
{
	usage->setDescription("earth command: osgearth相关的命令插件");
	usage->addCommandProcedureCall("lla(bool show)", "显示经度纬度和海拔");
	usage->addCommandProcedureCall("fly(float longitude, float latitude, float distance)", "将视点转移到指定经度纬度和高度处");
	usage->addCommandProcedureCall("dist()", "贴地测距");
	usage->addCommandProcedureCall("locate(string model, float height, float scale, bool repeat)", "在地球上放置模型");
}

void EarthCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	std::function<bool(osgViewer::View*&, osgEarth::MapNode*&, osgEarth::Util::EarthManipulator*&)> testError = [this]
	(osgViewer::View*& pView, osgEarth::MapNode*& pMapNode, osgEarth::Util::EarthManipulator*& pManipulator)
	{
		bool hasErr = false;
		do
		{
			if (!_osgEarthContext)
			{
				hasErr = true;
				break;
			}

			pView = _osgEarthContext->getEarthView();
			if (!pView)
			{
				hasErr = true;
				break;
			}

			pMapNode = _osgEarthContext->getEarthMapNode();
			pManipulator = _osgEarthContext->getEarthManipulator();
			if (!pMapNode || !pManipulator)
			{
				hasErr = true;
				break;
			}

		} while (0);

		if (hasErr)
			CmdManager::setTipMessage("请在场景中添加一个有效的地球组件！");

		return hasErr;
	};

	do
	{
		bool show;
		if (cmdarg.read("lla", show))
		{
			SignalTrigger::connect(subcmd, [this, show, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				if (show)
				{
					if (!_lonLatHandler)
					{
						_lonLatHandler = new LonLatDistHandler(_osgEarthContext, getEarthControls());
						_lonLatHandler->setMapNode(pMapNode);
						_lonLatHandler->setEarthManipulator(pManipulator);
						pView->addEventHandler(_lonLatHandler.get());
					}
				}
				else
				{
					if (_lonLatHandler)
					{
						pView->removeEventHandler(_lonLatHandler.get());
						_lonLatHandler = nullptr;
						getEarthControls()->removeLabelTextDisplay(lla_label_);
						getEarthControls()->removeLabelTextDisplay(ipt_label_);
					}
				}
			});
			break;
		}

		float lon, lat, dist;
		if (cmdarg.read("fly", lon, lat, dist))
		{
			SignalTrigger::connect(subcmd, [this, lon, lat, dist, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				Viewpoint vp = pManipulator->getViewpoint();
				vp.focalPoint().mutable_value().x() = lon;
				vp.focalPoint().mutable_value().y() = lat;
				vp.range() = dist;
				pManipulator->setViewpoint(vp, 2);
			});
			break;
		}

		if (cmdarg.read("dist"))
		{
			SignalTrigger::connect(subcmd, [this, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				osg::ref_ptr<MeasureDistHandler> measureDistanceHandler = new MeasureDistHandler(pMapNode, pManipulator, getEarthControls());
				pView->addEventHandler(measureDistanceHandler.get());
				CmdManager::getSingleton().block(true);
				getEarthControls()->removeLabelTextDisplay(dist_label_);
				pView->removeEventHandler(measureDistanceHandler.get());
			});
			break;
		}

		string model; float height, scale; bool repeat;
		if (cmdarg.read("locate", model, height, scale, repeat))
		{
			SignalTrigger::connect(subcmd, [this, model, height, scale, repeat, testError]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (testError(pView, pMapNode, pManipulator))
					return;

				osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + model);
				if (!node)
				{
					CmdManager::setTipMessage("模型文件[" + model + "]加载失败！");
					return;
				}

				osg::ref_ptr<LocateModelHandler> locateModelHandler = new LocateModelHandler(pMapNode, node, height, scale, repeat);
				pView->addEventHandler(locateModelHandler.get());
				CmdManager::getSingleton().block(true);
				pView->removeEventHandler(locateModelHandler.get());
			});
			break;
		}
	} while (0);
}
