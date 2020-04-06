#include "Scene.h"
#include "Effect.h"
#include "Weapon.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "StaticObj.h"
#include "EntitiesCache.h"
#include <zoo/DatabaseCSV.h>

using namespace zoo;

Scene::Scene()
{
}

Scene::~Scene()
{
	auto it = _entities.begin();
	for (; it != _entities.end(); ++it)
		delete it->second;
}

void Scene::visit(float dt)
{
	std::map<long long, Entity*> ents = _entities;
	auto it = ents.begin();
	auto itEnd = ents.end();
	for (; it != itEnd; ++it)
		it->second->update(dt);
}

Effect* Scene::createEffect(int id, TableCSV* pTable)
{
	Effect* pEffect = getEffect(id);
	if (!pEffect)
	{
		pEffect = dynamic_cast<Effect*>(EntitiesCache::getIns().getOut(ENTITY_EFFECT));
		if (!pEffect)
			pEffect = new Effect();

		pEffect->_id = id;
		addEntity(pEffect);
	}

	if (pTable)
		pEffect->deserialize(pTable);

	pEffect->init();
	return pEffect;
}

Effect* Scene::getEffect(int id)
{
	return dynamic_cast<Effect*>(findEntity(id, ENTITY_EFFECT));
}

void Scene::destroyEffect(int id)
{
	removeEntity(id, ENTITY_EFFECT);
}

Weapon* Scene::createWeapon(int id, TableCSV* pTable)
{
	Weapon* pWeapon = getWeapon(id);
	if (!pWeapon)
	{
		pWeapon = dynamic_cast<Weapon*>(EntitiesCache::getIns().getOut(ENTITY_WEAPON));
		if (!pWeapon)
			pWeapon = new Weapon();

		pWeapon->_id = id;
		addEntity(pWeapon);
	}

	if (pTable)
		pWeapon->deserialize(pTable);

	pWeapon->init();
	return pWeapon;
}

Weapon* Scene::getWeapon(int id)
{
	return dynamic_cast<Weapon*>(findEntity(id, ENTITY_WEAPON));
}

void Scene::destroyWeapon(int id)
{
	removeEntity(id, ENTITY_WEAPON);
}

RedArmy* Scene::createRedArmy(int id, TableCSV* pTable)
{
	RedArmy* pRedArmy = getRedArmy(id);
	if (!pRedArmy)
	{
		pRedArmy = dynamic_cast<RedArmy*>(EntitiesCache::getIns().getOut(ENTITY_REDARMY));
		if (!pRedArmy)
			pRedArmy = new RedArmy();

		pRedArmy->_id = id;
		addEntity(pRedArmy);
	}

	if (pTable)
		pRedArmy->deserialize(pTable);

	pRedArmy->init();
	return pRedArmy;
}

RedArmy* Scene::getRedArmy(int id)
{
	return dynamic_cast<RedArmy*>(findEntity(id, ENTITY_REDARMY));
}

void Scene::destroyRedArmy(int id)
{
	removeEntity(id, ENTITY_REDARMY);
}

BlueArmy* Scene::createBlueArmy(int id, TableCSV* pTable)
{
	BlueArmy* pBlueArmy = getBlueArmy(id);
	if (!pBlueArmy)
	{
		pBlueArmy = dynamic_cast<BlueArmy*>(EntitiesCache::getIns().getOut(ENTITY_BLUEARMY));
		if (!pBlueArmy)
			pBlueArmy = new BlueArmy();

		pBlueArmy->_id = id;
		addEntity(pBlueArmy);
	}

	if (pTable)
		pBlueArmy->deserialize(pTable);

	pBlueArmy->init();
	return pBlueArmy;
}

BlueArmy* Scene::getBlueArmy(int id)
{
	return dynamic_cast<BlueArmy*>(findEntity(id, ENTITY_BLUEARMY));
}

void Scene::destroyBlueArmy(int id)
{
	removeEntity(id, ENTITY_BLUEARMY);
}

AllyArmy* Scene::createAllyArmy(int id, TableCSV* pTable)
{
	AllyArmy* pAllyArmy = getAllyArmy(id);
	if (!pAllyArmy)
	{
		pAllyArmy = dynamic_cast<AllyArmy*>(EntitiesCache::getIns().getOut(ENTITY_ALLYARMY));
		if (!pAllyArmy)
			pAllyArmy = new AllyArmy();

		pAllyArmy->_id = id;
		addEntity(pAllyArmy);
	}

	if (pTable)
		pAllyArmy->deserialize(pTable);

	pAllyArmy->init();
	return pAllyArmy;
}

AllyArmy* Scene::getAllyArmy(int id)
{
	return dynamic_cast<AllyArmy*>(findEntity(id, ENTITY_ALLYARMY));
}

void Scene::destroyAllyArmy(int id)
{
	removeEntity(id, ENTITY_ALLYARMY);
}

StaticObj* Scene::createStaticObj(int id, TableCSV* pTable)
{
	StaticObj* pStaticObj = getStaticObj(id);
	if (!pStaticObj)
	{
		pStaticObj = dynamic_cast<StaticObj*>(EntitiesCache::getIns().getOut(ENTITY_STATICOBJ));
		if (!pStaticObj)
			pStaticObj = new StaticObj();

		pStaticObj->_id = id;
		addEntity(pStaticObj);
	}

	if (pTable)
		pStaticObj->deserialize(pTable);

	pStaticObj->init();
	return pStaticObj;
}

StaticObj* Scene::getStaticObj(int id)
{
	return dynamic_cast<StaticObj*>(findEntity(id, ENTITY_STATICOBJ));
}

void Scene::destroyStaticObj(int id)
{
	removeEntity(id, ENTITY_STATICOBJ);
}

void Scene::addEntity(Entity* ent)
{
	int id = ent->getID();
	int type = ent->getType();
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _entities.find(i64ID);
	if (it == _entities.end())
		_entities[i64ID] = ent;
}

void Scene::removeEntity(int id, int type)
{
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _entities.find(i64ID);
	if (it != _entities.end())
	{
		EntitiesCache::getIns().putIn(it->second);
		_entities.erase(it);
	}
}

Entity* Scene::findEntity(int id, int type)
{
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _entities.find(i64ID);
	if (it != _entities.end())
		return it->second;
	return nullptr;
}
