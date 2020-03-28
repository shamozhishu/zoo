#include "BlueArmy.h"

BlueArmy::BlueArmy(int id)
	: Entity(id)
{
}

BlueArmy::~BlueArmy()
{
}

void BlueArmy::update(float dt)
{

}

Entity* BlueArmy::clone()
{
	return new BlueArmy(*this);
}

ENTITY_TYPE BlueArmy::getType() const
{
	return ENTITY_BLUEARMY;
}
