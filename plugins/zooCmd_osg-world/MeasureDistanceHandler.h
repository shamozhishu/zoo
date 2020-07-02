#ifndef __MEASURE_DISTANCE_HANDLER_H__
#define __MEASURE_DISTANCE_HANDLER_H__

#include <zooCmd_osg/Support.h>

class WorldCmd;
class MeasureDistanceHandler : public osgGA::GUIEventHandler
{
public:
	MeasureDistanceHandler();
	~MeasureDistanceHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object*, osg::NodeVisitor*);

private:
	bool intersectPoint(osg::Vec3d& intersectPt, osg::Vec3d startPt, osg::Vec3d endPt, osg::ref_ptr<osg::Node> node);
	double getDistance(osg::Vec3dArray* ptset);

private:
	float _totalDistance;
	osg::Vec3d _startPoint;
	osg::Vec3d _endPoint;
	osg::ref_ptr<osg::Geode> _lineStrip;
	osg::ref_ptr<osg::Vec3dArray> _distPointSet;
};

#endif // __MEASURE_DISTANCE_HANDLER_H__
