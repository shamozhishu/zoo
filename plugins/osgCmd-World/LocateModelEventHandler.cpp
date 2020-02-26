#include "LocateModelEventHandler.h"
#include "WorldCmd.h"
#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>

using namespace osgEarth;

LocateModelEventHandler::LocateModelEventHandler(osg::Node* model, float height, float scale, bool repeat)
	: _modelNode(model)
	, _height(height)
	, _scale(scale)
	, _repeat(repeat)
{
	_nodePath.push_back(WorldCmd::getSingleton().getMapNode()->getTerrainEngine());
	_scaleTransform = new osg::MatrixTransform();
	_scaleTransform->setMatrix(osg::Matrix::scale(_scale, _scale, _scale));
	_locateTransform = new osg::MatrixTransform();
	_locateTransform->addChild(_scaleTransform);
	_scaleTransform->addChild(_modelNode);
	osgCmd::CmdManager::getSingleton().getRenderer()->getRootNode()->addChild(_locateTransform);
}

LocateModelEventHandler::~LocateModelEventHandler()
{
}

bool LocateModelEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (!viewer)
		return false;

	if (ea.getButtonMask() == ea.RIGHT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		osgCmd::CmdManager::getSingleton().getRenderer()->getRootNode()->removeChild(_locateTransform);
		osgCmd::CmdManager::getSingleton().block(false);
		return true;
	}

	if (ea.getButtonMask() == ea.LEFT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		if (_repeat)
		{
			_scaleTransform = new osg::MatrixTransform();
			_scaleTransform->setMatrix(osg::Matrix::scale(_scale, _scale, _scale));
			_locateTransform = new osg::MatrixTransform();
			_locateTransform->addChild(_scaleTransform);
			_scaleTransform->addChild(_modelNode);
			osgCmd::CmdManager::getSingleton().getRenderer()->getRootNode()->addChild(_locateTransform);
		}
		else
		{
			osgCmd::CmdManager::getSingleton().block(false);
		}

		return true;
	}
	
	if (ea.getEventType() == ea.MOVE)
	{
		osgUtil::LineSegmentIntersector::Intersections results;
		if (viewer->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
		{
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d world = first.getWorldIntersectPoint();
			GeoPoint mapPoint;
			mapPoint.fromWorld(WorldCmd::getSingleton().getMapNode()->getMapSRS(), world);
			mapPoint.z() += _height;
			osg::Matrix mat;
			mapPoint.createLocalToWorld(mat);
			_locateTransform->setMatrix(mat);
			return true;
		}
	}

	return false;
}
