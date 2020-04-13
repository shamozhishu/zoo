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

Scene::Scene(int id, string description)
	: _id(id)
	, _desc(description)
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
	std::list<Effect*> effects = _effects;
	{
		auto it = effects.begin();
		auto itEnd = effects.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<Weapon*> weapons = _weapons;
	{
		auto it = weapons.begin();
		auto itEnd = weapons.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<RedArmy*> redArmies = _redArmies;
	{
		auto it = redArmies.begin();
		auto itEnd = redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<BlueArmy*> blueArmies = _blueArmies;
	{
		auto it = blueArmies.begin();
		auto itEnd = blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<AllyArmy*> allyArmies = _allyArmies;
	{
		auto it = allyArmies.begin();
		auto itEnd = allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<StaticObj*> staticObjs = _staticObjs;
	{
		auto it = staticObjs.begin();
		auto itEnd = staticObjs.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}	
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
		_effects.push_back(pEffect);
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
		_weapons.push_back(pWeapon);
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
		_redArmies.push_back(pRedArmy);
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
		_blueArmies.push_back(pBlueArmy);
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
		_allyArmies.push_back(pAllyArmy);
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
		_staticObjs.push_back(pStaticObj);
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
	_entities[i64ID] = ent;
}

void Scene::removeEntity(int id, int type)
{
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _entities.find(i64ID);
	if (it != _entities.end())
	{
		Entity* pEnt = it->second;
		EntitiesCache::getIns().putIn(pEnt);
		_entities.erase(it);

		switch (type)
		{
		case ENTITY_EFFECT:
		{
			auto it = std::find(_effects.begin(), _effects.end(), pEnt);
			if (it != _effects.end())
				_effects.erase(it);
		}
		break;
		case ENTITY_WEAPON:
		{
			auto it = std::find(_weapons.begin(), _weapons.end(), pEnt);
			if (it != _weapons.end())
				_weapons.erase(it);
		}
		break;
		case ENTITY_REDARMY:
		{
			auto it = std::find(_redArmies.begin(), _redArmies.end(), pEnt);
			if (it != _redArmies.end())
				_redArmies.erase(it);
		}
		break;
		case ENTITY_BLUEARMY:
		{
			auto it = std::find(_blueArmies.begin(), _blueArmies.end(), pEnt);
			if (it != _blueArmies.end())
				_blueArmies.erase(it);
		}
		break;
		case ENTITY_ALLYARMY:
		{
			auto it = std::find(_allyArmies.begin(), _allyArmies.end(), pEnt);
			if (it != _allyArmies.end())
				_allyArmies.erase(it);
		}
		break;
		case ENTITY_STATICOBJ:
		{
			auto it = std::find(_staticObjs.begin(), _staticObjs.end(), pEnt);
			if (it != _staticObjs.end())
				_staticObjs.erase(it);
		}
		break;
		}
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
