#include "OsgEarthContextImpl.h"

osg::Group* OsgEarthContextImpl::getSceneNode()
{
	if (!_scene)
		_scene = new osg::Group;
	return _scene;
}

osgViewer::View* OsgEarthContextImpl::getOpView()
{
	return _view.get();
}

void OsgEarthContextImpl::setOpView(osgViewer::View* view)
{
	_view = view;
}

osgEarth::MapNode* OsgEarthContextImpl::getOpMapNode()
{
	return _mapNode.get();
}

void OsgEarthContextImpl::setOpMapNode(osgEarth::MapNode* mapNode)
{
	_mapNode = mapNode;
}

osgEarth::Util::EarthManipulator* OsgEarthContextImpl::getOpManipulator()
{
	return _manipulator.get();
}

void OsgEarthContextImpl::setOpManipulator(osgEarth::Util::EarthManipulator* manipulator)
{
	_manipulator = manipulator;
}
