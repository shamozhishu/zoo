#pragma once

#include "WarComponents.h"
#include <component/war/SharedComponents.h>

class WarReporter : public zoo::Entity, public Serializer { // tolua_export
	ZOO_REFLEX_DECLARE(WarReporter)
public:
	WarReporter();
	void init();
	void update();
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

protected:
	string _desc;
	Behavior* _behavior;

public:
	// tolua_begin
	int ID() const;
	int Kind() const;
};

// tolua_end
