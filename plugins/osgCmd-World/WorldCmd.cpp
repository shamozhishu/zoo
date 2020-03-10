#include "WorldCmd.h"
#include "WorldControls.h"
#include <osgCmd/Viewers.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Sigslot.h>
#include "LocateModelEventHandler.h"
#include "MeasureDistanceEventHandler.h"
#include "LongitudeLatitudeEventHandler.h"

using namespace osgEarth;
using namespace osgCmd;

REGISTER_OSGCMDPLUGIN(WorldCmd)
REFLEX_IMPLEMENT(WorldCmd);

WorldCmd::~WorldCmd()
{
	WorldControls::destroy();
}

bool WorldCmd::init()
{
	Viewers* pViewers = CmdManager::getSingleton().getViewers();
	_view = pViewers->createView(0, 1, 0, 1);
	osg::Group* pRootNode = pViewers->getRootNode(0);

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(getWorkDir() + "world.earth");
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
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

void WorldCmd::parseCmdArg(osg::ArgumentParser& cmdarg)
{
	do
	{
		bool show;
		if (cmdarg.read("--lla", show))
		{
			_subCommands.userData().setData(show);
			SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::LonLatAltitude);
			break;
		}

		float lon, lat, dist;
		if (cmdarg.read("--fly", lon, lat, dist))
		{
			_subCommands.userData().setData("lon", lon);
			_subCommands.userData().setData("lat", lat);
			_subCommands.userData().setData("dist", dist);
			SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::flyTo);
			break;
		}

		if (cmdarg.read("--dist"))
		{
			SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::measureDistance);
			break;
		}

		string model;  float height, scale; bool repeat;
		if (cmdarg.read("--locate", model, height, scale, repeat))
		{
			_subCommands.userData().setData("model", model);
			_subCommands.userData().setData("height", height);
			_subCommands.userData().setData("scale", scale);
			_subCommands.userData().setData("repeat", repeat);
			SignalTrigger::connect<WorldCmd>(_subCommands, this, &WorldCmd::locateModel);
			break;
		}

	} while (0);
}

void WorldCmd::helpInformation(osg::ApplicationUsage* usage)
{
	usage->setDescription("Word command: encapsulation of osgEarth.");
	usage->addCommandLineOption("--lla <show:bool>", "Display longitude, latitude and altitude information.");
	usage->addCommandLineOption("--fly <longitude:float> <latitude:float> <distance:float>", "Set viewpoint to specified longitude, latitude and distance.");
	usage->addCommandLineOption("--dist", "Ground measurement distance.");
	usage->addCommandLineOption("--locate <model:string> <height:float> <scale:float> <repeat:bool>", "Ground placement model.");
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

void WorldCmd::flyTo(const UserData& userdata)
{
	Viewpoint vp = _manipulator->getViewpoint();
	vp.focalPoint().mutable_value().x() = any_cast<float>(_subCommands.userData().getData("lon"));
	vp.focalPoint().mutable_value().y() = any_cast<float>(_subCommands.userData().getData("lat"));
	vp.range() = any_cast<float>(_subCommands.userData().getData("dist"));
	_manipulator->setViewpoint(vp, 2);
}

void WorldCmd::LonLatAltitude(const UserData& userdata)
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

void WorldCmd::measureDistance(const UserData& userdata)
{
	osg::ref_ptr<MeasureDistanceEventHandler> measureDistanceHandler = new MeasureDistanceEventHandler();
	_view->addEventHandler(measureDistanceHandler.get());
	CmdManager::getSingleton().block(true);
	WorldControls::getIns()->removeLabelTextDisplay(dist_label_);
	_view->removeEventHandler(measureDistanceHandler.get());
}

void WorldCmd::locateModel(const UserData& userdata)
{
	string model = any_cast<string>(_subCommands.userData().getData("model"));
	float height = any_cast<float>(_subCommands.userData().getData("height"));
	float scale = any_cast<float>(_subCommands.userData().getData("scale"));
	bool repeat = any_cast<bool>(_subCommands.userData().getData("repeat"));

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(getWorkDir() + model);
	if (node)
	{
		osg::ref_ptr<LocateModelEventHandler> locateModelHandler = new LocateModelEventHandler(node, height, scale, repeat);
		_view->addEventHandler(locateModelHandler.get());
		CmdManager::getSingleton().block(true);
		_view->removeEventHandler(locateModelHandler.get());
	}
}
