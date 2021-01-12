#include "OsgEarthContextImpl.h"
#include <zooCmd_osg/OsgComponentImpls.h>

ZOO_REFLEX_IMPLEMENT(OsgContextImpl);
osg::Group* OsgContextImpl::getSceneNode()
{
	if (!_sceneNode)
		_sceneNode = new osg::Group;
	return _sceneNode;
}

ZOO_REFLEX_IMPLEMENT(OsgEarthContextImpl);
osg::Group* OsgEarthContextImpl::getSceneNode()
{
	if (!_sceneNode)
		_sceneNode = new osg::Group;
	return _sceneNode;
}

osgViewer::View* OsgEarthContextImpl::getEarthView()
{
	return _earthView;
}

void OsgEarthContextImpl::setEarthView(osgViewer::View* view)
{
	_earthView = view;
}

osgEarth::MapNode* OsgEarthContextImpl::getEarthMapNode()
{
	return _earthMapNode;
}

void OsgEarthContextImpl::setEarthMapNode(osgEarth::MapNode* mapNode)
{
	_earthMapNode = mapNode;
}

osgEarth::Util::EarthManipulator* OsgEarthContextImpl::getEarthManipulator()
{
	return _earthManipu;
}

void OsgEarthContextImpl::setEarthManipulator(osgEarth::Util::EarthManipulator* manipulator)
{
	_earthManipu = manipulator;
}
