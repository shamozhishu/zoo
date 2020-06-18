#pragma once

#include <zoo/Log.h>
#include <zoo/Sigslot.h>
#include <zoo/BitState.h>
#include <zoo/Reflection.h>

namespace zoo {

class _zooExport Spawner
{
	PROPERTY_R(TableCSV*, _table, Table)
public:
	Spawner() : _breed(0), _table(nullptr) {}
	Spawner(int breed) : _breed(breed), _table(nullptr) {}
	Spawner(int breed, TableCSV* table) : _breed(breed), _table(table) {}
	operator int() { return _breed; }
	Spawner& operator()(int breed) { _breed = breed; return *this; }
	Spawner& operator()(int breed, TableCSV* table) { _breed = breed; _table = table; return *this; }
	stringstream& ss() { return _ss; }
	Entity* new_Entity(int id, const string& desc = "");
	Entity* find_Entity(int id);
	unordered_map<int, Entity*> gain_Entities();

	void awake();
	void load();
	void save(string filename);

protected:
	int _breed;
	stringstream _ss;
};

class Serializer
{
public:
	virtual void serialize(Spawner* spawner) = 0;
	virtual void deserialize(Spawner* spawner) = 0;
	virtual void serializeField(Spawner* spawner) = 0;
	virtual void serializeHeader(Spawner* spawner) = 0;
};

class _zooExport Component : public Serializer, public Type
{
	friend class Entity;
	Entity* _entity;
protected:
	BitState _dirty;
	unique_ptr<ComponentImpl> _imp;
public:
	Entity* getEntity() const;
};

class _zooExport ComponentImpl : public Type
{
	friend class Entity;
	Component* _component;
public:
	virtual void awake() {}
	virtual void update() {}
	Entity* getEntity() const { return _component->getEntity(); }
	Component* getComponent() const { return _component; }
	template<typename T>
	T* getComponent()
	{
		return dynamic_cast<T*>(getComponent());
	}
};

class _zooExport Entity : public Serializer
{
	DISALLOW_COPY_AND_ASSIGN(Entity)
	Entity();
	~Entity();
	friend class Spawner;
	friend class ComponentImpl;
public:
	int ID() const { return _id; }
	int Breed() const { return _breed; }
	string& Description() { return _desc; }
	const string& Description() const { return _desc; }

	Component* getComponent(string className);
	template<typename T>
	T* getComponent()
	{
		return dynamic_cast<T*>(getComponent(ReflexFactory<>::getTypeName(typeid(T).name())));
	}

	ComponentImpl* getComponentImpl(string className);
	template<typename T>
	T* getComponentImpl()
	{
		return dynamic_cast<T*>(getComponentImpl(ReflexFactory<>::getTypeName(typeid(T).name())));
	}

	Component* addComponent(string className);
	template<typename T>
	T* addComponent()
	{
		return dynamic_cast<T*>(addComponent(ReflexFactory<>::getTypeName(typeid(T).name())));
	}

	void removeComponent(string className);
	void removeComponent(Component* pComponent);
	void removeComponents();
	unordered_map<string, Component*> getComponents() const;

public:
	void awake();
	void update();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);
	void serializeField(Spawner* spawner);
	void serializeHeader(Spawner* spawner);

	Signal _AWAKED;
	Signal _UPDATED;
public:
	static void destroy(Entity* pEntity, bool bDelete = false);
	static void clear();

private:
	union
	{
		// 使用时的状态
		struct
		{
			int _id;
			int _breed;
			string _desc;
		};

		// 可重用时的状态
		Entity* _next;
	};
	bool _inUse;
	unordered_map<string, Component*> _components;

	static Entity* fetch();
	static Entity* find(int id, int breed);
	static void discard(Entity* pEntity);
	static const unordered_map<int, Entity*>& gain(int breed);

	static Entity* _firstAvailable; // 空闲列表
	static unordered_map<int, unordered_map<int, Entity*>> _entitiesPool;
};

}
