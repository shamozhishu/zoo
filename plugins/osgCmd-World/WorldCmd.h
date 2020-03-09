#pragma once

#include <osgCmd/Cmd.h>
#include <osgCmd/Singleton.h>

class LongitudeLatitudeEventHandler;
class WorldCmd : public osgCmd::Cmd, public osgCmd::Singleton<WorldCmd>
{
	REFLEX_DECLARE(WorldCmd)
public:
	~WorldCmd();
	bool init();
	void parseCmdArg(osg::ArgumentParser& cmdarg);
	void helpInformation(osg::ApplicationUsage* usage);
	osgViewer::View* getView() const;
	osgEarth::MapNode* getMapNode() const;
	osgEarth::Util::EarthManipulator* getEarthManipulator() const;

private slots:
	void flyTo(const osgCmd::UserData& userdata);
	void LonLatAltitude(const osgCmd::UserData& userdata);
	void measureDistance(const osgCmd::UserData& userdata);
	void locateModel(const osgCmd::UserData& userdata);

private:
	osgViewer::View* _view;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<LongitudeLatitudeEventHandler> _lonLatHandler;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
