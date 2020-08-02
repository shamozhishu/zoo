#ifndef __ZOO_SERVICE_LOCATOR_H__
#define __ZOO_SERVICE_LOCATOR_H__

#include <zoo/Support.h>

namespace zoo {

class _zooExport Service
{
	PROPERTY_R(string, _filter, Filter)
public:
	virtual ~Service();
	template<typename T> T* as() { return dynamic_cast<T*>(this); }

protected:
	Service(const char* type, const char* filter);

private:
	static unordered_map<string, Service*> s_services;
	static Service* locate(const char* type, const char* filter = "");
	template<typename T> friend class Provider;
	template<typename T> friend class ServiceLocator;
};

template<typename SUBCLASS>
class Provider
{
protected:
	template<typename SERVICE>
	SERVICE* getService()
	{
		return dynamic_cast<SERVICE*>(Service::locate(typeid(SERVICE).name(), typeid(SUBCLASS).name()));
	}
};

template<typename SERVICE>
class ServiceLocator
{
public:
	static SERVICE* getService()
	{
		return dynamic_cast<SERVICE*>(Service::locate(typeid(SERVICE).name()));
	}
};

}

#endif // __ZOO_SERVICE_LOCATOR_H__
