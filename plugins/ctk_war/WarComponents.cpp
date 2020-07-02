#include "WarComponents.h"
#include "LuaScript.h"
#include <zoo/Utils.h>
#include <cJSON/CJsonObject.h>
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
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Add("script", _scriptFile);
}

void Behavior::deserialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Get("script", _scriptFile);
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
	, _sx(0)
	, _sy(0)
	, _sz(0)
{
	_dirty.addState(dof_ | parent_);
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
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Add("x", _x);
	comJson.Add("y", _y);
	comJson.Add("z", _z);
	comJson.Add("heading", _heading);
	comJson.Add("pitch", _pitch);
	comJson.Add("roll", _roll);
	comJson.Add("sx", _sx);
	comJson.Add("sy", _sy);
	comJson.Add("sz", _sz);

	if (_parent)
	{
		_mountEntID = _parent->getEntity()->id();
		_mountEntBreed = _parent->getEntity()->breed();
		comJson.Add("mount_entity_id", _mountEntID);
		comJson.Add("mount_entity_breed", _mountEntBreed);
	}
	else
	{
		comJson.AddNull("mount_entity_id");
		comJson.AddNull("mount_entity_breed");
	}
}

void DoF::deserialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Get("x", _x);
	comJson.Get("y", _y);
	comJson.Get("z", _z);
	comJson.Get("heading", _heading);
	comJson.Get("pitch", _pitch);
	comJson.Get("roll", _roll);
	comJson.Get("sx", _sx);
	comJson.Get("sy", _sy);
	comJson.Get("sz", _sz);
	comJson.Get("mount_entity_id", _mountEntID);
	comJson.Get("mount_entity_breed", _mountEntBreed);
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
{
	_dirty.addState(visible_ | modelFile_);
}

void Model::serialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Add("visible", _visible);
	comJson.Add("model_file", _modelFile);
}

void Model::deserialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Get("visible", _visible);
	comJson.Get("model_file", _modelFile);
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
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Add("sound_file", _soundFile);
}

void Sound::deserialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Get("sound_file", _soundFile);
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
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Add("traj_file", _trajFile);
}

void Animator::deserialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Get("traj_file", _trajFile);
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
{
}

void Camera::serialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Add("ratio_left", _lRatio);
	comJson.Add("ratio_right", _rRatio);
	comJson.Add("ratio_bottom", _bRatio);
	comJson.Add("ratio_top", _tRatio);
	comJson.Add("track_entity_id", _trackEntID);
	comJson.Add("track_entity_breed", _trackEntBreed);
}

void Camera::deserialize(Spawner* spawner)
{
	CJsonObject& comJson = *(CJsonObject*)spawner->getParser();
	comJson.Get("ratio_left", _lRatio);
	comJson.Get("ratio_right", _rRatio);
	comJson.Get("ratio_bottom", _bRatio);
	comJson.Get("ratio_top", _tRatio);
	comJson.Get("track_entity_id", _trackEntID);
	comJson.Get("track_entity_breed", _trackEntBreed);
}

void Camera::setTrackEnt(int id, int breed)
{
	_trackEntID = id;
	_trackEntBreed = breed;
	_dirty.addState(trackEnt_);
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
