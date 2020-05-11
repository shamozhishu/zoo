#include <zoo/Component.h>

namespace zoo {

zoo::Entity* Component::getEntity() const
{
	return _entity;
}

ZOO_REFLEX_IMPLEMENT(Entity);
std::set<Entity*> Entity::_uselessEntities;
unordered_map<int, unordered_map<int, Entity*>> Entity::_entitiesPool;
Entity::Entity()
	: _id(-1)
	, _kind(0)
{
}

Entity::~Entity()
{
	removeComponents();
}

Component* Entity::getComponent(string className)
{
	auto it = _components.find(className);
	if (it != _components.end())
		return it->second;

	return nullptr;
}

Component* Entity::addComponent(string className)
{
	Component* pComponent = getComponent(className);
	if (pComponent)
		return pComponent;

	pComponent = ReflexFactory<>::getInstance().create<Component>(className);
	if (!pComponent)
	{
		zoo_warning("Component type \"%s\" does not exist!", className.c_str());
		return nullptr;
	}

	pComponent->_entity = this;
	_components[className] = pComponent;

	ComponentImpl* pComponentImpl = ReflexFactory<>::getInstance().create<ComponentImpl>(className + "Impl");
	if (pComponentImpl)
	{
		pComponentImpl->_component = pComponent;
		pComponent->_imp.reset(pComponentImpl);
	}
	else
	{
		zoo_warning("Component implement type \"%s\" does not exist!", (className + "Impl").c_str());
	}

	return pComponent;
}

void Entity::removeComponent(string className)
{
	auto it = _components.find(className);
	if (it != _components.end())
	{
		delete it->second;
		_components.erase(it);
	}
}

void Entity::removeComponent(Component* pComponent)
{
	removeComponent(pComponent->typeName());
}

void Entity::removeComponents()
{
	auto it = _components.begin();
	auto itEnd = _components.end();
	for (; it != itEnd; ++it)
		delete it->second;

	_components.clear();
}

unordered_map<string, Component*> Entity::getComponents() const
{
	return _components;
}

Entity* Entity::create(string className, int id, int kind /*= 0*/)
{
	Entity* pEntity = find(id, kind);
	if (pEntity)
	{
		zoo_warning("Entity already exists[id=%d,kind=%d]!", id, kind);
		return nullptr;
	}

	pEntity = fetch(className);
	if (!pEntity)
	{
		pEntity = ReflexFactory<>::getInstance().create<Entity>(className);
		if (!pEntity)
		{
			zoo_warning("Entity type does not exist!");
			return nullptr;
		}

		EntityImpl* pEntityImpl = ReflexFactory<>::getInstance().create<EntityImpl>(className + "Impl");
		if (pEntityImpl)
		{
			pEntityImpl->_entity = pEntity;
			pEntity->_imp.reset(pEntityImpl);
		}
		else
		{
			zoo_warning("Entity implement type \"%s\" does not exist!", (className + "Impl").c_str());
		}
	}

	pEntity->_id = id;
	pEntity->_kind = kind;
	_entitiesPool[kind][id] = pEntity;
	return pEntity;
}

unordered_map<int, Entity*> Entity::get(int kind /*= 0*/)
{
	auto it = _entitiesPool.find(kind);
	if (it != _entitiesPool.end())
		return it->second;

	return unordered_map<int, Entity*>();
}

void Entity::destroy(Entity* pEntity, bool bDelete /*= false*/)
{
	if (pEntity)
	{
		_entitiesPool[pEntity->Kind()][pEntity->ID()] = nullptr;
		if (bDelete)
			delete pEntity;
		else
			discard(pEntity);
	}
}

void Entity::clear()
{
	auto it = _entitiesPool.begin();
	auto itEnd = _entitiesPool.end();
	for (; it != itEnd; ++it)
	{
		unordered_map<int, Entity*>& entities = it->second;
		auto itor = entities.begin();
		auto itorEnd = entities.end();
		for (; itor != itorEnd; ++itor)
		{
			SAFE_DELETE(itor->second);
		}
	}

	auto itSet = _uselessEntities.begin();
	auto itSetEnd = _uselessEntities.end();
	for (; itSet != itSetEnd; ++itSet)
		delete *itSet;

	_entitiesPool.clear();
	_uselessEntities.clear();
}

Entity* Entity::find(int id, int kind /*= 0*/)
{
	auto it = _entitiesPool.find(kind);
	if (it == _entitiesPool.end())
		return nullptr;

	auto itor = it->second.find(id);
	if (itor == it->second.end())
		return nullptr;

	return itor->second;
}

Entity* Entity::fetch(string className)
{
	Entity* pEntity;
	auto it = _uselessEntities.begin();
	auto itEnd = _uselessEntities.end();
	for (; it != itEnd; ++it)
	{
		pEntity = *it;
		if (pEntity->typeName() == className)
		{
			_uselessEntities.erase(it);
			return pEntity;
		}
	}

	return nullptr;
}

void Entity::discard(Entity* pEntity)
{
	if (pEntity)
		_uselessEntities.insert(pEntity);
}

ComponentImpl* EntityImpl::getComponentImpl(string className)
{
	Component* pComponent = _entity->getComponent(className);
	if (pComponent)
		pComponent->_imp.get();
	return nullptr;
}

}
