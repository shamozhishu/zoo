#pragma once

#include "Entity.h"

class Effect;
class Weapon;
class RedArmy;
class BlueArmy;
class AllyArmy;
class StaticObj;
class Scene : public Entity
{
public:
	Scene(int id);
	~Scene();
	void update(float dt);
	Entity* clone();
	ENTITY_TYPE getType() const;
	Effect* createEffect(int id);
	void destroyEffect(int id);
	Weapon* createWeapon(int id);
	void destroyWeapon(int id);
	RedArmy* createRedArmy(int id);
	void destroyRedArmy(int id);
	BlueArmy* createBlueArmy(int id);
	void destroyBlueArmy(int id);
	AllyArmy* createAllyArmy(int id);
	void destroyAllyArmy(int id);
	StaticObj* createStaticObj(int id);
	void destroyStaticObj(int id);
};
