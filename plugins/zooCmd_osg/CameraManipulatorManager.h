#ifndef __CAMERA_MANIPULATOR_MANAGER_H__
#define __CAMERA_MANIPULATOR_MANAGER_H__

#include <zooCmd_osg/OsgEarthSupport.h>

class OsgEarthContext;
class CameraManipulatorManager : public osgGA::KeySwitchMatrixManipulator
{
public:
	CameraManipulatorManager(OsgEarthContext* context);
	~CameraManipulatorManager();
	bool isSwitchEnabled() const;
	void setSwitchEnabled(bool enabled);
	void home(double currentTime);
	void focus(int num, osg::Node* pNode);
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

protected:
	bool _switchEnabled;
	OsgEarthContext* _context;
};

#endif // __CAMERA_MANIPULATOR_MANAGER_H__
