#ifndef __CAMERA_MANIPULATOR_MANAGER_H__
#define __CAMERA_MANIPULATOR_MANAGER_H__

#include <zooCmd_osg/OsgEarthSupport.h>

class CameraManipulatorManager : public osgGA::KeySwitchMatrixManipulator
{
public:
	CameraManipulatorManager();
	~CameraManipulatorManager();
	bool isSwitchEnabled() const;
	void setSwitchEnabled(bool enabled);
	void home(double currentTime);
	void focus(int num, osg::Node* pNode);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
	bool _switchEnabled;
};

#endif // __CAMERA_MANIPULATOR_MANAGER_H__
