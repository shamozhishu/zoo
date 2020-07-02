#include <zoo/Component.h>
#include <zoo/DatabaseCSV.h>
#include <cJSON/CJsonObject.h>

namespace zoo {

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
	{
		delete it->second;
		it->second = nullptr;
	}

	_components.clear();
}

unordered_map<string, Component*> Entity::getComponents() const
{
	return _components;
}

void Entity::awake()
{
	Component* pComponent;
	ComponentImpl* pComponentImpl;
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		pComponent = it->second;
		if (pComponent)
		{
			pComponentImpl = pComponent->_imp.get();
			if (pComponentImpl)
				pComponentImpl->awake();
		}
	}

	if (SignalTrigger::hasSignal(*this))
	{
		SignalTrigger::trigger(*this);
		SignalTrigger::disconnect(*this);
	}
}

void Entity::update()
{
	Component* pComponent;
	ComponentImpl* pComponentImpl;
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		pComponent = it->second;
		if (pComponent)
		{
			pComponentImpl = pComponent->_imp.get();
			if (pComponentImpl && !pComponent->_dirty.isEmptyState())
			{
				pComponentImpl->update();
				pComponent->_dirty.clearState();
			}
		}
	}

	if (SignalTrigger::hasSignal(*this))
	{
		SignalTrigger::trigger(*this);
		SignalTrigger::disconnect(*this);
	}
}

void Entity::serialize(Spawner* spawner)
{
	if (!spawner || !spawner->getParser())
		return;

	CJsonObject& entJson = *(CJsonObject*)spawner->getParser();
	entJson.Add("id", _id);
	entJson.Add("breed", _breed);
	entJson.Add("desc", _desc);
	entJson.AddEmptySubArray("component");

	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		CJsonObject comJson;
		spawner->_parser = &comJson;
		it->second->serialize(spawner);
		entJson["component"].Add(comJson);
	}
}

void Entity::deserialize(Spawner* spawner)
{
	if (!spawner || !spawner->getParser())
		return;

	CJsonObject& entJson = *(CJsonObject*)spawner->getParser();
	entJson.Get("desc", _desc);

	int comCount = entJson["component"].GetArraySize();
	for (int i = 0; i < comCount; ++i)
	{
		CJsonObject comJson;
		entJson["component"].Get(i, comJson);
		spawner->_parser = &comJson;
		string type;
		comJson.Get("type", type);
		Component* pCom = addComponent(type);
		if (pCom)
			pCom->deserialize(spawner);
	}
}
//////////////////////////////////////////////////////////////////////////
std::unordered_map<int, Spawner*> Spawner::_spawners;
Spawner::Spawner()
	: _firstAvailable(nullptr)
{
}

Spawner::~Spawner()
{
	clear();
}

Spawner* Spawner::create(int id, const string& desc /*= ""*/)
{
	Spawner* spawner = find(id);
	if (spawner)
	{
		zoo_warning("Spawner already exists[id=%d,desc=%s]!", id, spawner->desc().c_str());
		return nullptr;
	}

	spawner = new Spawner;
	spawner->_id = id;
	spawner->desc() = desc;
	_spawners[id] = spawner;
	return spawner;
}

Spawner* Spawner::find(int id)
{
	auto it = _spawners.find(id);
	if (it != _spawners.end())
		return it->second;
	return nullptr;
}

void Spawner::destroy(Spawner* spawner)
{
	auto it = _spawners.find(spawner->id());
	if (it != _spawners.end())
	{
		_spawners.erase(it);
		delete spawner;
	}
}

Entity* Spawner::born(int id, int breed)
{
	Entity* pEnt = gain(id, breed);
	if (pEnt)
	{
		zoo_warning("Entity already exists[id=%d,breed=%d]!", id, breed);
		return nullptr;
	}

	pEnt = fetch();
	if (!pEnt)
		pEnt = new Entity;

	pEnt->_id = id;
	pEnt->_breed = breed;
	pEnt->_spawner = this;
	_entitiesPool[breed][id] = pEnt;
	return pEnt;
}

Entity* Spawner::gain(int id, int breed)
{
	auto it = _entitiesPool.find(breed);
	if (it != _entitiesPool.end())
	{
		auto itor = it->second.find(id);
		if (itor != it->second.end())
		{
			Entity* pEnt = itor->second;
			if (pEnt && pEnt->_inUse)
				return pEnt;
		}
	}

	return nullptr;
}

void Spawner::awake()
{
	Entity* pEnt;
	auto it = _entitiesPool.begin();
	for (; it != _entitiesPool.end(); ++it)
	{
		unordered_map<int, Entity*>& entMap = it->second;
		auto itEnts = entMap.begin();
		for (; itEnts != entMap.end(); ++itEnts)
		{
			pEnt = itEnts->second;
			if (pEnt)
				pEnt->awake();
		}
	}

	Entity::awake();
}

void Spawner::update()
{
	Entity* pEnt;
	auto it = _entitiesPool.begin();
	for (; it != _entitiesPool.end(); ++it)
	{
		unordered_map<int, Entity*>& entMap = it->second;
		auto itEnts = entMap.begin();
		for (; itEnts != entMap.end(); ++itEnts)
		{
			pEnt = itEnts->second;
			if (pEnt)
				pEnt->update();
		}
	}

	Entity::update();
}

void Spawner::remove(Entity* pEntity, bool bDelete /*= false*/)
{
	if (pEntity)
	{
		if (bDelete)
		{
			_entitiesPool[pEntity->breed()][pEntity->id()] = nullptr;
			delete pEntity;
		}
		else
			discard(pEntity);
	}
}

void Spawner::clear()
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
	_firstAvailable = nullptr;
}

bool Spawner::load(const string& filename)
{
	fstream fout(filename);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
		return false;
	}

	stringstream ss;
	ss << fout.rdbuf();

	CJsonObject sceneJson(ss.str());
	_parser = &sceneJson;
	deserialize(this);

	int entCount = sceneJson["entity"].GetArraySize();
	for (int i = 0; i < entCount; ++i)
	{
		CJsonObject entJson;
		sceneJson["entity"].Get(i, entJson);
		int id, breed;
		entJson.Get("id", id);
		entJson.Get("breed", breed);
		_parser = &entJson;
		born(id, breed)->deserialize(this);
	}

	return true;
}

void Spawner::save(const string& filename)
{
	fstream fout(filename, ios::out | ios::ate);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
		return;
	}

	CJsonObject sceneJson;
	_parser = &sceneJson;
	serialize(this);

	fout << sceneJson.ToFormattedString();
	fout.close();
}

Entity* Spawner::fetch()
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

void Spawner::discard(Entity* pEntity)
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

}
