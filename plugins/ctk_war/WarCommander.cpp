#include "WarCommander.h"
#include "Battlefield.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/Win32Service.h>
#include <toluaInput/Input.h>
#include <QCoreApplication>
#include <QFile>
#include <zoo/Log.h>
#include <zoo/Utils.h>
#include "WarActivator.h"

using namespace zoo;

WarCommander::WarCommander(string cmd, string table)
	: _cmd(cmd)
	, _battlefieldTable(nullptr)
	, _currentBattlefield(nullptr)
{
	new DatabaseCSV();
	_inputDevice = new Input(WarActivator::getService<Win32Service>()->getWnd());
	string csvTablePath = table;
	size_t pos = csvTablePath.find_last_of('/', csvTablePath.length());
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
	SAFE_DELETE(_inputDevice);
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
	if (!_currentBattlefield->load())
	{
		SAFE_DELETE(_currentBattlefield);
		zoo_error("加载场景[%d]失败", id);
		return false;
	}

	return true;
}

void WarCommander::exitCurBattlefield()
{
	if (_currentBattlefield)
	{
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

Input* WarCommander::getInputDevice() const
{
	return _inputDevice;
}

Battlefield* WarCommander::getCurBattlefield() const
{
	return _currentBattlefield;
}

void WarCommander::tick()
{
	_inputDevice->poll();
	if (_currentBattlefield)
		_currentBattlefield->stepping();

	zooCmd_Refresh();
	zooCmd_Render();
}
