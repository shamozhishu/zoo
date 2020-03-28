#include "Effect.h"

Effect::Effect(int id)
	: Entity(id)
{
}

Effect::~Effect()
{
}

void Effect::update(float dt)
{

}

Entity* Effect::clone()
{
	return new Effect(*this);
}

ENTITY_TYPE Effect::getType() const
{
	return ENTITY_EFFECT;
}
