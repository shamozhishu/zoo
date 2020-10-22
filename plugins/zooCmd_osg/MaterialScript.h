#ifndef __MATERIAL_SCRIPT_H__
#define __MATERIAL_SCRIPT_H__

#include <osgFX/Effect>
#include <zooCmd_osg/OsgMaterial.h>

//------------------------------------------------------------------------
// 描述: 材质技术,用于定义多通道
// 作者: shamozhishu
// 日期: 2020.9.25
//------------------------------------------------------------------------
class MaterialTechnique : public osgFX::Technique
{
public:
	MaterialTechnique();
	MaterialTechnique(const char* name);
	const char *techniqueName();
	const char *techniqueDescription();
	void pushPass(osg::StateSet* ss);
	void refresh(const map<string, vector<double>>& uniforms);

protected:
	void define_passes();

private:
	string _name;
	vector<osg::ref_ptr<osg::StateSet>> _passes;
};

//------------------------------------------------------------------------
// 描述: 材质效果,至少包含一个材质技术
// 作者: shamozhishu
// 日期: 2020.9.25
//------------------------------------------------------------------------
class MaterialEffect : public osgFX::Effect
{
public:
	MaterialEffect();
	MaterialEffect(const MaterialEffect& copy, const osg::CopyOp op = osg::CopyOp::SHALLOW_COPY);
	META_Effect(osgFX, MaterialEffect, "MaterialEffect", "My ShaderLab", "shamozhishu")
	void pushTechnique(MaterialTechnique* technique);
	void refresh(const map<string, vector<double>>& uniforms);

protected:
	bool define_techniques();

private:
	vector<osg::ref_ptr<MaterialTechnique>> _techniques;
};

//------------------------------------------------------------------------
// 描述: 材质脚本,每一个material材质脚本都会解析生成一个当前类型的对象
// 作者: shamozhishu
// 日期: 2020.9.25
//------------------------------------------------------------------------
class MaterialScript : public OsgMaterial
{
public:
	MaterialScript(const string name);
	string getMaterialName() const;
	void refresh(const map<string, vector<double>>& uniforms);
	void getMaterialConfigInfo(Material* material);
	osgFX::Effect* createEffect(Material* material);
	void setMaterialEffect(MaterialEffect* effect);

protected:
	const string _materialName;
	osg::ref_ptr<MaterialEffect> _effect;
};

#endif // __MATERIAL_SCRIPT_H__
