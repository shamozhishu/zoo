#include "WorldCmd.h"
#include "WorldControls.h"
#include <zooCmd_osg/InputDevice.h>
#include <zooCmd/CmdManager.h>
#include <zoo/Utils.h>
#include <zoo/Sigslot.h>
#include "LocateModelEventHandler.h"
#include "MeasureDistanceEventHandler.h"
#include "LongitudeLatitudeEventHandler.h"

using namespace osgEarth;
using namespace zooCmd_osg;

ZOO_REGISTER(WorldCmd)
REFLEX_IMPLEMENT(WorldCmd);

WorldCmd::~WorldCmd()
{
	WorldControls::destroy();
}

bool WorldCmd::init()
{
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOOCMD_DATA_DIR + "world.earth");
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
	if (!_mapNode)
		return false;

	_view = InputDevice::getIns()->createView(0, 1, 0, 1);
	osg::Group* pRootNode = InputDevice::getIns()->getRootNode(0);
	pRootNode->addChild(_mapNode);
	_manipulator = new osgEarth::Util::EarthManipulator();
	_manipulator->setNode(_mapNode);
	_manipulator->getSettings()->setArcViewpointTransitions(true);
	_view->setCameraManipulator(_manipulator);
	_mapNode->addExtension(osgEarth::Extension::create("sky_simple", osgEarth::ConfigOptions()));

	osgUtil::Optimizer optimzer;
	optimzer.optimize(pRootNode);
	_view->setSceneData(pRootNode);
	return true;
}

void WorldCmd::help(CmdUsage* usage)
{
	usage->setDescription("world command: encapsulation of osgEarth.");
	usage->addCommandProcedureCall("lla(bool show)", "Display longitude, latitude and altitude information.");
	usage->addCommandProcedureCall("fly(float longitude, float latitude, float distance)", "Set viewpoint to specified longitude, latitude and distance.");
	usage->addCommandProcedureCall("dist()", "Ground measurement distance.");
	usage->addCommandProcedureCall("locate(string model, float height, float scale, bool repeat)", "Ground placement model.");
}

void WorldCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	do
	{
		bool show;
		if (cmdarg.read("lla", show))
		{
			subcmd.userData().setData(show);
			SignalTrigger::connect<WorldCmd>(subcmd, this, &WorldCmd::onLonLatAltitude);
			break;
		}

		float lon, lat, dist;
		if (cmdarg.read("fly", lon, lat, dist))
		{
			subcmd.userData().setData("lon", lon);
			subcmd.userData().setData("lat", lat);
			subcmd.userData().setData("dist", dist);
			SignalTrigger::connect<WorldCmd>(subcmd, this, &WorldCmd::onFlyTo);
			break;
		}

		if (cmdarg.read("dist"))
		{
			SignalTrigger::connect<WorldCmd>(subcmd, this, &WorldCmd::onMeasureDistance);
			break;
		}

		string model;  float height, scale; bool repeat;
		if (cmdarg.read("locate", model, height, scale, repeat))
		{
			subcmd.userData().setData("model", model);
			subcmd.userData().setData("height", height);
			subcmd.userData().setData("scale", scale);
			subcmd.userData().setData("repeat", repeat);
			SignalTrigger::connect<WorldCmd>(subcmd, this, &WorldCmd::onLocateModel);
			break;
		}

	} while (0);
}

osgViewer::View* WorldCmd::getView() const
{
	return _view;
}

osgEarth::MapNode* WorldCmd::getMapNode() const
{
	return _mapNode.get();
}

osgEarth::Util::EarthManipulator* WorldCmd::getEarthManipulator() const
{
	return _manipulator.get();
}

void WorldCmd::onFlyTo(const UserData& userdata)
{
	Viewpoint vp = _manipulator->getViewpoint();
	vp.focalPoint().mutable_value().x() = any_cast<float>(userdata.getData("lon"));
	vp.focalPoint().mutable_value().y() = any_cast<float>(userdata.getData("lat"));
	vp.range() = any_cast<float>(userdata.getData("dist"));
	_manipulator->setViewpoint(vp, 2);
}

void WorldCmd::onLonLatAltitude(const UserData& userdata)
{
	bool show = any_cast<bool>(userdata.getData());
	if (show)
	{
		if (!_lonLatHandler)
		{
			_lonLatHandler = new LongitudeLatitudeEventHandler();
			_view->addEventHandler(_lonLatHandler.get());
		}
	}
	else
	{
		if (_lonLatHandler)
		{
			WorldControls::getIns()->removeLabelTextDisplay(lla_label_);
			WorldControls::getIns()->removeLabelTextDisplay(ipt_label_);
			_view->removeEventHandler(_lonLatHandler.get());
			_lonLatHandler = nullptr;
		}
	}
}

void WorldCmd::onMeasureDistance(const UserData& userdata)
{
	osg::ref_ptr<MeasureDistanceEventHandler> measureDistanceHandler = new MeasureDistanceEventHandler();
	_view->addEventHandler(measureDistanceHandler.get());
	CmdManager::getSingleton().block(true);
	WorldControls::getIns()->removeLabelTextDisplay(dist_label_);
	_view->removeEventHandler(measureDistanceHandler.get());
}

void WorldCmd::onLocateModel(const UserData& userdata)
{
	string model = any_cast<string>(userdata.getData("model"));
	float height = any_cast<float>(userdata.getData("height"));
	float scale = any_cast<float>(userdata.getData("scale"));
	bool repeat = any_cast<bool>(userdata.getData("repeat"));

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOOCMD_DATA_DIR + model);
	if (node)
	{
		osg::ref_ptr<LocateModelEventHandler> locateModelHandler = new LocateModelEventHandler(node, height, scale, repeat);
		_view->addEventHandler(locateModelHandler.get());
		CmdManager::getSingleton().block(true);
		_view->removeEventHandler(locateModelHandler.get());
	}
}
