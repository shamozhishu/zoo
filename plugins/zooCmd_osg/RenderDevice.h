#ifndef __RENDER_DEVICE_H__
#define __RENDER_DEVICE_H__

#include <zoo/Reflection.h>
#include <zooCmd/RenderAdapter.h>
#include <zooCmd_osg/OsgDevice.h>

class RenderDevice : public OsgDevice, public RenderAdapter, public zoo::Type
{
	ZOO_REFLEX_DECLARE(RenderDevice)
public:
	RenderDevice();
	~RenderDevice();
	osgViewer::CompositeViewer* getViewer() const;
	osgViewer::GraphicsWindow* getGraphicsWnd(int windowID);
	osgViewer::View* createView(int viewID, osgViewer::GraphicsWindow* gw, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop, const osg::Vec4& color = osg::Vec4(0, 0, 0, 0));
	osgViewer::View* getView(int viewID) const;
	void destroyView(int viewID);
	void destroyAllViews();
	void resizeView(osgViewer::View* view, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop);

public:
	int run();
	bool isDone();
	void setDone(bool done);
	void frame(double simulationTime);
	void* setup(int windowWidth, int windowHeight, float windowScale, int windowID = 0);

private:
	class OsgEarthUtilsImpl* _osgUtilsImpl;
	std::map<int, osgViewer::View*> _viewList;
	osg::ref_ptr<osgViewer::CompositeViewer> _compositeViewer;
	std::unordered_map<int, osg::ref_ptr<osgViewer::GraphicsWindow>> _osgWinEmbs;
};

#endif // __RENDER_DEVICE_H__
