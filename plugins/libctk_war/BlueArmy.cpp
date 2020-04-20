#include "BlueArmy.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"

BlueArmy::BlueArmy()
{
}

BlueArmy::~BlueArmy()
{
}

void BlueArmy::init()
{
	Entity::init();
	string model_file = _props.value(BlueArmy_TableField[MODEL_FILE]).toString().toStdString();
	zooCmdL_Send("war --bluearmy %d %s", _id, model_file.c_str());
}

ENTITY_TYPE BlueArmy::getType() const
{
	return ENTITY_BLUEARMY;
}

void BlueArmy::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(BlueArmy_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(BlueArmy_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(BlueArmy_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(BlueArmy_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void BlueArmy::deserialize(TableCSV* pTable)
{
	_props[BlueArmy_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[DESCRIPTION]));
	_props[BlueArmy_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[SCRIPT]));
	_props[BlueArmy_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[MODEL_FILE]));
	_props[BlueArmy_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[TRAJ_FILE]));
}
