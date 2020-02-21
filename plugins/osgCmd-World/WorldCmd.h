#pragma once

#include <osgCmd/Cmd.h>

class WorldEventHandler;
class WorldCmd : public osgCmd::Cmd
{
	REFLEX_DECLARE(WorldCmd)
public:
	bool init();
	osgEarth::MapNode* getMapNode() const;
	osgEarth::Util::EarthManipulator* getEarthManipulator() const;

protected:
	bool parseCmdArg(osg::ArgumentParser& cmdarg);

private slots:
	void flyTo(const osgCmd::UserData& userdata);

private:
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<WorldEventHandler> _eventHandler;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
