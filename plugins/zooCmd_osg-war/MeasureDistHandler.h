#ifndef __MEASURE_DIST_HANDLER_H__
#define __MEASURE_DIST_HANDLER_H__

#include <zooCmd_osg/OsgDevice.h>

class WarCmd;
class MeasureDistHandler : public osgGA::GUIEventHandler
{
public:
	MeasureDistHandler(osgEarth::Util::EarthManipulator* manipulator);
	~MeasureDistHandler();
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
	osg::observer_ptr<osgEarth::MapNode> _mapNode;
	osg::observer_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};

#endif // __MEASURE_DIST_HANDLER_H__
