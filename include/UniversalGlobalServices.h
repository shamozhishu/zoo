#ifndef __UNIVERSAL_GLOBAL_SERVICES_H__
#define __UNIVERSAL_GLOBAL_SERVICES_H__

#include <zoo/ServiceLocator.h>

class CoordTransformUtil : public zoo::Service
{
	ZOO_SERVICE(CoordTransformUtil)
public:
	class Converter
	{
	public:
		virtual ~Converter() {}
		template<typename T> T* to() { return dynamic_cast<T*>(this); }
	};

	virtual bool convertLLHToXYZ(Converter* converter, double longitude, double latitude, double height, double& X, double& Y, double& Z) = 0;
	virtual bool convertXYZToLLH(Converter* converter, double X, double Y, double Z, double& longitude, double& latitude, double& height) = 0;
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
	/* 功能: 切换网格并且从用户自定义的网格对象中提取出网格配置信息
	 * 参数: 1)指明要切换的网格;2)提取出用户自定义网格的配置信息并存储到当前对象里
	 * 返回值: 切换成功与否
	 * 作者: shamozhishu
	 * 日期: 2020.9.16
	 */
	virtual bool switchMesh(string meshName, struct Mesh* mesh) = 0;
	/* 功能: 编译指定的网格脚本文件
	 * 参数: 1)指定的网格脚本文件名;2)可以动态变更的网格参数存放的位置
	 * 返回值: 是否编译成功
	 * 作者: shamozhishu
	 * 日期: 2020.10.20
	 */
	virtual bool compileMesh(string meshFile, struct Mesh* mesh) = 0;
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
	/* 功能: 切换材质并且从用户自定义的材质对象中提取出材质配置信息
	 * 参数: 1)指明要切换的材质;2)提取出用户自定义材质的配置信息并存储到当前对象里
	 * 返回值: 切换成功与否
	 * 作者: shamozhishu
	 * 日期: 2020.9.11
	 */
	virtual bool switchMaterial(string materialName, struct Material* material) = 0;
	/* 功能: 编译指定的材质脚本文件
	 * 参数: 1)指定的材质脚本文件名;2)可以动态变更的材质参数存放的位置
	 * 返回值: 是否编译成功
	 * 作者: shamozhishu
	 * 日期: 2020.10.20
	 */
	virtual bool compileMaterial(string materialFile, struct Material* material) = 0;
};

#endif // __UNIVERSAL_GLOBAL_SERVICES_H__
