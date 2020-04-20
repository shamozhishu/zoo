#include "Weapon.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"

Weapon::Weapon()
{
}

Weapon::~Weapon()
{
}

void Weapon::init()
{
	Entity::init();
	string model_file = _props.value(Weapon_TableField[MODEL_FILE]).toString().toStdString();
	zooCmdL_Send("war --weapon %d %s", _id, model_file.c_str());
}

ENTITY_TYPE Weapon::getType() const
{
	return ENTITY_WEAPON;
}

void Weapon::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(Weapon_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(Weapon_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(Weapon_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(Weapon_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void Weapon::deserialize(TableCSV* pTable)
{
	_props[Weapon_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[DESCRIPTION]));
	_props[Weapon_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[SCRIPT]));
	_props[Weapon_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[MODEL_FILE]));
	_props[Weapon_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[TRAJ_FILE]));
}
