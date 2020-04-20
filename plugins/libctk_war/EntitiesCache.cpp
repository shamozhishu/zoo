#include "EntitiesCache.h"
#include "Entity.h"
#include "InstanceReleaser.h"

EntitiesCache* s_entitiesCache = nullptr;
EntitiesCache::EntitiesCache()
{
}

EntitiesCache::~EntitiesCache()
{
	Entity* pEnt = nullptr;
	auto it = _uselessEntities.begin();
	for (; it != _uselessEntities.end(); ++it)
	{
		pEnt = *it;
		delete pEnt;
	}

	_uselessEntities.clear();
}

EntitiesCache& EntitiesCache::getIns()
{
	if (!s_entitiesCache)
	{
		s_entitiesCache = new EntitiesCache;
		InstanceReleaser::addIns(EntitiesCache::destroy);
	}

	return *s_entitiesCache;
}

void EntitiesCache::destroy()
{
	delete s_entitiesCache;
	s_entitiesCache = nullptr;
}

void EntitiesCache::putIn(Entity* pEnt)
{
	auto it = std::find(_uselessEntities.begin(), _uselessEntities.end(), pEnt);
	if (it == _uselessEntities.end())
		_uselessEntities.push_back(pEnt);
}

Entity* EntitiesCache::getOut(ENTITY_TYPE type)
{
	Entity* pEnt = nullptr;
	auto it = _uselessEntities.begin();
	for (; it != _uselessEntities.end(); ++it)
	{
		pEnt = *it;
		if (pEnt->getType() == type)
		{
			_uselessEntities.erase(it);
			return pEnt;
		}
	}

	return nullptr;
}
