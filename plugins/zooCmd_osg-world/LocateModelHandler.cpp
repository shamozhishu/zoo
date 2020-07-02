#include "LocateModelHandler.h"
#include "WorldCmd.h"
#include <zooCmd/CmdManager.h>
#include <zooCmd_osg/InputDevice.h>

using namespace osgEarth;
using namespace zooCmd_osg;

LocateModelHandler::LocateModelHandler(osg::Node* model, float height, float scale, bool repeat)
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
	InputDevice::getIns()->getGroupNode(0)->addChild(_locateTransform);
}

bool LocateModelHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view)
		return false;

	if (ea.getButtonMask() == ea.RIGHT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		InputDevice::getIns()->getGroupNode(0)->removeChild(_locateTransform);
		zooCmd::CmdManager::getSingleton().block(false);
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
			InputDevice::getIns()->getGroupNode(0)->addChild(_locateTransform);
		}
		else
		{
			zooCmd::CmdManager::getSingleton().block(false);
		}

		return true;
	}
	
	if (ea.getEventType() == ea.MOVE)
	{
		osgUtil::LineSegmentIntersector::Intersections results;
		if (view->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
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
