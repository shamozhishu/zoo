#include "Entity.h"
#include "LuaScript.h"

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
	_hasScript = _script->executeScriptFile(_props.value("script").toString().toStdString());
	if (_hasScript)
		_script->executeGlobalFunction("Init");
}

void Entity::update(float dt)
{
	if (_hasScript)
		_script->executeGlobalFunction("Update");
}
