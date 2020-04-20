#include "AllyArmy.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"

AllyArmy::AllyArmy()
{
}

AllyArmy::~AllyArmy()
{
}

void AllyArmy::init()
{
	Entity::init();
	string model_file = _props.value(AllyArmy_TableField[MODEL_FILE]).toString().toStdString();
	zooCmdL_Send("war --allyarmy %d %s", _id, model_file.c_str());
}

ENTITY_TYPE AllyArmy::getType() const
{
	return ENTITY_ALLYARMY;
}

void AllyArmy::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(AllyArmy_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(AllyArmy_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(AllyArmy_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(AllyArmy_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void AllyArmy::deserialize(TableCSV* pTable)
{
	_props[AllyArmy_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, AllyArmy_TableField[DESCRIPTION]));
	_props[AllyArmy_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, AllyArmy_TableField[SCRIPT]));
	_props[AllyArmy_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, AllyArmy_TableField[MODEL_FILE]));
	_props[AllyArmy_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, AllyArmy_TableField[TRAJ_FILE]));
}
