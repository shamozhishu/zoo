#include "LongitudeLatitudeHandler.h"
#include "WorldCmd.h"
#include "WorldControls.h"
#include <zooCmd/CmdManager.h>
#include <zooCmd_osg/InputDevice.h>

using namespace osgEarth;

LongitudeLatitudeHandler::LongitudeLatitudeHandler()
{
	_nodePath.push_back(WorldCmd::getSingleton().getMapNode()->getTerrainEngine());
}

bool LongitudeLatitudeHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view)
		return false;

	if (ea.getEventType() == ea.FRAME)
	{
		char szbuf[512];
		Viewpoint vp = WorldCmd::getSingleton().getEarthManipulator()->getViewpoint();
		sprintf_s(szbuf, sizeof(szbuf), "[Viewpoint] Longitude: %.2f Latitude: %.2f Distance: %.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.getRange());
		WorldControls::getIns()->addLabelTextDisplay(szbuf, lla_label_);

		osgUtil::LineSegmentIntersector::Intersections results;
		if (view->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
		{
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d world = first.getWorldIntersectPoint();
			GeoPoint mapPoint;
			mapPoint.fromWorld(WorldCmd::getSingleton().getMapNode()->getMapSRS(), world);
			osg::Vec3d lla = mapPoint.vec3d();
			sprintf_s(szbuf, sizeof(szbuf), "[Mouse intersect point] Longitude: %.2f Latitude: %.2f Altitude: %.2f", lla.x(), lla.y(), lla.z());
			WorldControls::getIns()->addLabelTextDisplay(szbuf, ipt_label_);
		}
		else if (WorldControls::getIns()->isHasLabelControl(ipt_label_))
		{
			WorldControls::getIns()->removeLabelTextDisplay(ipt_label_);
		}
	}

	return false;
}
