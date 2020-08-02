#include "WarCmd.h"
#include "WarControls.h"
#include <zoo/Utils.h>
#include <zoo/Sigslot.h>
#include <zooCmd/CmdManager.h>
#include "LocateModelHandler.h"
#include "MeasureDistHandler.h"
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth;

ZOO_REGISTER(WarCmd)
ZOO_REFLEX_IMPLEMENT(WarCmd);

WarCmd::WarCmd()
	: _osgEarthContext(nullptr)
{
}

WarCmd::~WarCmd()
{
}

bool WarCmd::init()
{
	_osgEarthContext = ServiceLocator<OsgEarthContext>::getService();
	_lonLatHandler = new LonLatDistHandler;
	return true;
}

void WarCmd::handle(const zooCmd::Event& evt)
{
	if (evt.getTopic() == zooCmd_osg::EVENT_RESET_OSGEARTH_CONTEXT)
	{
		WarControls::destroy();
		osgViewer::View* pView = _osgEarthContext->getOpView();
		if (pView)
			pView->removeEventHandler(_lonLatHandler.get());
	}
}

void WarCmd::help(CmdUsage* usage)
{
	usage->setDescription("war command: 战场场景编辑器的命令插件.");
	usage->addCommandProcedureCall("lla(bool show)", "Display longitude, latitude and altitude information.");
	usage->addCommandProcedureCall("fly(float longitude, float latitude, float distance)", "Set viewpoint to specified longitude, latitude and distance.");
	usage->addCommandProcedureCall("dist()", "Ground measurement distance.");
	usage->addCommandProcedureCall("locate(string model, float height, float scale, bool repeat)", "Ground placement model.");
}

void WarCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	std::function<bool(osgViewer::View*&, osgEarth::MapNode*&, osgEarth::Util::EarthManipulator*&)> errorTipFunc = [this]
	(osgViewer::View*& pView, osgEarth::MapNode*& pMapNode, osgEarth::Util::EarthManipulator*& pManipulator)
	{
		pView = _osgEarthContext->getOpView();
		if (!pView)
		{
			CmdManager::setTipMessage("视图不存在！");
			return false;
		}

		pMapNode = _osgEarthContext->getOpMapNode();
		pManipulator = _osgEarthContext->getOpManipulator();
		if (!pMapNode || !pManipulator)
		{
			CmdManager::setTipMessage("请在场景中添加一个有效的地球组件！");
			return false;
		}

		return true;
	};

	do
	{
		bool show;
		if (cmdarg.read("lla", show))
		{
			SignalTrigger::connect(subcmd, [this, show, errorTipFunc]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (!errorTipFunc(pView, pMapNode, pManipulator))
					return;

				if (show)
				{
					_lonLatHandler->setMapNode(pMapNode);
					_lonLatHandler->setEarthManipulator(pManipulator);
					pView->addEventHandler(_lonLatHandler.get());
				}
				else
				{
					pView->removeEventHandler(_lonLatHandler.get());
					WarControls::getIns()->removeLabelTextDisplay(lla_label_);
					WarControls::getIns()->removeLabelTextDisplay(ipt_label_);
				}
			});
			break;
		}

		float lon, lat, dist;
		if (cmdarg.read("fly", lon, lat, dist))
		{
			SignalTrigger::connect(subcmd, [this, lon, lat, dist, errorTipFunc]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (!errorTipFunc(pView, pMapNode, pManipulator))
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
			SignalTrigger::connect(subcmd, [this, errorTipFunc]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (!errorTipFunc(pView, pMapNode, pManipulator))
					return;

				osg::ref_ptr<MeasureDistHandler> measureDistanceHandler = new MeasureDistHandler(pManipulator);
				pView->addEventHandler(measureDistanceHandler.get());
				CmdManager::getSingleton().block(true);
				WarControls::getIns()->removeLabelTextDisplay(dist_label_);
				pView->removeEventHandler(measureDistanceHandler.get());
			});
			break;
		}

		string model;  float height, scale; bool repeat;
		if (cmdarg.read("locate", model, height, scale, repeat))
		{
			SignalTrigger::connect(subcmd, [this, model, height, scale, repeat, errorTipFunc]
			{
				osgViewer::View* pView;
				osgEarth::MapNode* pMapNode;
				osgEarth::Util::EarthManipulator* pManipulator;
				if (!errorTipFunc(pView, pMapNode, pManipulator))
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
