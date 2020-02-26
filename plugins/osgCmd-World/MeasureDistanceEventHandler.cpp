#include "MeasureDistanceEventHandler.h"
#include "WorldCmd.h"
#include "WorldControls.h"
#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>

using namespace osgEarth;

MeasureDistanceEventHandler::MeasureDistanceEventHandler()
	: _totalDistance(0)
{
	_lineStrip = new osg::Geode();
	_lineStrip->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.0f), osg::StateAttribute::ON);
	_lineStrip->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	osgCmd::CmdManager::getSingleton().getRenderer()->getRootNode()->addChild(_lineStrip);
}

MeasureDistanceEventHandler::~MeasureDistanceEventHandler()
{
	WorldControls::getIns()->removeLabelTextDisplay(dist_label_);
	osgCmd::CmdManager::getSingleton().getRenderer()->getRootNode()->removeChild(_lineStrip);
}

bool MeasureDistanceEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
	if (!viewer)
		return false;

	if (ea.getButtonMask() == ea.RIGHT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		_distPointSet = nullptr;
		osgCmd::CmdManager::getSingleton().block(false);
		return true;
	}

	double vpRange = WorldCmd::getSingleton().getEarthManipulator()->getViewpoint().getRange();
	if (vpRange > 100000)
		return false;

	if (ea.getButtonMask() == ea.LEFT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		osg::Vec3d pt;
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (viewer->computeIntersections(ea.getX(), ea.getY(), intersections))
		{
			auto it = intersections.begin();
			if (it != intersections.end())
			{
				pt = it->getWorldIntersectPoint();

				do 
				{
					if (!_distPointSet)
					{
						_distPointSet = new osg::Vec3dArray();
						_distPointSet->push_back(pt);
						_startPoint = pt;
						_totalDistance = 0;
						break;
					}

					_endPoint = pt;
					GeoPoint mapPoint;
					mapPoint.fromWorld(WorldCmd::getSingleton().getMapNode()->getMapSRS(), _startPoint);
					osg::Vec3d startlla = mapPoint.vec3d();
					mapPoint.fromWorld(WorldCmd::getSingleton().getMapNode()->getMapSRS(), _endPoint);
					osg::Vec3d endlla = mapPoint.vec3d();

					int dist = (_endPoint - _startPoint).length();
					if (dist >= 1)
					{
						double deltaLon = (endlla.x() - startlla.x()) / dist;
						double deltaLat = (endlla.y() - startlla.y()) / dist;

						for (int i = 0; i < dist; ++i)
						{
							double lon = startlla.x() + i * deltaLon;
							double lat = startlla.y() + i * deltaLat;

							osg::Vec3d lowPt;
							GeoPoint point(WorldCmd::getSingleton().getMapNode()->getMapSRS(), lon, lat, -1000, osgEarth::ALTMODE_ABSOLUTE);
							point.toWorld(lowPt);

							osg::Vec3d heightPt;
							point.set(WorldCmd::getSingleton().getMapNode()->getMapSRS(), lon, lat, 80000, osgEarth::ALTMODE_ABSOLUTE);
							point.toWorld(heightPt);

							osg::Vec3d intersectPt;
							if (!intersectPoint(intersectPt, lowPt, heightPt, WorldCmd::getSingleton().getMapNode()->getTerrainEngine()))
								continue;

							_distPointSet->push_back(intersectPt);
						}
					}

					if (_distPointSet->size() >= 2)
					{
						osg::ref_ptr<osg::Geometry> geo = new osg::Geometry();
						_lineStrip->addChild(geo);
						geo->setVertexArray(_distPointSet);
						osg::ref_ptr<osg::Vec3dArray> color = new osg::Vec3dArray();
						color->push_back(osg::Vec3d(1.0, 0.0, 0.0));
						geo->setColorArray(color, osg::Array::BIND_OVERALL);
						geo->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, _distPointSet->size()));

						_totalDistance += getDistance(_distPointSet);
						_distPointSet = new osg::Vec3dArray();
						_distPointSet->push_back(pt);
						_startPoint = pt;

						char szbuf[128];
						sprintf_s(szbuf, sizeof(szbuf), "[Measure distance] Total distance: %.2f", _totalDistance);
						WorldControls::getIns()->addLabelTextDisplay(szbuf, dist_label_);
					}

				} while (0);

				return true;
			}
		}
	}

	return false;
}

bool MeasureDistanceEventHandler::intersectPoint(osg::Vec3d& intersectPt, osg::Vec3d startPt, osg::Vec3d endPt, osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osgUtil::LineSegmentIntersector> ls = new osgUtil::LineSegmentIntersector(startPt, endPt);
	osg::ref_ptr<osgUtil::IntersectionVisitor> iv = new osgUtil::IntersectionVisitor(ls);

	if (node)
		node->accept(*iv);

	if (ls->containsIntersections())
	{
		intersectPt = ls->getFirstIntersection().getWorldIntersectPoint();
		return true;
	}

	intersectPt = osg::Vec3d();
	return false;
}

double MeasureDistanceEventHandler::getDistance(osg::Vec3dArray* ptset)
{
	double dist = 0;
	for (auto it = ptset->begin(); it != ptset->end(); ++it)
	{
		auto itNext = it;
		++itNext;
		if (itNext == ptset->end())
			break;

		dist += ((*itNext) - (*it)).length();
	}

	return dist;
}
