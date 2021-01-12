#include "Battlefield.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>
#include "WarCommander.h"

Battlefield::Battlefield(int id, TableCSV* table)
	: WarSimulator(table->item2str(id, "script_file"))
	, _id(id)
	, _table(table)
{
	int sceneBreed = table->item2int(id, "breed");
	string context = sceneBreed == 0 ? "OsgContextImpl" : "OsgEarthContextImpl";
	_spawner = Spawner::create(id, sceneBreed, "DoF", context, table->item2str(id, "description"));
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
