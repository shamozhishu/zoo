#pragma once

#include <zooCmd/Cmd.h>
#include <zoo/Singleton.h>
#include <zooCmd_osg/Support.h>

class WarCmd : public Cmd, public Singleton<WarCmd>
{
	REFLEX_DECLARE(WarCmd)
public:
	WarCmd();
	~WarCmd();
	bool init();
	void update();
	void help(CmdUsage* usage);
	void parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue);
	osg::Group* getRootNode() const;
	osg::Group* getMainNode() const;

private:
	void onCreateView(const UserData& userdata);
	void onSetTrackNode(const UserData& userdata);

private:
	osg::Geode* createPanel();

private:
	osgViewer::View* m_mainView;
	osgViewer::View* m_bottom_leftView;
	osgViewer::View* m_bottom_rightView;

	osg::ref_ptr<osg::Group> m_root;
	osg::ref_ptr<osg::Group> m_main;
	osg::ref_ptr<osgText::Text> m_text;
	osg::ref_ptr<osg::Projection> m_HUDProjection;

	osg::ref_ptr<osgGA::NodeTrackerManipulator> m_tankTrack;
	osg::ref_ptr<osgGA::NodeTrackerManipulator> m_seekerTrack;
	osg::ref_ptr<osgGA::NodeTrackerManipulator> m_missileTrack;
	osg::ref_ptr<osgGA::NodeTrackerManipulator> m_helicopterTrack;
	osg::ref_ptr<osgGA::TrackballManipulator> m_top_view_default_track;
	osg::ref_ptr<osgGA::TrackballManipulator> m_bottom_left_view_default_track;
	osg::ref_ptr<osgGA::TrackballManipulator> m_bottom_right_view_default_track;
};
