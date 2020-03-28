#include "Weapon.h"

Weapon::Weapon(int id)
	: Entity(id)
{
}

Weapon::~Weapon()
{
}

void Weapon::update(float dt)
{

}

Entity* Weapon::clone()
{
	return new Weapon(*this);
}

ENTITY_TYPE Weapon::getType() const
{
	return ENTITY_WEAPON;
}
