#pragma once

#include "Entity.h"

class AllyArmy : public Entity
{
	friend class Scene;
	AllyArmy(int id);
	~AllyArmy();
public:
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
};
