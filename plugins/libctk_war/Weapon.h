#pragma once

#include "Entity.h"

class Weapon : public Entity { // tolua_export
	friend class Battlefield;
	Weapon();
	~Weapon();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);

public:
	// tolua_begin
};

// tolua_end
