#include "Weapon.h"
#include <zoo/DatabaseCSV.h>

Weapon::Weapon()
{
	_dof = new zoo::DoF(_id, ENTITY_WEAPON);
}

Weapon::~Weapon()
{
}

ENTITY_TYPE Weapon::getType() const
{
	return ENTITY_WEAPON;
}

void Weapon::serialize(stringstream& ss)
{
	ss << _id << "," << _props.value("description").toString().toStdString()
		<< "," << _props.value("model_file").toString().toStdString()
		<< "," << _props.value("traj_file").toString().toStdString() << std::endl;
}

void Weapon::deserialize(TableCSV* pTable)
{
	_props["description"] = QString::fromLocal8Bit(pTable->item2str(_id, "description"));
	_props["model_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "model_file"));
	_props["traj_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "traj_file"));
}
