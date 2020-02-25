#pragma once

#include <osgCmd/Support.h>

class WorldCmd;
class LongitudeLatitudeEventHandler : public osgGA::GUIEventHandler
{
public:
	LongitudeLatitudeEventHandler(WorldCmd* worldcmd);
	~LongitudeLatitudeEventHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

private:
	WorldCmd* _worldCmd;
	osg::NodePath _nodePath;
};
