#include "WarCommander.h"
#include "Battlefield.h"
#include "CommonDef.h"
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
#include <zooCmdLoader/ZooCmdLoader.h>

using namespace zoo;
WarCommander::WarCommander(string relatedCmd, string mainTable)
	: _relatedCmd(relatedCmd)
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

bool WarCommander::enterBattlefield(int id)
{
	auto itor = _battlefields.find(id);
	if (itor != _battlefields.end())
	{
		_currentBattlefield = itor->second;
		return true;
	}

	DatabaseCSV::getSingleton().clear();
	DatabaseCSV::getSingleton().init(ZOO_DATA_ROOT_DIR, _mainTable);
	TableCSV* pMainTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pMainTable)
	{
		zoo_error("战场CSV表不存在");
		return false;
	}

	_SimStates[Ready_].init(pMainTable->item2str(id, "Ready"));
	_SimStates[Running_].init(pMainTable->item2str(id, "Running"));
	_SimStates[Paused_].init(pMainTable->item2str(id, "Paused"));
	_SimStates[FinishedSuccess_].init(pMainTable->item2str(id, "FinishedSuccess"));
	_SimStates[FinishedFailed_].init(pMainTable->item2str(id, "FinishedFailed"));

	Battlefield* pBattlefield = new Battlefield(id, pMainTable->item2str(id, TableField[DESCRIPTION]));

	TableCSV* pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_WEAPON]));
	if (pTable)
	{
		const set<string>& weaponKeys = pTable->getMajorKeys();
		auto it = weaponKeys.begin();
		for (; it != weaponKeys.end(); ++it)
			pBattlefield->createWeapon(atoi((*it).c_str()));
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_EFFECT]));
	if (pTable)
	{
		const set<string>& effectKeys = pTable->getMajorKeys();
		auto it = effectKeys.begin();
		for (; it != effectKeys.end(); ++it)
			pBattlefield->createEffect(atoi((*it).c_str()));
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_REDARMY]));
	if (pTable)
	{
		const set<string>& redarmyKeys = pTable->getMajorKeys();
		auto it = redarmyKeys.begin();
		for (; it != redarmyKeys.end(); ++it)
			pBattlefield->createRedArmy(atoi((*it).c_str()));
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_BLUEARMY]));
	if (pTable)
	{
		const set<string>& bluearmyKeys = pTable->getMajorKeys();
		auto it = bluearmyKeys.begin();
		for (; it != bluearmyKeys.end(); ++it)
			pBattlefield->createBlueArmy(atoi((*it).c_str()));
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_ALLYARMY]));
	if (pTable)
	{
		const set<string>& allyarmyKeys = pTable->getMajorKeys();
		auto it = allyarmyKeys.begin();
		for (; it != allyarmyKeys.end(); ++it)
			pBattlefield->createAllyArmy(atoi((*it).c_str()));
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_STATIONARY]));
	if (pTable)
	{
		const set<string>& stationaryKeys = pTable->getMajorKeys();
		auto it = stationaryKeys.begin();
		for (; it != stationaryKeys.end(); ++it)
			pBattlefield->createStationary(atoi((*it).c_str()));
	}

	pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[ENTITY_WARREPORTER]));
	if (pTable)
	{
		const set<string>& warReporterKeys = pTable->getMajorKeys();
		auto it = warReporterKeys.begin();
		for (; it != warReporterKeys.end(); ++it)
			pBattlefield->createWarReporter(atoi((*it).c_str()));
	}

	pBattlefield->deserialize(pMainTable);
	pBattlefield->init();
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
	if (!_currentBattlefield)
	{
		zoo_error("当前没有要保存的战场");
		return;
	}

	zoo_debug("保存当前战场:%s[id:%d]", _currentBattlefield->getDesc().c_str(), _currentBattlefield->getID());
	stringstream ss;
	_currentBattlefield->serialize(ss);
}

const char* WarCommander::getRelatedCmd() const
{
	return _relatedCmd.c_str();
}

const char* WarCommander::getEntTypeName(ENTITY_TYPE entType) const
{
	return _entNames[entType];
}

void WarCommander::tick()
{
	if (_currentBattlefield)
		_currentBattlefield->update();

	WarSimulator::update();
}
