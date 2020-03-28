#pragma once

#include "Entity.h"

class RedArmy : public Entity
{
	friend class Scene;
	RedArmy(int id);
	~RedArmy();
public:
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
};
