#include "Weapon.h"
#include <zoo/DatabaseCSV.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <zoo/Utils.h>
#include "CommonDef.h"
#include "WarCommander.h"
#include "LuaScript.h"

ZOO_REFLEX_IMPLEMENT(Weapon);

Weapon::Weapon()
{
	_behavior = zoo::Entity::addComponent<Behavior>();
	_dof = zoo::Entity::addComponent<DoF>();
	_model = zoo::Entity::addComponent<Model>();
	_sound = zoo::Entity::addComponent<Sound>();
	_animator = zoo::Entity::addComponent<Animator>();
}

void Weapon::init()
{
	_behavior->getScript()->setVariable("dof", "DoF", _dof);
	_behavior->getScript()->setVariable("model", "Model", _model);
	_behavior->getScript()->setVariable("sound", "Sound", _sound);
	_behavior->getScript()->setVariable("animator", "Animator", _animator);
	_behavior->init();
}

void Weapon::update()
{
	_behavior->update();
}

void Weapon::serialize(stringstream& ss)
{
	ss << ID() << "," << _desc << ",";
	_behavior->serialize(ss);
	ss << ",";
	_dof->serialize(ss);
	ss << ",";
	_model->serialize(ss);
	ss << ",";
	_sound->serialize(ss);
	ss << ",";
	_animator->serialize(ss);
	ss << std::endl;
}

void Weapon::deserialize(zoo::TableCSV* pTable)
{
	_desc = pTable->item2str(ID(), TableField[DESCRIPTION]);
	_behavior->deserialize(pTable);
	_dof->deserialize(pTable);
	_model->deserialize(pTable);
	_sound->deserialize(pTable);
	_animator->deserialize(pTable);
}

int Weapon::ID() const
{
	return zoo::Entity::ID();
}

int Weapon::Kind() const
{
	return zoo::Entity::Kind();
}
