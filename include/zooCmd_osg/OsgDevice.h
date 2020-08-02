#ifndef __OSG_DEVICE_H__
#define __OSG_DEVICE_H__

#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthSupport.h>

class OsgDevice : public Service
{
	ZOO_SERVICE(OsgDevice)
public:
	virtual osgViewer::CompositeViewer* getViewer() const = 0;
	virtual osgViewer::View* createView(string name, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop, const osg::Vec4& color = osg::Vec4(0, 0, 0, 0)) = 0;
	virtual osgViewer::View* getView(string name) const = 0;
	virtual void destroyView(string name) = 0;
	virtual void destroyAllViews() = 0;
	virtual void resizeView(osgViewer::View* view, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop) = 0;
};

#endif // __OSG_DEVICE_H__
