#include "Effect.h"
#include <zoo/DatabaseCSV.h>

Effect::Effect()
{
	_dof = new zoo::DoF(_id, ENTITY_EFFECT);
}

Effect::~Effect()
{
}

ENTITY_TYPE Effect::getType() const
{
	return ENTITY_EFFECT;
}

void Effect::serialize(stringstream& ss)
{
	ss << _id << "," << _props.value("description").toString().toStdString()
		<< "," << _props.value("model_file").toString().toStdString() << std::endl;
}

void Effect::deserialize(TableCSV* pTable)
{
	_props["description"] = QString::fromLocal8Bit(pTable->item2str(_id, "description"));
	_props["model_file"] = QString::fromLocal8Bit(pTable->item2str(_id, "model_file"));
}
