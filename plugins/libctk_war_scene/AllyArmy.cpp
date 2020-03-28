#include "AllyArmy.h"

AllyArmy::AllyArmy(int id)
	: Entity(id)
{
}

AllyArmy::~AllyArmy()
{
}

void AllyArmy::update(float dt)
{

}

Entity* AllyArmy::clone()
{
	return new AllyArmy(*this);
}

ENTITY_TYPE AllyArmy::getType() const
{
	return ENTITY_ALLYARMY;
}
