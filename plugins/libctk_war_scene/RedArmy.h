#pragma once

#include "Entity.h"

class RedArmy : public Entity
{
	friend class Scene;
	RedArmy();
	~RedArmy();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);
};
