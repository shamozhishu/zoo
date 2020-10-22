#include "MaterialScript.h"

MaterialTechnique::MaterialTechnique()
	: _name("Default")
{
}

MaterialTechnique::MaterialTechnique(const char* name)
	: _name(name)
{
}

const char * MaterialTechnique::techniqueName()
{
	return _name.c_str();
}

const char * MaterialTechnique::techniqueDescription()
{
	string description = "This is the ";
	description += _name;
	description += " technique";
	return description.c_str();
}

void MaterialTechnique::pushPass(osg::StateSet* ss)
{
	_passes.push_back(ss);
}

void MaterialTechnique::refresh(const map<string, vector<double>>& uniforms)
{
	size_t len = _passes.size();
	for (size_t i = 0; i < len; ++i)
	{
		auto it = uniforms.begin();
		auto itEnd = uniforms.end();
		for (; it != itEnd; ++it)
		{
			osg::Uniform* pUniform = _passes[i]->getUniform(it->first);
			if (pUniform)
			{
				int num = it->second.size();
				switch (num)
				{
				case 1: pUniform->set(it->second[0]); break;
				case 2: pUniform->set(osg::Vec2d(it->second[0], it->second[1])); break;
				case 3: pUniform->set(osg::Vec3d(it->second[0], it->second[1], it->second[2])); break;
				case 4: pUniform->set(osg::Vec4d(it->second[0], it->second[1], it->second[2], it->second[3])); break;
				case 9: pUniform->set(osg::Matrix3d(it->second[0], it->second[1], it->second[2],
					it->second[3], it->second[4], it->second[5], it->second[6], it->second[7], it->second[8])); break;
				case 16: pUniform->set(osg::Matrixd(it->second[0], it->second[1], it->second[2], it->second[3],
					it->second[4], it->second[5], it->second[6], it->second[7],
					it->second[8], it->second[9], it->second[10], it->second[11],
					it->second[12], it->second[13], it->second[14], it->second[15])); break;
				default: zoo_warning("Uniform[%s]的参数个数[%d]非法！", it->first.c_str(), num); break;
				}
			}
		}
	}
}

void MaterialTechnique::define_passes()
{
	size_t len = _passes.size();
	for (size_t i = 0; i < len; ++i)
		addPass(_passes[i].get());
}

//////////////////////////////////////////////////////////////////////////
MaterialEffect::MaterialEffect()
{
}

MaterialEffect::MaterialEffect(const MaterialEffect& copy, const osg::CopyOp op /*= osg::CopyOp::SHALLOW_COPY*/)
	: osgFX::Effect(copy, op)
{
	_techniques = copy._techniques;
}

void MaterialEffect::pushTechnique(MaterialTechnique* technique)
{
	_techniques.push_back(technique);
}

void MaterialEffect::refresh(const map<string, vector<double>>& uniforms)
{
	size_t len = _techniques.size();
	for (size_t i = 0; i < len; ++i)
		_techniques[i]->refresh(uniforms);
}

bool MaterialEffect::define_techniques()
{
	size_t len = _techniques.size();
	for (size_t i = 0; i < len; ++i)
		addTechnique(_techniques[i].get());
	return true;
}

//////////////////////////////////////////////////////////////////////////
MaterialScript::MaterialScript(const string name)
	: _materialName(name)
{
	ServiceLocator<OsgMaterialManager>().getService()->addMaterial(this);
}

std::string MaterialScript::getMaterialName() const
{
	return _materialName;
}

void MaterialScript::refresh(const map<string, vector<double>>& uniforms)
{
	if (_effect.valid())
		_effect->refresh(uniforms);
}

void MaterialScript::getMaterialConfigInfo(Material* material)
{
}

osgFX::Effect* MaterialScript::createEffect(Material* material)
{
	if (_effect.valid())
		return new MaterialEffect(*_effect);
	return nullptr;
}

void MaterialScript::setMaterialEffect(MaterialEffect* effect)
{
	_effect = effect;
}
