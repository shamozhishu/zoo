#include "RedArmy.h"
#include "EntDoF.h"
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
	int branch_tag = _props.value(RedArmy_TableField[BRANCH_TAG]).toString().toInt();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "redarmy(%d, %s, %d, %s)", _id, model_file.c_str(), branch_tag, ((EntDoF*)_dof)->getCustomParent().c_str());
}

ENTITY_TYPE RedArmy::getType() const
{
	return ENTITY_REDARMY;
}

void RedArmy::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(RedArmy_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(RedArmy_TableField[SCRIPT]).toString().toStdString()
		<< "," << _dof->getPosX() << "," << _dof->getPosY() << "," << _dof->getPosZ()
		<< "," << _dof->getHeading() << "," << _dof->getPitch() << "," << _dof->getRoll()
		<< "," << _dof->getScaleX() << "," << _dof->getScaleY() << "," << _dof->getScaleZ()
		<< "," << _props.value(RedArmy_TableField[VISIBLE]).toString().toStdString()
		<< "," << ((EntDoF*)_dof)->getCustomParent()
		<< "," << _props.value(RedArmy_TableField[BRANCH_TAG]).toString().toStdString()
		<< "," << _props.value(RedArmy_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(RedArmy_TableField[SOUND_FILE]).toString().toStdString()
		<< "," << _props.value(RedArmy_TableField[TRAJ_FILE]).toString().toStdString() << std::endl;
}

void RedArmy::deserialize(TableCSV* pTable)
{
	_props[RedArmy_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[DESCRIPTION]));
	_props[RedArmy_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[SCRIPT]));
	_props[RedArmy_TableField[VISIBLE]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[VISIBLE]));
	_props[RedArmy_TableField[BRANCH_TAG]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[BRANCH_TAG]));
	_props[RedArmy_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[MODEL_FILE]));
	_props[RedArmy_TableField[SOUND_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[SOUND_FILE]));
	_props[RedArmy_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, RedArmy_TableField[TRAJ_FILE]));
	_dof->setPosX(pTable->item2float(_id, RedArmy_TableField[POSX]));
	_dof->setPosY(pTable->item2float(_id, RedArmy_TableField[POSY]));
	_dof->setPosZ(pTable->item2float(_id, RedArmy_TableField[POSZ]));
	_dof->setHeading(pTable->item2float(_id, RedArmy_TableField[HEADING]));
	_dof->setPitch(pTable->item2float(_id, RedArmy_TableField[PITCH]));
	_dof->setRoll(pTable->item2float(_id, RedArmy_TableField[ROLL]));
	_dof->setScaleX(pTable->item2float(_id, RedArmy_TableField[SCALEX]));
	_dof->setScaleY(pTable->item2float(_id, RedArmy_TableField[SCALEY]));
	_dof->setScaleZ(pTable->item2float(_id, RedArmy_TableField[SCALEZ]));
	((EntDoF*)_dof)->setCustomParent(pTable->item2str(_id, RedArmy_TableField[PARENT]));
}
