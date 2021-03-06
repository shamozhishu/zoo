#include <zoo/Component.h>
#include <cJSON/JsonObject.h>

namespace zoo {

Component::Component()
	: _dirty(0xffffffff)
	, _entity(nullptr)
	, _imp(nullptr)
{
}

Component::~Component()
{
	delete _imp;
}

void ComponentImpl::callback(bool cleanup /*= false*/)
{
	if (SignalTrigger::hasSignal(*this))
	{
		SignalTrigger::trigger(*this);
		if (cleanup)
		{
			SignalTrigger::disconnect(*this);
			userData().clear();
		}
	}
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
	auto it = _components.begin();
	auto itEnd = _components.end();
	for (; it != itEnd; ++it)
		delete it->second;
	_components.clear();
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

	notifyComponents(true);
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

	notifyComponents(true);
}

void Entity::serialize(Spawner* spawner)
{
	if (!spawner || !spawner->_parser)
		return;

	JsonObject& entJson = *(JsonObject*)spawner->_parser;
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
		JsonObject comJson;
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

	JsonObject& entJson = *(JsonObject*)spawner->_parser;
	entJson.Get("desc", _desc);

	int comCount = entJson["component"].GetArraySize();
	for (int i = 0; i < comCount; ++i)
	{
		JsonObject comJson;
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

void Entity::removeComponent(string className)
{
	if (className == _spawner->_fixedCom)
	{
		zoo_warning("%s component cannot be removed!", _spawner->_fixedCom.c_str());
		return;
	}

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
	while (it != _components.end())
	{
		if (it->first == _spawner->_fixedCom)
		{
			++it;
			continue;
		}

		delete it->second;
		it = _components.erase(it);
	}
}

void Entity::notifyComponents(bool cleanup/* = true*/)
{
	if (SignalTrigger::hasSignal(*this))
	{
		SignalTrigger::trigger(*this);
		if (cleanup)
		{
			SignalTrigger::disconnect(*this);
			userData().clear();
		}
	}
}

unordered_map<string, Component*> Entity::getComponents() const
{
	return _components;
}
//////////////////////////////////////////////////////////////////////////
std::unordered_map<int, Spawner*> Spawner::_spawners;
Spawner::Spawner()
	: _firstAvailable(nullptr)
	, _parser(nullptr)
	, _relateContext(nullptr)
{
	_spawner = this;
}

Spawner::~Spawner()
{
	clear();
}

Spawner* Spawner::create(int id, int breed, const string& fixedCom, const string& context /*= ""*/, const string& desc /*= ""*/)
{
	Spawner* spawner = find(id);
	if (spawner)
	{
		zoo_warning("Spawner already exists[id=%d,desc=%s]!", id, spawner->desc().c_str());
		return nullptr;
	}

	spawner = new Spawner;
	spawner->_id = id;
	spawner->_breed = breed;
	spawner->desc() = desc;
	spawner->_fixedCom = fixedCom;
	spawner->addComponent(fixedCom, fixedCom + "Impl")->getImp()->awake();

	spawner->_relateContext = ReflexFactory<>::getInstance().create<Context>(context);
	if (spawner->_relateContext)
		spawner->_relateContext->_spawner = spawner;

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

void Spawner::destroy(int id)
{
	auto it = _spawners.find(id);
	if (it != _spawners.end())
	{
		Context* context = it->second->_relateContext;
		delete it->second;
		_spawners.erase(it);
		delete context;
	}
}

void Spawner::destroy(Spawner* spawner)
{
	destroy(spawner->id());
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
	pEnt->addComponent(_fixedCom, _fixedCom + "Impl")->getImp()->awake();
	_entitiesPool[breed][id] = pEnt;
	return pEnt;
}

Entity* Spawner::gain(int id, int breed)
{
	Spawner* pSpawner = find(id);
	if (pSpawner && pSpawner->breed() == breed)
		return pSpawner;

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

	JsonObject sceneJson;
	if (!sceneJson.Parse(ss.str()))
		return false;

	_parser = &sceneJson;
	deserialize(this);

	int entCount = sceneJson["entity"].GetArraySize();
	for (int i = 0; i < entCount; ++i)
	{
		JsonObject entJson;
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

	JsonObject sceneJson;
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
			Entity* pEnt = itor->second;
			if (!pEnt)
				continue;
			JsonObject entJson;
			_parser = &entJson;
			pEnt->serialize(this);
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
	JsonObject* pJsonObj = (JsonObject*)_parser;
	if (pJsonObj)
		pJsonObj->AddNull(key);
}

void Spawner::setValue(const string& key, const void* val, string typeName)
{
	JsonObject* pJsonObj = (JsonObject*)_parser;
	if (!pJsonObj)
		return;

	if (typeName.substr(0, 6) == "const ")
		typeName = typeName.substr(6);

	if (typeName == typeid(string).name())
	{
		const string& strval = *(string*)(val);
		if (strval == "")
			pJsonObj->AddNull(key);
		else
			pJsonObj->Add(key, strval);
	}
	else if (typeName == typeid(int32).name())
	{
		int32 ival = *(int32*)(val);
		if (ival == -1)
			pJsonObj->AddNull(key);
		else
			pJsonObj->Add(key, ival);
	}
	else if (typeName == typeid(bool).name())
		pJsonObj->Add(key, *(bool*)(val), true);
	else if (typeName == typeid(float).name())
		pJsonObj->Add(key, *(float*)(val));
	else if (typeName == typeid(double).name())
		pJsonObj->Add(key, *(double*)(val));
	else if (typeName == typeid(uint32).name()
		|| string(typeName).substr(0, 5) == "enum ")
		pJsonObj->Add(key, *(uint32*)(val));
	else if (typeName == typeid(int64).name())
		pJsonObj->Add(key, *(int64*)(val));
	else if (typeName == typeid(uint64).name())
		pJsonObj->Add(key, *(uint64*)(val));
}

void Spawner::getValue(const string& key, void* val, string typeName)
{
	JsonObject* pJsonObj = (JsonObject*)_parser;
	if (!pJsonObj)
		return;

	if (typeName.substr(0, 6) == "const ")
		typeName = typeName.substr(6);

	if (typeName == typeid(string).name())
		pJsonObj->Get(key, *(string*)val);
	else if (typeName == typeid(int32).name())
		pJsonObj->Get(key, *(int32*)(val));
	else if (typeName == typeid(bool).name())
		pJsonObj->Get(key, *(bool*)(val));
	else if (typeName == typeid(float).name())
		pJsonObj->Get(key, *(float*)(val));
	else if (typeName == typeid(double).name())
		pJsonObj->Get(key, *(double*)(val));
	else if (typeName == typeid(uint32).name() || typeName.substr(0, 5) == "enum ")
		pJsonObj->Get(key, *(uint32*)(val));
	else if (typeName == typeid(int64).name())
		pJsonObj->Get(key, *(int64*)(val));
	else if (typeName == typeid(uint64).name())
		pJsonObj->Get(key, *(uint64*)(val));
}

}
