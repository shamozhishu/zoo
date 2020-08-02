#include "WarComponents.h"
#include "LuaScript.h"
#include <zoo/Utils.h>
#include "WarCommander.h"
#include "Battlefield.h"

ZOO_REFLEX_IMPLEMENT(Behavior);
Behavior::Behavior()
	: _scriptValid(false)
	, _scriptInited(false)
	, _script(new LuaScript)
{
	WarCommander::getSingleton().getCurBattlefield()->addBehavior(this);
}

Behavior::~Behavior()
{
	WarCommander::getSingleton().getCurBattlefield()->removeBehavior(this);
	SAFE_DELETE(_script);
}

void Behavior::exec()
{
	if (!_scriptInited)
	{
		_scriptInited = true;
		_scriptValid = false;
		if (_scriptFile != "")
		{
			_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + _scriptFile);
			if (_scriptValid)
			{
				Entity* pEnt = getEntity();
				auto components = pEnt->getComponents();
				auto it = components.begin();
				for (; it != components.end(); ++it)
				{
					if (it->first != "Behavior")
						_script->setVariable(it->first.c_str(), it->first.c_str(), it->second);
				}

				_script->setVariable("this", "Entity", getEntity());
				_script->executeGlobalFunction("Init");
			}
		}
	}

	if (_scriptValid)
		_script->executeGlobalFunction("Update");
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
AI::AI()
{
}

AI::~AI()
{
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(DoF);
DoF::DoF()
	: _parent(nullptr)
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
	spawner->getValue("mount_entity_id", _mountEntID);
	spawner->getValue("mount_entity_breed", _mountEntBreed);
	SignalTrigger::connect(*getEntity()->getSpawner(), this, &DoF::onSetParent);
}

void DoF::onSetParent(const UserData& userdata)
{
	Entity* pEnt = getEntity()->getSpawner()->gain(_mountEntID, _mountEntBreed);
	if (pEnt)
	{
		setParent(pEnt->getComponent<DoF>());
	}
}

void DoF::setPosX(double x)
{
	_x = x;
	_dirty.addState(dof_);
}

double DoF::getPosX() const
{
	return _x;
}

void DoF::setPosY(double y)
{
	_y = y;
	_dirty.addState(dof_);
}

double DoF::getPosY() const
{
	return _y;
}

void DoF::setPosZ(double z)
{
	_z = z;
	_dirty.addState(dof_);
}

double DoF::getPosZ() const
{
	return _z;
}

void DoF::setHeading(double h)
{
	_heading = h;
	_dirty.addState(dof_);
}

float DoF::getHeading() const
{
	return _heading;
}

void DoF::setPitch(double p)
{
	_pitch = p;
	_dirty.addState(dof_);
}

float DoF::getPitch() const
{
	return _pitch;
}

void DoF::setRoll(double r)
{
	_roll = r;
	_dirty.addState(dof_);
}

float DoF::getRoll() const
{
	return _roll;
}

void DoF::setScaleX(double x)
{
	_sx = x;
	_dirty.addState(dof_);
}

float DoF::getScaleX() const
{
	return _sx;
}

void DoF::setScaleY(double y)
{
	_sy = y;
	_dirty.addState(dof_);
}

float DoF::getScaleY() const
{
	return _sy;
}

void DoF::setScaleZ(double z)
{
	_sz = z;
	_dirty.addState(dof_);
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

	_dirty.addState(parent_);
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
}

void Model::serialize(Spawner* spawner)
{
	spawner->setValue("visible", _visible);
	spawner->setValue("model_file", _modelFile);
}

void Model::deserialize(Spawner* spawner)
{
	spawner->getValue("visible", _visible);
	spawner->getValue("model_file", _modelFile);
}

bool Model::isVisible() const
{
	return _visible;
}

void Model::setVisible(bool visible)
{
	_visible = visible;
	_dirty.addState(visible_);
}

void Model::setModelFile(const string& modelFile)
{
	_modelFile = modelFile;
	_dirty.addState(modelFile_);
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
ZOO_REFLEX_IMPLEMENT(Camera);
Camera::Camera()
	: _trackEntID(-1)
	, _trackEntBreed(-1)
	, _lRatio(0)
	, _rRatio(1)
	, _bRatio(0)
	, _tRatio(1)
	, _red(0)
	, _green(0)
	, _blue(0)
	, _alpha(255)
{
}

void Camera::serialize(Spawner* spawner)
{
	spawner->setValue("bgcolor_red", _red);
	spawner->setValue("bgcolor_green", _green);
	spawner->setValue("bgcolor_blue", _blue);
	spawner->setValue("bgcolor_alpha", _alpha);
	spawner->setValue("ratio_left", _lRatio);
	spawner->setValue("ratio_right", _rRatio);
	spawner->setValue("ratio_bottom", _bRatio);
	spawner->setValue("ratio_top", _tRatio);
	spawner->setValue("track_entity_id", _trackEntID);
	spawner->setValue("track_entity_breed", _trackEntBreed);
}

void Camera::deserialize(Spawner* spawner)
{
	spawner->getValue("bgcolor_red", _red);
	spawner->getValue("bgcolor_green", _green);
	spawner->getValue("bgcolor_blue", _blue);
	spawner->getValue("bgcolor_alpha", _alpha);
	spawner->getValue("ratio_left", _lRatio);
	spawner->getValue("ratio_right", _rRatio);
	spawner->getValue("ratio_bottom", _bRatio);
	spawner->getValue("ratio_top", _tRatio);
	spawner->getValue("track_entity_id", _trackEntID);
	spawner->getValue("track_entity_breed", _trackEntBreed);
}

void Camera::setTrackEnt(int id, int breed)
{
	_trackEntID = id;
	_trackEntBreed = breed;
	_dirty.addState(trackEnt_);
}

void Camera::setBgColor(int r, int g, int b, int a /*= 255*/)
{
	_red = r; _green = g; _blue = b; _alpha = a;
	_dirty.addState(bgcolour_);
}

void Camera::setViewport(float leftRatio, float rightRatio, float bottomRatio, float topRatio)
{
	_lRatio = leftRatio;
	_rRatio = rightRatio;
	_bRatio = bottomRatio;
	_tRatio = topRatio;
	_dirty.addState(viewport_);
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Environment);
Environment::Environment()
{

}
void Environment::serialize(Spawner* spawner)
{
	
}

void Environment::deserialize(Spawner* spawner)
{
	
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Earth);
Earth::Earth()
	: _sunlightIntensity(0.0f)
{
	_dirty.eraseState(sunlightIntensity_);
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
	_skyVisibility[sun_] = visible;
	_dirty.addState(sunVisible_);
}

void Earth::setMoonVisible(bool visible)
{
	_skyVisibility[moon_] = visible;
	_dirty.addState(moonVisible_);
}

void Earth::setStarVisible(bool visible)
{
	_skyVisibility[star_] = visible;
	_dirty.addState(starVisible_);
}

void Earth::setNebulaVisible(bool visible)
{
	_skyVisibility[nebula_] = visible;
	_dirty.addState(nebulaVisible_);
}

void Earth::setAtmosphereVisible(bool visible)
{
	_skyVisibility[atmosphere_] = visible;
	_dirty.addState(atmosphereVisible_);
}

void Earth::setSunlightIntensity(float intensity)
{
	_sunlightIntensity = intensity;
	_dirty.addState(sunlightIntensity_);
}
