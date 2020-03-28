#include "RedArmy.h"

RedArmy::RedArmy(int id)
	: Entity(id)
{
}

RedArmy::~RedArmy()
{
}

void RedArmy::update(float dt)
{

}

Entity* RedArmy::clone()
{
	return new RedArmy(*this);
}

ENTITY_TYPE RedArmy::getType() const
{
	return ENTITY_REDARMY;
}
