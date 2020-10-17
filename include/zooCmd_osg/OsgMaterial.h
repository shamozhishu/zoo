/************************************************************************\
* 概述: 材质系统
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __OSG_MATERIAL_H__
#define __OSG_MATERIAL_H__

#include <osgFX/Effect>
#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthSupport.h>
#include <component/war/WarComponents.h>
//------------------------------------------------------------------------
// 描述: OSG实现的材质类(内存释放交由材质管理器负责)
// 作者: shamozhishu
// 日期: 2020.9.7
//------------------------------------------------------------------------
class OsgMaterial
{
public:
	virtual ~OsgMaterial() {}
	/* 功能: 获取材质名称
	 * 参数: 空
	 * 返回值: 材质名称
	 * 作者: shamozhishu
	 * 日期: 2020.9.8
	 */
	virtual string getMaterialName() const = 0;
	/* 功能: 刷新材质效果, 主要是Uniform的实时更新
	 * 参数: 需要更新的Uniforms
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.28
	 */
	virtual void refresh(const map<string, vector<double>>& uniforms) { (void)uniforms; }
	/* 功能: 如果节点需要改造, 则重写该成员函数, 比如给网格模型添加切线和副法线的顶点属性
	 * 参数: 要改造的节点
	 * 返回值: 返回TRUE表示可以执行setupStateSet(), 返回FALSE则不执行setupStateSet().
	 * 作者: shamozhishu
	 * 日期: 2020.9.10
	 */
	virtual bool reform(osg::Node* node) { return true; }
	/* 功能: 恢复改造后的节点到初始状态
	 * 参数: 要恢复的节点
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.10
	 */
	virtual void restore(osg::Node* node) {}
	/* 功能: 反馈给客户代码使用了哪些着色器和统一值等
	 * 参数: 配置的信息由该参数返回
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.11
	 */
	virtual void getMaterialConfigInfo(Material* material) = 0;
	/* 功能: 配置渲染状态集
	 * 参数: 1)获取外部的材质配置信息,用于材质的初始化工作;2)材质关联的渲染状态集
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.7
	 */
	virtual void setupStateSet(Material* material, osg::StateSet* stateset) { (void)material; (void)stateset; }
	/* 功能: 创建效果节点
	 * 参数: 获取外部的材质配置信息,用于创建效果时使用
	 * 返回值: 返回效果节点对象
	 * 作者: shamozhishu
	 * 日期: 2020.9.7
	 */
	virtual osgFX::Effect* createEffect(Material* material) { (void)material; return nullptr; }
};
//------------------------------------------------------------------------
// 描述: 材质管理器服务
// 作者: shamozhishu
// 日期: 2020.9.7
//------------------------------------------------------------------------
class OsgMaterialManager : public Service
{
	ZOO_SERVICE(OsgMaterialManager)
public:
	/* 功能: 附加材质
	 * 参数: 1)选择要附加的材质,2)材质影响的节点
	 * 返回值: 成功返回对应的osg材质,失败返回NULL
	 * 作者: shamozhishu
	 * 日期: 2020.9.10
	 */
	virtual OsgMaterial* attach(Material* material, osg::Node* node) = 0;
	/* 功能: 解除材质
	 * 参数: 1)选择要解除的材质,2)材质影响的节点
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.10
	 */
	virtual void detach(const string& materialName, osg::Node* node) = 0;
	/* 功能: 添加材质对象
	 * 参数: 用户自定义的材质对象
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.10
	 */
	virtual void addMaterial(OsgMaterial* osgMaterial) = 0;
	/* 功能: 移除材质对象
	 * 参数: 用户自定义的材质对象
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.10
	 */
	virtual void removeMaterial(OsgMaterial* osgMaterial) = 0;
};

#endif // __OSG_MATERIAL_H__
