#pragma once

#include <osgCmd/Support.h>

class WorldCmd;
class WorldEventHandler : public osgGA::GUIEventHandler
{
public:
	WorldEventHandler(WorldCmd* worldcmd);
	~WorldEventHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

public slots:
	void showLabelControl(const osgCmd::UserData& userdata);

private:
	WorldCmd* _worldCmd;
	osg::NodePath _nodePath;
	osg::ref_ptr<osgEarth::Util::Controls::LabelControl> _labelControl;
};
