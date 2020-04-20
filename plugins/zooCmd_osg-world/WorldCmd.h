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
	void help(CmdUsage* usage);
	void parse(Signal& subcmd, CmdParser& cmdarg, zoo::UserData& returnValue);
	osgViewer::View* getView() const;
	osgEarth::MapNode* getMapNode() const;
	osgEarth::Util::EarthManipulator* getEarthManipulator() const;

private:
	void onFlyTo(const zoo::UserData& userdata);
	void onLonLatAltitude(const zoo::UserData& userdata);
	void onMeasureDistance(const zoo::UserData& userdata);
	void onLocateModel(const zoo::UserData& userdata);

private:
	osgViewer::View* _view;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<LongitudeLatitudeEventHandler> _lonLatHandler;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
