#include "LonLatDistHandler.h"
#include "EarthControls.h"
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth;

LonLatDistHandler::LonLatDistHandler(OsgEarthContext* context, EarthControls* controls)
	: _context(context)
	, _controls(controls)
{
}

bool LonLatDistHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view || view != _context->getEarthView())
		return false;

	if (ea.getEventType() == ea.FRAME)
	{
		char szbuf[512];
		Viewpoint vp = _manipulator->getViewpoint();
		sprintf_s(szbuf, sizeof(szbuf), "[Viewpoint] Longitude: %.2f Latitude: %.2f Distance: %.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.getRange());
		_controls->addLabelTextDisplay(szbuf, lla_label_);

		osgUtil::LineSegmentIntersector::Intersections results;
		if (view->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
		{
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d world = first.getWorldIntersectPoint();
			GeoPoint mapPoint;
			mapPoint.fromWorld(_mapNode->getMapSRS(), world);
			osg::Vec3d lla = mapPoint.vec3d();
			sprintf_s(szbuf, sizeof(szbuf), "[Mouse intersect point] Longitude: %.2f Latitude: %.2f Altitude: %.2f", lla.x(), lla.y(), lla.z());
			_controls->addLabelTextDisplay(szbuf, ipt_label_);
		}
		else if (_controls->isHasLabelControl(ipt_label_))
		{
			_controls->removeLabelTextDisplay(ipt_label_);
		}
	}

	return false;
}

void LonLatDistHandler::setMapNode(osgEarth::MapNode* mapNode)
{
	_mapNode = mapNode;
	_nodePath.clear();
	_nodePath.push_back(_mapNode->getTerrainEngine());
}

void LonLatDistHandler::setEarthManipulator(osgEarth::Util::EarthManipulator* manipulator)
{
	_manipulator = manipulator;
}
