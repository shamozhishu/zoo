#pragma once

#include <zooCmd/Cmd.h>
#include <zoo/Singleton.h>
#include <zooCmd_osg/Support.h>

class LongitudeLatitudeEventHandler;
class WorldCmd : public zooCmd::Cmd, public zoo::Singleton<WorldCmd>
{
	REFLEX_DECLARE(WorldCmd)
public:
	~WorldCmd();
	bool init();
	void parseCmdArg(CmdParser& cmdarg, zoo::UserData& retValue);
	void helpInformation(AppUsage* usage);
	osgViewer::View* getView() const;
	osgEarth::MapNode* getMapNode() const;
	osgEarth::Util::EarthManipulator* getEarthManipulator() const;

private zoo_slots:
	void flyTo(const zoo::UserData& userdata);
	void LonLatAltitude(const zoo::UserData& userdata);
	void measureDistance(const zoo::UserData& userdata);
	void locateModel(const zoo::UserData& userdata);

private:
	osgViewer::View* _view;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<LongitudeLatitudeEventHandler> _lonLatHandler;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
