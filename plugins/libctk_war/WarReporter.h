#pragma once

#include "Entity.h"

class WarReporter : public Entity { // tolua_export
	friend class Battlefield;
	WarReporter();
	~WarReporter();
public:
	void init();
	ENTITY_TYPE getType() const;
	void serialize(stringstream& ss);
	void deserialize(TableCSV* pTable);

public:
	// tolua_begin
};

// tolua_end
