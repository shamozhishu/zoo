#include "Entity.h"
#include "LuaScript.h"
#include "CommonDef.h"
#include "WarCommander.h"
#include <zooCmdLoader/ZooCmdLoader.h>

class EntDoF : public DoF
{
public:
	EntDoF(int id, int group) : DoF(id, group) {}
	bool addChild(DoF* child)
	{
		if (DoF::addChild(child))
		{
			zooCmdL_Send("war --child-add %d %d %d %d", child->getID(), child->getGroup(), _id, _group);
			return true;
		}

		return false;
	}

	bool removeChild(DoF* child)
	{
		if (DoF::removeChild(child))
		{
			zooCmdL_Send("war --child-remove %d %d %d %d", child->getID(), child->getGroup(), _id, _group);
			return true;
		}

		return false;
	}
};

Entity::Entity()
	: _id(-1)
	, _dof(nullptr)
	, _hasScript(false)
	, _scriptInited(false)
	, _script(new LuaScript)
{
}

Entity::~Entity()
{
	SAFE_DELETE(_dof);
	SAFE_DELETE(_script);
}

ctkProperties& Entity::getProps()
{
	return _props;
}

const ctkProperties& Entity::getProps() const
{
	return _props;
}

void Entity::init()
{
	_hasScript = false;
	_scriptInited = false;

	SAFE_DELETE(_dof);
	_dof = new EntDoF(_id, getType());

	string filename = _props.value(Default_TableField[SCRIPT]).toString().toStdString();
	if (filename != "")
		_hasScript = _script->executeScriptFile(WarCommander::getSingleton().getResPath() + filename);
}

void Entity::update(float dt)
{
	if (!_hasScript)
		return;

	if (!_scriptInited)
	{
		_scriptInited = true;
		_script->setVariable("this", WarCommander::getSingleton().getEntTypeName(getType()), this);
		_script->executeGlobalFunction("Init");
	}

	_script->executeGlobalFunction("Update");
}

void Entity::setPos(double x, double y, double z)
{
	_dof->setX(x);
	_dof->setY(y);
	_dof->setZ(z);
}

void Entity::setRot(float h, float p, float r)
{
	_dof->setH(h);
	_dof->setP(p);
	_dof->setR(r);
}

double Entity::getX() const
{
	return _dof->getX();
}

double Entity::getY() const
{
	return _dof->getY();
}

double Entity::getZ() const
{
	return _dof->getZ();
}

float Entity::getH() const
{
	return _dof->getH();
}

float Entity::getP() const
{
	return _dof->getP();
}

float Entity::getR() const
{
	return _dof->getR();
}
