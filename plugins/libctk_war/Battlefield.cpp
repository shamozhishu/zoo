#include "Battlefield.h"
#include <zoo/DatabaseCSV.h>
#include <zoo/Utils.h>
#include "LuaScript.h"
#include "WarCommander.h"

Battlefield::Battlefield(int id, string description)
	: _id(id)
	, _desc(description)
{
	TableCSV* pMainTable = DatabaseCSV::getSingleton().getMainTable();
	if (pMainTable)
	{
		for (int i = ENTITY_WEAPON; i < ENTITY_COUNT; ++i)
			_spawner[i](i, DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName((ENTITY_TYPE)i))));
	}
	else
	{
		for (int i = ENTITY_WEAPON; i < ENTITY_COUNT; ++i)
			_spawner[i](i);
	}
}

Battlefield::~Battlefield()
{
	Entity::clear();
}

void Battlefield::awake()
{
	for (int i = ENTITY_WEAPON; i < ENTITY_COUNT; ++i)
	{
		_spawner[i].awake();
	}
}

void Battlefield::load()
{
	for (int i = ENTITY_WEAPON; i < ENTITY_COUNT; ++i)
	{
		_spawner[i].load();
	}
}

void Battlefield::save()
{
	TableCSV* pMainTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pMainTable)
		return;

	for (int i = ENTITY_WEAPON; i < ENTITY_COUNT; ++i)
	{
		_spawner[i].save(ZOO_DATA_ROOT_DIR + pMainTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName((ENTITY_TYPE)i)));
	}
}

void Battlefield::execScripts()
{
	auto it = _behaviors.begin();
	auto itEnd = _behaviors.end();
	for (; it != itEnd; ++it)
		(*it)->exec();
}

void Battlefield::buildWeapon(Entity* pEnt)
{
	DoF* dof = pEnt->addComponent<DoF>();
	Model* model = pEnt->addComponent<Model>();
	Sound* sound = pEnt->addComponent<Sound>();
	Animator* animator = pEnt->addComponent<Animator>();
	Behavior* behavior = pEnt->addComponent<Behavior>();

	behavior->getScript()->setVariable("dof", "DoF", dof);
	behavior->getScript()->setVariable("model", "Model", model);
	behavior->getScript()->setVariable("sound", "Sound", sound);
	behavior->getScript()->setVariable("animator", "Animator", animator);
}

void Battlefield::buildEffect(Entity* pEnt)
{
	buildWeapon(pEnt);
}

void Battlefield::buildRedArmy(Entity* pEnt)
{
	buildWeapon(pEnt);
}

void Battlefield::buildBlueArmy(Entity* pEnt)
{
	buildWeapon(pEnt);
}

void Battlefield::buildAllyArmy(Entity* pEnt)
{
	buildWeapon(pEnt);
}

void Battlefield::buildStationary(Entity* pEnt)
{
	buildWeapon(pEnt);
}

void Battlefield::buildWarReporter(Entity* pEnt)
{
	Behavior* behavior = pEnt->addComponent<Behavior>();
	Camera* camera = pEnt->addComponent<Camera>();

	behavior->getScript()->setVariable("camera", "Camera", camera);
}

Entity* Battlefield::createEntity(int id, ENTITY_TYPE entType)
{
	Entity* pEnt = _spawner[entType].new_Entity(id);
	if (!pEnt)
		return nullptr;

	stringstream ss;
	switch (entType)
	{
	case ENTITY_WEAPON: buildWeapon(pEnt); break;
	case ENTITY_EFFECT: buildEffect(pEnt); break;
	case ENTITY_REDARMY: buildRedArmy(pEnt); break;
	case ENTITY_BLUEARMY: buildBlueArmy(pEnt); break;
	case ENTITY_ALLYARMY: buildAllyArmy(pEnt); break;
	case ENTITY_STATIONARY: buildStationary(pEnt); break;
	case ENTITY_WARREPORTER: buildWarReporter(pEnt); break;
	}

	return pEnt;
}

Entity* Battlefield::getEntity(int id, ENTITY_TYPE entType)
{
	return _spawner[entType].find_Entity(id);
}

void Battlefield::destroyEntity(int id, ENTITY_TYPE entType)
{
	Entity* pEnt = getEntity(id, entType);
	if (pEnt)
		Entity::destroy(pEnt);
}
