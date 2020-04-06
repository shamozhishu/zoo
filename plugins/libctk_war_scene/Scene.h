#pragma once

#include <map>
#include <zoo/Support.h>

class Entity;
class Effect;
class Weapon;
class RedArmy;
class BlueArmy;
class AllyArmy;
class StaticObj;
class Scene
{
public:
	Scene();
	~Scene();
	void visit(float dt);

public:
	Effect* createEffect(int id, zoo::TableCSV* pTable);
	Effect* getEffect(int id);
	void destroyEffect(int id);
	Weapon* createWeapon(int id, zoo::TableCSV* pTable);
	Weapon* getWeapon(int id);
	void destroyWeapon(int id);
	RedArmy* createRedArmy(int id, zoo::TableCSV* pTable);
	RedArmy* getRedArmy(int id);
	void destroyRedArmy(int id);
	BlueArmy* createBlueArmy(int id, zoo::TableCSV* pTable);
	BlueArmy* getBlueArmy(int id);
	void destroyBlueArmy(int id);
	AllyArmy* createAllyArmy(int id, zoo::TableCSV* pTable);
	AllyArmy* getAllyArmy(int id);
	void destroyAllyArmy(int id);
	StaticObj* createStaticObj(int id, zoo::TableCSV* pTable);
	StaticObj* getStaticObj(int id);
	void destroyStaticObj(int id);

private:
	void addEntity(Entity* ent);
	void removeEntity(int id, int type);
	Entity* findEntity(int id, int type);

private:
	std::map<long long, Entity*> _entities;
};
