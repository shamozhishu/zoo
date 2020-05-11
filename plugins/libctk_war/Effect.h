#pragma once

#include "WarComponents.h"
#include <component/war/SharedComponents.h>

class Effect : public zoo::Entity, public Serializer { // tolua_export
	ZOO_REFLEX_DECLARE(Effect)
public:
	Effect();
	void init();
	void update();
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

protected:
	string _desc;
	Behavior* _behavior;
	DoF* _dof;
	Model* _model;
	Sound* _sound;
	Animator* _animator;

public:
	// tolua_begin
	int ID() const;
	int Kind() const;
};

// tolua_end
