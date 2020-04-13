#include "Entity.h"
#include "LuaScript.h"
#include "WarManager.h"
#include <zooCmd/zooCmd.h>

class EntDoF : public DoF
{
public:
	EntDoF(int id, int group) : DoF(id, group) {}
	bool addChild(DoF* child)
	{
		if (DoF::addChild(child))
		{
			zooCmd_Send("war --child-add %d %d %d %d", child->getID(), child->getGroup(), _id, _group);
			return true;
		}

		return false;
	}

	bool removeChild(DoF* child)
	{
		if (DoF::removeChild(child))
		{
			zooCmd_Send("war --child-remove %d %d %d %d", child->getID(), child->getGroup(), _id, _group);
			return true;
		}

		return false;
	}
};

Entity::Entity()
	: _id(-1)
	, _dof(nullptr)
	, _hasScript(false)
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
	SAFE_DELETE(_dof);
	_dof = new EntDoF(_id, getType());
	string filename = _props.value("script").toString().toStdString();
	if (filename != "")
		_hasScript = _script->executeScriptFile(WarManager::getSingleton().getResPath() + filename);
	if (_hasScript)
		_script->executeGlobalFunction("Init");
}

void Entity::update(float dt)
{
	if (_hasScript)
		_script->executeGlobalFunction("Update");
}
