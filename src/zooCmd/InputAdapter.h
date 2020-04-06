#pragma once

namespace zooCmd {

class InputAdapter
{
public:
	virtual ~InputAdapter() {};
	virtual int  run() = 0;
	virtual bool init() = 0;
	virtual bool isDone() = 0;
	virtual void setDone(bool done) = 0;
	virtual void frame(double simulationTime) = 0;
	virtual void setup(int windowWidth, int windowHeight, float windowScale) = 0;
	virtual void resize(int windowWidth, int windowHeight, float windowScale) = 0;
	virtual void keyPress(int key, unsigned int modkey) = 0;
	virtual void keyRelease(int key, unsigned int modkey) = 0;
	virtual void mouseMove(int x, int y, unsigned int modkey) = 0;
	virtual void wheel(int x, int y, unsigned int modkey, unsigned int scroll) = 0;
	virtual void mousePress(int x, int y, unsigned int modkey, unsigned int button) = 0;
	virtual void mouseRelease(int x, int y, unsigned int modkey, unsigned int button) = 0;
	virtual void mouseDoubleClick(int x, int y, unsigned int modkey, unsigned int button) = 0;
};

}
