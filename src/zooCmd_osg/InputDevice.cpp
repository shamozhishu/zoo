#include <zooCmd_osg/InputDevice.h>
#include <zoo/BitState.h>
#include <zoo/Interlock.h>
#include <zooCmd/ZooCmd.h>
#include <zooCmd/CmdManager.h>

ZOO_REGISTER(zooCmd_osg::InputDevice)

namespace zooCmd_osg {

class Viewers : public osgViewer::CompositeViewer
{
	osg::Timer _frameTime;
public:
	Viewers() { _frameTime.setStartTick(0); }
	void frame(double simulationTime = USE_REFERENCE_TIME)
	{
		// limit the frame rate
		if (getRunMaxFrameRate() > 0.0)
		{
			double dt = _frameTime.time_s();
			double minFrameTime = 1.0 / getRunMaxFrameRate();
			if (dt < minFrameTime)
				OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0 * (minFrameTime - dt)));
		}

		// avoid excessive CPU loading when no frame is required in ON_DEMAND mode
		if (getRunFrameScheme() == osgViewer::ViewerBase::ON_DEMAND)
		{
			double dt = _frameTime.time_s();
			if (dt < 0.01)
				OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0 * (0.01 - dt)));
		}

		// record start frame time
		_frameTime.setStartTick();

		CmdManager::waitExchanged();
		osgViewer::CompositeViewer::frame(simulationTime);
		CmdManager::releaseWait();
	}
};

ZOO_REFLEX_IMPLEMENT(InputDevice);
InputDevice::InputDevice()
	: _osgInited(false)
	, _windowScale(1.0f)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	ds->setNvOptimusEnablement(1);
	ds->setStereo(false);
	_compositeViewer = new Viewers;
}

InputDevice::~InputDevice()
{
}

InputDevice* InputDevice::getIns()
{
	return dynamic_cast<InputDevice*>(CmdManager::getSingleton().getInputAdapter());
}

osgViewer::CompositeViewer* InputDevice::getViewer() const
{
	return _compositeViewer.get();
}

osg::Group* InputDevice::getGroupNode(unsigned int idx /*= -1*/, bool createIfNot /*= true*/)
{
	if (!createIfNot)
	{
		if (idx < s_groupNodeCount)
			return _groupNodes[idx].get();
		return nullptr;
	}

	if (idx < s_groupNodeCount)
	{
		if (!_groupNodes[idx].get())
		{
			osg::ref_ptr<osg::Group> ptr(new osg::Group);
			_groupNodes[idx] = ptr;
		}

		return _groupNodes[idx].get();
	}


	for (unsigned int i = 0; i < s_groupNodeCount; ++i)
	{
		if (!_groupNodes[i].get())
		{
			osg::ref_ptr<osg::Group> ptr(new osg::Group);
			_groupNodes[i] = ptr;
			return _groupNodes[i].get();
		}
	}

	return nullptr;
}

osgViewer::View* InputDevice::createView(float ratioLeft, float ratioRight, float ratioBottom, float ratioTop, const osg::Vec4& color /*= osg::Vec4(0, 0, 0, 0)*/)
{
	osgViewer::View* view = new osgViewer::View;
	_compositeViewer->addView(view);

	if (_osgWinEmb)
	{
		resizeView(view, ratioLeft, ratioRight, ratioBottom, ratioTop);
		view->getCamera()->setGraphicsContext(_osgWinEmb.get());
	}

	view->getCamera()->setNearFarRatio(0.00003);
	view->getCamera()->setClearColor(color);
	view->addEventHandler(new osgViewer::StatsHandler);
	view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));
	return view;
}

void InputDevice::resizeView(osgViewer::View* view, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop)
{
	if (view && _osgWinEmb)
	{
		int x, y, width, height;
		_osgWinEmb->getWindowRectangle(x, y, width, height);

		double left = (double)(x + width) * ratioLeft, right = (double)(x + width) * ratioRight;
		double top = (double)(y + height) * ratioTop, bottom = (double)(y + height) * ratioBottom;

		x = left, y = bottom, width = right - left; height = top - bottom;

		double frusht, fruswid, fudge, near_ref = 1.0, far_ref = 100000.0;
		double aspectratio = static_cast<double>(width) / static_cast<double>(height);


		view->getCamera()->setViewport(new osg::Viewport(x, y, width, height));
		view->getCamera()->getProjectionMatrixAsFrustum(left, right, bottom, top, near_ref, far_ref);

		bool gotfrustum = true;
		if (gotfrustum)
		{
			aspectratio = (double)width / (double)height;
			frusht = top - bottom;
			fruswid = right - left;
			fudge = frusht * aspectratio / fruswid;
			right = right * fudge;
			left = left * fudge;
			view->getCamera()->setProjectionMatrixAsFrustum(left, right, bottom, top, near_ref, far_ref);
		}
	}
}

int InputDevice::run()
{
	return _compositeViewer->run();
}

bool InputDevice::init()
{
	if (_compositeViewer->getNumViews() == 0)
	{
		osgViewer::View* view = createView(0, 1, 0, 1);
		osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
		keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator);
		keyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator);
		keyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator);
		keyswitchManipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator);
		keyswitchManipulator->addMatrixManipulator('5', "Orbit", new osgGA::OrbitManipulator);
		keyswitchManipulator->addMatrixManipulator('6', "FirstPerson", new osgGA::FirstPersonManipulator);
		keyswitchManipulator->addMatrixManipulator('7', "Spherical", new osgGA::SphericalManipulator);
		view->setCameraManipulator(keyswitchManipulator.get());
	}

	_compositeViewer->realize();
	return true;
}

bool InputDevice::isDone()
{
	return _compositeViewer->done();
}

void InputDevice::setDone(bool done)
{
	_compositeViewer->setDone(done);
}

void InputDevice::frame(double simulationTime)
{
	_compositeViewer->frame(simulationTime);
}

void InputDevice::setup(int windowWidth, int windowHeight, float windowScale)
{
	if (windowWidth > 0 && windowHeight > 0)
	{
		_osgInited = true;
		_windowScale = windowScale;
		_osgWinEmb = new osgViewer::GraphicsWindowEmbedded(0, 0, windowWidth * windowScale, windowHeight * windowScale);
		_osgWinEmb->getEventQueue()->syncWindowRectangleWithGraphicsContext();
		_compositeViewer->setReleaseContextAtEndOfFrameHint(false);
		_compositeViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	}

	_compositeViewer->setKeyEventSetsDone(0);
}

void InputDevice::resize(int windowWidth, int windowHeight, float windowScale)
{
	if (_osgInited)
	{
		_windowScale = windowScale;
		_osgWinEmb->getEventQueue()->windowResize(0, 0, windowWidth * windowScale, windowHeight * windowScale);
		_osgWinEmb->resized(0, 0, windowWidth * windowScale, windowHeight * windowScale);
	}
}

void InputDevice::keyPress(int key, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->keyPress(remap(key));
}

void InputDevice::keyRelease(int key, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->keyRelease(remap(key));
}

void InputDevice::mouseMove(int x, int y, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseMotion(x * _windowScale, y * _windowScale);
}

void InputDevice::wheel(int x, int y, unsigned int modkey, unsigned int scroll)
{
	osgGA::GUIEventAdapter::ScrollingMotion sm = osgGA::GUIEventAdapter::SCROLL_NONE;
	switch (scroll)
	{
	case 1:
		sm = osgGA::GUIEventAdapter::SCROLL_LEFT;
		break;
	case 2:
		sm = osgGA::GUIEventAdapter::SCROLL_RIGHT;
		break;
	case 3:
		sm = osgGA::GUIEventAdapter::SCROLL_UP;
		break;
	case 4:
		sm = osgGA::GUIEventAdapter::SCROLL_DOWN;
		break;
	case 5:
		sm = osgGA::GUIEventAdapter::SCROLL_2D;
		break;
	default:
		break;
	}

	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseMotion(x * _windowScale, y * _windowScale);
	_osgWinEmb->getEventQueue()->mouseScroll(sm);
}

void InputDevice::mousePress(int x, int y, unsigned int modkey, unsigned int button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseButtonPress(x * _windowScale, y * _windowScale, button);
}

void InputDevice::mouseRelease(int x, int y, unsigned int modkey, unsigned int button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseButtonRelease(x * _windowScale, y * _windowScale, button);
}

void InputDevice::mouseDoubleClick(int x, int y, unsigned int modkey, unsigned int button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseDoubleButtonPress(x * _windowScale, y * _windowScale, button);
}

int InputDevice::remap(int key)
{
	auto it = _keyboardMap.find(key);
	if (it == _keyboardMap.end())
		return key;

	int k = _keyboardMap[key];
	switch (k)
	{
	case zooCmd_Key_Escape: key = osgGA::GUIEventAdapter::KEY_Escape; break;
	case zooCmd_Key_Delete: key = osgGA::GUIEventAdapter::KEY_Delete; break;
	case zooCmd_Key_Home: key = osgGA::GUIEventAdapter::KEY_Home; break;
	case zooCmd_Key_Enter: key = osgGA::GUIEventAdapter::KEY_KP_Enter; break;
	case zooCmd_Key_End: key = osgGA::GUIEventAdapter::KEY_End; break;
	case zooCmd_Key_Return: key = osgGA::GUIEventAdapter::KEY_Return; break;
	case zooCmd_Key_PageUp: key = osgGA::GUIEventAdapter::KEY_Page_Up; break;
	case zooCmd_Key_PageDown: key = osgGA::GUIEventAdapter::KEY_Page_Down; break;
	case zooCmd_Key_Left: key = osgGA::GUIEventAdapter::KEY_Left; break;
	case zooCmd_Key_Right: key = osgGA::GUIEventAdapter::KEY_Right; break;
	case zooCmd_Key_Up: key = osgGA::GUIEventAdapter::KEY_Up; break;
	case zooCmd_Key_Down: key = osgGA::GUIEventAdapter::KEY_Down; break;
	case zooCmd_Key_Backspace: key = osgGA::GUIEventAdapter::KEY_BackSpace; break;
	case zooCmd_Key_Tab: key = osgGA::GUIEventAdapter::KEY_Tab; break;
	case zooCmd_Key_Space: key = osgGA::GUIEventAdapter::KEY_Space; break;
	case zooCmd_Key_Alt: key = osgGA::GUIEventAdapter::KEY_Alt_L; break;
	case zooCmd_Key_Shift: key = osgGA::GUIEventAdapter::KEY_Shift_L; break;
	case zooCmd_Key_Control: key = osgGA::GUIEventAdapter::KEY_Control_L; break;
	case zooCmd_Key_Meta: key = osgGA::GUIEventAdapter::KEY_Meta_L; break;
	case zooCmd_Key_F1: key = osgGA::GUIEventAdapter::KEY_F1; break;
	case zooCmd_Key_F2: key = osgGA::GUIEventAdapter::KEY_F2; break;
	case zooCmd_Key_F3: key = osgGA::GUIEventAdapter::KEY_F3; break;
	case zooCmd_Key_F4: key = osgGA::GUIEventAdapter::KEY_F4; break;
	case zooCmd_Key_F5: key = osgGA::GUIEventAdapter::KEY_F5; break;
	case zooCmd_Key_F6: key = osgGA::GUIEventAdapter::KEY_F6; break;
	case zooCmd_Key_F7: key = osgGA::GUIEventAdapter::KEY_F7; break;
	case zooCmd_Key_F8: key = osgGA::GUIEventAdapter::KEY_F8; break;
	case zooCmd_Key_F9: key = osgGA::GUIEventAdapter::KEY_F9; break;
	case zooCmd_Key_F10: key = osgGA::GUIEventAdapter::KEY_F10; break;
	case zooCmd_Key_F11: key = osgGA::GUIEventAdapter::KEY_F11; break;
	case zooCmd_Key_F12: key = osgGA::GUIEventAdapter::KEY_F12; break;
	case zooCmd_Key_F13: key = osgGA::GUIEventAdapter::KEY_F13; break;
	case zooCmd_Key_F14: key = osgGA::GUIEventAdapter::KEY_F14; break;
	case zooCmd_Key_F15: key = osgGA::GUIEventAdapter::KEY_F15; break;
	case zooCmd_Key_F16: key = osgGA::GUIEventAdapter::KEY_F16; break;
	case zooCmd_Key_F17: key = osgGA::GUIEventAdapter::KEY_F17; break;
	case zooCmd_Key_F18: key = osgGA::GUIEventAdapter::KEY_F18; break;
	case zooCmd_Key_F19: key = osgGA::GUIEventAdapter::KEY_F19; break;
	case zooCmd_Key_F20: key = osgGA::GUIEventAdapter::KEY_F20; break;
	case zooCmd_Key_Hyphen: key = '-'; break;
	case zooCmd_Key_Equal: key = '='; break;
	case zooCmd_Key_Division: key = osgGA::GUIEventAdapter::KEY_KP_Divide; break;
	case zooCmd_Key_Multiply: key = osgGA::GUIEventAdapter::KEY_KP_Multiply; break;
	case zooCmd_Key_Minus: key = '-'; break;
	case zooCmd_Key_Plus: key = '+'; break;
	case zooCmd_Key_Insert: key = osgGA::GUIEventAdapter::KEY_KP_Insert; break;
	default: break;
	}

	return key;
}

void InputDevice::setKeyboardModifiers(unsigned int modkey)
{
	BitState bitmodkey = modkey;
	unsigned int mask = 0;

	if (bitmodkey.checkState(ESTATE_01))
		mask |= osgGA::GUIEventAdapter::MODKEY_SHIFT;

	if (bitmodkey.checkState(ESTATE_02))
		mask |= osgGA::GUIEventAdapter::MODKEY_CTRL;

	if (bitmodkey.checkState(ESTATE_03))
		mask |= osgGA::GUIEventAdapter::MODKEY_ALT;

	_osgWinEmb->getEventQueue()->getCurrentEventState()->setModKeyMask(mask);
}

}
