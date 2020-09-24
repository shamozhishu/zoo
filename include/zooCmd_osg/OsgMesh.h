/************************************************************************\
* 概述: 网格资源
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __OSG_MESH_H__
#define __OSG_MESH_H__

#include <osg/Geode>
#include <zoo/ServiceLocator.h>
#include <component/war/WarComponents.h>

//------------------------------------------------------------------------
// 描述: 在子类构造函数中将其addMesh进网格管理器，无需delete，由网格管理器释放内存
// 作者: shamozhishu
// 日期: 2020.9.16
//------------------------------------------------------------------------
class OsgMesh
{
public:
	virtual ~OsgMesh() {}
	/* 功能: 获取网格名称
	 * 参数: 空
	 * 返回值: 网格名称
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual string getMeshName() const = 0;
	/* 功能: 反馈给客户代码是否使用模型文件加载网格
	 * 参数: 配置的信息由该参数返回
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual void getMeshConfigInfo(Mesh* mesh) = 0;
	/* 功能: 配置网格模型
	 * 参数: 获取外部的网格模型配置信息,用于网格的初始化工作
	 * 返回值: 返回包含了网格模型的叶子节点
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual osg::Geode* setupMeshModel(Mesh* mesh) { (void)mesh; return nullptr; }
};
//------------------------------------------------------------------------
// 描述: 网格管理器服务
// 作者: shamozhishu
// 日期: 2020.9.16
//------------------------------------------------------------------------
class OsgMeshManager : public Service
{
	ZOO_SERVICE(OsgMeshManager)
public:
	/* 功能: 附加网格
	 * 参数: 1)选择要附加的网格,2)网格挂载的节点
	 * 返回值: 成功返回true,失败返回false
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual bool attach(Mesh* mesh, osg::Group* meshNode) = 0;
	/* 功能: 解除网格
	 * 参数: 网格挂载的节点
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual void detach(osg::Group* meshNode) = 0;
	/* 功能: 添加网格对象
	 * 参数: 用户自定义的网格对象
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual void addMesh(OsgMesh* osgMesh) = 0;
	/* 功能: 移除网格对象
	 * 参数: 用户自定义的网格对象
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual void removeMesh(OsgMesh* osgMesh) = 0;
};

#endif // __OSG_MESH_H__
