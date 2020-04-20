#include "Stationary.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"

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
	zooCmdL_Send("war --stationary %d %s", _id, model_file.c_str());
}

ENTITY_TYPE Stationary::getType() const
{
	return ENTITY_STATIONARY;
}

void Stationary::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(Stationary_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(Stationary_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(Stationary_TableField[MODEL_FILE]).toString().toStdString() << std::endl;
}

void Stationary::deserialize(TableCSV* pTable)
{
	_props[Stationary_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[DESCRIPTION]));
	_props[Stationary_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[SCRIPT]));
	_props[Stationary_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, Stationary_TableField[MODEL_FILE]));
}
