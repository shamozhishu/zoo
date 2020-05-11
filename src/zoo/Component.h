#pragma once

#include <zoo/Log.h>
#include <zoo/Reflection.h>

namespace zoo {

class _zooExport Component : public Type
{
	friend class Entity;
	friend class EntityImpl;
	Entity* _entity;
	unique_ptr<ComponentImpl> _imp;
public:
	virtual Entity* getEntity() const;
	template<typename T>
	T* getEntity()
	{
		return dynamic_cast<T*>(getEntity());
	}
};

class ComponentImpl : public Type
{
	friend class Entity;
	Component* _component;
public:
	virtual Component* getComponent() const { return _component; }
	template<typename T>
	T* getComponent()
	{
		return dynamic_cast<T*>(getComponent());
	}
};

class _zooExport Entity : public Type
{
	ZOO_REFLEX_DECLARE(Entity)
	DISALLOW_COPY_AND_ASSIGN(Entity)
public:
	Entity();
	virtual ~Entity();
	virtual int ID() const { return _id; }
	virtual int Kind() const { return _kind; }

	virtual Component* getComponent(string className);
	template<typename T>
	T* getComponent()
	{
		return dynamic_cast<T*>(getComponent(ReflexFactory<>::getTypeName(typeid(T).name())));
	}

	virtual Component* addComponent(string className);
	template<typename T>
	T* addComponent()
	{
		return dynamic_cast<T*>(addComponent(ReflexFactory<>::getTypeName(typeid(T).name())));
	}

	virtual void removeComponent(string className);
	virtual void removeComponent(Component* pComponent);
	virtual void removeComponents();
	unordered_map<string, Component*> getComponents() const;

public:
	static Entity* create(string className, int id, int kind = 0);
	template<typename T>
	static T* create(int id, int kind = 0)
	{
		return dynamic_cast<T*>(create(ReflexFactory<>::getTypeName(typeid(T).name()), id, kind));
	}

	static unordered_map<int, Entity*> get(int kind = 0);
	static void destroy(Entity* pEntity, bool bDelete = false);
	static void clear();
	
	static Entity* find(int id, int kind = 0);
	template<typename T>
	static T* find(int id, int kind = 0)
	{
		return dynamic_cast<T*>(find(id, kind));
	}

private:
	int _id;
	int _kind;
	unique_ptr<EntityImpl> _imp;
	unordered_map<string, Component*> _components;
	static Entity* fetch(string className);
	static void discard(Entity* pEntity);
	static std::set<Entity*> _uselessEntities;
	static unordered_map<int, unordered_map<int, Entity*>> _entitiesPool;
};

class _zooExport EntityImpl : public Type
{
	friend class Entity;
	Entity* _entity;
public:
	virtual Entity* getEntity() const { return _entity; }
	template<typename T>
	T* getEntity()
	{
		return dynamic_cast<T*>(getEntity());
	}

	virtual ComponentImpl* getComponentImpl(string className);
	template<typename T>
	T* getComponentImpl()
	{
		return dynamic_cast<T*>(getComponentImpl(ReflexFactory<>::getTypeName(typeid(T).name())));
	}
};

}
