#ifndef __CAMERA_MANIPULATOR_MANAGER_H__
#define __CAMERA_MANIPULATOR_MANAGER_H__

#include <zooCmd_osg/OsgManipulatorMgr.h>

class CameraManipulatorMgr : public OsgManipulatorMgr
{
public:
	CameraManipulatorMgr(OsgContext* context, bool isEarth);
	~CameraManipulatorMgr();
	void home(double currentTime);
	void focus(int num, osg::Node* pNode);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:
	bool _isEarth;
};

#endif // __CAMERA_MANIPULATOR_MANAGER_H__
