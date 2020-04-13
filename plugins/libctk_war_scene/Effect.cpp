#include "Effect.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmd/zooCmd.h>
#include <zoo/Utils.h>

Effect::Effect()
{
}

Effect::~Effect()
{
}

void Effect::init()
{
	Entity::init();
	string model_file = _props.value("model_file").toString().toStdString();
	zooCmd_Send("war --effect %d %s", _id, model_file.c_str());
}

ENTITY_TYPE Effect::getType() const
{
	return ENTITY_EFFECT;
}

void Effect::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value("description").toString().toStdString())
		<< "," << _props.value("script").toString().toStdString()
		<< "," << _props.value("model_file").toString().toStdString() << std::endl;
}

void Effect::deserialize(TableCSV* pTable)
{
	_props["description"] = QString::fromLocal8Bit(pTable->item2str(_id, "description"));
	_props["script"] = QString::fromLocal8Bit(pTable->item2str(_id, "script"));
	_props["model_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "model_file"));
}
