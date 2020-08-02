#ifndef __OSG_EARTH_CONTEXT_H__
#define __OSG_EARTH_CONTEXT_H__

#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthSupport.h>

namespace zooCmd_osg
{
	const string EVENT_RESET_OSGEARTH_CONTEXT = "event.reset.osgearth.context";
}

class OsgEarthContext : public Service
{
	ZOO_SERVICE(OsgEarthContext)
public:
	virtual osgViewer::View* getOpView() = 0;
	virtual void setOpView(osgViewer::View* view) = 0;
	virtual osgEarth::MapNode* getOpMapNode() = 0;
	virtual void setOpMapNode(osgEarth::MapNode* mapNode) = 0;
	virtual osgEarth::Util::EarthManipulator* getOpManipulator() = 0;
	virtual void setOpManipulator(osgEarth::Util::EarthManipulator* manipulator) = 0;
};

#endif // __OSG_EARTH_CONTEXT_H__
