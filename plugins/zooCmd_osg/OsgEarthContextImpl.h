#ifndef __OSG_EARTH_CONTEXT_IMPL_H__
#define __OSG_EARTH_CONTEXT_IMPL_H__

#include <zooCmd_osg/OsgEarthContext.h>

class OsgEarthContextImpl : public OsgEarthContext
{
public:
	osg::Group* getSceneNode();
	osgViewer::View* getOpView();
	void setOpView(osgViewer::View* view);
	osgEarth::MapNode* getOpMapNode();
	void setOpMapNode(osgEarth::MapNode* mapNode);
	osgEarth::Util::EarthManipulator* getOpManipulator();
	void setOpManipulator(osgEarth::Util::EarthManipulator* manipulator);

private:
	osg::ref_ptr<osg::Group> _scene;
	osg::ref_ptr<osgViewer::View> _view;
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};

#endif // __OSG_EARTH_CONTEXT_IMPL_H__
