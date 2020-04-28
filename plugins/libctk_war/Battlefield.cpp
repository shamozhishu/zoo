#include "Battlefield.h"
#include "EntDoF.h"
#include "Effect.h"
#include "Weapon.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "Stationary.h"
#include "WarReporter.h"
#include "EntitiesCache.h"
#include <zoo/DatabaseCSV.h>

using namespace zoo;

Battlefield::Battlefield(int id, string description)
	: _id(id)
	, _desc(description)
{
}

Battlefield::~Battlefield()
{
	auto it = _entities.begin();
	for (; it != _entities.end(); ++it)
		delete it->second;
}

void Battlefield::visit(float dt)
{
	std::list<Weapon*> weapons = _weapons;
	{
		auto it = weapons.begin();
		auto itEnd = weapons.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<Effect*> effects = _effects;
	{
		auto it = effects.begin();
		auto itEnd = effects.end();
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

	std::list<Stationary*> stationaries = _stationaries;
	{
		auto it = stationaries.begin();
		auto itEnd = stationaries.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}

	std::list<WarReporter*> warReporters = _warReporters;
	{
		auto it = warReporters.begin();
		auto itEnd = warReporters.end();
		for (; it != itEnd; ++it)
			(*it)->update(dt);
	}
}

Weapon* Battlefield::createWeapon(int id, TableCSV* pTable)
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

	SAFE_DELETE(pWeapon->_dof);
	pWeapon->_dof = new EntDoF(id, ENTITY_WEAPON);
	if (pTable)
		pWeapon->deserialize(pTable);

	pWeapon->init();
	return pWeapon;
}

Weapon* Battlefield::getWeapon(int id)
{
	return dynamic_cast<Weapon*>(findEntity(id, ENTITY_WEAPON));
}

void Battlefield::destroyWeapon(int id)
{
	removeEntity(id, ENTITY_WEAPON);
}

Effect* Battlefield::createEffect(int id, TableCSV* pTable)
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

	SAFE_DELETE(pEffect->_dof);
	pEffect->_dof = new EntDoF(id, ENTITY_EFFECT);
	if (pTable)
		pEffect->deserialize(pTable);

	pEffect->init();
	return pEffect;
}

Effect* Battlefield::getEffect(int id)
{
	return dynamic_cast<Effect*>(findEntity(id, ENTITY_EFFECT));
}

void Battlefield::destroyEffect(int id)
{
	removeEntity(id, ENTITY_EFFECT);
}

RedArmy* Battlefield::createRedArmy(int id, TableCSV* pTable)
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

	SAFE_DELETE(pRedArmy->_dof);
	pRedArmy->_dof = new EntDoF(id, ENTITY_REDARMY);
	if (pTable)
		pRedArmy->deserialize(pTable);

	pRedArmy->init();
	return pRedArmy;
}

RedArmy* Battlefield::getRedArmy(int id)
{
	return dynamic_cast<RedArmy*>(findEntity(id, ENTITY_REDARMY));
}

void Battlefield::destroyRedArmy(int id)
{
	removeEntity(id, ENTITY_REDARMY);
}

BlueArmy* Battlefield::createBlueArmy(int id, TableCSV* pTable)
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

	SAFE_DELETE(pBlueArmy->_dof);
	pBlueArmy->_dof = new EntDoF(id, ENTITY_BLUEARMY);
	if (pTable)
		pBlueArmy->deserialize(pTable);

	pBlueArmy->init();
	return pBlueArmy;
}

BlueArmy* Battlefield::getBlueArmy(int id)
{
	return dynamic_cast<BlueArmy*>(findEntity(id, ENTITY_BLUEARMY));
}

void Battlefield::destroyBlueArmy(int id)
{
	removeEntity(id, ENTITY_BLUEARMY);
}

AllyArmy* Battlefield::createAllyArmy(int id, TableCSV* pTable)
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

	SAFE_DELETE(pAllyArmy->_dof);
	pAllyArmy->_dof = new EntDoF(id, ENTITY_ALLYARMY);
	if (pTable)
		pAllyArmy->deserialize(pTable);

	pAllyArmy->init();
	return pAllyArmy;
}

AllyArmy* Battlefield::getAllyArmy(int id)
{
	return dynamic_cast<AllyArmy*>(findEntity(id, ENTITY_ALLYARMY));
}

void Battlefield::destroyAllyArmy(int id)
{
	removeEntity(id, ENTITY_ALLYARMY);
}

Stationary* Battlefield::createStationary(int id, TableCSV* pTable)
{
	Stationary* pStationary = getStationary(id);
	if (!pStationary)
	{
		pStationary = dynamic_cast<Stationary*>(EntitiesCache::getIns().getOut(ENTITY_STATIONARY));
		if (!pStationary)
			pStationary = new Stationary();

		pStationary->_id = id;
		addEntity(pStationary);
		_stationaries.push_back(pStationary);
	}

	SAFE_DELETE(pStationary->_dof);
	pStationary->_dof = new EntDoF(id, ENTITY_STATIONARY);
	if (pTable)
		pStationary->deserialize(pTable);

	pStationary->init();
	return pStationary;
}

Stationary* Battlefield::getStationary(int id)
{
	return dynamic_cast<Stationary*>(findEntity(id, ENTITY_STATIONARY));
}

void Battlefield::destroyStationary(int id)
{
	removeEntity(id, ENTITY_STATIONARY);
}

WarReporter* Battlefield::createWarReporter(int id, zoo::TableCSV* pTable)
{
	WarReporter* pWarReporter = getWarReporter(id);
	if (!pWarReporter)
	{
		pWarReporter = dynamic_cast<WarReporter*>(EntitiesCache::getIns().getOut(ENTITY_WARREPORTER));
		if (!pWarReporter)
			pWarReporter = new WarReporter();

		pWarReporter->_id = id;
		addEntity(pWarReporter);
		_warReporters.push_back(pWarReporter);
	}

	SAFE_DELETE(pWarReporter->_dof);
	pWarReporter->_dof = new EntDoF(id, ENTITY_WARREPORTER);
	if (pTable)
		pWarReporter->deserialize(pTable);

	pWarReporter->init();
	return pWarReporter;
}

WarReporter* Battlefield::getWarReporter(int id)
{
	return dynamic_cast<WarReporter*>(findEntity(id, ENTITY_WARREPORTER));
}

void Battlefield::destroyWarReporter(int id)
{
	removeEntity(id, ENTITY_WARREPORTER);
}

void Battlefield::addEntity(Entity* ent)
{
	int id = ent->getID();
	int type = ent->getType();
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	_entities[i64ID] = ent;
}

void Battlefield::removeEntity(int id, int type)
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
		case ENTITY_WEAPON:
		{
			auto it = std::find(_weapons.begin(), _weapons.end(), pEnt);
			if (it != _weapons.end())
				_weapons.erase(it);
		}
		break;
		case ENTITY_EFFECT:
		{
			auto it = std::find(_effects.begin(), _effects.end(), pEnt);
			if (it != _effects.end())
				_effects.erase(it);
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
		case ENTITY_STATIONARY:
		{
			auto it = std::find(_stationaries.begin(), _stationaries.end(), pEnt);
			if (it != _stationaries.end())
				_stationaries.erase(it);
		}
		break;
		case ENTITY_WARREPORTER:
		{
			auto it = std::find(_warReporters.begin(), _warReporters.end(), pEnt);
			if (it != _warReporters.end())
				_warReporters.erase(it);
		}
		break;
		}
	}
}

Entity* Battlefield::findEntity(int id, int type)
{
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _entities.find(i64ID);
	if (it != _entities.end())
		return it->second;
	return nullptr;
}
