#include "WorldEventHandler.h"
#include "WorldCmd.h"
#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>

using namespace osgEarth;

WorldEventHandler::WorldEventHandler(WorldCmd* worldcmd)
	: _worldCmd(worldcmd)
{
	_nodePath.push_back(_worldCmd->getMapNode()->getTerrainEngine());

	_labelControl = new osgEarth::Util::Controls::LabelControl("", osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f), 14);
	_labelControl->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
	_labelControl->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_TOP);
}

WorldEventHandler::~WorldEventHandler()
{
}

bool WorldEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (!viewer)
		return false;

	if (ea.getEventType() == ea.FRAME)
	{
		char szbuf[512];
		Viewpoint vp = _worldCmd->getEarthManipulator()->getViewpoint();
		sprintf_s(szbuf, sizeof(szbuf), "[Viewpoint] Longitude: %.2f Latitude: %.2f Distance: %.2f", vp.focalPoint().get().x(), vp.focalPoint().get().y(), vp.getRange());

		osgUtil::LineSegmentIntersector::Intersections results;
		if (viewer->computeIntersections(ea.getX(), ea.getY(), _nodePath, results))
		{
			osgUtil::LineSegmentIntersector::Intersection first = *(results.begin());
			osg::Vec3d world = first.getWorldIntersectPoint();
			GeoPoint mapPoint;
			mapPoint.fromWorld(_worldCmd->getMapNode()->getMapSRS(), world);
			osg::Vec3d lla = mapPoint.vec3d();
			sprintf_s(szbuf, sizeof(szbuf), "%s\n[Mouse Intersect Point] Longitude: %.2f Latitude: %.2f Altitude: %.2f", szbuf, lla.x(), lla.y(), lla.z());
		}

		_labelControl->setText(szbuf);
	}

	return false;
}

void WorldEventHandler::showLabelControl(const osgCmd::UserData& userdata)
{
	bool show = osgCmd::any_cast<bool>(userdata.getData());
	osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::get(osgCmd::CmdManager::getSingleton().getRenderer());
	if (show)
	{
		osgCmd::CmdManager::getSingleton().getRenderer()->addEventHandler(this);
		canvas->addControl(_labelControl.get());
	}
	else
	{
		osgCmd::CmdManager::getSingleton().getRenderer()->removeEventHandler(this);
		canvas->removeControl(_labelControl.get());
	}
}
