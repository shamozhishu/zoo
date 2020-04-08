#include "WarManager.h"
#include "InstanceReleaser.h"
#include <zoo/DatabaseCSV.h>
#include <QCoreApplication>
#include "Scene.h"

using namespace zoo;
WarManager::WarManager(string resPath, string mainTable)
	: _resPath(resPath)
	, _startup(false)
	, _curScene(nullptr)
{
	new zoo::DatabaseCSV();
	zoo::DatabaseCSV::getSingleton().init(_resPath, mainTable);
	connect(&_tickTimer, SIGNAL(timeout()), this, SLOT(tick()));
	_tickTimer.start(20);
}

WarManager::~WarManager()
{
	_tickTimer.stop();
	auto it = _scenes.begin();
	for (; it != _scenes.end(); ++it)
		delete it->second;

	InstanceReleaser::destroyIns();
	delete zoo::DatabaseCSV::getSingletonPtr();
}

const string& WarManager::getResPath() const
{
	return _resPath;
}

bool WarManager::enterScene(int sceneId)
{
	_startup = false;
	auto sceneIt = _scenes.find(sceneId);
	if (sceneIt != _scenes.end())
	{
		_curScene = sceneIt->second;
		_startup = true;
		return true;
	}

	TableCSV* pTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pTable)
		return false;

	string dofXml = pTable->item2str(sceneId, "dof");
	TableCSV* effectTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "effect"));
	TableCSV* weaponTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "weapon"));
	TableCSV* redarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "redarmy"));
	TableCSV* bluearmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "bluearmy"));
	TableCSV* allyarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "allyarmy"));
	TableCSV* staticobjTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "staticobj"));

	if (!effectTable || !weaponTable || !redarmyTable || !bluearmyTable || !allyarmyTable || !staticobjTable)
		return false;

	_curScene = new Scene;
	_scenes.insert(make_pair(sceneId, _curScene));

	const set<string>& effectKeys = effectTable->getMajorKeys();
	auto it = effectKeys.begin();
	for (; it != effectKeys.end(); ++it)
		_curScene->createEffect(atoi((*it).c_str()), effectTable);

	const set<string>& weaponKeys = weaponTable->getMajorKeys();
	it = weaponKeys.begin();
	for (; it != weaponKeys.end(); ++it)
		_curScene->createWeapon(atoi((*it).c_str()), weaponTable);

	const set<string>& redarmyKeys = redarmyTable->getMajorKeys();
	it = redarmyKeys.begin();
	for (; it != redarmyKeys.end(); ++it)
		_curScene->createRedArmy(atoi((*it).c_str()), redarmyTable);

	const set<string>& bluearmyKeys = bluearmyTable->getMajorKeys();
	it = bluearmyKeys.begin();
	for (; it != bluearmyKeys.end(); ++it)
		_curScene->createBlueArmy(atoi((*it).c_str()), bluearmyTable);

	const set<string>& allyarmyKeys = allyarmyTable->getMajorKeys();
	it = allyarmyKeys.begin();
	for (; it != allyarmyKeys.end(); ++it)
		_curScene->createAllyArmy(atoi((*it).c_str()), allyarmyTable);

	const set<string>& staticobjKeys = staticobjTable->getMajorKeys();
	it = staticobjKeys.begin();
	for (; it != staticobjKeys.end(); ++it)
		_curScene->createStaticObj(atoi((*it).c_str()), staticobjTable);

	_startup = true;
	return true;
}

void WarManager::exitScene(int sceneId)
{
	auto sceneIt = _scenes.find(sceneId);
	if (sceneIt != _scenes.end())
	{
		if (sceneIt->second == _curScene)
		{
			_startup = false;
			_curScene = nullptr;
		}

		delete sceneIt->second;
		_scenes.erase(sceneIt);
	}
}

void WarManager::tick()
{
	if (!_startup)
		return;

	if (_curScene)
		_curScene->visit(_tickTimer.interval());
}
