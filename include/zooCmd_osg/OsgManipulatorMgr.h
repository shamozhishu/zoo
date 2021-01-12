#ifndef __OSG_MANIPULATOR_MGR_H__
#define __OSG_MANIPULATOR_MGR_H__

#include <zooCmd_osg/OsgEarthSupport.h>

class OsgContext;
class OsgManipulatorMgr : public osgGA::KeySwitchMatrixManipulator
{
public:
	OsgManipulatorMgr(OsgContext* context) : _context(context), _switchEnabled(false) {}
	virtual ~OsgManipulatorMgr() {}
	bool isSwitchEnabled() const { return _switchEnabled; }
	void setSwitchEnabled(bool enabled) { _switchEnabled = enabled; }
	virtual void home(double currentTime) = 0;
	virtual void focus(int num, osg::Node* pNode) = 0;

protected:
	bool _switchEnabled;
	OsgContext* _context;
};

#endif // __OSG_MANIPULATOR_MGR_H__
