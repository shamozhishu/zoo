#pragma once

#include <osgCmd/Support.h>

namespace osgCmd {

class _osgCmdExport Viewers : public osgViewer::CompositeViewer
{
public:
	Viewers();
	~Viewers();
	void init(int windowWidth, int windowHeight, float windowScale);
	void frame(double simulationTime = USE_REFERENCE_TIME) override;
	void resize(int windowWidth, int windowHeight, float windowScale);
	osg::Group* getRootNode(unsigned int idx = -1, bool createIfNot = true);
	osgViewer::View* createView(float ratioLeft, float ratioRight, float ratioBottom, float ratioTop);

	void keyPressEvent(int key, unsigned int modkey);
	void keyReleaseEvent(int key, unsigned int modkey);
	void mousePressEvent(int x, int y, unsigned int modkey, unsigned int button);
	void mouseReleaseEvent(int x, int y, unsigned int modkey, unsigned int button);
	void mouseDoubleClickEvent(int x, int y, unsigned int modkey, unsigned int button);
	void mouseMoveEvent(int x, int y, unsigned int modkey);
	void wheelEvent(int x, int y, unsigned int modkey, unsigned int scroll);

protected:
	void setKeyboardModifiers(unsigned int modkey);

private:
	bool                                    _osgInited;
	osg::Timer                              _frameTime;
	osg::ref_ptr<osgViewer::GraphicsWindow> _osgWinEmb;
	float                                   _windowScale;
	static const int                        s_rootNodeCount = 100;
	osg::ref_ptr<osg::Group>                _rootNodes[s_rootNodeCount];
};

}
