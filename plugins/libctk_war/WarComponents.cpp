#include "WarComponents.h"
#include "LuaScript.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>
#include "WarCommander.h"
#include "Battlefield.h"

enum { ID = 0, DESCRIPTION, SCRIPT, POSX, POSY, POSZ, HEADING, PITCH, ROLL, SCALEX, SCALEY, SCALEZ, PARENT, VISIBLE, MODEL_FILE, SOUND_FILE, TRAJ_FILE };
enum { RATIO_LEFT = 0, RATIO_RIGHT, RATIO_BOTTOM, RATIO_TOP, TRACK_ENTITY };

static const char* TableHeader[] = { "编号", "描述", "脚本", "位置X", "位置Y", "位置Z", "旋转H", "旋转P", "旋转R", "缩放X", "缩放Y", "缩放Z", "挂载实体",
"可见性", "模型文件", "音效文件", "轨迹文件" };
static const char* TableField[] = { "id", "description", "script", "x", "y", "z", "heading", "pitch", "roll", "sx", "sy", "sz", "parent",
"visible", "model_file", "sound_file", "traj_file" };

static const char* Camera_TableHeader[] = { "视口左侧比例", "视口右侧比例", "视口底部比例", "视口顶部比例", "跟踪实体" };
static const char* Camera_TableField[] = { "ratio_left", "ratio_right", "ratio_bottom", "ratio_top", "track_entity" };

ZOO_REFLEX_IMPLEMENT(Behavior);
Behavior::Behavior()
	: _scriptValid(false)
	, _scriptInited(false)
	, _script(new LuaScript)
{
	WarCommander::getSingleton().getCurBattlefield()->_behaviors.push_back(this);
}

Behavior::~Behavior()
{
	list<Behavior*>& behaviors = WarCommander::getSingleton().getCurBattlefield()->_behaviors;
	auto it = std::find(behaviors.begin(), behaviors.end(), this);
	if (it != behaviors.end())
		behaviors.erase(it);

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
	spawner->ss() << _scriptFile;
}

void Behavior::deserialize(Spawner* spawner)
{
	_scriptFile = spawner->getTable()->item2str(getEntity()->ID(), TableField[SCRIPT]);
}

void Behavior::serializeField(Spawner* spawner)
{
	spawner->ss() << TableField[SCRIPT];
}

void Behavior::serializeHeader(Spawner* spawner)
{
	spawner->ss() << TableHeader[SCRIPT];
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
}

void DoF::serialize(Spawner* spawner)
{
	string parent;
	if (_parent)
	{
		parent = std::to_string(_parent->getEntity()->ID());
		parent += ":";
		parent += std::to_string(_parent->getEntity()->Breed());
	}

	spawner->ss() << getPosX() << "," << getPosY() << "," << getPosZ()
		<< "," << getHeading() << "," << getPitch() << "," << getRoll()
		<< "," << getScaleX() << "," << getScaleY() << "," << getScaleZ()
		<< "," << parent;
}

void DoF::deserialize(Spawner* spawner)
{
	setPosX(spawner->getTable()->item2float(getEntity()->ID(), TableField[POSX]));
	setPosY(spawner->getTable()->item2float(getEntity()->ID(), TableField[POSY]));
	setPosZ(spawner->getTable()->item2float(getEntity()->ID(), TableField[POSZ]));
	setHeading(spawner->getTable()->item2float(getEntity()->ID(), TableField[HEADING]));
	setPitch(spawner->getTable()->item2float(getEntity()->ID(), TableField[PITCH]));
	setRoll(spawner->getTable()->item2float(getEntity()->ID(), TableField[ROLL]));
	setScaleX(spawner->getTable()->item2float(getEntity()->ID(), TableField[SCALEX]));
	setScaleY(spawner->getTable()->item2float(getEntity()->ID(), TableField[SCALEY]));
	setScaleZ(spawner->getTable()->item2float(getEntity()->ID(), TableField[SCALEZ]));
	string parent = spawner->getTable()->item2str(getEntity()->ID(), TableField[PARENT]);
	vector<string> container;
	zoo::stringtok(container, parent, ":");
	if (container.size() > 2)
	{
		int id = atoi(container[0].c_str());
		int breed = atoi(container[1].c_str());
		Spawner spawner(breed);
		setParent(spawner.find_Entity(id)->getComponent<DoF>());
	}
}

void DoF::serializeField(Spawner* spawner)
{
	spawner->ss() << TableField[POSX] << "," << TableField[POSY] << "," << TableField[POSZ]
		<< "," << TableField[HEADING] << "," << TableField[PITCH] << "," << TableField[ROLL]
		<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << "," << TableField[SCALEZ]
		<< "," << TableField[PARENT];
}

void DoF::serializeHeader(Spawner* spawner)
{
	spawner->ss() << TableHeader[POSX] << "," << TableHeader[POSY] << "," << TableHeader[POSZ]
		<< "," << TableHeader[HEADING] << "," << TableHeader[PITCH] << "," << TableHeader[ROLL]
		<< "," << TableHeader[SCALEX] << "," << TableHeader[SCALEY] << "," << TableHeader[SCALEZ]
		<< "," << TableHeader[PARENT];
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
	spawner->ss() << _visible << "," << _modelFile;
}

void Model::deserialize(Spawner* spawner)
{
	_visible = spawner->getTable()->item2int(getEntity()->ID(), TableField[VISIBLE]) == 1;
	_modelFile = spawner->getTable()->item2str(getEntity()->ID(), TableField[MODEL_FILE]);
}

void Model::serializeField(Spawner* spawner)
{
	spawner->ss() << TableField[VISIBLE] << "," << TableField[MODEL_FILE];
}

void Model::serializeHeader(Spawner* spawner)
{
	spawner->ss() << TableHeader[VISIBLE] << "," << TableHeader[MODEL_FILE];
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
	spawner->ss() << _soundFile;
}

void Sound::deserialize(Spawner* spawner)
{
	_soundFile = spawner->getTable()->item2str(getEntity()->ID(), TableField[SOUND_FILE]);
}

void Sound::serializeField(Spawner* spawner)
{
	spawner->ss() << TableField[SOUND_FILE];
}

void Sound::serializeHeader(Spawner* spawner)
{
	spawner->ss() << TableHeader[SOUND_FILE];
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
	spawner->ss() << _trajFile;
}

void Animator::deserialize(Spawner* spawner)
{
	_trajFile = spawner->getTable()->item2str(getEntity()->ID(), TableField[TRAJ_FILE]);
}

void Animator::serializeField(Spawner* spawner)
{
	spawner->ss() << TableField[TRAJ_FILE];
}

void Animator::serializeHeader(Spawner* spawner)
{
	spawner->ss() << TableHeader[TRAJ_FILE];
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Camera);
void Camera::serialize(Spawner* spawner)
{
}

void Camera::deserialize(Spawner* spawner)
{
}

void Camera::serializeField(Spawner* spawner)
{
	spawner->ss() << "," << Camera_TableField[RATIO_LEFT] << "," << Camera_TableField[RATIO_RIGHT]
		<< "," << Camera_TableField[RATIO_BOTTOM] << "," << Camera_TableField[RATIO_TOP]
		<< "," << Camera_TableField[TRACK_ENTITY];
}

void Camera::serializeHeader(Spawner* spawner)
{
	spawner->ss() << "," << Camera_TableHeader[RATIO_LEFT] << "," << Camera_TableHeader[RATIO_RIGHT]
		<< "," << Camera_TableHeader[RATIO_BOTTOM] << "," << Camera_TableHeader[RATIO_TOP]
		<< "," << Camera_TableHeader[TRACK_ENTITY];
}
