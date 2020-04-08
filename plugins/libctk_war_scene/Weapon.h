#pragma once

#include "Entity.h"

class Weapon : public Entity
{
	friend class Scene;
	Weapon();
	~Weapon();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);
};
