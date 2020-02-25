#pragma once

#include <osgCmd/Cmd.h>

class LongitudeLatitudeEventHandler;
class WorldCmd : public osgCmd::Cmd
{
	REFLEX_DECLARE(WorldCmd)
public:
	~WorldCmd();
	bool init();
	void parseCmdArg(osg::ArgumentParser& cmdarg);
	void helpInformation(osg::ApplicationUsage* usage);
	osgEarth::MapNode* getMapNode() const;
	osgEarth::Util::EarthManipulator* getEarthManipulator() const;

private slots:
	void flyTo(const osgCmd::UserData& userdata);
	void LonLatAltitude(const osgCmd::UserData& userdata);
	void measureDistance(const osgCmd::UserData& userdata);

private:
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<LongitudeLatitudeEventHandler> _lonLatHandler;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
