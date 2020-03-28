#include "Scene.h"
#include "Effect.h"
#include "Weapon.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "StaticObj.h"

Scene::Scene(int id)
	: Entity(id)
{
}

Scene::~Scene()
{
}

void Scene::update(float dt)
{

}

Entity* Scene::clone()
{
	return new Scene(*this);
}

ENTITY_TYPE Scene::getType() const
{
	return ENTITY_SCENE;
}

Effect* Scene::createEffect(int id)
{
	Effect* pEffect = new Effect(id);
	return pEffect;
}

void Scene::destroyEffect(int id)
{

}

Weapon* Scene::createWeapon(int id)
{
	return nullptr;
}

void Scene::destroyWeapon(int id)
{

}

RedArmy* Scene::createRedArmy(int id)
{
	return nullptr;
}

void Scene::destroyRedArmy(int id)
{

}

BlueArmy* Scene::createBlueArmy(int id)
{
	return nullptr;
}

void Scene::destroyBlueArmy(int id)
{

}

AllyArmy* Scene::createAllyArmy(int id)
{
	return nullptr;
}

void Scene::destroyAllyArmy(int id)
{

}

StaticObj* Scene::createStaticObj(int id)
{
	return nullptr;
}

void Scene::destroyStaticObj(int id)
{

}
