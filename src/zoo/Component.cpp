#include <zoo/Component.h>
#include <zoo/DatabaseCSV.h>
#include <cJSON/CJsonObject.h>

namespace zoo {

Component::Component()
	: _dirty(0xffffffff)
{		
}

Component::~Component()
{
	delete _imp;
}

Entity::Entity()
	: _id(-1)
	, _breed(-1)
	, _inUse(true)
	, _spawner(nullptr)
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
		return pComponent->_imp;
	return nullptr;
}

Component* Entity::addComponent(string className, string implName /*= ""*/)
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

	pComponent->_imp = nullptr;
	pComponent->_entity = this;
	_components[className] = pComponent;

	if (implName != "")
	{
		ComponentImpl* pComponentImpl = ReflexFactory<>::getInstance().create<ComponentImpl>(implName);
		if (pComponentImpl)
		{
			pComponentImpl->_component = pComponent;
			pComponent->_imp = pComponentImpl;
		}
		else
		{
			zoo_warning("Component implement type \"%s\" does not exist!", implName.c_str());
		}
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

void Entity::notifyComponents()
{
	if (SignalTrigger::hasSignal(*this))
	{
		SignalTrigger::trigger(*this);
		SignalTrigger::disconnect(*this);
		userData().clear();
	}
}

unordered_map<string, Component*> Entity::getComponents() const
{
	return _components;
}

void Entity::awakeAll()
{
	Component* pComponent;
	ComponentImpl* pComponentImpl;
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		pComponent = it->second;
		if (pComponent)
		{
			pComponentImpl = pComponent->_imp;
			if (pComponentImpl)
				pComponentImpl->awake();
		}
	}

	notifyComponents();
}

void Entity::updateAll()
{
	Component* pComponent;
	ComponentImpl* pComponentImpl;
	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		pComponent = it->second;
		if (pComponent)
		{
			pComponentImpl = pComponent->_imp;
			if (pComponentImpl && !pComponent->_dirty.isEmptyState())
			{
				pComponentImpl->update();
				pComponent->_dirty.clearState();
			}
		}
	}

	notifyComponents();
}

void Entity::serialize(Spawner* spawner)
{
	if (!spawner || !spawner->_parser)
		return;

	CJsonObject& entJson = *(CJsonObject*)spawner->_parser;
	if (_id != -1)
		entJson.Add("id", _id);
	else
		entJson.AddNull("id");
	if (_breed != -1)
		entJson.Add("breed", _breed);
	else
		entJson.AddNull("breed");
	if (_desc != "")
		entJson.Add("desc", _desc);
	else
		entJson.AddNull("desc");

	entJson.AddEmptySubArray("component");

	auto it = _components.begin();
	for (; it != _components.end(); ++it)
	{
		CJsonObject comJson;
		spawner->_parser = &comJson;
		comJson.Add("type", it->second->typeName());

		if (it->second->_imp)
			comJson.Add("impl", it->second->_imp->typeName());
		else
			comJson.AddNull("impl");

		it->second->serialize(spawner);
		entJson["component"].Add(comJson);
	}
}

void Entity::deserialize(Spawner* spawner)
{
	if (!spawner || !spawner->_parser)
		return;

	CJsonObject& entJson = *(CJsonObject*)spawner->_parser;
	entJson.Get("desc", _desc);

	int comCount = entJson["component"].GetArraySize();
	for (int i = 0; i < comCount; ++i)
	{
		CJsonObject comJson;
		entJson["component"].Get(i, comJson);
		spawner->_parser = &comJson;
		string type, impl;
		comJson.Get("type", type);
		comJson.Get("impl", impl);
		Component* pCom = addComponent(type, impl);
		if (pCom)
			pCom->deserialize(spawner);
	}
}
//////////////////////////////////////////////////////////////////////////
std::unordered_map<int, Spawner*> Spawner::_spawners;
Spawner::Spawner()
	: _firstAvailable(nullptr)
	, _parser(nullptr)
{
	_spawner = this;
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
	if (breed == -1)
		return find(id);

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

void Spawner::awakeAll()
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
				pEnt->awakeAll();
		}
	}

	Entity::awakeAll();
}

void Spawner::updateAll()
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
				pEnt->updateAll();
		}
	}

	Entity::updateAll();
}

void Spawner::remove(Entity* pEntity, bool bDelete /*= false*/)
{
	if (pEntity && !pEntity->isSpawner())
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
	fstream fout(filename, ios::out);
	if (!fout)
	{
		zoo_error("打开文件[%s]失败", filename.c_str());
		return;
	}

	CJsonObject sceneJson;
	_parser = &sceneJson;
	serialize(this);

	sceneJson.AddEmptySubArray("entity");
	auto it = _entitiesPool.begin();
	auto itEnd = _entitiesPool.end();
	for (; it != itEnd; ++it)
	{
		unordered_map<int, Entity*>& entities = it->second;
		auto itor = entities.begin();
		auto itorEnd = entities.end();
		for (; itor != itorEnd; ++itor)
		{
			CJsonObject entJson;
			_parser = &entJson;
			itor->second->serialize(this);
			sceneJson["entity"].Add(entJson);
		}
	}

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

void Spawner::setNull(const string& key)
{
	CJsonObject* pJsonObj = (CJsonObject*)_parser;
	if (pJsonObj)
		pJsonObj->AddNull(key);
}

void Spawner::setValue(const string& key, const void* val, const char* typeName)
{
	CJsonObject* pJsonObj = (CJsonObject*)_parser;
	if (!pJsonObj)
		return;

	if (ReflexFactory<>::getTypeName(typeid(string).name()) == typeName)
	{
		const string& strval = *(string*)(val);
		if (strval == "")
			pJsonObj->AddNull(key);
		else
			pJsonObj->Add(key, strval);
	}
	else if (ReflexFactory<>::getTypeName(typeid(int32).name()) == typeName)
	{
		int32 ival = *(int32*)(val);
		if (ival == -1)
			pJsonObj->AddNull(key);
		else
			pJsonObj->Add(key, ival);
	}
	else if (ReflexFactory<>::getTypeName(typeid(bool).name()) == typeName)
		pJsonObj->Add(key, *(bool*)(val), true);
	else if (ReflexFactory<>::getTypeName(typeid(float).name()) == typeName)
		pJsonObj->Add(key, *(float*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(double).name()) == typeName)
		pJsonObj->Add(key, *(double*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(uint32).name()) == typeName)
		pJsonObj->Add(key, *(uint32*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(int64).name()) == typeName)
		pJsonObj->Add(key, *(int64*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(uint64).name()) == typeName)
		pJsonObj->Add(key, *(uint64*)(val));
}

void Spawner::getValue(const string& key, void* val, const char* typeName)
{
	CJsonObject* pJsonObj = (CJsonObject*)_parser;
	if (!pJsonObj)
		return;

	if (ReflexFactory<>::getTypeName(typeid(string).name()) == typeName)
		pJsonObj->Get(key, *(string*)val);
	else if (ReflexFactory<>::getTypeName(typeid(int32).name()) == typeName)
		pJsonObj->Get(key, *(int32*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(bool).name()) == typeName)
		pJsonObj->Get(key, *(bool*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(float).name()) == typeName)
		pJsonObj->Get(key, *(float*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(double).name()) == typeName)
		pJsonObj->Get(key, *(double*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(uint32).name()) == typeName)
		pJsonObj->Get(key, *(uint32*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(int64).name()) == typeName)
		pJsonObj->Get(key, *(int64*)(val));
	else if (ReflexFactory<>::getTypeName(typeid(uint64).name()) == typeName)
		pJsonObj->Get(key, *(uint64*)(val));
}

}
