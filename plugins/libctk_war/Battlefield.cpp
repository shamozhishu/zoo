#include "Battlefield.h"
#include <zoo/DatabaseCSV.h>
#include <zoo/Utils.h>
#include "Effect.h"
#include "Weapon.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "Stationary.h"
#include "WarReporter.h"
#include "WarCommander.h"
#include "CommonDef.h"

using namespace zoo;

Battlefield::Battlefield(int id, string description)
	: _id(id)
	, _desc(description)
{
}

Battlefield::~Battlefield()
{
	zoo::Entity::clear();
}

void Battlefield::init()
{
	{
		auto it = _weapons.begin();
		auto itEnd = _weapons.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}

	{
		auto it = _effects.begin();
		auto itEnd = _effects.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}

	{
		auto it = _redArmies.begin();
		auto itEnd = _redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}

	{
		auto it = _blueArmies.begin();
		auto itEnd = _blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}

	{
		auto it = _allyArmies.begin();
		auto itEnd = _allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}

	{
		auto it = _stationaries.begin();
		auto itEnd = _stationaries.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}

	{
		auto it = _warReporters.begin();
		auto itEnd = _warReporters.end();
		for (; it != itEnd; ++it)
			(*it)->init();
	}
}

void Battlefield::update()
{
	std::list<Weapon*> weapons = _weapons;
	{
		auto it = weapons.begin();
		auto itEnd = weapons.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}

	std::list<Effect*> effects = _effects;
	{
		auto it = effects.begin();
		auto itEnd = effects.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}

	std::list<RedArmy*> redArmies = _redArmies;
	{
		auto it = redArmies.begin();
		auto itEnd = redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}

	std::list<BlueArmy*> blueArmies = _blueArmies;
	{
		auto it = blueArmies.begin();
		auto itEnd = blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}

	std::list<AllyArmy*> allyArmies = _allyArmies;
	{
		auto it = allyArmies.begin();
		auto itEnd = allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}

	std::list<Stationary*> stationaries = _stationaries;
	{
		auto it = stationaries.begin();
		auto itEnd = stationaries.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}

	std::list<WarReporter*> warReporters = _warReporters;
	{
		auto it = warReporters.begin();
		auto itEnd = warReporters.end();
		for (; it != itEnd; ++it)
			(*it)->update();
	}
}

void Battlefield::serialize(stringstream& ss)
{
	TableCSV* pTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pTable)
		return;

	string filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_WEAPON));
	fstream fout(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
			<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
			<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
			<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
			<< "," << Chs_TableField[PARENT] << "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[MODEL_FILE]
			<< "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
		ss << TableField[ID] << "," << TableField[DESCRIPTION] << "," << TableField[SCRIPT]
			<< "," << TableField[POSX] << "," << TableField[POSY] << TableField[POSZ]
			<< "," << TableField[HEADING] << "," << TableField[PITCH] << TableField[ROLL]
			<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << TableField[SCALEZ]
			<< "," << TableField[PARENT] << "," << TableField[VISIBLE] << "," << TableField[MODEL_FILE]
			<< "," << TableField[SOUND_FILE] << TableField[TRAJ_FILE] << std::endl;

		auto it = _weapons.begin();
		auto itEnd = _weapons.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}

	filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_EFFECT));
	fout.open(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
			<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
			<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
			<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
			<< "," << Chs_TableField[PARENT] << "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[MODEL_FILE]
			<< "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
		ss << TableField[ID] << "," << TableField[DESCRIPTION] << "," << TableField[SCRIPT]
			<< "," << TableField[POSX] << "," << TableField[POSY] << TableField[POSZ]
			<< "," << TableField[HEADING] << "," << TableField[PITCH] << TableField[ROLL]
			<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << TableField[SCALEZ]
			<< "," << TableField[PARENT] << "," << TableField[VISIBLE] << "," << TableField[MODEL_FILE]
			<< "," << TableField[SOUND_FILE] << TableField[TRAJ_FILE] << std::endl;

		auto it = _effects.begin();
		auto itEnd = _effects.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}

	filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_REDARMY));
	fout.open(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
			<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
			<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
			<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
			<< "," << Chs_TableField[PARENT] << "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[MODEL_FILE]
			<< "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
		ss << TableField[ID] << "," << TableField[DESCRIPTION] << "," << TableField[SCRIPT]
			<< "," << TableField[POSX] << "," << TableField[POSY] << TableField[POSZ]
			<< "," << TableField[HEADING] << "," << TableField[PITCH] << TableField[ROLL]
			<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << TableField[SCALEZ]
			<< "," << TableField[PARENT] << "," << TableField[VISIBLE] << "," << TableField[MODEL_FILE]
			<< "," << TableField[SOUND_FILE] << TableField[TRAJ_FILE] << std::endl;

		auto it = _redArmies.begin();
		auto itEnd = _redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}

	filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_BLUEARMY));
	fout.open(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
			<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
			<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
			<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
			<< "," << Chs_TableField[PARENT] << "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[MODEL_FILE]
			<< "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
		ss << TableField[ID] << "," << TableField[DESCRIPTION] << "," << TableField[SCRIPT]
			<< "," << TableField[POSX] << "," << TableField[POSY] << TableField[POSZ]
			<< "," << TableField[HEADING] << "," << TableField[PITCH] << TableField[ROLL]
			<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << TableField[SCALEZ]
			<< "," << TableField[PARENT] << "," << TableField[VISIBLE] << "," << TableField[MODEL_FILE]
			<< "," << TableField[SOUND_FILE] << TableField[TRAJ_FILE] << std::endl;

		auto it = _blueArmies.begin();
		auto itEnd = _blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}

	filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_ALLYARMY));
	fout.open(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
			<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
			<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
			<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
			<< "," << Chs_TableField[PARENT] << "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[MODEL_FILE]
			<< "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
		ss << TableField[ID] << "," << TableField[DESCRIPTION] << "," << TableField[SCRIPT]
			<< "," << TableField[POSX] << "," << TableField[POSY] << TableField[POSZ]
			<< "," << TableField[HEADING] << "," << TableField[PITCH] << TableField[ROLL]
			<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << TableField[SCALEZ]
			<< "," << TableField[PARENT] << "," << TableField[VISIBLE] << "," << TableField[MODEL_FILE]
			<< "," << TableField[SOUND_FILE] << TableField[TRAJ_FILE] << std::endl;

		auto it = _allyArmies.begin();
		auto itEnd = _allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}

	filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_STATIONARY));
	fout.open(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
			<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
			<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
			<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
			<< "," << Chs_TableField[PARENT] << "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[MODEL_FILE]
			<< "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
		ss << TableField[ID] << "," << TableField[DESCRIPTION] << "," << TableField[SCRIPT]
			<< "," << TableField[POSX] << "," << TableField[POSY] << TableField[POSZ]
			<< "," << TableField[HEADING] << "," << TableField[PITCH] << TableField[ROLL]
			<< "," << TableField[SCALEX] << "," << TableField[SCALEY] << TableField[SCALEZ]
			<< "," << TableField[PARENT] << "," << TableField[VISIBLE] << "," << TableField[MODEL_FILE]
			<< "," << TableField[SOUND_FILE] << TableField[TRAJ_FILE] << std::endl;

		auto it = _stationaries.begin();
		auto itEnd = _stationaries.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}

	filename = ZOO_DATA_ROOT_DIR + pTable->item2str(getID(), WarCommander::getSingleton().getEntTypeName(ENTITY_WARREPORTER));
	fout.open(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
	}
	else
	{
		ss << Chs_WarReporter_TableField[ID] << "," << Chs_WarReporter_TableField[DESCRIPTION] << "," << Chs_WarReporter_TableField[SCRIPT]
			<< "," << Chs_WarReporter_TableField[RATIO_LEFT] << "," << Chs_WarReporter_TableField[RATIO_RIGHT]
			<< "," << Chs_WarReporter_TableField[RATIO_BOTTOM] << "," << Chs_WarReporter_TableField[RATIO_TOP]
			<< "," << Chs_WarReporter_TableField[TRACK_ENTITY] << std::endl;
		ss << WarReporter_TableField[ID] << "," << WarReporter_TableField[DESCRIPTION] << "," << WarReporter_TableField[SCRIPT]
			<< "," << WarReporter_TableField[RATIO_LEFT] << "," << WarReporter_TableField[RATIO_RIGHT]
			<< "," << WarReporter_TableField[RATIO_BOTTOM] << "," << WarReporter_TableField[RATIO_TOP]
			<< "," << WarReporter_TableField[TRACK_ENTITY] << std::endl;

		auto it = _warReporters.begin();
		auto itEnd = _warReporters.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);

		fout << ss.str();
		fout.close();
		ss.clear();
		ss.str("");
	}
}

void Battlefield::deserialize(zoo::TableCSV* pTable)
{
	if (!pTable)
		return;

	TableCSV* pMainTable = pTable;

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_WEAPON)));
	if (pTable)
	{
		auto it = _weapons.begin();
		auto itEnd = _weapons.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_EFFECT)));
	if (pTable)
	{
		auto it = _effects.begin();
		auto itEnd = _effects.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_REDARMY)));
	if (pTable)
	{
		auto it = _redArmies.begin();
		auto itEnd = _redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_BLUEARMY)));
	if (pTable)
	{
		auto it = _blueArmies.begin();
		auto itEnd = _blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_ALLYARMY)));
	if (pTable)
	{
		auto it = _allyArmies.begin();
		auto itEnd = _allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_STATIONARY)));
	if (pTable)
	{
		auto it = _stationaries.begin();
		auto itEnd = _stationaries.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(_id, WarCommander::getSingleton().getEntTypeName(ENTITY_WARREPORTER)));
	if (pTable)
	{
		auto it = _warReporters.begin();
		auto itEnd = _warReporters.end();
		for (; it != itEnd; ++it)
			(*it)->deserialize(pTable);
	}
}

Weapon* Battlefield::createWeapon(int id)
{
	Weapon* pWeapon = zoo::Entity::create<Weapon>(id, ENTITY_WEAPON);
	if (!pWeapon)
		return nullptr;
	_weapons.push_back(pWeapon);
	return pWeapon;
}

Weapon* Battlefield::getWeapon(int id)
{
	return zoo::Entity::find<Weapon>(id, ENTITY_WEAPON);
}

void Battlefield::destroyWeapon(int id)
{
	Weapon* pWeapon = getWeapon(id);
	if (pWeapon)
	{
		zoo::Entity::destroy(pWeapon);
		auto it = std::find(_weapons.begin(), _weapons.end(), pWeapon);
		if (it != _weapons.end())
			_weapons.erase(it);
	}
}

Effect* Battlefield::createEffect(int id)
{
	Effect* pEffect = zoo::Entity::create<Effect>(id, ENTITY_EFFECT);
	if (!pEffect)
		return nullptr;
	_effects.push_back(pEffect);
	return pEffect;
}

Effect* Battlefield::getEffect(int id)
{
	return zoo::Entity::find<Effect>(id, ENTITY_EFFECT);
}

void Battlefield::destroyEffect(int id)
{
	Effect* pEffect = getEffect(id);
	if (pEffect)
	{
		zoo::Entity::destroy(pEffect);
		auto it = std::find(_effects.begin(), _effects.end(), pEffect);
		if (it != _effects.end())
			_effects.erase(it);
	}
}

RedArmy* Battlefield::createRedArmy(int id)
{
	RedArmy* pRedArmy = zoo::Entity::create<RedArmy>(id, ENTITY_REDARMY);
	if (!pRedArmy)
		return nullptr;
	_redArmies.push_back(pRedArmy);
	return pRedArmy;
}

RedArmy* Battlefield::getRedArmy(int id)
{
	return zoo::Entity::find<RedArmy>(id, ENTITY_REDARMY);
}

void Battlefield::destroyRedArmy(int id)
{
	RedArmy* pRedArmy = getRedArmy(id);
	if (pRedArmy)
	{
		zoo::Entity::destroy(pRedArmy);
		auto it = std::find(_redArmies.begin(), _redArmies.end(), pRedArmy);
		if (it != _redArmies.end())
			_redArmies.erase(it);
	}
}

BlueArmy* Battlefield::createBlueArmy(int id)
{
	BlueArmy* pBlueArmy = zoo::Entity::create<BlueArmy>(id, ENTITY_BLUEARMY);
	if (!pBlueArmy)
		return nullptr;
	_blueArmies.push_back(pBlueArmy);
	return pBlueArmy;
}

BlueArmy* Battlefield::getBlueArmy(int id)
{
	return zoo::Entity::find<BlueArmy>(id, ENTITY_BLUEARMY);
}

void Battlefield::destroyBlueArmy(int id)
{
	BlueArmy* pBlueArmy = getBlueArmy(id);
	if (pBlueArmy)
	{
		zoo::Entity::destroy(pBlueArmy);
		auto it = std::find(_blueArmies.begin(), _blueArmies.end(), pBlueArmy);
		if (it != _blueArmies.end())
			_blueArmies.erase(it);
	}
}

AllyArmy* Battlefield::createAllyArmy(int id)
{
	AllyArmy* pAllyArmy = zoo::Entity::create<AllyArmy>(id, ENTITY_ALLYARMY);
	if (!pAllyArmy)
		return nullptr;
	_allyArmies.push_back(pAllyArmy);
	return pAllyArmy;
}

AllyArmy* Battlefield::getAllyArmy(int id)
{
	return zoo::Entity::find<AllyArmy>(id, ENTITY_ALLYARMY);
}

void Battlefield::destroyAllyArmy(int id)
{
	AllyArmy* pAllyArmy = getAllyArmy(id);
	if (pAllyArmy)
	{
		zoo::Entity::destroy(pAllyArmy);
		auto it = std::find(_allyArmies.begin(), _allyArmies.end(), pAllyArmy);
		if (it != _allyArmies.end())
			_allyArmies.erase(it);
	}
}

Stationary* Battlefield::createStationary(int id)
{
	Stationary* pStationary = zoo::Entity::create<Stationary>(id, ENTITY_STATIONARY);
	if (!pStationary)
		return nullptr;
	_stationaries.push_back(pStationary);
	return pStationary;
}

Stationary* Battlefield::getStationary(int id)
{
	return zoo::Entity::find<Stationary>(id, ENTITY_STATIONARY);
}

void Battlefield::destroyStationary(int id)
{
	Stationary* pStationary = getStationary(id);
	if (pStationary)
	{
		zoo::Entity::destroy(pStationary);
		auto it = std::find(_stationaries.begin(), _stationaries.end(), pStationary);
		if (it != _stationaries.end())
			_stationaries.erase(it);
	}
}

WarReporter* Battlefield::createWarReporter(int id)
{
	WarReporter* pWarReporter = zoo::Entity::create<WarReporter>(id, ENTITY_WARREPORTER);
	if (!pWarReporter)
		return nullptr;
	_warReporters.push_back(pWarReporter);
	return pWarReporter;
}

WarReporter* Battlefield::getWarReporter(int id)
{
	return zoo::Entity::find<WarReporter>(id, ENTITY_WARREPORTER);
}

void Battlefield::destroyWarReporter(int id)
{
	WarReporter* pWarReporter = getWarReporter(id);
	if (pWarReporter)
	{
		zoo::Entity::destroy(pWarReporter);
		auto it = std::find(_warReporters.begin(), _warReporters.end(), pWarReporter);
		if (it != _warReporters.end())
			_warReporters.erase(it);
	}
}
