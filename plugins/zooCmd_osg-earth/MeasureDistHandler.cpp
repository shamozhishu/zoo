#include "MeasureDistHandler.h"
#include "EarthControls.h"
#include <zooCmd/CmdManager.h>

using namespace osgEarth;

MeasureDistHandler::MeasureDistHandler(osgEarth::MapNode* mapNode, osgEarth::Util::EarthManipulator* manipulator, EarthControls* controls)
	: _totalDistance(0)
	, _controls(controls)
	, _mapNode(mapNode)
	, _manipulator(manipulator)
{
	_lineStrip = new osg::Geode();
	_lineStrip->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.0f), osg::StateAttribute::ON);
	_lineStrip->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	_lineStrip->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	_lineStrip->getOrCreateStateSet()->setRenderBinDetails(11, "RenderBin");
	_mapNode->addChild(_lineStrip);

	Viewpoint vp = _manipulator->getViewpoint();
	if (vp.range() > 100000)
	{
		vp.range() = 80000;
		_manipulator->setViewpoint(vp, 2);
	}
}

MeasureDistHandler::~MeasureDistHandler()
{
	_mapNode->removeChild(_lineStrip);
}

bool MeasureDistHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*)
{
	osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
	if (!view)
		return false;

	if (ea.getButtonMask() == ea.RIGHT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		_distPointSet = nullptr;
		CmdManager::getSingleton().block(false);
		return true;
	}

	double vpRange = _manipulator->getViewpoint().getRange();
	if (vpRange > 100000)
		return false;

	if (ea.getButtonMask() == ea.LEFT_MOUSE_BUTTON && ea.getEventType() == ea.DOUBLECLICK)
	{
		osg::Vec3d pt;
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (view->computeIntersections(ea.getX(), ea.getY(), intersections))
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
					mapPoint.fromWorld(_mapNode->getMapSRS(), _startPoint);
					osg::Vec3d startlla = mapPoint.vec3d();
					mapPoint.fromWorld(_mapNode->getMapSRS(), _endPoint);
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
							GeoPoint point(_mapNode->getMapSRS(), lon, lat, -1000, osgEarth::ALTMODE_ABSOLUTE);
							point.toWorld(lowPt);

							osg::Vec3d heightPt;
							point.set(_mapNode->getMapSRS(), lon, lat, 80000, osgEarth::ALTMODE_ABSOLUTE);
							point.toWorld(heightPt);

							osg::Vec3d intersectPt;
							if (!intersectPoint(intersectPt, lowPt, heightPt, _mapNode->getTerrainEngine()))
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
						_controls->addLabelTextDisplay(szbuf, dist_label_);
					}

				} while (0);

				return true;
			}
		}
	}

	return false;
}

bool MeasureDistHandler::intersectPoint(osg::Vec3d& intersectPt, osg::Vec3d startPt, osg::Vec3d endPt, osg::ref_ptr<osg::Node> node)
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

double MeasureDistHandler::getDistance(osg::Vec3dArray* ptset)
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
