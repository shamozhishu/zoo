#include "RedArmy.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmd/zooCmd.h>
#include <zoo/Utils.h>

RedArmy::RedArmy()
{
}

RedArmy::~RedArmy()
{
}

void RedArmy::init()
{
	Entity::init();
	string model_file = _props.value("model_file").toString().toStdString();
	zooCmd_Send("war --redarmy %d %s", _id, model_file.c_str());
}

ENTITY_TYPE RedArmy::getType() const
{
	return ENTITY_REDARMY;
}

void RedArmy::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value("description").toString().toStdString())
		<< "," << _props.value("script").toString().toStdString()
		<< "," << _props.value("model_file").toString().toStdString()
		<< "," << _props.value("traj_file").toString().toStdString() << std::endl;
}

void RedArmy::deserialize(TableCSV* pTable)
{
	_props["description"] = QString::fromLocal8Bit(pTable->item2str(_id, "description"));
	_props["script"] = QString::fromLocal8Bit(pTable->item2str(_id, "script"));
	_props["model_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "model_file"));
	_props["traj_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "traj_file"));
}
