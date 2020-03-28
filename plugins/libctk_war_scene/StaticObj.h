#pragma once

#include "Entity.h"

class StaticObj : public Entity
{
	friend class Scene;
	StaticObj(int id);
	~StaticObj();
public:
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
};
