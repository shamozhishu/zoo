#include "MaterialManager.h"

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
	auto it = _materials.begin();
	auto itEnd = _materials.end();
	for (; it != itEnd; ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

vector<string> MaterialManager::getMaterialList() const
{
	vector<string> matList(_materials.size() + 1);
	matList[0] = "Default";
	auto it = _materials.begin();
	auto itEnd = _materials.end();
	for (int i = 1; it != itEnd; ++it, ++i)
		matList[i] = it->first;
	return matList;
}

bool MaterialManager::switchMaterial(string materialName, Material* material)
{
	material->_uniforms.clear();

	for (int i = 0; i < Material::TexUnitNum; ++i)
		material->_textureFiles[i] = make_pair("", "");

	auto it = _materials.find(materialName);
	if (it != _materials.end())
	{
		OsgMaterial* pOsgMaterial = it->second;
		if (pOsgMaterial)
			pOsgMaterial->getMaterialConfigInfo(material);
	}
	else
	{
		materialName = "Default";
	}

	if (material->_currentUseMatName != materialName)
	{
		material->_currentUseMatName = materialName;
		material->getParent()->dirtyBit().addState(Material::Changed_);
		return true;
	}

	return false;
}

bool MaterialManager::compileMaterial(string materialFile, Material* material)
{
	extern string compileMaterialScript(const string& materialFile);
	return compileMaterialScript(materialFile) != "Default";
}

OsgMaterial* MaterialManager::attach(Material* material, osg::Node* node)
{
	if (!material || !node || material->_currentUseMatName == "Default")
		return nullptr;

	auto it = _materials.find(material->_currentUseMatName);
	if (it == _materials.end())
	{
		zoo_warning("不存在的材质[%s]，默认使用缺省材质！", material->_currentUseMatName.c_str());
		return nullptr;
	}

	OsgMaterial* pOsgMaterial = it->second;
	if (pOsgMaterial)
	{
		if (pOsgMaterial->reform(node))
		{
			osg::StateSet* ss = new osg::StateSet;
			node->setStateSet(ss);
			pOsgMaterial->setupStateSet(material, ss);
		}

		osg::ref_ptr<osgFX::Effect> effect = pOsgMaterial->createEffect(material);
		if (effect.valid())
		{
			osg::Group* parent = node->getParent(0);
			if (parent)
			{
				effect->addChild(node);
				parent->removeChild(node);
				parent->addChild(effect);
			}
		}
	}

	return pOsgMaterial;
}

void MaterialManager::detach(const string& materialName, osg::Node* node)
{
	auto it = _materials.find(materialName);
	if (it == _materials.end())
	{
		if (materialName != "Default")
			zoo_warning("要解除的材质[%s]不存在！", materialName.c_str());
		return;
	}

	osgFX::Effect* effect = dynamic_cast<osgFX::Effect*>(node->getParent(0));
	if (effect)
	{
		osg::Group* parent = effect->getParent(0);
		effect->removeChild(node);
		parent->addChild(node);
		parent->removeChild(effect);
	}

	OsgMaterial* pOsgMaterial = it->second;
	if (pOsgMaterial)
		pOsgMaterial->restore(node);
}

void MaterialManager::addMaterial(OsgMaterial* osgMaterial)
{
	if (osgMaterial)
	{
		auto it = _materials.find(osgMaterial->getMaterialName());
		if (it != _materials.end())
			delete it->second;
		_materials[osgMaterial->getMaterialName()] = osgMaterial;
	}
}

void MaterialManager::removeMaterial(OsgMaterial* osgMaterial)
{
	if (osgMaterial)
	{
		auto it = _materials.find(osgMaterial->getMaterialName());
		if (it != _materials.end())
		{
			delete it->second;
			_materials.erase(it);
		}
	}
}
