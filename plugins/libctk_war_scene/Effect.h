#pragma once

#include "Entity.h"

class Effect : public Entity
{
	friend class Scene;
	Effect(int id);
	~Effect();
public:
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
};
