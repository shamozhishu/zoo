#ifndef __OSG_EARTH_CONTEXT_H__
#define __OSG_EARTH_CONTEXT_H__

#include <zoo/Component.h>
#include <UniversalGlobalServices.h>
#include <zooCmd_osg/OsgEarthSupport.h>

namespace zooCmd_osg
{
	const string EVENT_RESET_OSGEARTH_CONTEXT = "event.reset.osgearth.context";
}

class OsgEarthContext : public Spawner::Context, public CoordTransformUtil::Converter
{
public:
	virtual osg::Group* getSceneNode() = 0;
	virtual osgViewer::View* getEarthView() = 0;
	virtual void setEarthView(osgViewer::View* view) = 0;
	virtual osgEarth::MapNode* getEarthMapNode() = 0;
	virtual void setEarthMapNode(osgEarth::MapNode* mapNode) = 0;
	virtual osgEarth::Util::EarthManipulator* getEarthManipulator() = 0;
	virtual void setEarthManipulator(osgEarth::Util::EarthManipulator* manipulator) = 0;
};

#endif // __OSG_EARTH_CONTEXT_H__
