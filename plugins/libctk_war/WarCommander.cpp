#include "WarCommander.h"
#include "Battlefield.h"
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
	SAFE_DELETE(_currentBattlefield);
	InstanceReleaser::destroyIns();
	delete DatabaseCSV::getSingletonPtr();
}

bool WarCommander::enterBattlefield(int id)
{
	if (_currentBattlefield && _currentBattlefield->getID() == id)
		return true;

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

	SAFE_DELETE(_currentBattlefield);
	_currentBattlefield = new Battlefield(id, pMainTable->item2str(id, "description"));

	for (int i = ENTITY_WEAPON; i < ENTITY_COUNT; ++i)
	{
		TableCSV* pTable = DatabaseCSV::getSingleton().getTable(ZOO_DATA_ROOT_DIR + pMainTable->item2str(id, _entNames[i]));
		if (pTable)
		{
			auto keys = pTable->getMajorKeys();
			for (auto it = keys.begin(); it != keys.end(); ++it)
				_currentBattlefield->createEntity(atoi((*it).c_str()), (ENTITY_TYPE)i);
		}
	}

	_currentBattlefield->load();
	_currentBattlefield->awake();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "enter_battlefield()");
	return true;
}

void WarCommander::exitCurBattlefield()
{
	SAFE_DELETE(_currentBattlefield);
}

void WarCommander::saveCurBattlefield()
{
	if (!_currentBattlefield)
	{
		zoo_error("当前没有要保存的战场");
		return;
	}

	zoo_debug("保存当前战场:%s[id:%d]", _currentBattlefield->getDesc().c_str(), _currentBattlefield->getID());
	_currentBattlefield->save();
}

Battlefield* WarCommander::getCurBattlefield()
{
	return _currentBattlefield;
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
		_currentBattlefield->execScripts();

	WarSimulator::update();
}
