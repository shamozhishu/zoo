#pragma once

#include "Entity.h"

class Weapon : public Entity
{
	friend class Scene;
	Weapon(int id);
	~Weapon();
public:
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
};
