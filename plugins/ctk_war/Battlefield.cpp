#include "Battlefield.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>
#include "LuaScript.h"
#include "WarCommander.h"

Battlefield::Battlefield(int id, TableCSV* table)
	: _id(id)
	, _table(table)
{
	_spawner = Spawner::create(id, table->item2str(id, "description"));
}

Battlefield::~Battlefield()
{
	Spawner::destroy(_spawner);
}

void Battlefield::stepping()
{
	WarSimulator::stepping();
	_spawner->updateAll();
}

bool Battlefield::load()
{
	string sceneFile = _table->item2str(_id, "scene_file");
	if (sceneFile != "" && _spawner->load(ZOO_DATA_ROOT_DIR + sceneFile))
	{
		_spawner->awakeAll();
		_simStates[ready_].init(_table->item2str(_id, "ready"), this);
		_simStates[running_].init(_table->item2str(_id, "running"), this);
		_simStates[paused_].init(_table->item2str(_id, "paused"), this);
		return true;
	}

	return false;
}

void Battlefield::save()
{
	string sceneFile = _table->item2str(_id, "scene_file");
	if (sceneFile != "")
		_spawner->save(ZOO_DATA_ROOT_DIR + sceneFile);
}

Entity* Battlefield::createArmy(int id, int breed)
{
	return _spawner->born(id, breed);
}

Entity* Battlefield::getArmy(int id, int breed)
{
	return _spawner->gain(id, breed);
}

void Battlefield::destroyArmy(int id, int breed)
{
	Entity* pEnt = getArmy(id, breed);
	if (pEnt)
		_spawner->remove(pEnt);
}
