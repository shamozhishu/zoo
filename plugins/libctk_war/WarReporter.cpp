#include "WarReporter.h"
#include "CommonDef.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>

WarReporter::WarReporter()
{
}

WarReporter::~WarReporter()
{
}

void WarReporter::init()
{
	Entity::init();
}

ENTITY_TYPE WarReporter::getType() const
{
	return ENTITY_WARREPORTER;
}

void WarReporter::serialize(stringstream& ss)
{
	ss << _id << "," << utf8ToAnsi(_props.value(WarReporter_TableField[DESCRIPTION]).toString().toStdString())
		<< "," << _props.value(WarReporter_TableField[SCRIPT]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_LEFT]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_RIGHT]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_BOTTOM]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_TOP]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[TRACK_ENTITY_ID]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[TRACK_ENTITY_TYPE]).toString().toStdString() << std::endl;
}

void WarReporter::deserialize(TableCSV* pTable)
{
	_props[WarReporter_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[DESCRIPTION]));
	_props[WarReporter_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[SCRIPT]));
	_props[WarReporter_TableField[RATIO_LEFT]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_LEFT]));
	_props[WarReporter_TableField[RATIO_RIGHT]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_RIGHT]));
	_props[WarReporter_TableField[RATIO_BOTTOM]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_BOTTOM]));
	_props[WarReporter_TableField[RATIO_TOP]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_TOP]));
	_props[WarReporter_TableField[TRACK_ENTITY_ID]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[TRACK_ENTITY_ID]));
	_props[WarReporter_TableField[TRACK_ENTITY_TYPE]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[TRACK_ENTITY_TYPE]));
}
