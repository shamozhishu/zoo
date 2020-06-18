#pragma once

#include "PublicEnum.h"
#include "WarComponents.h"

using namespace zoo;

class Battlefield { // tolua_export
	friend class Behavior;
	list<Behavior*> _behaviors;
	Spawner _spawner[ENTITY_COUNT];

	PROPERTY_R(int, _id, ID)
	PROPERTY_R(string, _desc, Desc)

public:
	Battlefield(int id, string description);
	~Battlefield();
	void awake();
	void load();
	void save();
	void execScripts();

private:
	void buildWeapon(Entity* pEnt);
	void buildEffect(Entity* pEnt);
	void buildRedArmy(Entity* pEnt);
	void buildBlueArmy(Entity* pEnt);
	void buildAllyArmy(Entity* pEnt);
	void buildStationary(Entity* pEnt);
	void buildWarReporter(Entity* pEnt);

public:
	// tolua_begin
	Entity* createEntity(int id, ENTITY_TYPE entType);
	Entity* getEntity(int id, ENTITY_TYPE entType);
	void destroyEntity(int id, ENTITY_TYPE entType);
};

// tolua_end
