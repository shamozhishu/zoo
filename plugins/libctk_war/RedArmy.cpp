#include "RedArmy.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"

RedArmy::RedArmy()
{
}

RedArmy::~RedArmy()
{
}

void RedArmy::init()
{
	Entity::init();
	string model_file = _props.value(RedArmy_TableField[MODEL_FILE]).toString().toStdString();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "--redarmy %d %s", _id, model_file.c_str());
}

ENTITY_TYPE RedArmy::getType() const
{
	return ENTITY_REDARMY;
}

void RedArmy::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(RedArmy_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(RedArmy_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(RedArmy_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(RedArmy_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void RedArmy::deserialize(TableCSV* pTable)
{
	_props[RedArmy_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[DESCRIPTION]));
	_props[RedArmy_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[SCRIPT]));
	_props[RedArmy_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[MODEL_FILE]));
	_props[RedArmy_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[TRAJ_FILE]));
}
