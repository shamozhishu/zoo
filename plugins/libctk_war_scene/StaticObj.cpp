#include "StaticObj.h"
#include <zoo/DatabaseCSV.h>

StaticObj::StaticObj()
{
	_dof = new zoo::DoF(_id, ENTITY_STATICOBJ);
}

StaticObj::~StaticObj()
{
}

ENTITY_TYPE StaticObj::getType() const
{
	return ENTITY_STATICOBJ;
}

void StaticObj::serialize(stringstream& ss)
{
	ss << _id << "," << _props.value("description").toString().toStdString()
		<< "," << _props.value("model_file").toString().toStdString() << std::endl;
}

void StaticObj::deserialize(TableCSV* pTable)
{
	_props["description"] = QString::fromLocal8Bit(pTable->item2str(_id, "description"));
	_props["model_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "model_file"));
}
