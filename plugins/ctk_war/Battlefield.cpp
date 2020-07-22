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
	_SimStates[ready_].init(table->item2str(id, "ready"));
	_SimStates[running_].init(table->item2str(id, "running"));
	_SimStates[paused_].init(table->item2str(id, "paused"));
}

Battlefield::~Battlefield()
{
	Spawner::destroy(_spawner);
}

void Battlefield::enter()
{
	string sceneFile = _table->item2str(_id, "scene_file");
	if (sceneFile != "" && _spawner->load(ZOO_DATA_ROOT_DIR + sceneFile))
		_spawner->awakeAll();
}

void Battlefield::save()
{
	string sceneFile = _table->item2str(_id, "scene_file");
	if (sceneFile != "")
		_spawner->save(ZOO_DATA_ROOT_DIR + sceneFile);
}

void Battlefield::exit()
{
	_spawner->clear();
}

void Battlefield::refresh()
{
	auto it = _behaviors.begin();
	auto itEnd = _behaviors.end();
	for (; it != itEnd; ++it)
		(*it)->exec();

	WarSimulator::refresh();
	_spawner->updateAll();
}

void Battlefield::addBehavior(Behavior* behavior)
{
	_behaviors.push_back(behavior);
}

void Battlefield::removeBehavior(Behavior* behavior)
{
	auto it = std::find(_behaviors.begin(), _behaviors.end(), behavior);
	if (it != _behaviors.end())
		_behaviors.erase(it);
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
