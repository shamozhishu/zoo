#pragma once

#include "Entity.h"

class Effect : public Entity
{
	friend class Scene;
	Effect();
	~Effect();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);
};
