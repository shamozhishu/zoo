#include <zoo/ServiceLocator.h>
#include <zoo/Reflection.h>

namespace zoo {

unordered_map<string, Service*> Service::s_services;
Service* Service::locate(const char* type, const char* filter /*= ""*/)
{
	auto it = s_services.find(ReflexFactory<>::getTypeName(type));
	if (it != s_services.end())
	{
		Service* pService = it->second;
		if (strcmp(filter, "") == 0 || pService->getFilter() == ReflexFactory<>::getTypeName(filter))
			return pService;
	}

	return nullptr;
}

Service::Service(const char* type, const char* filter)
	: _filter(filter)
{
	s_services[type] = this;
}

Service::~Service()
{
	auto it = s_services.begin();
	for (; it != s_services.end(); ++it)
	{
		if (it->second == this)
		{
			s_services.erase(it);
			break;
		}
	}
}

}
