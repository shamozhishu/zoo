#include <component/WarComponents.h>
#include <luaScript/LuaScriptStack.h>
#include <toluaInput/Input.h>
#include <zoo/Utils.h>
#include "WarCommander.h"
#include "Battlefield.h"
#include <zoo/ServiceLocator.h>
#include <UniversalGlobalServices.h>

TOLUA_OPEN_FUNC_EXTERN(ToLuaWar);
TOLUA_OPEN_FUNC_EXTERN(ToLuaInput);
TOLUA_OPEN_FUNC g_func[] = { TOLUA_OPEN_FUNC_PTR(ToLuaWar), TOLUA_OPEN_FUNC_PTR(ToLuaInput), nullptr };
ZOO_REFLEX_IMPLEMENT(Behavior);
Behavior::Behavior()
	: _scriptValid(false)
	, _scriptInited(false)
	, _script(new LuaScriptStack(g_func))
	, _btfield(WarCommander::getSingleton().getCurBattlefield())
{
	_btfield->addBehavior(this);
}

Behavior::~Behavior()
{
	_btfield->removeBehavior(this);
	SAFE_DELETE(_script);
}

void Behavior::exec(const char* func)
{
	if (!_scriptInited)
	{
		_scriptInited = true;
		_scriptValid = false;
		if (_scriptFile != "")
		{
			LuaScriptStack::clearScriptFileName2ContentCache(ZOO_DATA_ROOT_DIR + _scriptFile);
			_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + _scriptFile);
			if (_scriptValid)
			{
				Entity* pEnt = getEntity();
				auto components = pEnt->getComponents();
				auto it = components.begin();
				for (; it != components.end(); ++it)
				{
					if (it->first != "Behavior")
						_script->setVariable(zoo::strToLower(it->first).c_str(), it->first.c_str(), it->second);
				}

				static Log logger;
				_script->setVariable("logger", "Log", &logger);
				_script->setVariable("this", "Entity", getEntity());
				_script->setVariable("input", "Input", WarCommander::getSingleton().getInputDevice());
				_script->executeGlobalFunction("Init");
			}
		}
	}

	if (_scriptValid)
		_script->executeGlobalFunction(func);
}

void Behavior::reset()
{
	_scriptInited = false;
}

void Behavior::serialize(Spawner* spawner)
{
	spawner->setValue("script", _scriptFile);
}

void Behavior::deserialize(Spawner* spawner)
{
	spawner->getValue("script", _scriptFile);
}
//////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
	: _currentUseMeshName("Default")
	, _enableResource(true)
	, _parent(nullptr)
{
}

void Mesh::serialize(Spawner* spawner)
{
	spawner->setValue("mesh_name", _currentUseMeshName);
	spawner->setValue("resource_file", _resourceFile);
	spawner->setValue("enable_resource", _enableResource);

	string params;
	auto it = _params.begin();
	auto itEnd = _params.end();
	for (; it != itEnd; it++)
	{
		stringstream ss;
		const vector<double> val = it->second;
		int siz = val.size();
		if (siz > 0)
		{
			ss << it->first << ":";
			for (int i = 0; i < siz; ++i)
			{
				ss << val[i];
				if (i != siz - 1)
					ss << ",";
				else
					ss << ";";
			}
		}

		params += ss.str();
	}

	spawner->setValue("params", params);
}

void Mesh::deserialize(Spawner* spawner)
{
	spawner->getValue("mesh_name", _currentUseMeshName);
	spawner->getValue("resource_file", _resourceFile);
	spawner->getValue("enable_resource", _enableResource);

	_params.clear();
	string params;
	vector<string> container;
	spawner->getValue("params", params);
	zoo::stringtok(container, params, ";");
	int siz = container.size();
	if (siz > 0)
	{
		for (int i = 0; i < siz; ++i)
		{
			vector<string> vars;
			zoo::stringtok(vars, container[i], ":");
			if (vars.size() != 2)
				continue;

			vector<string> vals;
			zoo::stringtok(vals, vars[1], ",");
			int len = vals.size();
			if (len == 0)
				continue;

			_params[vars[0]] = vector<double>(len);
			for (int j = 0; j < len; ++j)
				_params[vars[0]][j] = atof(vals[j].c_str());
		}
	}
}

void Mesh::changeMesh(const string& name)
{
	ServiceLocator<MeshList>().getService()->switchMesh(name, this);
}

void Mesh::changeParam(const string& var, const vector<double>& val)
{
	auto it = _params.find(var);
	if (it == _params.end())
	{
		zoo_warning("网格[%s]的参数[%s]不存在！", _currentUseMeshName.c_str(), var.c_str());
		return;
	}

	_params[var] = val;
	_parent->dirtyBit().addState(Mesh::Changed_);
}
//////////////////////////////////////////////////////////////////////////
Material::Material()
	: _currentUseMatName("Default")
	, _parent(nullptr)
{
}

void Material::serialize(Spawner* spawner)
{
	spawner->setValue("material_name", _currentUseMatName);

	// 序列化纹理单元
	string textures;
	for (int i = 0; i < TexUnitNum; ++i)
	{
		if (_textureFiles[i].first != "")
		{
			stringstream ss;
			ss << i << ":" << _textureFiles[i].second << ";";
			textures += ss.str();
		}
	}

	spawner->setValue("textures", textures);

	// 序列化统一值
	string uniforms;
	auto it = _uniforms.begin();
	auto itEnd = _uniforms.end();
	for (; it != itEnd; it++)
	{
		stringstream ss;
		const vector<double> val = it->second;
		int siz = val.size();
		if (siz > 0)
		{
			ss << it->first << ":";
			for (int i = 0; i < siz; ++i)
			{
				ss << val[i];
				if (i != siz - 1)
					ss << ",";
				else
					ss << ";";
			}
		}

		uniforms += ss.str();
	}

	spawner->setValue("uniforms", uniforms);
}

void Material::deserialize(Spawner* spawner)
{
	spawner->getValue("material_name", _currentUseMatName);

	string strin;
	vector<string> vars;
	vector<string> container;

	// 反序列化纹理单元
	container.clear();
	spawner->getValue("textures", strin);
	zoo::stringtok(container, strin, ";");
	size_t siz = container.size();
	if (siz > 0)
	{
		for (int i = 0; i < siz; ++i)
		{
			vars.clear();
			zoo::stringtok(vars, container[i], ":");
			if (vars.size() != 2)
				continue;

			_textureFiles[atoi(vars[0].c_str())] = make_pair(vars[0], vars[1]);
		}
	}

	// 反序列化统一值
	_uniforms.clear();
	container.clear();
	spawner->getValue("uniforms", strin);
	zoo::stringtok(container, strin, ";");
	siz = container.size();
	if (siz > 0)
	{
		for (int i = 0; i < siz; ++i)
		{
			vars.clear();
			zoo::stringtok(vars, container[i], ":");
			if (vars.size() != 2)
				continue;

			vector<string> vals;
			zoo::stringtok(vals, vars[1], ",");
			int len = vals.size();
			if (len == 0)
				continue;

			_uniforms[vars[0]] = vector<double>(len);
			for (int j = 0; j < len; ++j)
				_uniforms[vars[0]][j] = atof(vals[j].c_str());
		}
	}
}

void Material::changeMat(const string& name)
{
	ServiceLocator<MaterialList>().getService()->switchMaterial(name, this);
}

void Material::changeUniform(const string& uniform, const vector<double>& val)
{
	auto it = _uniforms.find(uniform);
	if (it == _uniforms.end())
	{
		zoo_warning("材质[%s]的Uniform[%s]不存在！", _currentUseMatName.c_str(), uniform.c_str());
		return;
	}

	_uniforms[uniform] = val;
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(DoF);
DoF::DoF()
	: _parent(nullptr)
	, _sceneType(-1)
	, _lonLatHeight(false)
	, _mountEntID(-1)
	, _mountEntBreed(-1)
	, _x(0)
	, _y(0)
	, _z(0)
	, _heading(0)
	, _pitch(0)
	, _roll(0)
	, _sx(1)
	, _sy(1)
	, _sz(1)
{
}

DoF::~DoF()
{
	setParent(nullptr);
	DoF* child;
	vector<DoF*> children = _children;
	auto it = children.begin();
	for (; it != children.end(); ++it)
	{
		child = *it;
		if (child)
			child->setParent(nullptr);
	}
}

void DoF::init()
{
	const_cast<int&>(_sceneType) = getEntity()->getSpawner()->breed();
	const_cast<bool&>(_lonLatHeight) = _sceneType == 0 ? false : true;
}

void DoF::serialize(Spawner* spawner)
{
	spawner->setValue("x", _x);
	spawner->setValue("y", _y);
	spawner->setValue("z", _z);
	spawner->setValue("heading", _heading);
	spawner->setValue("pitch", _pitch);
	spawner->setValue("roll", _roll);
	spawner->setValue("sx", _sx);
	spawner->setValue("sy", _sy);
	spawner->setValue("sz", _sz);
	spawner->setValue("lon_lat_height", _lonLatHeight);

	if (_parent)
	{
		_mountEntID = _parent->getEntity()->id();
		_mountEntBreed = _parent->getEntity()->breed();
		spawner->setValue("mount_entity_id", _mountEntID);
		spawner->setValue("mount_entity_breed", _mountEntBreed);
	}
	else
	{
		spawner->setNull("mount_entity_id");
		spawner->setNull("mount_entity_breed");
	}
}

void DoF::deserialize(Spawner* spawner)
{
	spawner->getValue("x", _x);
	spawner->getValue("y", _y);
	spawner->getValue("z", _z);
	spawner->getValue("heading", _heading);
	spawner->getValue("pitch", _pitch);
	spawner->getValue("roll", _roll);
	spawner->getValue("sx", _sx);
	spawner->getValue("sy", _sy);
	spawner->getValue("sz", _sz);
	spawner->getValue("lon_lat_height", _lonLatHeight);
	spawner->getValue("mount_entity_id", _mountEntID);
	spawner->getValue("mount_entity_breed", _mountEntBreed);

	SignalTrigger::connect(*getEntity()->getSpawner(), [this]
	{
		Entity* pEnt = getEntity()->getSpawner()->gain(_mountEntID, _mountEntBreed);
		if (pEnt)
			setParent(pEnt->getComponent<DoF>());
	});
}

bool DoF::isLLH() const
{
	return _lonLatHeight;
}

void DoF::setPos(double x, double y, double z, bool lon_lat_height /*= false*/)
{
	if (!equals(_x, x) || !equals(_y, y) || !equals(_z, z))
	{
		_x = x;
		_y = y;
		_z = z;
		_dirty.addState(Dof_);
	}

	if (_sceneType != 0 && _lonLatHeight != lon_lat_height)
	{
		const_cast<bool&>(_lonLatHeight) = lon_lat_height;
		_dirty.addState(Parent_);
	}
}

double DoF::getPosX() const
{
	return _x;
}

double DoF::getPosY() const
{
	return _y;
}

double DoF::getPosZ() const
{
	return _z;
}

void DoF::setRot(float h, float p, float r)
{
	if (!equals(_heading, h) || !equals(_pitch, p) || !equals(_roll, r))
	{
		_heading = h;
		_pitch = p;
		_roll = r;
		_dirty.addState(Dof_);
	}
}

float DoF::getHeading() const
{
	return _heading;
}

float DoF::getPitch() const
{
	return _pitch;
}

float DoF::getRoll() const
{
	return _roll;
}

void DoF::setScale(float sx, float sy, float sz)
{
	if (!equals(_sx, sx) || !equals(_sy, sy) || !equals(_sz, sz))
	{
		_sx = sx;
		_sy = sy;
		_sz = sz;
		_dirty.addState(Dof_);
	}
}

float DoF::getScaleX() const
{
	return _sx;
}

float DoF::getScaleY() const
{
	return _sy;
}

float DoF::getScaleZ() const
{
	return _sz;
}

void DoF::setParent(DoF* parent)
{
	if (_parent == parent)
		return;

	if (_parent)
	{
		auto it = std::find(_parent->_children.begin(), _parent->_children.end(), this);
		if (it != _parent->_children.end())
			_parent->_children.erase(it);
	}

	_parent = parent;

	if (parent)
		parent->_children.push_back(this);

	_dirty.addState(Parent_);
}

DoF* DoF::getParent() const
{
	return _parent;
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Model);
Model::Model()
	: _visible(true)
{
	_mesh.setParent(this);
	_material.setParent(this);
}

void Model::serialize(Spawner* spawner)
{
	spawner->setValue("visible", _visible);
	_mesh.serialize(spawner);
	_material.serialize(spawner);
}

void Model::deserialize(Spawner* spawner)
{
	spawner->getValue("visible", _visible);
	_mesh.deserialize(spawner);
	_material.deserialize(spawner);
}

bool Model::isVisible() const
{
	return _visible;
}

void Model::setVisible(bool visible)
{
	if (_visible != visible)
	{
		_visible = visible;
		_dirty.addState(Visible_);
	}
}

Mesh* Model::getMesh()
{
	return &_mesh;
}

Material* Model::getMaterial()
{
	return &_material;
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Sound);
void Sound::serialize(Spawner* spawner)
{
	spawner->setValue("sound_file", _soundFile);
}

void Sound::deserialize(Spawner* spawner)
{
	spawner->getValue("sound_file", _soundFile);
}

void Sound::play()
{
	_isPlay = true;
}

void Sound::stop()
{
	_isPlay = false;
}

void Sound::pause()
{
	_isPlay = false;
}

bool Sound::isLoop() const
{
	return _isLoop;
}

void Sound::setLoop(bool loop)
{
	_isLoop = loop;
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Animator);
void Animator::serialize(Spawner* spawner)
{
	spawner->setValue("traj_file", _trajFile);
}

void Animator::deserialize(Spawner* spawner)
{
	spawner->getValue("traj_file", _trajFile);
}
//////////////////////////////////////////////////////////////////////////
static int s_unique_viewID = 0;
ZOO_REFLEX_IMPLEMENT(Camera);
Reflex<Camera, int> Camera::_dynReflex_INT;
Camera::Camera()
	: _windowID(0)
	, _viewID(s_unique_viewID++)
	, _trackEntID(-1)
	, _trackEntBreed(-1)
	, _manipulatorKey(Terrain_)
	, _lRatio(0)
	, _rRatio(1)
	, _bRatio(0)
	, _tRatio(1)
	, _red(0)
	, _green(0)
	, _blue(0)
	, _alpha(255)
{
	_passes[0]._rt = Window_;
	for (int i = 0; i < MaxPassCount; ++i)
		_passes[i]._material.setParent(this);
}

Camera::Camera(int windowID)
	: _windowID(windowID)
	, _viewID(s_unique_viewID++)
	, _trackEntID(-1)
	, _trackEntBreed(-1)
	, _manipulatorKey(0)
	, _lRatio(0)
	, _rRatio(1)
	, _bRatio(0)
	, _tRatio(1)
	, _red(0)
	, _green(0)
	, _blue(0)
	, _alpha(255)
{
	_passes[0]._rt = Window_;
	for (int i = 0; i < MaxPassCount; ++i)
		_passes[i]._material.setParent(this);
}

void Camera::serialize(Spawner* spawner)
{
	spawner->setValue("window_id", _windowID);
	spawner->setValue("bgcolor_red", _red);
	spawner->setValue("bgcolor_green", _green);
	spawner->setValue("bgcolor_blue", _blue);
	spawner->setValue("bgcolor_alpha", _alpha);
	spawner->setValue("ratio_left", _lRatio);
	spawner->setValue("ratio_right", _rRatio);
	spawner->setValue("ratio_bottom", _bRatio);
	spawner->setValue("ratio_top", _tRatio);
	spawner->setValue("manipulator", _manipulatorKey);
	spawner->setValue("track_entity_id", _trackEntID);
	spawner->setValue("track_entity_breed", _trackEntBreed);
	for (PassIndex i = 0; i < MaxPassCount; ++i)
	{
		spawner->setValue("pass_rt_" + std::to_string(i), _passes[i]._rt);
		_passes[i]._material.serialize(spawner);
	}
}

void Camera::deserialize(Spawner* spawner)
{
	spawner->getValue("window_id", _windowID);
	spawner->getValue("bgcolor_red", _red);
	spawner->getValue("bgcolor_green", _green);
	spawner->getValue("bgcolor_blue", _blue);
	spawner->getValue("bgcolor_alpha", _alpha);
	spawner->getValue("ratio_left", _lRatio);
	spawner->getValue("ratio_right", _rRatio);
	spawner->getValue("ratio_bottom", _bRatio);
	spawner->getValue("ratio_top", _tRatio);
	spawner->getValue("manipulator", _manipulatorKey);
	spawner->getValue("track_entity_id", _trackEntID);
	spawner->getValue("track_entity_breed", _trackEntBreed);
	for (PassIndex i = 0; i < MaxPassCount; ++i)
	{
		spawner->getValue("pass_rt_" + std::to_string(i), _passes[i]._rt);
		_passes[i]._material.deserialize(spawner);
	}
}

void Camera::setManipulator(int key)
{
	if (key < 0)
		key = 0;

	if (_manipulatorKey != key)
	{
		_manipulatorKey = key;
		_dirty.addState(Manipulator_);
	}
}

Material* Camera::getMaterial(PassIndex pass)
{
	if (pass >= MaxPassCount)
		return nullptr;

	return &_passes[pass]._material;
}

void Camera::setTrackEntity(int id, int breed)
{
	if (_trackEntID != id || _trackEntBreed != breed)
	{
		_trackEntID = id;
		_trackEntBreed = breed;
		_dirty.addState(TrackEnt_);
	}
}

void Camera::setBgColor(int r, int g, int b, int a /*= 255*/)
{
	if (_red != r || _green != g || _blue != b || _alpha != a)
	{
		_red = r; _green = g; _blue = b; _alpha = a;
		_dirty.addState(Bgcolour_);
	}
}

void Camera::setRenderTarget(PassIndex pass, RenderTarget rt)
{
	if (pass >= MaxPassCount)
		return;

	if (_passes[pass]._rt != rt)
	{
		_passes[pass]._rt = rt;
		unsigned int mask = Pass1_;
		switch (pass)
		{
		case 0: mask = Pass1_; break;
		case 1: mask = Pass2_; break;
		case 2: mask = Pass3_; break;
		case 3: mask = Pass4_; break;
		case 4: mask = Pass5_; break;
		case 5: mask = Pass6_; break;
		case 6: mask = Pass7_; break;
		case 7: mask = Pass8_; break;
		}
		_dirty.addState(mask);
	}
}

void Camera::setViewport(float leftRatio, float rightRatio, float bottomRatio, float topRatio)
{
	if (!equals(_lRatio, leftRatio) || !equals(_rRatio, rightRatio) || !equals(_bRatio, bottomRatio) || !equals(_tRatio, topRatio))
	{
		_lRatio = leftRatio;
		_rRatio = rightRatio;
		_bRatio = bottomRatio;
		_tRatio = topRatio;
		_dirty.addState(Viewport_);
	}
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Environment);
Environment::Environment()
	: _type(Sunny_)
	, _intensity(0.3f)
{
	_skyBox.setParent(this);
	_skyBox._currentUseMatName = "";
	_skyBox._textureFiles[0] = make_pair("+X：", "");
	_skyBox._textureFiles[1] = make_pair("-X：", "");
	_skyBox._textureFiles[2] = make_pair("+Y：", "");
	_skyBox._textureFiles[3] = make_pair("-Y：", "");
	_skyBox._textureFiles[4] = make_pair("+Z：", "");
	_skyBox._textureFiles[5] = make_pair("-Z：", "");
}

void Environment::serialize(Spawner* spawner)
{
	spawner->setValue("weather", _type);
	spawner->setValue("intensity", _intensity);
	_skyBox.serialize(spawner);
}

void Environment::deserialize(Spawner* spawner)
{
	spawner->getValue("weather", _type);
	spawner->getValue("intensity", _intensity);
	_skyBox.deserialize(spawner);
	_skyBox._textureFiles[0].first = "+X：";
	_skyBox._textureFiles[1].first = "-X：";
	_skyBox._textureFiles[2].first = "+Y：";
	_skyBox._textureFiles[3].first = "-Y：";
	_skyBox._textureFiles[4].first = "+Z：";
	_skyBox._textureFiles[5].first = "-Z：";
}

void Environment::showSkyBox()
{
	_skyBox._currentUseMatName = ZOO_STRING(SkyBox);
	_dirty.addState(SkyBox_);
}

void Environment::hideSkyBox()
{
	_skyBox._currentUseMatName = "";
	_dirty.addState(SkyBox_);
}

void Environment::setWeather(Weather type, float intensity)
{
	if (!equals(_intensity, intensity))
	{
		_intensity = intensity;
		_dirty.addState(Weather_);
	}

	if (_type != type)
	{
		_type = type;
		_dirty.addState(Weather_);
	}
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Earth);
Earth::Earth()
	: _sunlightIntensity(0.0f)
{
	_dirty.eraseState(SunlightIntensity_);
	for (int i = sun_; i < count_; ++i)
		_skyVisibility[i] = true;
}

void Earth::serialize(Spawner* spawner)
{
	spawner->setValue("earth_file", _earthFile);
	spawner->setValue("sun_visible", _skyVisibility[sun_]);
	spawner->setValue("moon_visible", _skyVisibility[moon_]);
	spawner->setValue("star_visible", _skyVisibility[star_]);
	spawner->setValue("nebula_visible", _skyVisibility[nebula_]);
	spawner->setValue("atmosphere_visible", _skyVisibility[atmosphere_]);
}

void Earth::deserialize(Spawner* spawner)
{
	spawner->getValue("earth_file", _earthFile);
	spawner->getValue("sun_visible", _skyVisibility[sun_]);
	spawner->getValue("moon_visible", _skyVisibility[moon_]);
	spawner->getValue("star_visible", _skyVisibility[star_]);
	spawner->getValue("nebula_visible", _skyVisibility[nebula_]);
	spawner->getValue("atmosphere_visible", _skyVisibility[atmosphere_]);
}

void Earth::setSunVisible(bool visible)
{
	if (_skyVisibility[sun_] != visible)
	{
		_skyVisibility[sun_] = visible;
		_dirty.addState(SunVisible_);
	}
}

void Earth::setMoonVisible(bool visible)
{
	if (_skyVisibility[moon_] != visible)
	{
		_skyVisibility[moon_] = visible;
		_dirty.addState(MoonVisible_);
	}
}

void Earth::setStarVisible(bool visible)
{
	if (_skyVisibility[star_] != visible)
	{
		_skyVisibility[star_] = visible;
		_dirty.addState(StarVisible_);
	}
}

void Earth::setNebulaVisible(bool visible)
{
	if (_skyVisibility[nebula_] != visible)
	{
		_skyVisibility[nebula_] = visible;
		_dirty.addState(NebulaVisible_);
	}
}

void Earth::setAtmosphereVisible(bool visible)
{
	if (_skyVisibility[atmosphere_] != visible)
	{
		_skyVisibility[atmosphere_] = visible;
		_dirty.addState(AtmosphereVisible_);
	}
}

void Earth::setSunlightIntensity(float intensity)
{
	if (!equals(_sunlightIntensity, intensity))
	{
		_sunlightIntensity = intensity;
		_dirty.addState(SunlightIntensity_);
	}
}
//////////////////////////////////////////////////////////////////////////
AI::AI()
{
}

AI::~AI()
{
}
