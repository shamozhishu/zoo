#ifndef __MESH_MANAGER_H__
#define __MESH_MANAGER_H__

#include <zoo/Singleton.h>
#include <zooCmd_osg/OsgMesh.h>
#include <UniversalGlobalServices.h>
#include <zooCmd_osg/OsgEarthSupport.h>
//------------------------------------------------------------------------
// 描述: 网格管理器，创建的网格对象可以赋给任意的模型组件使用
// 作者: shamozhishu
// 日期: 2020.9.16
//------------------------------------------------------------------------
class MeshManager : public zoo::Singleton<MeshManager>, public OsgMeshManager, public MeshList
{
public:
	MeshManager();
	~MeshManager();
	vector<string> getMeshList() const;
	void getMeshConfigInfo(string meshName, Mesh* mesh);
	bool attach(Mesh* mesh, osg::Group* meshNode);
	void detach(osg::Group* meshNode);
	void addMesh(OsgMesh* osgMesh);
	void removeMesh(OsgMesh* osgMesh);

private:
	unordered_map<string, OsgMesh*> _meshs;
};

#endif // __MESH_MANAGER_H__
