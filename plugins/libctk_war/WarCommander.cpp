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
#include <QtXml/QDomDocument>
#include <QFile>
#include <zoo/Log.h>
#include <zoo/Utils.h>
#include "CommonDef.h"

using namespace zoo;
WarCommander::WarCommander(string resPath, string mainTable)
	: _resPath(resPath)
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

	TableCSV* weaponTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_WEAPON]));
	if (!weaponTable)
	{
		zoo_error("武器CSV表不存在[战场编号:%d]", id);
		return false;
	}

	TableCSV* effectTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_EFFECT]));
	if (!effectTable)
	{
		zoo_error("特效CSV表不存在[战场编号:%d]", id);
		return false;
	}

	TableCSV* redarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_REDARMY]));
	if (!redarmyTable)
	{
		zoo_error("红军CSV表不存在[战场编号:%d]", id);
		return false;
	}

	TableCSV* bluearmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_BLUEARMY]));
	if (!bluearmyTable)
	{
		zoo_error("蓝军CSV表不存在[战场编号:%d]", id);
		return false;
	}

	TableCSV* allyarmyTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_ALLYARMY]));
	if (!allyarmyTable)
	{
		zoo_error("盟军CSV表不存在[战场编号:%d]", id);
		return false;
	}

	TableCSV* stationaryTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_STATIONARY]));
	if (!stationaryTable)
	{
		zoo_error("静物CSV表不存在[战场编号:%d]", id);
		return false;
	}

	TableCSV* warReporterTable = DatabaseCSV::getSingleton().getTable(_resPath + pTable->item2str(id, _entNames[ENTITY_WARREPORTER]));
	if (!warReporterTable)
	{
		zoo_error("战地记者CSV表不存在[战场编号:%d]", id);
		return false;
	}

	string dofFile = _resPath + pTable->item2str(id, "DoF");
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

	Battlefield* pBattlefield = new Battlefield(id, pTable->item2str(id, Default_TableField[DESCRIPTION]));

	const set<string>& weaponKeys = weaponTable->getMajorKeys();
	auto it = weaponKeys.begin();
	for (; it != weaponKeys.end(); ++it)
		pBattlefield->createWeapon(atoi((*it).c_str()), weaponTable);

	const set<string>& effectKeys = effectTable->getMajorKeys();
	it = effectKeys.begin();
	for (; it != effectKeys.end(); ++it)
		pBattlefield->createEffect(atoi((*it).c_str()), effectTable);

	const set<string>& redarmyKeys = redarmyTable->getMajorKeys();
	it = redarmyKeys.begin();
	for (; it != redarmyKeys.end(); ++it)
		pBattlefield->createRedArmy(atoi((*it).c_str()), redarmyTable);

	const set<string>& bluearmyKeys = bluearmyTable->getMajorKeys();
	it = bluearmyKeys.begin();
	for (; it != bluearmyKeys.end(); ++it)
		pBattlefield->createBlueArmy(atoi((*it).c_str()), bluearmyTable);

	const set<string>& allyarmyKeys = allyarmyTable->getMajorKeys();
	it = allyarmyKeys.begin();
	for (; it != allyarmyKeys.end(); ++it)
		pBattlefield->createAllyArmy(atoi((*it).c_str()), allyarmyTable);

	const set<string>& stationaryKeys = stationaryTable->getMajorKeys();
	it = stationaryKeys.begin();
	for (; it != stationaryKeys.end(); ++it)
		pBattlefield->createStationary(atoi((*it).c_str()), stationaryTable);

	_currentBattlefield = pBattlefield;
	_battlefields.insert(make_pair(id, _currentBattlefield));

	QDomElement rootElem = xml.documentElement();
	QDomElement dofElem = rootElem.firstChildElement();
	while (!dofElem.isNull())
	{
		loadDoF(dofElem, nullptr);
		dofElem = dofElem.nextSiblingElement();
	}

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
	ss << "武器编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << Weapon_TableField[ID] << "," << Weapon_TableField[DESCRIPTION] << "," << Weapon_TableField[SCRIPT] << "," << Weapon_TableField[MODEL_FILE] << "," << Weapon_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_WEAPON, ss, pTable);

	ss << "特效编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << std::endl;
	ss << Effect_TableField[ID] << "," << Effect_TableField[DESCRIPTION] << "," << Effect_TableField[SCRIPT] << "," << Effect_TableField[MODEL_FILE] << std::endl;
	writeFile(ENTITY_EFFECT, ss, pTable);

	ss << "红军编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << RedArmy_TableField[ID] << "," << RedArmy_TableField[DESCRIPTION] << "," << RedArmy_TableField[SCRIPT] << "," << RedArmy_TableField[MODEL_FILE] << "," << RedArmy_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_REDARMY, ss, pTable);

	ss << "蓝军编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << BlueArmy_TableField[ID] << "," << BlueArmy_TableField[DESCRIPTION] << "," << BlueArmy_TableField[SCRIPT] << "," << BlueArmy_TableField[MODEL_FILE] << "," << BlueArmy_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_BLUEARMY, ss, pTable);

	ss << "盟军编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << "," << "轨迹文件" << std::endl;
	ss << AllyArmy_TableField[ID] << "," << AllyArmy_TableField[DESCRIPTION] << "," << AllyArmy_TableField[SCRIPT] << "," << AllyArmy_TableField[MODEL_FILE] << "," << AllyArmy_TableField[TRAJ_FILE] << std::endl;
	writeFile(ENTITY_ALLYARMY, ss, pTable);

	ss << "静物编号" << "," << "描述" << "," << "脚本" << "," << "模型文件" << std::endl;
	ss << Stationary_TableField[ID] << "," << Stationary_TableField[DESCRIPTION] << "," << Stationary_TableField[SCRIPT] << "," << Stationary_TableField[MODEL_FILE] << std::endl;
	writeFile(ENTITY_STATIONARY, ss, pTable);

	ss << "战地记者编号" << "," << "描述" << "," << "脚本" << "," << "视口左侧比例" << "," << "视口右侧比例" << "," << "视口底部比例" << "," << "视口顶部比例" << "," << "跟踪实体编号" << "," << "跟踪实体类型" << std::endl;
	ss << WarReporter_TableField[ID] << "," << WarReporter_TableField[DESCRIPTION] << "," << WarReporter_TableField[SCRIPT] << "," << WarReporter_TableField[RATIO_LEFT] << "," << WarReporter_TableField[RATIO_RIGHT]
		<< "," << WarReporter_TableField[RATIO_BOTTOM] << "," << WarReporter_TableField[RATIO_TOP] << "," << WarReporter_TableField[TRACK_ENTITY_ID] << "," << WarReporter_TableField[TRACK_ENTITY_TYPE] << std::endl;
	writeFile(ENTITY_WARREPORTER, ss, pTable);
}

const char* WarCommander::getEntTypeName(ENTITY_TYPE entType)
{
	return _entNames[entType];
}

void WarCommander::loadDoF(QDomElement& elem, DoF* parent)
{
	int id = elem.attribute("id").toInt();
	ENTITY_TYPE entType = (ENTITY_TYPE)-1;
	string typeStr = elem.attribute("type").toStdString();
	if (typeStr == _entNames[ENTITY_WEAPON])
		entType = ENTITY_WEAPON;
	else if (typeStr == _entNames[ENTITY_EFFECT])
		entType = ENTITY_EFFECT;
	else if (typeStr == _entNames[ENTITY_REDARMY])
		entType = ENTITY_REDARMY;
	else if (typeStr == _entNames[ENTITY_BLUEARMY])
		entType = ENTITY_BLUEARMY;
	else if (typeStr == _entNames[ENTITY_ALLYARMY])
		entType = ENTITY_ALLYARMY;
	else if (typeStr == _entNames[ENTITY_STATIONARY])
		entType = ENTITY_STATIONARY;
	else if (typeStr == _entNames[ENTITY_WARREPORTER])
		entType = ENTITY_WARREPORTER;
	else
	{
		zoo_warning("Entity type does not exist(id:%d,type:%s)", id, typeStr.c_str());
		return;
	}

	Entity* ent = _currentBattlefield->findEntity(id, entType);
	if (!ent)
	{
		zoo_warning("Entity does not exist(id:%d,type:%s)", id, _entNames[entType]);
		return;
	}

	DoF* dof = ent->getDoF();
	if (!dof)
	{
		zoo_warning("DoF does not exist(id:%d,type:%s)", id, _entNames[entType]);
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

void WarCommander::saveDoF(QDomElement& elem, DoF* parent, QDomDocument& doc)
{

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
