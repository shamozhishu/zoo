#ifndef __OSG_DEVICE_H__
#define __OSG_DEVICE_H__

#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthSupport.h>

class OsgDevice : public Service
{
	ZOO_SERVICE(OsgDevice)
public:
	virtual osgViewer::CompositeViewer* getViewer() const = 0;
	virtual osgViewer::GraphicsWindow* getGraphicsWnd(int windowID) = 0;
	virtual osgViewer::View* createView(int viewID, osgViewer::GraphicsWindow* gw, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop, const osg::Vec4& color = osg::Vec4(0, 0, 0, 0)) = 0;
	virtual osgViewer::View* getView(int viewID) const = 0;
	virtual void destroyView(int viewID) = 0;
	virtual void destroyAllViews() = 0;
	virtual void resizeView(osgViewer::View* view, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop) = 0;
};

#endif // __OSG_DEVICE_H__
