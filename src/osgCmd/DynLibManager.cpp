#include <osgCmd/DynLibManager.h>
#include <osgCmd/DynLib.h>

namespace osgCmd {

DynLibManager::DynLibManager()
{
}

DynLibManager::~DynLibManager()
{
	map<string, DynLib*>::iterator it = _dynLibList.begin();
	for (; it != _dynLibList.end(); ++it)
	{
		it->second->unload();
		delete it->second;
	}
	_dynLibList.clear();
}

DynLib* DynLibManager::load(const string& filename)
{
	map<string, DynLib*>::iterator it = _dynLibList.find(filename);
	if (it != _dynLibList.end())
		return it->second;

	DynLib* pLib = new DynLib(filename);
	if (pLib->load())
	{
		_dynLibList[pLib->getName()] = pLib;
		return pLib;
	}

	SAFE_DELETE(pLib);
	return NULL;
}

void DynLibManager::unload(const string& filename)
{
	map<string, DynLib*>::iterator it = _dynLibList.find(filename);
	if (it != _dynLibList.end())
	{
		it->second->unload();
		delete it->second;
		_dynLibList.erase(it);
	}
}

}
