#include "WorldCmd.h"
#include "WorldEventHandler.h"
#include <osgCmd/Renderer.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Sigslot.h>

using namespace osgEarth;

REGISTER_OSGCMDPLUGIN(WorldCmd)
REFLEX_IMPLEMENT(WorldCmd);

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
	_eventHandler = new WorldEventHandler(this);
	return true;
}

void WorldCmd::parseCmdArg(osg::ArgumentParser& cmdarg)
{
	do
	{
		bool showGUI;
		if (cmdarg.read("--gui", showGUI))
		{
			_subCmds.userData().setData(showGUI);
			osgCmd::SignalTrigger::connect<WorldEventHandler>(_subCmds, _eventHandler.get(), &WorldEventHandler::showLabelControl);
		}

		float lon, lat, dist;
		if (cmdarg.read("--fly", lon, lat, dist))
		{
			_subCmds.userData().setData("lon", lon);
			_subCmds.userData().setData("lat", lat);
			_subCmds.userData().setData("dist", dist);
			osgCmd::SignalTrigger::connect<WorldCmd>(_subCmds, this, &WorldCmd::flyTo);
		}

		if (cmdarg.read("--dist"))
		{
		}

	} while (0);
}

void WorldCmd::helpInformation(osg::ApplicationUsage* usage)
{
	usage->setDescription("Word command: encapsulation of osgEarth.");
	usage->addCommandLineOption("--gui", "Display panel information.");
	usage->addCommandLineOption("--fly", "Set viewpoint to specified latitude and longitude.");
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
	vp.focalPoint().mutable_value().x() = osgCmd::any_cast<float>(_subCmds.userData().getData("lon"));
	vp.focalPoint().mutable_value().y() = osgCmd::any_cast<float>(_subCmds.userData().getData("lat"));
	vp.range() = osgCmd::any_cast<float>(_subCmds.userData().getData("dist"));
	_manipulator->setViewpoint(vp, 2);
}
