#pragma once

#include "WarComponents.h"
#include <component/war/SharedComponents.h>

class Weapon : public zoo::Entity, public Serializer { // tolua_export
	ZOO_REFLEX_DECLARE(Weapon)
public:
	Weapon();
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
