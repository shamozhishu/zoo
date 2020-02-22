#pragma once

#include <osgCmd/Cmd.h>

class WorldEventHandler;
class WorldCmd : public osgCmd::Cmd
{
	REFLEX_DECLARE(WorldCmd)
public:
	bool init();
	void parseCmdArg(osg::ArgumentParser& cmdarg);
	void helpInformation(osg::ApplicationUsage* usage);
	osgEarth::MapNode* getMapNode() const;
	osgEarth::Util::EarthManipulator* getEarthManipulator() const;

private slots:
	void flyTo(const osgCmd::UserData& userdata);

private:
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<WorldEventHandler> _eventHandler;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
