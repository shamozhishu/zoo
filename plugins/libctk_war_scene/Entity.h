#pragma once

#include <sstream>
#include "PublicEnum.h"
#include <SharedObject/DoF.h>
#include <ctkPluginFramework_global.h>

using namespace std;
using namespace zoo;
class LuaScript;

class Entity
{
	friend class Scene;
	friend class EntitiesCache;
	PROPERTY_R(int, _id, ID)
	PROPERTY_R(DoF*, _dof, DoF)
protected:
	Entity();
	virtual ~Entity();
	ctkProperties& getProps();
	const ctkProperties& getProps() const;

public:
	virtual void init();
	virtual void update(float dt);
	virtual ENTITY_TYPE getType() const = 0;
	virtual void serialize(stringstream& ss) = 0;
	virtual void deserialize(TableCSV* pTable) = 0;

protected:
	bool _hasScript;
	LuaScript* _script;
	ctkProperties _props;
};
