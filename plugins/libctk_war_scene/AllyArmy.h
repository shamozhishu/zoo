#pragma once

#include "Entity.h"

class AllyArmy : public Entity
{
	friend class Scene;
	AllyArmy();
	~AllyArmy();
public:
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);
};
