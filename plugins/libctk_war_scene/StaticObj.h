#pragma once

#include "Entity.h"

class StaticObj : public Entity
{
	friend class Scene;
	StaticObj();
	~StaticObj();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);
};
