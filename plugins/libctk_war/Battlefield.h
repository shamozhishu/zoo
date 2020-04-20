#pragma once

#include <zoo/Support.h>

class Entity;
class Effect;
class Weapon;
class RedArmy;
class BlueArmy;
class AllyArmy;
class Stationary;
class WarReporter;
class Battlefield
{
	friend class WarCommander;
	PROPERTY_R(int, _id, ID)
	PROPERTY_R(string, _desc, Desc)
public:
	Battlefield(int id, string description);
	~Battlefield();
	void visit(float dt);

public:
	Weapon* createWeapon(int id, zoo::TableCSV* pTable);
	Weapon* getWeapon(int id);
	void destroyWeapon(int id);

	Effect* createEffect(int id, zoo::TableCSV* pTable);
	Effect* getEffect(int id);
	void destroyEffect(int id);

	RedArmy* createRedArmy(int id, zoo::TableCSV* pTable);
	RedArmy* getRedArmy(int id);
	void destroyRedArmy(int id);

	BlueArmy* createBlueArmy(int id, zoo::TableCSV* pTable);
	BlueArmy* getBlueArmy(int id);
	void destroyBlueArmy(int id);

	AllyArmy* createAllyArmy(int id, zoo::TableCSV* pTable);
	AllyArmy* getAllyArmy(int id);
	void destroyAllyArmy(int id);

	Stationary* createStationary(int id, zoo::TableCSV* pTable);
	Stationary* getStationary(int id);
	void destroyStationary(int id);

	WarReporter* createWarReporter(int id, zoo::TableCSV* pTable);
	WarReporter* getWarReporter(int id);
	void destroyWarReporter(int id);

private:
	void addEntity(Entity* ent);
	void removeEntity(int id, int type);
	Entity* findEntity(int id, int type);

private:
	std::list<Weapon*> _weapons;
	std::list<Effect*> _effects;
	std::list<RedArmy*> _redArmies;
	std::list<BlueArmy*> _blueArmies;
	std::list<AllyArmy*> _allyArmies;
	std::list<Stationary*> _stationaries;
	std::list<WarReporter*> _warReporters;
	std::map<long long, Entity*> _entities;
};
