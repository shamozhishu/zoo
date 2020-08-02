#ifndef __ZOO_COMPONENT_H__
#define __ZOO_COMPONENT_H__

#include <zoo/Log.h>
#include <zoo/Sigslot.h>
#include <zoo/BitState.h>
#include <zoo/Reflection.h>

namespace zoo {

class Serializer
{
public:
	virtual void serialize(Spawner* spawner) = 0;
	virtual void deserialize(Spawner* spawner) = 0;
};

class _zooExport Component : public Serializer, public Type
{
	friend class Entity;
	Entity* _entity;
protected:
	BitState _dirty;
	ComponentImpl* _imp;
public:
	Component();
	virtual ~Component();
	Entity* getEntity() const { return _entity; }
	const BitState& dirtyBit() const { return _dirty; }
	ComponentImpl* getImp() const { return _imp; }
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

class _zooExport Entity : public Serializer, public Signal
{
	friend class Spawner;
	friend class ComponentImpl;
	PROPERTY_R(Spawner*, _spawner, Spawner)
	DISALLOW_COPY_AND_ASSIGN(Entity)
private:
	Entity();
	~Entity();
public:
	int id() const { return _id; }
	int breed() const { return _breed; }
	string& desc() { return _desc; }
	const string& desc() const { return _desc; }
	virtual bool isSpawner() { return false; }

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

	Component* addComponent(string className, string implName = "");
	template<typename T>
	T* addComponent(string implName = "")
	{
		return dynamic_cast<T*>(addComponent(ReflexFactory<>::getTypeName(typeid(T).name()), implName));
	}

	void removeComponent(string className);
	void removeComponent(Component* pComponent);
	void removeComponents();
	void notifyComponents();
	unordered_map<string, Component*> getComponents() const;

private:
	virtual void awakeAll();
	virtual void updateAll();
	virtual void serialize(Spawner* spawner);
	virtual void deserialize(Spawner* spawner);

private:
	union
	{
		// 使用时的状态
		struct
		{
			int _id;
			int _breed;
		};

		// 可重用时的状态
		Entity* _next;
	};
	bool _inUse;
	string _desc;
	unordered_map<string, Component*> _components;
};

class _zooExport Spawner : public Entity
{
	friend class Entity;
	Spawner();
	~Spawner();
	DISALLOW_COPY_AND_ASSIGN(Spawner)
public:
	static Spawner* create(int id, const string& desc = "");
	static Spawner* find(int id);
	static void destroy(Spawner* spawner);
	bool isSpawner() { return true; }
	Entity* born(int id, int breed);
	Entity* gain(int id, int breed);
	void awakeAll();
	void updateAll();
	void remove(Entity* pEntity, bool bDelete = false);
	void clear();
	bool load(const string& filename);
	void save(const string& filename);
	void setNull(const string& key);
	template<typename T>
	void setValue(const string& key, const T& value)
	{
		setValue(key, &value, ReflexFactory<>::getTypeName(typeid(value).name()).c_str());
	}
	template<typename T>
	void getValue(const string& key, T& value)
	{
		getValue(key, &value, ReflexFactory<>::getTypeName(typeid(value).name()).c_str());
	}

private:
	Entity* fetch();
	void discard(Entity* pEntity);
	void setValue(const string& key, const void* val, const char* typeName);
	void getValue(const string& key, void* val, const char* typeName);

private:
	void* _parser;
	Entity* _firstAvailable;
	static unordered_map<int, Spawner*> _spawners;
	unordered_map<int, unordered_map<int, Entity*>> _entitiesPool;
};

}

#endif // __ZOO_COMPONENT_H__
