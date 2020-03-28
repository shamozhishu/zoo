#include "StaticObj.h"

StaticObj::StaticObj(int id)
	: Entity(id)
{
}

StaticObj::~StaticObj()
{
}

void StaticObj::update(float dt)
{

}

Entity* StaticObj::clone()
{
	return new StaticObj(*this);
}

ENTITY_TYPE StaticObj::getType() const
{
	return ENTITY_STATICOBJ;
}
