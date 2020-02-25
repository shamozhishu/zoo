#include "LongitudeLatitudeEventHandler.h"
#include "WorldCmd.h"
#include "WorldControls.h"
#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>

using namespace osgEarth;

LongitudeLatitudeEventHandler::LongitudeLatitudeEventHandler(WorldCmd* worldcmd)
	: _worldCmd(worldcmd)
{
	_nodePath.push_back(_worldCmd->getMapNode()->getTerrainEngine());
}

LongitudeLatitudeEventHandler::~LongitudeLatitudeEventHandler()
{
	WorldControls::getIns()->removeLabelTextDisplay(lla_label_);
	WorldControls::getIns()->removeLabelTextDisplay(ipt_label_);
}

bool LongitudeLatitudeEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (!viewer)
		return false;

	if (ea.getEventType() == ea.FRAME)
	{
		char szbuf[512];
		Viewpoint vp = _worldCmd->getEarthManipulator()->getViewpoint();
		sprintf_s(szbuf, sizeof(szbuf), "[Viewpoint] Longitude: %.2f Latitude: %.2f Distance: %.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.getRange());
		WorldControls::getIns()->addLabelTextDisplay(szbuf, lla_label_);

		osgUtil::LineSegmentIntersector::Intersections results;
		if (viewer->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
		{
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d world = first.getWorldIntersectPoint();
			GeoPoint mapPoint;
			mapPoint.fromWorld(_worldCmd->getMapNode()->getMapSRS(), world);
			osg::Vec3d lla = mapPoint.vec3d();
			sprintf_s(szbuf, sizeof(szbuf), "[Mouse intersect point] Longitude: %.2f Latitude: %.2f Altitude: %.2f", lla.x(), lla.y(), lla.z());
			WorldControls::getIns()->addLabelTextDisplay(szbuf, ipt_label_);
		}
		else
		{
			WorldControls::getIns()->removeLabelTextDisplay(ipt_label_);
		}
	}

	return false;
}
