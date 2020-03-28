#pragma once

#include "Entity.h"

class BlueArmy : public Entity
{
	friend class Scene;
	BlueArmy(int id);
	~BlueArmy();
public:
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
};
