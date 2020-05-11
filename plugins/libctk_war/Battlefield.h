#pragma once

#include <component/war/SharedComponents.h>

class Weapon;
class Effect;
class RedArmy;
class BlueArmy;
class AllyArmy;
class Stationary;
class WarReporter;
class Battlefield : public Serializer { // tolua_export
	std::list<Weapon*> _weapons;
	std::list<Effect*> _effects;
	std::list<RedArmy*> _redArmies;
	std::list<BlueArmy*> _blueArmies;
	std::list<AllyArmy*> _allyArmies;
	std::list<Stationary*> _stationaries;
	std::list<WarReporter*> _warReporters;
	friend class WarCommander;
	PROPERTY_R(int, _id, ID)
	PROPERTY_R(string, _desc, Desc)
public:
	Battlefield(int id, string description);
	~Battlefield();
	void init();
	void update();
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

public:
	// tolua_begin
	Weapon* createWeapon(int id);
	Weapon* getWeapon(int id);
	void destroyWeapon(int id);

	Effect* createEffect(int id);
	Effect* getEffect(int id);
	void destroyEffect(int id);

	RedArmy* createRedArmy(int id);
	RedArmy* getRedArmy(int id);
	void destroyRedArmy(int id);

	BlueArmy* createBlueArmy(int id);
	BlueArmy* getBlueArmy(int id);
	void destroyBlueArmy(int id);

	AllyArmy* createAllyArmy(int id);
	AllyArmy* getAllyArmy(int id);
	void destroyAllyArmy(int id);

	Stationary* createStationary(int id);
	Stationary* getStationary(int id);
	void destroyStationary(int id);

	WarReporter* createWarReporter(int id);
	WarReporter* getWarReporter(int id);
	void destroyWarReporter(int id);
};

// tolua_end
