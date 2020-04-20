#include "Effect.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"

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
	zooCmdL_Send("war --effect %d %s", _id, model_file.c_str());
}

ENTITY_TYPE Effect::getType() const
{
	return ENTITY_EFFECT;
}

void Effect::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(Effect_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(Effect_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(Effect_TableField[MODEL_FILE]).toString().toStdString() << std::endl;
}

void Effect::deserialize(TableCSV* pTable)
{
	_props[Effect_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[DESCRIPTION]));
	_props[Effect_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[SCRIPT]));
	_props[Effect_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Effect_TableField[MODEL_FILE]));
}
