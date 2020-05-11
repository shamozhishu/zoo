#include "WarComponents.h"
#include "LuaScript.h"
#include "CommonDef.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>

ZOO_REFLEX_IMPLEMENT(Behavior);
Behavior::Behavior()
	: _scriptValid(false)
	, _script(new LuaScript)
{
}

Behavior::~Behavior()
{
	SAFE_DELETE(_script);
}

bool Behavior::init()
{
	_scriptValid = false;
	if (_scriptFile == "")
		return false;

	_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + _scriptFile);
	if (_scriptValid)
	{
		_script->setVariable("this", getEntity()->typeName().c_str(), getEntity());
		_script->executeGlobalFunction("Init");
	}

	return _scriptValid;
}

void Behavior::update()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Update");
}

void Behavior::serialize(stringstream& ss)
{
	ss << _scriptFile;
}

void Behavior::deserialize(zoo::TableCSV* pTable)
{
	_scriptFile = pTable->item2str(getEntity()->ID(), TableField[SCRIPT]);
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
}

DoF::~DoF()
{
	setParent(nullptr);
}

void DoF::serialize(stringstream& ss)
{
	string parent;
	if (_parent)
	{
		parent = std::to_string(_parent->getEntity()->ID());
		parent += ":";
		parent += std::to_string(_parent->getEntity()->Kind());
	}

	ss << getPosX() << "," << getPosY() << "," << getPosZ()
		<< "," << getHeading() << "," << getPitch() << "," << getRoll()
		<< "," << getScaleX() << "," << getScaleY() << "," << getScaleZ()
		<< "," << parent;
}

void DoF::deserialize(zoo::TableCSV* pTable)
{
	setPosX(pTable->item2float(getEntity()->ID(), TableField[POSX]));
	setPosY(pTable->item2float(getEntity()->ID(), TableField[POSY]));
	setPosZ(pTable->item2float(getEntity()->ID(), TableField[POSZ]));
	setHeading(pTable->item2float(getEntity()->ID(), TableField[HEADING]));
	setPitch(pTable->item2float(getEntity()->ID(), TableField[PITCH]));
	setRoll(pTable->item2float(getEntity()->ID(), TableField[ROLL]));
	setScaleX(pTable->item2float(getEntity()->ID(), TableField[SCALEX]));
	setScaleY(pTable->item2float(getEntity()->ID(), TableField[SCALEY]));
	setScaleZ(pTable->item2float(getEntity()->ID(), TableField[SCALEZ]));
	string parent = pTable->item2str(getEntity()->ID(), TableField[PARENT]);
	vector<string> container;
	zoo::stringtok(container, parent, ":");
	if (container.size() > 2)
	{
		int id = atoi(container[0].c_str());
		int kind = atoi(container[1].c_str());
		setParent(zoo::Entity::find(id, kind)->getComponent<DoF>());
	}
}

void DoF::setPosX(double x)
{
	_x = x;
}

double DoF::getPosX() const
{
	return _x;
}

void DoF::setPosY(double y)
{
	_y = y;
}

double DoF::getPosY() const
{
	return _y;
}

void DoF::setPosZ(double z)
{
	_z = z;
}

double DoF::getPosZ() const
{
	return _z;
}

void DoF::setHeading(double h)
{
	_heading = h;
}

float DoF::getHeading() const
{
	return _heading;
}

void DoF::setPitch(double p)
{
	_pitch = p;
}

float DoF::getPitch() const
{
	return _pitch;
}

void DoF::setRoll(double r)
{
	_roll = r;
}

float DoF::getRoll() const
{
	return _roll;
}

void DoF::setScaleX(double x)
{
	_sx = x;
}

float DoF::getScaleX() const
{
	return _sx;
}

void DoF::setScaleY(double y)
{
	_sy = y;
}

float DoF::getScaleY() const
{
	return _sy;
}

void DoF::setScaleZ(double z)
{
	_sz = z;
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
}

DoF* DoF::getParent() const
{
	return _parent;
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Model);
void Model::serialize(stringstream& ss)
{
	ss << _visible << "," << _modelFile;
}

void Model::deserialize(zoo::TableCSV* pTable)
{
	_visible = pTable->item2int(getEntity()->ID(), TableField[VISIBLE]) == 1;
	_modelFile = pTable->item2str(getEntity()->ID(), TableField[MODEL_FILE]);
}

bool Model::isVisible() const
{
	return _visible;
}

void Model::setVisible(bool visible)
{
	_visible = visible;
}

void Model::setModelFile(const string& modelFile)
{
	_modelFile = modelFile;
}
//////////////////////////////////////////////////////////////////////////
ZOO_REFLEX_IMPLEMENT(Sound);
void Sound::serialize(stringstream& ss)
{
	ss << _soundFile;
}

void Sound::deserialize(zoo::TableCSV* pTable)
{
	_soundFile = pTable->item2str(getEntity()->ID(), TableField[SOUND_FILE]);
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
void Animator::serialize(stringstream& ss)
{
	ss << _trajFile;
}

void Animator::deserialize(zoo::TableCSV* pTable)
{
	_trajFile = pTable->item2str(getEntity()->ID(), TableField[TRAJ_FILE]);
}
