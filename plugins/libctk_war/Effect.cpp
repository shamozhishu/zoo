#include "Effect.h"
#include "EntDoF.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"

Effect::Effect()
{
}

Effect::~Effect()
{
}

void Effect::init()
{
	Entity::init();
	string model_file = _props.value(Effect_TableField[MODEL_FILE]).toString().toStdString();
	int branch_tag = _props.value(Effect_TableField[BRANCH_TAG]).toString().toInt();
	zooCmdL_Send(WarCommander::getSingleton().getRelatedCmd(), "effect(%d, %s, %d, %s)", _id, model_file.c_str(), branch_tag, ((EntDoF*)_dof)->getCustomParent().c_str());
}

ENTITY_TYPE Effect::getType() const
{
	return ENTITY_EFFECT;
}

void Effect::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(Effect_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(Effect_TableField[SCRIPT]).toString().toStdString()
		<< "," << _dof->getPosX() << "," << _dof->getPosY() << "," << _dof->getPosZ()
		<< "," << _dof->getHeading() << "," << _dof->getPitch() << "," << _dof->getRoll()
		<< "," << _dof->getScaleX() << "," << _dof->getScaleY() << "," << _dof->getScaleZ()
		<< "," << _props.value(Effect_TableField[VISIBLE]).toString().toStdString()
		<< "," << ((EntDoF*)_dof)->getCustomParent()
		<< "," << _props.value(Effect_TableField[BRANCH_TAG]).toString().toStdString()
		<< "," << _props.value(Effect_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(Effect_TableField[SOUND_FILE]).toString().toStdString() << std::endl;
}

void Effect::deserialize(TableCSV* pTable)
{
	_props[Effect_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[DESCRIPTION]));
	_props[Effect_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[SCRIPT]));
	_props[Effect_TableField[VISIBLE]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[VISIBLE]));
	_props[Effect_TableField[BRANCH_TAG]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[BRANCH_TAG]));
	_props[Effect_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[MODEL_FILE]));
	_props[Effect_TableField[SOUND_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[SOUND_FILE]));
	_dof->setPosX(pTable->item2float(_id, Effect_TableField[POSX]));
	_dof->setPosY(pTable->item2float(_id, Effect_TableField[POSY]));
	_dof->setPosZ(pTable->item2float(_id, Effect_TableField[POSZ]));
	_dof->setHeading(pTable->item2float(_id, Effect_TableField[HEADING]));
	_dof->setPitch(pTable->item2float(_id, Effect_TableField[PITCH]));
	_dof->setRoll(pTable->item2float(_id, Effect_TableField[ROLL]));
	_dof->setScaleX(pTable->item2float(_id, Effect_TableField[SCALEX]));
	_dof->setScaleY(pTable->item2float(_id, Effect_TableField[SCALEY]));
	_dof->setScaleZ(pTable->item2float(_id, Effect_TableField[SCALEZ]));
	((EntDoF*)_dof)->setCustomParent(pTable->item2str(_id, Effect_TableField[PARENT]));
}
