/************************************************************************\
* 概述: 渲染适配器接口
* https://github.com/shamozhishu
* Copyright (c) 2020-2021 authored by shamozhishu
\************************************************************************/
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
	/* 功能: 获取窗口控件或者底层窗口句柄的指针
	 * 参数: windowID-窗口ID,wndHandle-选择是否获取底层窗口句柄
	 * 返回值: 返回窗口控件或者底层窗口句柄的指针
	 * 作者: shamozhishu
	 * 日期: 2021.1.13
	 */
	virtual void* getWnd(int windowID, bool wndHandle = false) = 0;
	/* 功能: 创建指定尺寸的窗口控件
	 * 参数: windowWidth-宽,windowHeight-高,windowScale-缩放比,windowID-窗口ID
	 * 返回值: 返回封装的窗口控件对象的指针
	 * 作者: shamozhishu
	 * 日期: 2021.1.13
	 */
	virtual void* setup(int windowWidth, int windowHeight, float windowScale, int windowID = 0) = 0;
};

}

#endif // __ZOOCMD_RENDER_ADAPTER_H__
