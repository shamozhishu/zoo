#pragma once

#include "Entity.h"

class BlueArmy : public Entity { // tolua_export
	friend class Battlefield;
	BlueArmy();
	~BlueArmy();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);

public:
	// tolua_begin
};

// tolua_end
