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

WarCommander::WarCommander(string cmd, string table)
	: _cmd(cmd)
	, _battlefieldTable(nullptr)
	, _currentBattlefield(nullptr)
{
	new DatabaseCSV();
	string csvTablePath = table;
	size_t pos = csvTablePath.find_last_of(L'/', csvTablePath.length());
	csvTablePath = csvTablePath.substr(0, pos + 1);
	DatabaseCSV::getSingleton().init(csvTablePath);
	_battlefieldTable = DatabaseCSV::getSingleton().getTable(table);
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

const char* WarCommander::getCmd() const
{
	return _cmd.c_str();
}

bool WarCommander::enterBattlefield(int id)
{
	if (_currentBattlefield && _currentBattlefield->getID() == id)
		return true;

	if (!_battlefieldTable)
	{
		zoo_error("战场CSV表不存在");
		return false;
	}

	SAFE_DELETE(_currentBattlefield);
	_currentBattlefield = new Battlefield(id, _battlefieldTable);
	_currentBattlefield->enter();

	zooCmdL_Send(WarCommander::getSingleton().getCmd(), "enter_battlefield()");
	return true;
}

void WarCommander::exitCurBattlefield()
{
	if (_currentBattlefield)
	{
		_currentBattlefield->exit();
		delete _currentBattlefield;
		_currentBattlefield = nullptr;
	}
}

void WarCommander::saveCurBattlefield()
{
	if (!_currentBattlefield)
	{
		zoo_warning("当前没有要保存的战场");
		return;
	}

	_currentBattlefield->save();
}

Battlefield* WarCommander::getCurBattlefield()
{
	return _currentBattlefield;
}

void WarCommander::tick()
{
	if (_currentBattlefield)
		_currentBattlefield->stepping();
}
