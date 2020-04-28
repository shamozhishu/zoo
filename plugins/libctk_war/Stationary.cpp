#include "Stationary.h"
#include "EntDoF.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"

Stationary::Stationary()
{
}

Stationary::~Stationary()
{
}

void Stationary::init()
{
	Entity::init();
	string model_file = _props.value(Stationary_TableField[MODEL_FILE]).toString().toStdString();
	int branch_tag = _props.value(Stationary_TableField[BRANCH_TAG]).toString().toInt();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "stationary(%d, %s, %d, %s)", _id, model_file.c_str(), branch_tag, ((EntDoF*)_dof)->getCustomParent().c_str());
}

ENTITY_TYPE Stationary::getType() const
{
	return ENTITY_STATIONARY;
}

void Stationary::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(Stationary_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(Stationary_TableField[SCRIPT]).toString().toStdString()
		<< "," << _dof->getPosX() << "," << _dof->getPosY() << "," << _dof->getPosZ()
		<< "," << _dof->getHeading() << "," << _dof->getPitch() << "," << _dof->getRoll()
		<< "," << _dof->getScaleX() << "," << _dof->getScaleY() << "," << _dof->getScaleZ()
		<< "," << _props.value(Stationary_TableField[VISIBLE]).toString().toStdString()
		<< "," << ((EntDoF*)_dof)->getCustomParent()
		<< "," << _props.value(Stationary_TableField[BRANCH_TAG]).toString().toStdString()
		<< "," << _props.value(Stationary_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(Stationary_TableField[SOUND_FILE]).toString().toStdString() << std::endl;
}

void Stationary::deserialize(TableCSV* pTable)
{
	_props[Stationary_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[DESCRIPTION]));
	_props[Stationary_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[SCRIPT]));
	_props[Stationary_TableField[VISIBLE]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[VISIBLE]));
	_props[Stationary_TableField[BRANCH_TAG]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[BRANCH_TAG]));
	_props[Stationary_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[MODEL_FILE]));
	_props[Stationary_TableField[SOUND_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[SOUND_FILE]));
	_dof->setPosX(pTable->item2float(_id, Stationary_TableField[POSX]));
	_dof->setPosY(pTable->item2float(_id, Stationary_TableField[POSY]));
	_dof->setPosZ(pTable->item2float(_id, Stationary_TableField[POSZ]));
	_dof->setHeading(pTable->item2float(_id, Stationary_TableField[HEADING]));
	_dof->setPitch(pTable->item2float(_id, Stationary_TableField[PITCH]));
	_dof->setRoll(pTable->item2float(_id, Stationary_TableField[ROLL]));
	_dof->setScaleX(pTable->item2float(_id, Stationary_TableField[SCALEX]));
	_dof->setScaleY(pTable->item2float(_id, Stationary_TableField[SCALEY]));
	_dof->setScaleZ(pTable->item2float(_id, Stationary_TableField[SCALEZ]));
	((EntDoF*)_dof)->setCustomParent(pTable->item2str(_id, Stationary_TableField[PARENT]));
}
