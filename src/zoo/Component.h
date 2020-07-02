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
	unique_ptr<ComponentImpl> _imp;
public:
	Entity* getEntity() const { return _entity; }
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

private:
	void awake();
	void update();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

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
};

class _zooExport Spawner : public Entity
{
	friend class Entity;
	Spawner();
	~Spawner();
	DISALLOW_COPY_AND_ASSIGN(Spawner)
	PROPERTY_R(void*, _parser, Parser)
public:
	static Spawner* create(int id, const string& desc = "");
	static Spawner* find(int id);
	static void destroy(Spawner* spawner);
	Entity* born(int id, int breed);
	Entity* gain(int id, int breed);
	void awake();
	void update();
	void remove(Entity* pEntity, bool bDelete = false);
	void clear();
	bool load(const string& filename);
	void save(const string& filename);

private:
	Entity* fetch();
	void discard(Entity* pEntity);

private:
	Entity* _firstAvailable;
	static unordered_map<int, Spawner*> _spawners;
	unordered_map<int, unordered_map<int, Entity*>> _entitiesPool;
};

}

#endif // __ZOO_COMPONENT_H__
