#pragma once

#include <zoo/Reflection.h>
#include <zooCmd_osg/Support.h>
#include <zooCmd/InputAdapter.h>

namespace zooCmd_osg {

class _zooCmd_OSG_Export InputDevice : public InputAdapter, public zoo::Type
{
	REFLEX_DECLARE(InputDevice)
public:
	InputDevice();
	~InputDevice();
	static InputDevice* getIns();
	osgViewer::CompositeViewer* getViewer() const;
	osg::Group* getRootNode(unsigned int idx = -1, bool createIfNot = true);
	osgViewer::View* createView(float ratioLeft, float ratioRight, float ratioBottom, float ratioTop);

public:
	int  run();
	bool init();
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
	bool                                     _osgInited;
	osg::ref_ptr<osgViewer::GraphicsWindow>  _osgWinEmb;
	float                                    _windowScale;
	osg::ref_ptr<osgViewer::CompositeViewer> _compositeViewer;
	static const int                         s_rootNodeCount = 100;
	osg::ref_ptr<osg::Group>                 _rootNodes[s_rootNodeCount];
};

}
