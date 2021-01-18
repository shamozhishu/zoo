#include "RenderDevice.h"
#include <zoo/BitState.h>
#include <zoo/Interlock.h>
#include <zooCmd/ZooCmd.h>
#include <zooCmd/CmdManager.h>
#include "OsgEarthUtilsImpl.h"
#include "MaterialManager.h"
#include "BuiltinMaterial.h"
#include "MeshManager.h"
#include "BuiltinMesh.h"
#include <osgQt/GraphicsWindowQt.h>

ZOO_REGISTER(RenderDevice)

class Viewers : public osgViewer::CompositeViewer
{
public:
	void frame(double simulationTime = USE_REFERENCE_TIME)
	{
		double minFrameTime = _runMaxFrameRate > 0.0 ? 1.0 / _runMaxFrameRate : 0.0;
		osg::Timer_t startFrameTick = osg::Timer::instance()->tick();
		if (_runFrameScheme == ON_DEMAND)
		{
			if (checkNeedToDoFrame())
			{
				CmdManager::waitExchanged();
				osgViewer::CompositeViewer::frame();
				CmdManager::releaseWait();
			}
			else
			{
				// we don't need to render a frame but we don't want to spin the run loop so make sure the minimum
				// loop time is 1/100th of second, if not otherwise set, so enabling the frame microSleep below to
				// avoid consume excessive CPU resources.
				if (minFrameTime == 0.0) minFrameTime = 0.01;
			}
		}
		else
		{
			CmdManager::waitExchanged();
			osgViewer::CompositeViewer::frame();
			CmdManager::releaseWait();
		}

		// work out if we need to force a sleep to hold back the frame rate
		osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
		double frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
		if (frameTime < minFrameTime) OpenThreads::Thread::microSleep(static_cast<unsigned int>(1000000.0*(minFrameTime - frameTime)));
	}
};

ZOO_REFLEX_IMPLEMENT(RenderDevice);
RenderDevice::RenderDevice()
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	ds->setNvOptimusEnablement(1);
	ds->setStereo(false);

	_compositeViewer = new Viewers;
	_compositeViewer->setReleaseContextAtEndOfFrameHint(false);
	_compositeViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	_compositeViewer->setKeyEventSetsDone(0);
	_osgUtilsImpl = new OsgEarthUtilsImpl;
	//-------------------------------------------
	// 初始化网格管理器和所有的内建网格
	new MeshManager;
	new DefaultMesh;
	new SphereMesh;
	new BoxMesh;
	new BillboardMesh;
	//-------------------------------------------
	// 初始化材质管理器和所有的内建材质
	new MaterialManager;
	new BumpMapping;
}

RenderDevice::~RenderDevice()
{
	delete MaterialManager::getSingletonPtr();
	delete MeshManager::getSingletonPtr();
	SAFE_DELETE(_osgUtilsImpl);
	destroyAllViews();
}

osgViewer::CompositeViewer* RenderDevice::getViewer() const
{
	return _compositeViewer.get();
}

osgViewer::GraphicsWindow* RenderDevice::getGraphicsWnd(int windowID)
{
	return _osgWinEmbs[windowID].get();
}

osgViewer::View* RenderDevice::createView(int viewID, osgViewer::GraphicsWindow* gw, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop, const osg::Vec4& color /*= osg::Vec4(0, 0, 0, 0)*/)
{
	osgViewer::View* view = getView(viewID);
	if (view)
		_compositeViewer->removeView(view);

	view = new osgViewer::View;
	_viewList[viewID] = view;
	_compositeViewer->addView(view);

	if (gw)
	{
		view->getCamera()->setGraphicsContext(gw);
		resizeView(view, ratioLeft, ratioRight, ratioBottom, ratioTop);
	}

	view->getCamera()->setNearFarRatio(0.00008);
	view->getCamera()->setClearColor(color);
	view->addEventHandler(new osgViewer::StatsHandler);
	view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));
	return view;
}

osgViewer::View* RenderDevice::getView(int viewID) const
{
	auto it = _viewList.find(viewID);
	if (it != _viewList.end())
		return it->second;
	return nullptr;
}

void RenderDevice::destroyView(int viewID)
{
	auto it = _viewList.find(viewID);
	if (it != _viewList.end())
	{
		_compositeViewer->removeView(it->second);
		_viewList.erase(it);
	}
}

void RenderDevice::destroyAllViews()
{
	auto it = _viewList.begin();
	auto itEnd = _viewList.end();
	for (; it != itEnd; ++it)
		_compositeViewer->removeView(it->second);
	_viewList.clear();
}

void RenderDevice::resizeView(osgViewer::View* view, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop)
{
	osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>(view->getCamera()->getGraphicsContext());
	if (view && gw)
	{
		int x, y, width, height;
		gw->getWindowRectangle(x, y, width, height);

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

int RenderDevice::run()
{
	return _compositeViewer->run();
}

bool RenderDevice::isDone()
{
	return _compositeViewer->done();
}

void RenderDevice::setDone(bool done)
{
	_compositeViewer->setDone(done);
}

void RenderDevice::frame(double simulationTime)
{
	_compositeViewer->frame(simulationTime);
}

void* RenderDevice::getWnd(int windowID, bool wndHandle /*= false*/)
{
	auto it = _osgWinEmbs.find(windowID);
	if (it == _osgWinEmbs.end())
		return nullptr;

	osgQt::GraphicsWindowQt* pWndQt = dynamic_cast<osgQt::GraphicsWindowQt*>(it->second.get());
	if (!pWndQt)
		return nullptr;

	if (wndHandle)
		return pWndQt->getWndHandle();

	return pWndQt->getGLWidget();
}

void* RenderDevice::setup(int windowWidth, int windowHeight, float windowScale, int windowID/* = 0*/)
{
	osgQt::GraphicsWindowQt* pGW = nullptr;

	if (windowWidth > 0 && windowHeight > 0)
	{
		osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
		ds->setNumMultiSamples(4);

		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits();
		traits->readDISPLAY();
		if (traits->displayNum < 0)
			traits->displayNum = 0;

		traits->windowName = "GraphicsWindowQt";
		traits->windowDecoration = false;
		traits->x = 0;
		traits->y = 0;
		traits->width = windowWidth * windowScale;
		traits->height = windowHeight * windowScale;
		traits->doubleBuffer = true;
		traits->sharedContext = NULL;

		traits->alpha = ds->getMinimumNumAlphaBits();
		traits->stencil = ds->getMinimumNumStencilBits();
		traits->sampleBuffers = ds->getMultiSamples();
		traits->samples = ds->getNumMultiSamples();

		pGW = new osgQt::GraphicsWindowQt(traits);
		_osgWinEmbs[windowID] = pGW;
	}

	if (_compositeViewer->getNumViews() == 0)
	{
		osgViewer::View* pView = new osgViewer::View;
		_compositeViewer->addView(pView);
		pView->getCamera()->setGraphicsContext(pGW);
		pView->getCamera()->setClearColor(osg::Vec4(0, 0, 0, 0));
		resizeView(pView, 0, 1, 0, 1);
	}

	return pGW ? pGW->getGLWidget() : nullptr;
}
