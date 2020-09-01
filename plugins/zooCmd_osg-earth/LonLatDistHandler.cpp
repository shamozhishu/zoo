#include "LonLatDistHandler.h"
#include "EarthControls.h"
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth;

bool LonLatDistHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view || view != ServiceLocator<OsgEarthContext>::getService()->getOpView())
		return false;

	if (ea.getEventType() == ea.FRAME)
	{
		char szbuf[512];
		Viewpoint vp = _manipulator->getViewpoint();
		sprintf_s(szbuf, sizeof(szbuf), "[Viewpoint] Longitude: %.2f Latitude: %.2f Distance: %.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.getRange());
		EarthControls::getIns()->addLabelTextDisplay(szbuf, lla_label_);

		osgUtil::LineSegmentIntersector::Intersections results;
		if (view->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
		{
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d world = first.getWorldIntersectPoint();
			GeoPoint mapPoint;
			mapPoint.fromWorld(_mapNode->getMapSRS(), world);
			osg::Vec3d lla = mapPoint.vec3d();
			sprintf_s(szbuf, sizeof(szbuf), "[Mouse intersect point] Longitude: %.2f Latitude: %.2f Altitude: %.2f", lla.x(), lla.y(), lla.z());
			EarthControls::getIns()->addLabelTextDisplay(szbuf, ipt_label_);
		}
		else if (EarthControls::getIns()->isHasLabelControl(ipt_label_))
		{
			EarthControls::getIns()->removeLabelTextDisplay(ipt_label_);
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
