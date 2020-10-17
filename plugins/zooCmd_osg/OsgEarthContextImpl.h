#ifndef __OSG_EARTH_CONTEXT_IMPL_H__
#define __OSG_EARTH_CONTEXT_IMPL_H__

#include <zooCmd_osg/OsgEarthContext.h>

class OsgEarthContextImpl : public OsgEarthContext
{
	ZOO_REFLEX_DECLARE(OsgEarthContextImpl)
public:
	osg::Group* getSceneNode();
	osgViewer::View* getEarthView();
	void setEarthView(osgViewer::View* view);
	osgEarth::MapNode* getEarthMapNode();
	void setEarthMapNode(osgEarth::MapNode* mapNode);
	osgEarth::Util::EarthManipulator* getEarthManipulator();
	void setEarthManipulator(osgEarth::Util::EarthManipulator* manipulator);

private:
	osg::ref_ptr<osg::Group> _sceneNode;
	osg::ref_ptr<osgViewer::View> _earthView;
	osg::ref_ptr<osgEarth::MapNode> _earthMapNode;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _earthManipu;
};

#endif // __OSG_EARTH_CONTEXT_IMPL_H__
