#ifndef __INPUT_DEVICE_H__
#define __INPUT_DEVICE_H__

#include <zoo/Reflection.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd_osg/OsgDevice.h>

class InputDevice : public OsgDevice, public InputAdapter, public zoo::Type
{
	ZOO_REFLEX_DECLARE(InputDevice)
public:
	InputDevice();
	~InputDevice();
	osgViewer::CompositeViewer* getViewer() const;
	osgViewer::View* createView(string name, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop, const osg::Vec4& color = osg::Vec4(0, 0, 0, 0));
	osgViewer::View* getView(string name) const;
	void destroyView(string name);
	void destroyAllViews();
	void resizeView(osgViewer::View* view, float ratioLeft, float ratioRight, float ratioBottom, float ratioTop);

public:
	int  run();
	bool isDone();
	void setDone(bool done);
	void frame(double simulationTime);
	void setup(int windowWidth, int windowHeight, float windowScale);
	void resize(int windowWidth, int windowHeight, float windowScale);
	void keyPress(int key, unsigned int modkey);
	void keyRelease(int key, unsigned int modkey);
	void mouseMove(int x, int y, unsigned int modkey);
	void wheel(int x, int y, unsigned int modkey, unsigned int scroll);
	void mousePress(int x, int y, unsigned int modkey, unsigned int button);
	void mouseRelease(int x, int y, unsigned int modkey, unsigned int button);
	void mouseDoubleClick(int x, int y, unsigned int modkey, unsigned int button);

protected:
	int remap(int key);
	void setKeyboardModifiers(unsigned int modkey);

private:
	map<string, osgViewer::View*>            _viewList;
	bool                                     _osgInited;
	osg::ref_ptr<osgViewer::GraphicsWindow>  _osgWinEmb;
	float                                    _windowScale;
	class OsgEarthContextImpl*               _contextImpl;
	osg::ref_ptr<osgViewer::CompositeViewer> _compositeViewer;
};

#endif // __INPUT_DEVICE_H__
