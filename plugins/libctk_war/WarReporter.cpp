#include "WarReporter.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"
#include "LuaScript.h"

ZOO_REFLEX_IMPLEMENT(WarReporter);

WarReporter::WarReporter()
{
	_behavior = zoo::Entity::addComponent<Behavior>();
}

void WarReporter::init()
{
	_behavior->init();
}

void WarReporter::update()
{
	_behavior->update();
}

void WarReporter::serialize(stringstream& ss)
{
	ss << ID() << "," << _desc << ",";
	_behavior->serialize(ss);
	ss << std::endl;
}

void WarReporter::deserialize(zoo::TableCSV* pTable)
{
	_desc = pTable->item2str(ID(), TableField[DESCRIPTION]);
	_behavior->deserialize(pTable);
}

int WarReporter::ID() const
{
	return zoo::Entity::ID();
}

int WarReporter::Kind() const
{
	return zoo::Entity::Kind();
}
