#include <zoo/Component.h>
#include <zoo/DatabaseCSV.h>

namespace zoo {

Entity* Spawner::new_Entity(int id, const string& desc /*= ""*/)
{
	Entity* pEntity = Entity::find(id, _breed);
	if (pEntity)
	{
		zoo_warning("Entity already exists[id=%d,breed=%d]!", id, _breed);
		return nullptr;
	}

	pEntity = Entity::fetch();
	if (!pEntity)
		pEntity = new Entity;

	pEntity->_id = id;
	pEntity->_breed = _breed;
	pEntity->_desc = desc;
	Entity::_entitiesPool[_breed][id] = pEntity;
	return pEntity;
}

Entity* Spawner::find_Entity(int id)
{
	return Entity::find(id, _breed);
}

unordered_map<int, Entity*> Spawner::gain_Entities()
{
	return Entity::gain(_breed);
}

void Spawner::awake()
{
	Entity* pEnt = nullptr;
	auto entities = gain_Entities();
	auto it = entities.begin();
	auto itEnd = entities.end();
	for (; it != itEnd; ++it)
	{
		pEnt = it->second;
		if (pEnt)
			pEnt->awake();
	}
}

void Spawner::load()
{
	Entity* pEnt = nullptr;
	auto entities = gain_Entities();
	auto it = entities.begin();
	auto itEnd = entities.end();
	for (; it != itEnd; ++it)
	{
		pEnt = it->second;
		if (pEnt)
			pEnt->deserialize(this);
	}
}

void Spawner::save(string filename)
{
	fstream fout(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
		return;
	}

	Entity* pEnt = nullptr;
	bool isSeriHeader = false;
	auto entities = gain_Entities();
	auto it = entities.begin();
	auto itEnd = entities.end();
	for (; it != itEnd; ++it)
	{
		pEnt = it->second;
		if (pEnt)
		{
			if (!isSeriHeader)
			{
				isSeriHeader = true;
				pEnt->serializeHeader(this);
				pEnt->serializeField(this);
			}

			pEnt->serialize(this);
		}
	}

	fout << ss().str();
	fout.close();
	ss().clear();
	ss().str("");
}

Entity* Component::getEntity() const
{
	return _entity;
}

Entity* Entity::_firstAvailable = nullptr;
unordered_map<int, unordered_map<int, Entity*>> Entity::_entitiesPool;
Entity::Entity()
	: _id(-1)
	, _breed(-1)
	, _inUse(true)
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

ComponentImpl* Entity::getComponentImpl(string className)
{
	Component* pComponent = getComponent(className.substr(0, className.size() - 4));
	if (pComponent)
		return pComponent->_imp.get();
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

void Entity::awake()
{
	ComponentImpl* pComponentImpl;
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		pComponentImpl = it->second->_imp.get();
		if (pComponentImpl)
			pComponentImpl->awake();
	}

	SignalTrigger::trigger(_AWAKED);
	SignalTrigger::disconnect(_AWAKED);
}

void Entity::update()
{
	ComponentImpl* pComponentImpl;
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		pComponentImpl = it->second->_imp.get();
		if (pComponentImpl)
			pComponentImpl->update();
	}

	SignalTrigger::trigger(_UPDATED);
	SignalTrigger::disconnect(_UPDATED);
}

void Entity::serialize(Spawner* spawner)
{
	if (!spawner)
		return;

	spawner->ss() << ID() << "," << Description();
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		spawner->ss() << ",";
		it->second->serialize(spawner);
	}
	spawner->ss() << std::endl;
}

void Entity::deserialize(Spawner* spawner)
{
	if (!spawner)
		return;

	TableCSV* pTable = spawner->getTable();
	if (!pTable)
		return;

	_desc = pTable->item2str(_id, "description");

	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		it->second->deserialize(spawner);
	}
}

void Entity::serializeField(Spawner* spawner)
{
	if (!spawner)
		return;

	spawner->ss() << "id" << "," << "description";
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		spawner->ss() << ",";
		it->second->serializeField(spawner);
	}
	spawner->ss() << std::endl;
}

void Entity::serializeHeader(Spawner* spawner)
{
	if (!spawner)
		return;

	spawner->ss() << "编号" << "," << "描述";
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		spawner->ss() << ",";
		it->second->serializeHeader(spawner);
	}
	spawner->ss() << std::endl;
}

void Entity::destroy(Entity* pEntity, bool bDelete /*= false*/)
{
	if (pEntity)
	{
		if (bDelete)
		{
			_entitiesPool[pEntity->Breed()][pEntity->ID()] = nullptr;
			delete pEntity;
		}
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

	_entitiesPool.clear();
}

Entity* Entity::fetch()
{
	Entity* pEntity = nullptr;
	if (_firstAvailable)
	{
		pEntity = _firstAvailable;
		pEntity->_inUse = true;
		_firstAvailable = pEntity->_next;
	}

	return pEntity;
}

Entity* Entity::find(int id, int breed)
{
	auto it = _entitiesPool.find(breed);
	if (it == _entitiesPool.end())
		return nullptr;

	auto itor = it->second.find(id);
	if (itor == it->second.end())
		return nullptr;

	Entity* pEnt = itor->second;
	if (pEnt && !pEnt->_inUse)
		return nullptr;

	return pEnt;
}

void Entity::discard(Entity* pEntity)
{
	if (!pEntity)
		return;

	pEntity->_inUse = false;
	pEntity->_next = nullptr;
	if (!_firstAvailable)
	{
		_firstAvailable = pEntity;
		return;
	}

	pEntity->_next = _firstAvailable;
	_firstAvailable = pEntity;
}

const unordered_map<int, Entity*>& Entity::gain(int breed)
{
	static unordered_map<int, Entity*> s_emptyEntities;
	auto it = _entitiesPool.find(breed);
	if (it != _entitiesPool.end())
		return it->second;

	return s_emptyEntities;
}

}
