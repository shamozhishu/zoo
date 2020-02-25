#include "WorldCmd.h"
#include "WorldControls.h"
#include "MeasureDistanceEventHandler.h"
#include "LongitudeLatitudeEventHandler.h"
#include <osgCmd/Renderer.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Sigslot.h>

using namespace osgEarth;

REGISTER_OSGCMDPLUGIN(WorldCmd)
REFLEX_IMPLEMENT(WorldCmd);

WorldCmd::~WorldCmd()
{
	WorldControls::destroy();
}

bool WorldCmd::init()
{
	osg::Group* pRootNode = osgCmd::CmdManager::getSingleton().getRenderer()->getRootNode();
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(osgCmd::getWorkDir() + "world.earth");
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
	pRootNode->addChild(_mapNode);
	_manipulator = new osgEarth::Util::EarthManipulator();
	_manipulator->setNode(_mapNode);
	_manipulator->getSettings()->setArcViewpointTransitions(true);
	osgCmd::CmdManager::getSingleton().getRenderer()->setCameraManipulator(_manipulator);
	_mapNode->addExtension(osgEarth::Extension::create("sky_simple", osgEarth::ConfigOptions()));
	return true;
}

void WorldCmd::parseCmdArg(osg::ArgumentParser& cmdarg)
{
	do
	{
		bool isShow;
		if (cmdarg.read("--lla", isShow))
		{
			_subCommands.userData().setData(isShow);
			osgCmd::SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::LonLatAltitude);
			break;
		}

		float lon, lat, dist;
		if (cmdarg.read("--fly", lon, lat, dist))
		{
			_subCommands.userData().setData("lon", lon);
			_subCommands.userData().setData("lat", lat);
			_subCommands.userData().setData("dist", dist);
			osgCmd::SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::flyTo);
			break;
		}

		if (cmdarg.read("--dist"))
		{
			osgCmd::SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::measureDistance);
			break;
		}

	} while (0);
}

void WorldCmd::helpInformation(osg::ApplicationUsage* usage)
{
	usage->setDescription("Word command: encapsulation of osgEarth.");
	usage->addCommandLineOption("--lla <isShow:bool>", "Display panel information.");
	usage->addCommandLineOption("--fly <longitude:float> <latitude:float> <distance:float>", "Set viewpoint to specified longitude, latitude and distance.");
	usage->addCommandLineOption("--dist", "Ground measurement distance.");
}

osgEarth::MapNode* WorldCmd::getMapNode() const
{
	return _mapNode.get();
}

osgEarth::Util::EarthManipulator* WorldCmd::getEarthManipulator() const
{
	return _manipulator.get();
}

void WorldCmd::flyTo(const osgCmd::UserData& userdata)
{
	Viewpoint vp = _manipulator->getViewpoint();
	vp.focalPoint().mutable_value().x() = osgCmd::any_cast<float>(_subCommands.userData().getData("lon"));
	vp.focalPoint().mutable_value().y() = osgCmd::any_cast<float>(_subCommands.userData().getData("lat"));
	vp.range() = osgCmd::any_cast<float>(_subCommands.userData().getData("dist"));
	_manipulator->setViewpoint(vp, 2);
}

void WorldCmd::LonLatAltitude(const osgCmd::UserData& userdata)
{
	bool show = osgCmd::any_cast<bool>(userdata.getData());
	if (show)
	{
		if (!_lonLatHandler)
		{
			_lonLatHandler = new LongitudeLatitudeEventHandler(this);
			osgCmd::CmdManager::getSingleton().getRenderer()->addEventHandler(_lonLatHandler.get());
		}
	}
	else
	{
		if (_lonLatHandler)
		{
			osgCmd::CmdManager::getSingleton().getRenderer()->removeEventHandler(_lonLatHandler.get());
			_lonLatHandler = nullptr;
		}
	}
}

void WorldCmd::measureDistance(const osgCmd::UserData& userdata)
{
	osg::ref_ptr<MeasureDistanceEventHandler> measureDistanceHandler = new MeasureDistanceEventHandler(this);
	osgCmd::CmdManager::getSingleton().getRenderer()->addEventHandler(measureDistanceHandler.get());
	osgCmd::CmdManager::getSingleton().block(true);
	osgCmd::CmdManager::getSingleton().getRenderer()->removeEventHandler(measureDistanceHandler.get());
}
