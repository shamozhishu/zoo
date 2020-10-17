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
	virtual void serialize(Spawner* spawner) {}
	virtual void deserialize(Spawner* spawner) {}
};

struct _zooExport Component : public Serializer, public Type
{
	Component();
	virtual ~Component();
	Entity* getEntity() const { return _entity; }
	ComponentImpl* getImp() const { return _imp; }
	BitState& dirtyBit() { return _dirty; }
	const BitState& dirtyBit() const { return _dirty; }
protected:
	Entity* _entity;
	BitState _dirty;
	ComponentImpl* _imp;
	friend class Entity;
};

// 基类Signal用于自定义函数的注册与调用, 解决组件类在不清楚组件实现类细节的情况下进行组件类到组件实现类的单向通信的要求
class _zooExport ComponentImpl : public Type, public Signal
{
	friend class Entity;
	Component* _component;
public:
	virtual void awake() {}
	virtual void update() {}
	virtual void callback(bool cleanup = false);
	Entity* getEntity() const { return _component->getEntity(); }
	Component* getComponent() const { return _component; }
	template<typename T>
	T* getComponent()
	{
		return dynamic_cast<T*>(getComponent());
	}
};

// 基类Signal用于Entity和Spawner在awakeAll与updateAll之后的对注册函数的调用, 解决组件之间或者实体之间交互时对时序耦合的依赖导致的问题
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

	template<typename T, typename ...Args>
	T* addComponent(string implName = "", Args... args)
	{
		return dynamic_cast<T*>(addComponent(ReflexFactory<Args...>::getTypeName(typeid(T).name()), implName, args...));
	}

	template<typename ...Args>
	Component* addComponent(string className, string implName, Args... args)
	{
		Component* pComponent = getComponent(className);
		if (pComponent)
			return pComponent;

		pComponent = ReflexFactory<Args...>::getInstance().create<Component>(className, args...);
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

	void removeComponent(string className);
	void removeComponent(Component* pComponent);
	void removeComponents();
	void notifyComponents(bool cleanup = true);
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
	class Context : public Type { public: virtual ~Context() {} };
	static Spawner* create(int id, const string& context, const string& desc = "");
	static Spawner* find(int id);
	static void destroy(int id);
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
	template<typename T> T* getContext() { return dynamic_cast<T*>(_relateContext); }
	template<typename T> void setValue(const string& key, const T& value) { setValue(key, &value, typeid(value).name()); }
	template<typename T> void getValue(const string& key, T& value) { getValue(key, (void*)&value, typeid(value).name()); }

private:
	Entity* fetch();
	void discard(Entity* pEntity);
	void setValue(const string& key, const void* val, string typeName);
	void getValue(const string& key, void* val, string typeName);

private:
	void* _parser;
	Entity* _firstAvailable;
	Context* _relateContext;
	static unordered_map<int, Spawner*> _spawners;
	unordered_map<int, unordered_map<int, Entity*>> _entitiesPool;
};

}

#endif // __ZOO_COMPONENT_H__
