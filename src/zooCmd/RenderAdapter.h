#ifndef __ZOOCMD_RENDER_ADAPTER_H__
#define __ZOOCMD_RENDER_ADAPTER_H__

namespace zooCmd {

class RenderAdapter
{
public:
	virtual ~RenderAdapter() {}
	virtual int   run() = 0;
	virtual bool  isDone() = 0;
	virtual void  setDone(bool done) = 0;
	virtual void  frame(double simulationTime) = 0;
	virtual void* setup(int windowWidth, int windowHeight, float windowScale, int windowID = 0) = 0;
};

}

#endif // __ZOOCMD_RENDER_ADAPTER_H__
