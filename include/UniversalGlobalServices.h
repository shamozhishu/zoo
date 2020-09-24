#ifndef __UNIVERSAL_GLOBAL_SERVICES_H__
#define __UNIVERSAL_GLOBAL_SERVICES_H__

#include <zoo/ServiceLocator.h>

class CoordTransformUtil : public zoo::Service
{
	ZOO_SERVICE(CoordTransformUtil)
public:
	virtual bool convertLLHToXYZ(double longitude, double latitude, double height, double& X, double& Y, double& Z) = 0;
	virtual bool convertXYZToLLH(double X, double Y, double Z, double& longitude, double& latitude, double& height) = 0;
};

class MeshList : public zoo::Service
{
	ZOO_SERVICE(MeshList)
public:
	/* 功能: 获取网格列表
	 * 参数: 空
	 * 返回值: 网格列表,涵盖了用户自定义的所有网格对象
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual vector<string> getMeshList() const = 0;
	/* 功能: 从用户自定义的网格对象中提取出网格配置信息
	 * 参数: 1)指明要提取哪一个用户自定义网格的信息;2)配置信息存放的位置
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual void getMeshConfigInfo(string meshName, struct Mesh* mesh) = 0;
};

class MaterialList : public zoo::Service
{
	ZOO_SERVICE(MaterialList)
public:
	/* 功能: 获取材质列表
	 * 参数: 空
	 * 返回值: 材质列表,涵盖了用户自定义的所有材质对象
	 * 作者: shamozhishu
	 * 日期: 2020.9.11
	 */
	virtual vector<string> getMaterialList() const = 0;
	/* 功能: 从用户自定义的材质对象中提取出材质配置信息
	 * 参数: 1)指明要提取哪一个用户自定义材质的信息;2)配置信息存放的位置
	 * 返回值: 空
	 * 作者: shamozhishu
	 * 日期: 2020.9.11
	 */
	virtual void getMaterialConfigInfo(string materialName, struct Material* material) = 0;
};

#endif // __UNIVERSAL_GLOBAL_SERVICES_H__
