#pragma once

#include <list>
#include "PublicEnum.h"

class Entity;
class EntitiesCache
{
public:
	EntitiesCache();
	~EntitiesCache();
	static EntitiesCache& getIns();
	static void destroy();
	void putIn(Entity* pEnt);
	Entity* getOut(ENTITY_TYPE type);

private:
	std::list<Entity*> _uselessEntities;
};
