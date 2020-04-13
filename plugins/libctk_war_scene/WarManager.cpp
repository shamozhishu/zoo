#include "WarManager.h"
#include "Scene.h"
#include "Effect.h"
#include "Weapon.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "StaticObj.h"
#include "InstanceReleaser.h"
#include <zoo/DatabaseCSV.h>
#include <QCoreApplication>
#include <QtXml/QDomDocument>
#include <QFile>
#include <zoo/Log.h>
#include <zoo/Utils.h>

using namespace zoo;
WarManager::WarManager(string resPath, string mainTable)
	: _resPath(resPath)
	, _mainTable(mainTable)
	, _curScene(nullptr)
{
	_entNames[ENTITY_EFFECT] = "effect";
	_entNames[ENTITY_WEAPON] = "weapon";
	_entNames[ENTITY_REDARMY] = "redarmy";
	_entNames[ENTITY_BLUEARMY] = "bluearmy";
	_entNames[ENTITY_ALLYARMY] = "allyarmy";
	_entNames[ENTITY_STATICOBJ] = "staticobj";

	new DatabaseCSV();
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
	delete DatabaseCSV::getSingletonPtr();
}

const string& WarManager::getResPath() const
{
	return _resPath;
}

bool WarManager::enterScene(int sceneId)
{
	auto sceneIt = _scenes.find(sceneId);
	if (sceneIt != _scenes.end())
	{
		_curScene = sceneIt->second;
		return true;
	}

	DatabaseCSV::getSingleton().clear();
	DatabaseCSV::getSingleton().init(_resPath, _mainTable);
	TableCSV* pTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pTable)
	{
		zoo_error("场景CSV表不存在");
		return false;
	}

	TableCSV* effectTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "effect"));
	if (!effectTable)
	{
		zoo_error("特效CSV表不存在[场景ID:%d]", sceneId);
		return false;
	}

	TableCSV* weaponTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "weapon"));
	if (!weaponTable)
	{
		zoo_error("武器CSV表不存在[场景ID:%d]", sceneId);
		return false;
	}

	TableCSV* redarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "redarmy"));
	if (!redarmyTable)
	{
		zoo_error("红军CSV表不存在[场景ID:%d]", sceneId);
		return false;
	}

	TableCSV* bluearmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "bluearmy"));
	if (!bluearmyTable)
	{
		zoo_error("蓝军CSV表不存在[场景ID:%d]", sceneId);
		return false;
	}

	TableCSV* allyarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "allyarmy"));
	if (!allyarmyTable)
	{
		zoo_error("盟军CSV表不存在[场景ID:%d]", sceneId);
		return false;
	}

	TableCSV* staticobjTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(sceneId, "staticobj"));
	if (!staticobjTable)
	{
		zoo_error("静物CSV表不存在[场景ID:%d]", sceneId);
		return false;
	}

	string dofFile = _resPath + pTable->item2str(sceneId, "dof");
	QFile xmlFile(QString::fromLocal8Bit(dofFile.c_str()));
	if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		zoo_error("打开[%s]文件失败", dofFile.c_str());
		return false;
	}

	QDomDocument xml;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!xml.setContent(&xmlFile, &errorMsg, &errorLine, &errorColumn))
	{
		zoo_error("解析[%s]文件失败(%s, %d %d)", dofFile.c_str(), errorMsg.toStdString().c_str(), errorLine, errorColumn);
		return false;
	}

	_curScene = new Scene(sceneId, pTable->item2str(sceneId, "description"));
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

	QDomElement rootElem = xml.documentElement();
	QDomElement dofElem = rootElem.firstChildElement();
	while (!dofElem.isNull())
	{
		loadDoF(dofElem, nullptr);
		dofElem = dofElem.nextSiblingElement();
	}

	return true;
}

void WarManager::exitCurScene()
{
	if (_curScene)
	{
		auto sceneIt = _scenes.find(_curScene->getID());
		if (sceneIt != _scenes.end())
		{
			_curScene = nullptr;
			delete sceneIt->second;
			_scenes.erase(sceneIt);
		}
	}
}

void WarManager::saveCurScene()
{
	TableCSV* pTable = DatabaseCSV::getSingleton().getMainTable();
	if (!pTable)
	{
		zoo_error("场景CSV表不存在");
		return;
	}

	if (!_curScene || _curScene->_entities.size() == 0)
	{
		zoo_error("当前场景:%s[id:%d]为空", _curScene->getDesc().c_str(), _curScene->getID());
		return;
	}

	stringstream ss;
	ss << "特效编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << std::endl;
	ss << "id" << "," << "description" << "," << "script" << "," << "model_file" << std::endl;
	writeFile(ENTITY_EFFECT, ss, pTable);

	ss << "武器编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << "id" << "," << "description" << "," << "script" << "," << "model_file" << "," << "traj_file" << std::endl;
	writeFile(ENTITY_WEAPON, ss, pTable);

	ss << "红军编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << "id" << "," << "description" << "," << "script" << "," << "model_file" << "," << "traj_file" << std::endl;
	writeFile(ENTITY_REDARMY, ss, pTable);

	ss << "蓝军编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << "id" << "," << "description" << "," << "script" << "," << "model_file" << "," << "traj_file" << std::endl;
	writeFile(ENTITY_BLUEARMY, ss, pTable);

	ss << "盟军编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << "id" << "," << "description" << "," << "script" << "," << "model_file" << "," << "traj_file" << std::endl;
	writeFile(ENTITY_ALLYARMY, ss, pTable);

	ss << "静物编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << std::endl;
	ss << "id" << "," << "description" << "," << "script" << "," << "model_file" << std::endl;
	writeFile(ENTITY_STATICOBJ, ss, pTable);
}

void WarManager::loadDoF(QDomElement& elem, DoF* parent)
{
	int id = elem.attribute("id").toInt();
	int group = elem.attribute("group").toInt();
	if (group < 0 || group >= ENTITY_COUNT)
	{
		zoo_warning("Entity type does not exist(id:%d,group:%d)", id, group);
		return;
	}

	Entity* ent = _curScene->findEntity(id, group);
	if (!ent)
	{
		zoo_warning("Entity does not exist(id:%d,type:%s)", id, _entNames[group].c_str());
		return;
	}

	DoF* dof = ent->getDoF();
	if (!dof)
	{
		zoo_warning("DoF does not exist(id:%d,group:%d)", id, group);
		return;
	}

	dof->setX(elem.attribute("x").toDouble());
	dof->setY(elem.attribute("y").toDouble());
	dof->setZ(elem.attribute("z").toDouble());
	dof->setH(elem.attribute("h").toFloat());
	dof->setP(elem.attribute("p").toFloat());
	dof->setR(elem.attribute("r").toFloat());
	dof->setScale(elem.attribute("scale").toFloat());
	if (parent)
		parent->addChild(dof);

	QDomElement child = elem.firstChildElement();
	while (!child.isNull())
	{
		loadDoF(child, dof);
		child = child.nextSiblingElement();
	}
}

void WarManager::saveDoF(QDomElement& elem, DoF* parent, QDomDocument& doc)
{

}

void WarManager::writeFile(ENTITY_TYPE entType, stringstream& ss, TableCSV* pTable)
{
	string filename = _resPath + pTable->item2str(_curScene->getID(), _entNames[entType].c_str());
	fstream fout(filename, ios::out | ios::ate);
	if (!fout)
	{
		fout.close();
		zoo_error("打开文件[%s]失败", filename.c_str());
		return;
	}

	switch (entType)
	{
	case ENTITY_EFFECT:
	{
		auto it = _curScene->_effects.begin();
		auto itEnd = _curScene->_effects.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_WEAPON:
	{
		auto it = _curScene->_weapons.begin();
		auto itEnd = _curScene->_weapons.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_REDARMY:
	{
		auto it = _curScene->_redArmies.begin();
		auto itEnd = _curScene->_redArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_BLUEARMY:
	{
		auto it = _curScene->_blueArmies.begin();
		auto itEnd = _curScene->_blueArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_ALLYARMY:
	{
		auto it = _curScene->_allyArmies.begin();
		auto itEnd = _curScene->_allyArmies.end();
		for (; it != itEnd; ++it)
			(*it)->serialize(ss);
	}
	break;
	case ENTITY_STATICOBJ:
	{
		auto it = _curScene->_staticObjs.begin();
		auto itEnd = _curScene->_staticObjs.end();
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

void WarManager::tick()
{
	if (_curScene)
		_curScene->visit(_tickTimer.interval());
}
