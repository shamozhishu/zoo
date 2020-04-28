#include "Weapon.h"
#include "EntDoF.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"

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
	int branch_tag = _props.value(Weapon_TableField[BRANCH_TAG]).toString().toInt();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "weapon(%d, %s, %d, %s)", _id, model_file.c_str(), branch_tag, ((EntDoF*)_dof)->getCustomParent().c_str());
}

ENTITY_TYPE Weapon::getType() const
{
	return ENTITY_WEAPON;
}

void Weapon::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(Weapon_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(Weapon_TableField[SCRIPT]).toString().toStdString()
		<< "," << _dof->getPosX() << "," << _dof->getPosY() << "," << _dof->getPosZ()
		<< "," << _dof->getHeading() << "," << _dof->getPitch() << "," << _dof->getRoll()
		<< "," << _dof->getScaleX() << "," << _dof->getScaleY() << "," << _dof->getScaleZ()
		<< "," << _props.value(Weapon_TableField[VISIBLE]).toString().toStdString()
		<< "," << ((EntDoF*)_dof)->getCustomParent()
		<< "," << _props.value(Weapon_TableField[BRANCH_TAG]).toString().toStdString()
		<< "," << _props.value(Weapon_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(Weapon_TableField[SOUND_FILE]).toString().toStdString()
		<< "," << _props.value(Weapon_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void Weapon::deserialize(TableCSV* pTable)
{
	_props[Weapon_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[DESCRIPTION]));
	_props[Weapon_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[SCRIPT]));
	_props[Weapon_TableField[VISIBLE]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[VISIBLE]));
	_props[Weapon_TableField[BRANCH_TAG]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[BRANCH_TAG]));
	_props[Weapon_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[MODEL_FILE]));
	_props[Weapon_TableField[SOUND_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[SOUND_FILE]));
	_props[Weapon_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Weapon_TableField[TRAJ_FILE]));
	_dof->setPosX(pTable->item2float(_id, Weapon_TableField[POSX]));
	_dof->setPosY(pTable->item2float(_id, Weapon_TableField[POSY]));
	_dof->setPosZ(pTable->item2float(_id, Weapon_TableField[POSZ]));
	_dof->setHeading(pTable->item2float(_id, Weapon_TableField[HEADING]));
	_dof->setPitch(pTable->item2float(_id, Weapon_TableField[PITCH]));
	_dof->setRoll(pTable->item2float(_id, Weapon_TableField[ROLL]));
	_dof->setScaleX(pTable->item2float(_id, Weapon_TableField[SCALEX]));
	_dof->setScaleY(pTable->item2float(_id, Weapon_TableField[SCALEY]));
	_dof->setScaleZ(pTable->item2float(_id, Weapon_TableField[SCALEZ]));
	((EntDoF*)_dof)->setCustomParent(pTable->item2str(_id, Weapon_TableField[PARENT]));
}
