#ifndef __ZOO_REFLECTION_H__
#define __ZOO_REFLECTION_H__

#include <zoo/Support.h>

namespace zoo {

class _zooExport Type
{
public:
	virtual ~Type() {}
	virtual string typeName() const;
protected:
	Type() {}
};

template<typename ...Args>
class ReflexFactory
{
public:
	static ReflexFactory& getInstance()
	{
		static ReflexFactory factory;
		return factory;
	}

	static string getTypeName(const char* name)
	{
#ifdef __GNUC__
		char* real_name = abi::__cxa_demangle(name, nullptr, nullptr, nullptr);
		std::string real_name_string(real_name);
		free(real_name);
		return real_name_string;
#else
		const char* p = strstr(name, " ");
		if (p)
		{
			size_t prev_len = (size_t)(p - name);
			if (memcmp(name, "class", prev_len) == 0 ||
				memcmp(name, "struct", prev_len) == 0 ||
				memcmp(name, "enum", prev_len) == 0 ||
				memcmp(name, "union", prev_len) == 0)
			{
				p += 1;
				return std::string(p);
			}
		}
		return std::string(name);
#endif
	}

	Type* create(const string& typeName, Args... args)
	{
		if (typeName.empty())
			return NULL;
		typename unordered_map<string, function<Type*(Args...)>>::iterator it = _createFuncMap.find(typeName);
		if (it == _createFuncMap.end())
			return NULL;
		return it->second(args...);
	}

	template<typename T>
	T* create(const string& typeName, Args... args)
	{
		Type* pTypeObject = create(typeName, args...);
		if (NULL == pTypeObject)
			return NULL;
		T* pRealObject = dynamic_cast<T*>(pTypeObject);
		if (NULL == pRealObject)
			delete pTypeObject;
		return pRealObject;
	}

	bool enroll(const char* name, function<Type*(Args...)> func)
	{
		if (NULL == name || 0 == strcmp(name, ""))
			return false;
		string typeName = getTypeName(name);
		return _createFuncMap.insert(make_pair(typeName, func)).second;
	}

	bool unenroll(const char* name)
	{
		if (NULL == name || 0 == strcmp(name, ""))
			return false;
		string typeName = getTypeName(name);
		auto it = _createFuncMap.find(typeName);
		if (it == _createFuncMap.end())
			return false;
		_createFuncMap.erase(it);
		return true;
	}

private:
	unordered_map<string, function<Type*(Args...)>> _createFuncMap;
};

template<typename T, typename ...Args>
class Reflex
{
public:
	static Type* dynCreate(Args... args)
	{
		return new T(args...);
	}
	Reflex()
	{
		if (!ReflexFactory<Args...>::getInstance().enroll(typeid(T).name(), dynCreate))
		{
			ZOO_ASSERT(false && "ReflexFactory.enroll() has failed!");
		}
	}
	~Reflex()
	{
		if (!ReflexFactory<Args...>::getInstance().unenroll(typeid(T).name()))
		{
			ZOO_ASSERT(false && "ReflexFactory.unenroll() has failed!");
		}
	}
};

}

#endif // __ZOO_REFLECTION_H__
