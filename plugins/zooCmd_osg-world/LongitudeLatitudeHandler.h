#ifndef __LONGITUDE_LATITUDE_HANDLER_H__
#define __LONGITUDE_LATITUDE_HANDLER_H__

#include <zooCmd_osg/Support.h>

class WorldCmd;
class LongitudeLatitudeHandler : public osgGA::GUIEventHandler
{
public:
	LongitudeLatitudeHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

private:
	osg::NodePath _nodePath;
};

#endif // __LONGITUDE_LATITUDE_HANDLER_H__
