#include <osgCmd/Renderer.h>
#include <osgCmd/BitState.h>

namespace osgCmd {

class KeyboardMap
{
public:
	KeyboardMap()
	{
		_keyMap[Key_Escape] = osgGA::GUIEventAdapter::KEY_Escape;
		_keyMap[Key_Delete] = osgGA::GUIEventAdapter::KEY_Delete;
		_keyMap[Key_Home] = osgGA::GUIEventAdapter::KEY_Home;
		_keyMap[Key_Enter] = osgGA::GUIEventAdapter::KEY_KP_Enter;
		_keyMap[Key_End] = osgGA::GUIEventAdapter::KEY_End;
		_keyMap[Key_Return] = osgGA::GUIEventAdapter::KEY_Return;
		_keyMap[Key_PageUp] = osgGA::GUIEventAdapter::KEY_Page_Up;
		_keyMap[Key_PageDown] = osgGA::GUIEventAdapter::KEY_Page_Down;
		_keyMap[Key_Left] = osgGA::GUIEventAdapter::KEY_Left;
		_keyMap[Key_Right] = osgGA::GUIEventAdapter::KEY_Right;
		_keyMap[Key_Up] = osgGA::GUIEventAdapter::KEY_Up;
		_keyMap[Key_Down] = osgGA::GUIEventAdapter::KEY_Down;
		_keyMap[Key_Backspace] = osgGA::GUIEventAdapter::KEY_BackSpace;
		_keyMap[Key_Tab] = osgGA::GUIEventAdapter::KEY_Tab;
		_keyMap[Key_Space] = osgGA::GUIEventAdapter::KEY_Space;
		_keyMap[Key_Alt] = osgGA::GUIEventAdapter::KEY_Alt_L;
		_keyMap[Key_Shift] = osgGA::GUIEventAdapter::KEY_Shift_L;
		_keyMap[Key_Control] = osgGA::GUIEventAdapter::KEY_Control_L;
		_keyMap[Key_Meta] = osgGA::GUIEventAdapter::KEY_Meta_L;
		_keyMap[Key_F1] = osgGA::GUIEventAdapter::KEY_F1;
		_keyMap[Key_F2] = osgGA::GUIEventAdapter::KEY_F2;
		_keyMap[Key_F3] = osgGA::GUIEventAdapter::KEY_F3;
		_keyMap[Key_F4] = osgGA::GUIEventAdapter::KEY_F4;
		_keyMap[Key_F5] = osgGA::GUIEventAdapter::KEY_F5;
		_keyMap[Key_F6] = osgGA::GUIEventAdapter::KEY_F6;
		_keyMap[Key_F7] = osgGA::GUIEventAdapter::KEY_F7;
		_keyMap[Key_F8] = osgGA::GUIEventAdapter::KEY_F8;
		_keyMap[Key_F9] = osgGA::GUIEventAdapter::KEY_F9;
		_keyMap[Key_F10] = osgGA::GUIEventAdapter::KEY_F10;
		_keyMap[Key_F11] = osgGA::GUIEventAdapter::KEY_F11;
		_keyMap[Key_F12] = osgGA::GUIEventAdapter::KEY_F12;
		_keyMap[Key_F13] = osgGA::GUIEventAdapter::KEY_F13;
		_keyMap[Key_F14] = osgGA::GUIEventAdapter::KEY_F14;
		_keyMap[Key_F15] = osgGA::GUIEventAdapter::KEY_F15;
		_keyMap[Key_F16] = osgGA::GUIEventAdapter::KEY_F16;
		_keyMap[Key_F17] = osgGA::GUIEventAdapter::KEY_F17;
		_keyMap[Key_F18] = osgGA::GUIEventAdapter::KEY_F18;
		_keyMap[Key_F19] = osgGA::GUIEventAdapter::KEY_F19;
		_keyMap[Key_F20] = osgGA::GUIEventAdapter::KEY_F20;
		_keyMap[Key_hyphen] = '-';
		_keyMap[Key_Equal] = '=';
		_keyMap[Key_division] = osgGA::GUIEventAdapter::KEY_KP_Divide;
		_keyMap[Key_multiply] = osgGA::GUIEventAdapter::KEY_KP_Multiply;
		_keyMap[Key_Minus] = '-';
		_keyMap[Key_Plus] = '+';
		_keyMap[Key_Insert] = osgGA::GUIEventAdapter::KEY_KP_Insert;
	}

	int remapKey(Key key)
	{
		KeyMap::iterator itr = _keyMap.find(key);

		if (itr == _keyMap.end())
			return -1;
		else
			return itr->second;
	}

private:
	typedef std::map<unsigned int, int> KeyMap;
	KeyMap _keyMap;
};

KeyboardMap g_keyboardMap;

Renderer::Renderer()
	: _windowScale(1.0f)
	, _osgInitialized(false)
	, _continuousUpdate(true)
	, _osgWantsToRenderFrame(true)
	, _applicationAboutToQuit(false)
{
}

Renderer::~Renderer()
{
}

void Renderer::update()
{
}

void Renderer::setupOSG(int windowWidth, int windowHeight, float windowScale)
{
	_osgInitialized = true;
	_windowScale = windowScale;
	_osgWinEmb = new osgViewer::GraphicsWindowEmbedded(0, 0, windowWidth * windowScale, windowHeight * windowScale);
	_osgWinEmb->getEventQueue()->syncWindowRectangleWithGraphicsContext();
	_camera->setViewport(new osg::Viewport(0, 0, windowWidth * windowScale, windowHeight * windowScale));
	_camera->setGraphicsContext(_osgWinEmb.get());
	setKeyEventSetsDone(0);
	setReleaseContextAtEndOfFrameHint(false);
	setThreadingModel(osgViewer::Viewer::SingleThreaded);

	osgViewer::Viewer::Windows windows;
	getWindows(windows);
	_lastFrameStartTime.setStartTick(0);
}

bool Renderer::checkNeedToDoFrame()
{
	// check if any event handler has prompted a redraw
	if (_requestRedraw)
		return true;

	if (_requestContinousUpdate)
		return true;

	// check if the view needs to update the scene graph
	// this check if camera has update callback and if scene requires to update scene graph
	if (requiresUpdateSceneGraph())
		return true;

	// check if the database pager needs to update the scene
	if (getDatabasePager()->requiresUpdateSceneGraph())
		return true;

	// check if the image pager needs to update the scene
	if (getImagePager()->requiresUpdateSceneGraph())
		return true;

	// check if the scene needs to be redrawn
	if (requiresRedraw())
		return true;

	// check if events are available and need processing
	if (checkEvents())
		return true;

	// and check again if any event handler has prompted a redraw
	if (_requestRedraw)
		return true;

	if (_requestContinousUpdate)
		return true;

	return false;
}

void Renderer::frame(double simulationTime)
{
	// limit the frame rate
	if (getRunMaxFrameRate() > 0.0)
	{
		double dt = _lastFrameStartTime.time_s();
		double minFrameTime = 1.0 / getRunMaxFrameRate();
		if (dt < minFrameTime)
			OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0 * (minFrameTime - dt)));
	}

	// avoid excessive CPU loading when no frame is required in ON_DEMAND mode
	if (getRunFrameScheme() == osgViewer::ViewerBase::ON_DEMAND)
	{
		double dt = _lastFrameStartTime.time_s();
		if (dt < 0.01)
			OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0 * (0.01 - dt)));
	}

	// record start frame time
	_lastFrameStartTime.setStartTick();

	// make frame
#if 1
	osgViewer::Viewer::frame(simulationTime);
#else
	if (_done)
		return;

	if (_firstFrame)
	{
		viewerInit();
		if (!isRealized())
			realize();
		_firstFrame = false;
	}

	advance(simulationTime);
	eventTraversal();
	updateTraversal();
	renderingTraversals();
#endif
}

void Renderer::requestRedraw()
{
	osgViewer::Viewer::requestRedraw();
}

bool Renderer::checkEvents()
{
	// check events from any attached sources
	for (Devices::iterator itor = _eventSources.begin(); itor != _eventSources.end(); ++itor)
	{
		osgGA::Device* es = itor->get();
		if (es->getCapabilities() & osgGA::Device::RECEIVE_EVENTS)
		{
			if (es->checkEvents())
				return true;
		}
	}

	// get events from all windows attached to Viewer.
	Windows windows;
	getWindows(windows);

	for (Windows::iterator witr = windows.begin(); witr != windows.end(); ++witr)
	{
		if ((*witr)->checkEvents())
			return true;
	}

	return false;
}

bool Renderer::getContinuousUpdate() const
{
	return _continuousUpdate;
}

void Renderer::setContinuousUpdate(bool continuousUpdate)
{
	_continuousUpdate = continuousUpdate;
}

void Renderer::resize(int windowWidth, int windowHeight, float windowScale)
{
	if (!_osgInitialized)
		return;

	_windowScale = windowScale;
	_osgWinEmb->resized(0, 0, windowWidth * windowScale, windowHeight * windowScale);
	_osgWinEmb->getEventQueue()->windowResize(0, 0, windowWidth * windowScale, windowHeight * windowScale);
	update();
}

void Renderer::keyPressEvent(Key key, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	int value = g_keyboardMap.remapKey(key);
	_osgWinEmb->getEventQueue()->keyPress(value);
}

void Renderer::keyReleaseEvent(Key key, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	int value = g_keyboardMap.remapKey(key);
	_osgWinEmb->getEventQueue()->keyRelease(value);
}

void Renderer::mousePressEvent(int x, int y, unsigned int modkey, MouseButton button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseButtonPress(x * _windowScale, y * _windowScale, button);
}

void Renderer::mouseReleaseEvent(int x, int y, unsigned int modkey, MouseButton button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseButtonRelease(x * _windowScale, y * _windowScale, button);
}

void Renderer::mouseDoubleClickEvent(int x, int y, unsigned int modkey, MouseButton button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseDoubleButtonPress(x * _windowScale, y * _windowScale, button);
}

void Renderer::mouseMoveEvent(int x, int y, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseMotion(x * _windowScale, y * _windowScale);
}

void Renderer::wheelEvent(int x, int y, unsigned int modkey, Scroll scroll)
{
	osgGA::GUIEventAdapter::ScrollingMotion sm = osgGA::GUIEventAdapter::SCROLL_NONE;
	switch (scroll)
	{
	case SCROLL_LEFT:
		sm = osgGA::GUIEventAdapter::SCROLL_LEFT;
		break;
	case SCROLL_RIGHT:
		sm = osgGA::GUIEventAdapter::SCROLL_RIGHT;
		break;
	case SCROLL_UP:
		sm = osgGA::GUIEventAdapter::SCROLL_UP;
		break;
	case SCROLL_DOWN:
		sm = osgGA::GUIEventAdapter::SCROLL_DOWN;
		break;
	case SCROLL_2D:
		sm = osgGA::GUIEventAdapter::SCROLL_2D;
		break;
	default:
		break;
	}

	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseMotion(x * _windowScale, y * _windowScale);
	_osgWinEmb->getEventQueue()->mouseScroll(sm);
}

void Renderer::timerEvent()
{
	// application is about to quit, just return
	if (_applicationAboutToQuit)
		return;

	// ask ViewerWidget to update 3D view
	if (getRunFrameScheme() != osgViewer::ViewerBase::ON_DEMAND || checkNeedToDoFrame())
		update();
}

void Renderer::setKeyboardModifiers(unsigned int modkey)
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
