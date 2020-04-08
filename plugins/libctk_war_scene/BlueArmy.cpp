#include "BlueArmy.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmd/zooCmd.h>

BlueArmy::BlueArmy()
{
	_dof = new zoo::DoF(_id, ENTITY_BLUEARMY);
}

BlueArmy::~BlueArmy()
{
}

void BlueArmy::init()
{
	Entity::init();
	string model_file = _props.value("model_file").toString().toStdString();
	zooCmd_Send("war --bluearmy %d %s", _id, model_file.c_str());
}

ENTITY_TYPE BlueArmy::getType() const
{
	return ENTITY_BLUEARMY;
}

void BlueArmy::serialize(stringstream& ss)
{
	ss << _id << "," << _props.value("description").toString().toStdString()
		<< "," << _props.value("model_file").toString().toStdString()
		<< "," << _props.value("traj_file").toString().toStdString() << std::endl;
}

void BlueArmy::deserialize(TableCSV* pTable)
{
	_props["description"] = QString::fromLocal8Bit(pTable->item2str(_id, "description"));
	_props["model_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "model_file"));
	_props["traj_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "traj_file"));
}
