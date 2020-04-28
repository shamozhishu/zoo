#include "BlueArmy.h"
#include "EntDoF.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"

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
	int branch_tag = _props.value(BlueArmy_TableField[BRANCH_TAG]).toString().toInt();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "bluearmy(%d, %s, %d, %s)", _id, model_file.c_str(), branch_tag, ((EntDoF*)_dof)->getCustomParent().c_str());
}

ENTITY_TYPE BlueArmy::getType() const
{
	return ENTITY_BLUEARMY;
}

void BlueArmy::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(BlueArmy_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(BlueArmy_TableField[SCRIPT]).toString().toStdString()
		<< "," << _dof->getPosX() << "," << _dof->getPosY() << "," << _dof->getPosZ()
		<< "," << _dof->getHeading() << "," << _dof->getPitch() << "," << _dof->getRoll()
		<< "," << _dof->getScaleX() << "," << _dof->getScaleY() << "," << _dof->getScaleZ()
		<< "," << _props.value(BlueArmy_TableField[VISIBLE]).toString().toStdString()
		<< "," << ((EntDoF*)_dof)->getCustomParent()
		<< "," << _props.value(BlueArmy_TableField[BRANCH_TAG]).toString().toStdString()
		<< "," << _props.value(BlueArmy_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(BlueArmy_TableField[SOUND_FILE]).toString().toStdString()
		<< "," << _props.value(BlueArmy_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void BlueArmy::deserialize(TableCSV* pTable)
{
	_props[BlueArmy_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[DESCRIPTION]));
	_props[BlueArmy_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[SCRIPT]));
	_props[BlueArmy_TableField[VISIBLE]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[VISIBLE]));
	_props[BlueArmy_TableField[BRANCH_TAG]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[BRANCH_TAG]));
	_props[BlueArmy_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[MODEL_FILE]));
	_props[BlueArmy_TableField[SOUND_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[SOUND_FILE]));
	_props[BlueArmy_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, BlueArmy_TableField[TRAJ_FILE]));
	_dof->setPosX(pTable->item2float(_id, BlueArmy_TableField[POSX]));
	_dof->setPosY(pTable->item2float(_id, BlueArmy_TableField[POSY]));
	_dof->setPosZ(pTable->item2float(_id, BlueArmy_TableField[POSZ]));
	_dof->setHeading(pTable->item2float(_id, BlueArmy_TableField[HEADING]));
	_dof->setPitch(pTable->item2float(_id, BlueArmy_TableField[PITCH]));
	_dof->setRoll(pTable->item2float(_id, BlueArmy_TableField[ROLL]));
	_dof->setScaleX(pTable->item2float(_id, BlueArmy_TableField[SCALEX]));
	_dof->setScaleY(pTable->item2float(_id, BlueArmy_TableField[SCALEY]));
	_dof->setScaleZ(pTable->item2float(_id, BlueArmy_TableField[SCALEZ]));
	((EntDoF*)_dof)->setCustomParent(pTable->item2str(_id, BlueArmy_TableField[PARENT]));
}
