#include "WarCommander.h"
#include "Battlefield.h"
#include "Effect.h"
#include "Weapon.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "Stationary.h"
#include "WarReporter.h"
#include "InstanceReleaser.h"
#include <zoo/DatabaseCSV.h>
#include <QCoreApplication>
#include <QFile>
#include <zoo/Log.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include <zooCmdLoader/ZooCmdLoader.h>

using namespace zoo;
WarCommander::WarCommander(string relatedCmd, string resPath, string mainTable)
	: _relatedCmd(relatedCmd)
	, _resPath(resPath)
	, _mainTable(mainTable)
	, _currentBattlefield(nullptr)
{
	_entNames[ENTITY_WEAPON] = "Weapon";
	_entNames[ENTITY_EFFECT] = "Effect";
	_entNames[ENTITY_REDARMY] = "RedArmy";
	_entNames[ENTITY_BLUEARMY] = "BlueArmy";
	_entNames[ENTITY_ALLYARMY] = "AllyArmy";
	_entNames[ENTITY_STATIONARY] = "Stationary";
	_entNames[ENTITY_WARREPORTER] = "WarReporter";

	new DatabaseCSV();
	connect(&_tickTimer, SIGNAL(timeout()), this, SLOT(tick()));
	_tickTimer.start(20);
}

WarCommander::~WarCommander()
{
	_tickTimer.stop();
	auto it = _battlefields.begin();
	for (; it != _battlefields.end(); ++it)
		delete it->second;

	InstanceReleaser::destroyIns();
	delete DatabaseCSV::getSingletonPtr();
}

const string& WarCommander::getResPath() const
{
	return _resPath;
}

bool WarCommander::enterBattlefield(int id)
{
	auto itor = _battlefields.find(id);
	if (itor != _battlefields.end())
	{
		_currentBattlefield = itor->second;
		return true;
	}

	DatabaseCSV::getSingleton().clear();
	DatabaseCSV::getSingleton().init(_resPath, _mainTable);
	TableCSV* pTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pTable)
	{
		zoo_error("战场CSV表不存在");
		return false;
	}

	Battlefield* pBattlefield = new Battlefield(id, pTable->item2str(id, Default_TableField[DESCRIPTION]));

	TableCSV* weaponTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_WEAPON]));
	if (weaponTable)
	{
		const set<string>& weaponKeys = weaponTable->getMajorKeys();
		auto it = weaponKeys.begin();
		for (; it != weaponKeys.end(); ++it)
			pBattlefield->createWeapon(atoi((*it).c_str()), weaponTable);
	}

	TableCSV* effectTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_EFFECT]));
	if (effectTable)
	{
		const set<string>& effectKeys = effectTable->getMajorKeys();
		auto it = effectKeys.begin();
		for (; it != effectKeys.end(); ++it)
			pBattlefield->createEffect(atoi((*it).c_str()), effectTable);
	}

	TableCSV* redarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_REDARMY]));
	if (redarmyTable)
	{
		const set<string>& redarmyKeys = redarmyTable->getMajorKeys();
		auto it = redarmyKeys.begin();
		for (; it != redarmyKeys.end(); ++it)
			pBattlefield->createRedArmy(atoi((*it).c_str()), redarmyTable);
	}

	TableCSV* bluearmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_BLUEARMY]));
	if (bluearmyTable)
	{
		const set<string>& bluearmyKeys = bluearmyTable->getMajorKeys();
		auto it = bluearmyKeys.begin();
		for (; it != bluearmyKeys.end(); ++it)
			pBattlefield->createBlueArmy(atoi((*it).c_str()), bluearmyTable);
	}

	TableCSV* allyarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_ALLYARMY]));
	if (allyarmyTable)
	{
		const set<string>& allyarmyKeys = allyarmyTable->getMajorKeys();
		auto it = allyarmyKeys.begin();
		for (; it != allyarmyKeys.end(); ++it)
			pBattlefield->createAllyArmy(atoi((*it).c_str()), allyarmyTable);
	}

	TableCSV* stationaryTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_STATIONARY]));
	if (stationaryTable)
	{
		const set<string>& stationaryKeys = stationaryTable->getMajorKeys();
		auto it = stationaryKeys.begin();
		for (; it != stationaryKeys.end(); ++it)
			pBattlefield->createStationary(atoi((*it).c_str()), stationaryTable);
	}

	TableCSV* warReporterTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_WARREPORTER]));
	if (warReporterTable)
	{
	}

	_currentBattlefield = pBattlefield;
	_battlefields.insert(make_pair(id, _currentBattlefield));
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "enter_battlefield()");
	return true;
}

void WarCommander::exitCurBattlefield()
{
	if (_currentBattlefield)
	{
		auto itor = _battlefields.find(_currentBattlefield->getID());
		if (itor != _battlefields.end())
		{
			_currentBattlefield = nullptr;
			delete itor->second;
			_battlefields.erase(itor);
		}
	}
}

void WarCommander::saveCurBattlefield()
{
	TableCSV* pTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pTable)
	{
		zoo_error("战场CSV表不存在");
		return;
	}

	if (!_currentBattlefield || _currentBattlefield->_entities.size() == 0)
	{
		zoo_error("当前战场:%s[id:%d]为空", _currentBattlefield->getDesc().c_str(), _currentBattlefield->getID());
		return;
	}

	stringstream ss;
	ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
		<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
		<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
		<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
		<< "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[PARENT] << Chs_TableField[BRANCH_TAG]
		<< "," << Chs_TableField[MODEL_FILE] << "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
	ss << Weapon_TableField[ID] << "," << Weapon_TableField[DESCRIPTION] << "," << Weapon_TableField[SCRIPT]
		<< "," << Weapon_TableField[POSX] << "," << Weapon_TableField[POSY] << Weapon_TableField[POSZ]
		<< "," << Weapon_TableField[HEADING] << "," << Weapon_TableField[PITCH] << Weapon_TableField[ROLL]
		<< "," << Weapon_TableField[SCALEX] << "," << Weapon_TableField[SCALEY] << Weapon_TableField[SCALEZ]
		<< "," << Weapon_TableField[VISIBLE] << "," << Weapon_TableField[PARENT] << Weapon_TableField[BRANCH_TAG]
		<< "," << Weapon_TableField[MODEL_FILE] << "," << Weapon_TableField[SOUND_FILE] << Weapon_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_WEAPON, ss, pTable);

	ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
		<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
		<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
		<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
		<< "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[PARENT] << Chs_TableField[BRANCH_TAG]
		<< "," << Chs_TableField[MODEL_FILE] << "," << Chs_TableField[SOUND_FILE] << std::endl;
	ss << Effect_TableField[ID] << "," << Effect_TableField[DESCRIPTION] << "," << Effect_TableField[SCRIPT]
		<< "," << Effect_TableField[POSX] << "," << Effect_TableField[POSY] << Effect_TableField[POSZ]
		<< "," << Effect_TableField[HEADING] << "," << Effect_TableField[PITCH] << Effect_TableField[ROLL]
		<< "," << Effect_TableField[SCALEX] << "," << Effect_TableField[SCALEY] << Effect_TableField[SCALEZ]
		<< "," << Effect_TableField[VISIBLE] << "," << Effect_TableField[PARENT] << Effect_TableField[BRANCH_TAG]
		<< "," << Effect_TableField[MODEL_FILE] << "," << Effect_TableField[SOUND_FILE] << std::endl;
	writeFile(ENTITY_EFFECT, ss, pTable);

	ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
		<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
		<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
		<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
		<< "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[PARENT] << Chs_TableField[BRANCH_TAG]
		<< "," << Chs_TableField[MODEL_FILE] << "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
	ss << RedArmy_TableField[ID] << "," << RedArmy_TableField[DESCRIPTION] << "," << RedArmy_TableField[SCRIPT]
		<< "," << RedArmy_TableField[POSX] << "," << RedArmy_TableField[POSY] << RedArmy_TableField[POSZ]
		<< "," << RedArmy_TableField[HEADING] << "," << RedArmy_TableField[PITCH] << RedArmy_TableField[ROLL]
		<< "," << RedArmy_TableField[SCALEX] << "," << RedArmy_TableField[SCALEY] << RedArmy_TableField[SCALEZ]
		<< "," << RedArmy_TableField[VISIBLE] << "," << RedArmy_TableField[PARENT] << RedArmy_TableField[BRANCH_TAG]
		<< "," << RedArmy_TableField[MODEL_FILE] << "," << RedArmy_TableField[SOUND_FILE] << RedArmy_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_REDARMY, ss, pTable);

	ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
		<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
		<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
		<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
		<< "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[PARENT] << Chs_TableField[BRANCH_TAG]
		<< "," << Chs_TableField[MODEL_FILE] << "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
	ss << BlueArmy_TableField[ID] << "," << BlueArmy_TableField[DESCRIPTION] << "," << BlueArmy_TableField[SCRIPT]
		<< "," << BlueArmy_TableField[POSX] << "," << BlueArmy_TableField[POSY] << BlueArmy_TableField[POSZ]
		<< "," << BlueArmy_TableField[HEADING] << "," << BlueArmy_TableField[PITCH] << BlueArmy_TableField[ROLL]
		<< "," << BlueArmy_TableField[SCALEX] << "," << BlueArmy_TableField[SCALEY] << BlueArmy_TableField[SCALEZ]
		<< "," << BlueArmy_TableField[VISIBLE] << "," << BlueArmy_TableField[PARENT] << BlueArmy_TableField[BRANCH_TAG]
		<< "," << BlueArmy_TableField[MODEL_FILE] << "," << BlueArmy_TableField[SOUND_FILE] << BlueArmy_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_BLUEARMY, ss, pTable);

	ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
		<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
		<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
		<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
		<< "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[PARENT] << Chs_TableField[BRANCH_TAG]
		<< "," << Chs_TableField[MODEL_FILE] << "," << Chs_TableField[SOUND_FILE] << Chs_TableField[TRAJ_FILE] << std::endl;
	ss << AllyArmy_TableField[ID] << "," << AllyArmy_TableField[DESCRIPTION] << "," << AllyArmy_TableField[SCRIPT]
		<< "," << AllyArmy_TableField[POSX] << "," << AllyArmy_TableField[POSY] << AllyArmy_TableField[POSZ]
		<< "," << AllyArmy_TableField[HEADING] << "," << AllyArmy_TableField[PITCH] << AllyArmy_TableField[ROLL]
		<< "," << AllyArmy_TableField[SCALEX] << "," << AllyArmy_TableField[SCALEY] << AllyArmy_TableField[SCALEZ]
		<< "," << AllyArmy_TableField[VISIBLE] << "," << AllyArmy_TableField[PARENT] << AllyArmy_TableField[BRANCH_TAG]
		<< "," << AllyArmy_TableField[MODEL_FILE] << "," << AllyArmy_TableField[SOUND_FILE] << AllyArmy_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_ALLYARMY, ss, pTable);

	ss << Chs_TableField[ID] << "," << Chs_TableField[DESCRIPTION] << "," << Chs_TableField[SCRIPT]
		<< "," << Chs_TableField[POSX] << "," << Chs_TableField[POSY] << Chs_TableField[POSZ]
		<< "," << Chs_TableField[HEADING] << "," << Chs_TableField[PITCH] << Chs_TableField[ROLL]
		<< "," << Chs_TableField[SCALEX] << "," << Chs_TableField[SCALEY] << Chs_TableField[SCALEZ]
		<< "," << Chs_TableField[VISIBLE] << "," << Chs_TableField[PARENT] << Chs_TableField[BRANCH_TAG]
		<< "," << Chs_TableField[MODEL_FILE] << "," << Chs_TableField[SOUND_FILE] << std::endl;
	ss << Stationary_TableField[ID] << "," << Stationary_TableField[DESCRIPTION] << "," << Stationary_TableField[SCRIPT]
		<< "," << Stationary_TableField[POSX] << "," << Stationary_TableField[POSY] << Stationary_TableField[POSZ]
		<< "," << Stationary_TableField[HEADING] << "," << Stationary_TableField[PITCH] << Stationary_TableField[ROLL]
		<< "," << Stationary_TableField[SCALEX] << "," << Stationary_TableField[SCALEY] << Stationary_TableField[SCALEZ]
		<< "," << Stationary_TableField[VISIBLE] << "," << Stationary_TableField[PARENT] << Stationary_TableField[BRANCH_TAG]
		<< "," << Stationary_TableField[MODEL_FILE] << "," << Stationary_TableField[SOUND_FILE] << std::endl;
	writeFile(ENTITY_STATIONARY, ss, pTable);

	ss << Chs_WarReporter_TableField[ID] << "," << Chs_WarReporter_TableField[DESCRIPTION] << "," << Chs_WarReporter_TableField[SCRIPT]
		<< "," << Chs_WarReporter_TableField[RATIO_LEFT] << "," << Chs_WarReporter_TableField[RATIO_RIGHT]
		<< "," << Chs_WarReporter_TableField[RATIO_BOTTOM] << "," << Chs_WarReporter_TableField[RATIO_TOP]
		<< "," << Chs_WarReporter_TableField[TRACK_ENTITY] << std::endl;
	ss << WarReporter_TableField[ID] << "," << WarReporter_TableField[DESCRIPTION] << "," << WarReporter_TableField[SCRIPT]
		<< "," << WarReporter_TableField[RATIO_LEFT] << "," << WarReporter_TableField[RATIO_RIGHT]
		<< "," << WarReporter_TableField[RATIO_BOTTOM] << "," << WarReporter_TableField[RATIO_TOP]
		<< "," << WarReporter_TableField[TRACK_ENTITY] << std::endl;
	writeFile(ENTITY_WARREPORTER, ss, pTable);
}

const char* WarCommander::getRelatedCmd() const
{
	return _relatedCmd.c_str();
}

const char* WarCommander::getEntTypeName(ENTITY_TYPE entType) const
{
	return _entNames[entType];
}

void WarCommander::writeFile(ENTITY_TYPE entType, stringstream& ss, TableCSV* pTable)
{
	string filename = _resPath + pTable->item2str(_currentBattlefield->getID(), _entNames[entType]);
	fstream fout(filename, ios::out | ios::ate);
	if (!fout)
	{
		fout.close();
		zoo_error("打开文件[%s]失败", filename.c_str());
		return;
	}

	switch (entType)
	{
	case ENTITY_WEAPON:
	{
		auto it = _currentBattlefield->_weapons.begin();
		auto itEnd = _currentBattlefield->_weapons.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_EFFECT:
	{
		auto it = _currentBattlefield->_effects.begin();
		auto itEnd = _currentBattlefield->_effects.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_REDARMY:
	{
		auto it = _currentBattlefield->_redArmies.begin();
		auto itEnd = _currentBattlefield->_redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_BLUEARMY:
	{
		auto it = _currentBattlefield->_blueArmies.begin();
		auto itEnd = _currentBattlefield->_blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_ALLYARMY:
	{
		auto it = _currentBattlefield->_allyArmies.begin();
		auto itEnd = _currentBattlefield->_allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_STATIONARY:
	{
		auto it = _currentBattlefield->_stationaries.begin();
		auto itEnd = _currentBattlefield->_stationaries.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_WARREPORTER:
	{
		auto it = _currentBattlefield->_warReporters.begin();
		auto itEnd = _currentBattlefield->_warReporters.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	}

	fout << ss.str();
	fout.close();
	ss.clear();
	ss.str("");
}

void WarCommander::tick()
{
	if (_currentBattlefield)
		_currentBattlefield->visit(_tickTimer.interval());
}
