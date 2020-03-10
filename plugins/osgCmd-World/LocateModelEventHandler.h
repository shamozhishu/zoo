#pragma once

#include <osgCmd/Support.h>

class LocateModelEventHandler : public osgGA::GUIEventHandler
{
public:
	LocateModelEventHandler(osg::Node* model, float height, float scale, bool repeat);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

private:
	float _height;
	float _scale;
	bool _repeat;
	osg::NodePath _nodePath;
	osg::ref_ptr<osg::Node> _modelNode;
	osg::ref_ptr<osg::MatrixTransform> _scaleTransform;
	osg::ref_ptr<osg::MatrixTransform> _locateTransform;
};
