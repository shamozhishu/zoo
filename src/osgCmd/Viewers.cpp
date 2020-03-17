#include <osgCmd/Viewers.h>
#include <osgCmd/BitState.h>
#include <osgCmd/Interlock.h>

namespace osgCmd {

extern Interlock g_interlock;
extern std::map<unsigned int, int> g_keyboardMap;

Viewers::Viewers()
	: _osgInited(false)
	, _windowScale(1.0f)
{
}

Viewers::~Viewers()
{
}

osg::Group* Viewers::getRootNode(unsigned int idx /*= -1*/)
{
	if (idx < s_rootNodeCount)
	{
		if (!_rootNodes[idx].get())
		{
			osg::ref_ptr<osg::Group> ptr(new osg::Group);
			_rootNodes[idx] = ptr;
		}

		return _rootNodes[idx].get();
	}
	

	for (unsigned int i = 0; i < s_rootNodeCount; ++i)
	{
		if (!_rootNodes[i].get())
		{
			osg::ref_ptr<osg::Group> ptr(new osg::Group);
			_rootNodes[i] = ptr;
			return _rootNodes[i].get();
		}
	}

	return nullptr;
}

osgViewer::View* Viewers::createView(float ratioLeft, float ratioRight, float ratioBottom, float ratioTop)
{
	osgViewer::View* view = new osgViewer::View;
	addView(view);

	if (_osgWinEmb)
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

		view->getCamera()->setGraphicsContext(_osgWinEmb.get());
	}

	view->getCamera()->setNearFarRatio(0.00003);
	view->getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	view->addEventHandler(new osgViewer::StatsHandler);
	view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));
	return view;
}

void Viewers::init(int windowWidth, int windowHeight, float windowScale)
{
	if (windowWidth > 0 && windowHeight > 0)
	{
		_osgInited = true;
		_windowScale = windowScale;
		_osgWinEmb = new osgViewer::GraphicsWindowEmbedded(0, 0, windowWidth * windowScale, windowHeight * windowScale);
		_osgWinEmb->getEventQueue()->syncWindowRectangleWithGraphicsContext();
		setReleaseContextAtEndOfFrameHint(false);
		setThreadingModel(osgViewer::Viewer::SingleThreaded);
		_frameTime.setStartTick(0);
	}

	setKeyEventSetsDone(0);
}

void Viewers::frame(double simulationTime)
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

	g_interlock.waitExchanged();
	osgViewer::CompositeViewer::frame(simulationTime);
	g_interlock.releaseWait();
}

void Viewers::resize(int windowWidth, int windowHeight, float windowScale)
{
	if (_osgInited)
	{
		_windowScale = windowScale;
		_osgWinEmb->getEventQueue()->windowResize(0, 0, windowWidth * windowScale, windowHeight * windowScale);
		_osgWinEmb->resized(0, 0, windowWidth * windowScale, windowHeight * windowScale);
	}
}

void Viewers::keyPressEvent(int key, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	auto it = g_keyboardMap.find(key);
	if (it != g_keyboardMap.end())
		key = g_keyboardMap[key];
	_osgWinEmb->getEventQueue()->keyPress(key);
}

void Viewers::keyReleaseEvent(int key, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	auto it = g_keyboardMap.find(key);
	if (it != g_keyboardMap.end())
		key = g_keyboardMap[key];
	_osgWinEmb->getEventQueue()->keyRelease(key);
}

void Viewers::mousePressEvent(int x, int y, unsigned int modkey, unsigned int button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseButtonPress(x * _windowScale, y * _windowScale, button);
}

void Viewers::mouseReleaseEvent(int x, int y, unsigned int modkey, unsigned int button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseButtonRelease(x * _windowScale, y * _windowScale, button);
}

void Viewers::mouseDoubleClickEvent(int x, int y, unsigned int modkey, unsigned int button)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseDoubleButtonPress(x * _windowScale, y * _windowScale, button);
}

void Viewers::mouseMoveEvent(int x, int y, unsigned int modkey)
{
	setKeyboardModifiers(modkey);
	_osgWinEmb->getEventQueue()->mouseMotion(x * _windowScale, y * _windowScale);
}

void Viewers::wheelEvent(int x, int y, unsigned int modkey, unsigned int scroll)
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

void Viewers::setKeyboardModifiers(unsigned int modkey)
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
