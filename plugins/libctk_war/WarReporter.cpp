#include "WarReporter.h"
#include "EntDoF.h"
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
		<< "," << _dof->getPosX() << "," << _dof->getPosY() << "," << _dof->getPosZ()
		<< "," << _dof->getHeading() << "," << _dof->getPitch() << "," << _dof->getRoll()
		<< "," << _dof->getScaleX() << "," << _dof->getScaleY() << "," << _dof->getScaleZ()
		<< "," << _props.value(WarReporter_TableField[VISIBLE]).toString().toStdString()
		<< "," << ((EntDoF*)_dof)->getCustomParent()
		<< "," << _props.value(WarReporter_TableField[BRANCH_TAG]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[MODEL_FILE]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[SOUND_FILE]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[TRAJ_FILE]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_LEFT]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_RIGHT]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_BOTTOM]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[RATIO_TOP]).toString().toStdString()
		<< "," << _props.value(WarReporter_TableField[TRACK_ENTITY]).toString().toStdString() << std::endl;
}

void WarReporter::deserialize(TableCSV* pTable)
{
	_props[WarReporter_TableField[DESCRIPTION]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[DESCRIPTION]));
	_props[WarReporter_TableField[SCRIPT]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[SCRIPT]));
	_props[WarReporter_TableField[VISIBLE]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[VISIBLE]));
	_props[WarReporter_TableField[BRANCH_TAG]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[BRANCH_TAG]));
	_props[WarReporter_TableField[MODEL_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[MODEL_FILE]));
	_props[WarReporter_TableField[SOUND_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[SOUND_FILE]));
	_props[WarReporter_TableField[TRAJ_FILE]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[TRAJ_FILE]));
	_props[WarReporter_TableField[RATIO_LEFT]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_LEFT]));
	_props[WarReporter_TableField[RATIO_RIGHT]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_RIGHT]));
	_props[WarReporter_TableField[RATIO_BOTTOM]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_BOTTOM]));
	_props[WarReporter_TableField[RATIO_TOP]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[RATIO_TOP]));
	_props[WarReporter_TableField[TRACK_ENTITY]] = QString::fromLocal8Bit(pTable->item2str(_id, WarReporter_TableField[TRACK_ENTITY]));
	_dof->setPosX(pTable->item2float(_id, WarReporter_TableField[POSX]));
	_dof->setPosY(pTable->item2float(_id, WarReporter_TableField[POSY]));
	_dof->setPosZ(pTable->item2float(_id, WarReporter_TableField[POSZ]));
	_dof->setHeading(pTable->item2float(_id, WarReporter_TableField[HEADING]));
	_dof->setPitch(pTable->item2float(_id, WarReporter_TableField[PITCH]));
	_dof->setRoll(pTable->item2float(_id, WarReporter_TableField[ROLL]));
	_dof->setScaleX(pTable->item2float(_id, WarReporter_TableField[SCALEX]));
	_dof->setScaleY(pTable->item2float(_id, WarReporter_TableField[SCALEY]));
	_dof->setScaleZ(pTable->item2float(_id, WarReporter_TableField[SCALEZ]));
	((EntDoF*)_dof)->setCustomParent(pTable->item2str(_id, Weapon_TableField[PARENT]));
}
