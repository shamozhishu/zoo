#pragma once

#include <osgCmd/Support.h>

namespace osgCmd {

class _osgCmdExport Renderer : public osgViewer::Viewer
{
public:
	Renderer();
	~Renderer();
	void update();
	void setupOSG(int windowWidth, int windowHeight, float windowScale);
	bool checkNeedToDoFrame() override;
	void frame(double simulationTime = USE_REFERENCE_TIME) override;
	void requestRedraw() override;
	bool checkEvents() override;
	bool getContinuousUpdate() const;
	void setContinuousUpdate(bool continuousUpdate);
	void resize(int windowWidth, int windowHeight, float windowScale);

	void keyPressEvent(Key key, unsigned int modkey);
	void keyReleaseEvent(Key key, unsigned int modkey);
	void mousePressEvent(int x, int y, unsigned int modkey, MouseButton button);
	void mouseReleaseEvent(int x, int y, unsigned int modkey, MouseButton button);
	void mouseDoubleClickEvent(int x, int y, unsigned int modkey, MouseButton button);
	void mouseMoveEvent(int x, int y, unsigned int modkey);
	void wheelEvent(int x, int y, unsigned int modkey, Scroll scroll);

protected:
	void timerEvent();
	void setKeyboardModifiers(unsigned int modkey);

private:
	osg::ref_ptr<osgViewer::GraphicsWindow> _osgWinEmb;
	float                                   _windowScale;
	bool                                    _osgInitialized;
	bool                                    _continuousUpdate;
	osg::Timer                              _lastFrameStartTime;
	bool                                    _osgWantsToRenderFrame;
	bool                                    _applicationAboutToQuit;
};

}
