#pragma once

#include <osgCmd/Support.h>

class WorldCmd;
class MeasureDistanceEventHandler : public osgGA::GUIEventHandler
{
public:
	MeasureDistanceEventHandler(WorldCmd* worldcmd);
	~MeasureDistanceEventHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

private:
	bool intersectPoint(osg::Vec3d& intersectPt, osg::Vec3d startPt, osg::Vec3d endPt, osg::ref_ptr<osg::Node> node);
	double getDistance(osg::Vec3dArray* ptset);

private:
	WorldCmd* _worldCmd;
	float _totalDistance;
	osg::Vec3d _startPoint;
	osg::Vec3d _endPoint;
	osg::ref_ptr<osg::Geode> _lineStrip;
	osg::ref_ptr<osg::Vec3dArray> _distPointSet;
};
