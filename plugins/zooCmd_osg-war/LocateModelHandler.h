#ifndef __LOCATE_MODEL_HANDLER_H__
#define __LOCATE_MODEL_HANDLER_H__

#include <zooCmd_osg/OsgDevice.h>

class LocateModelHandler : public osgGA::GUIEventHandler
{
public:
	LocateModelHandler(osgEarth::MapNode* mapNode, osg::Node* model, float height, float scale, bool repeat);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

private:
	float _height;
	float _scale;
	bool _repeat;
	osg::NodePath _nodePath;
	osg::ref_ptr<osg::Node> _modelNode;
	osg::observer_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<osg::MatrixTransform> _scaleTransform;
	osg::ref_ptr<osg::MatrixTransform> _locateTransform;
};

#endif // __LOCATE_MODEL_HANDLER_H__
