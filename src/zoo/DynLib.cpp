#include <zoo/DynLib.h>
#include <zoo/Log.h>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace zoo {

void* DynLib::getSymbol(const string& strName) const
{
	return (void*)DYNLIB_GETSYM(_hInst, strName.c_str());
}

DynLib::DynLib(const string& name)
	: _name(name)
	, _hInst(NULL)
{
}

DynLib::~DynLib()
{
	if (_hInst)
		unload();
}

bool DynLib::load()
{
	zoo_debug("Loading library %s.dll", _name.c_str());
	_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(_name.c_str());
	if (!_hInst)
	{
		zoo_error("Could not load dynamic library %s.dll. System Error:%s", _name.c_str(), dynlibError().c_str());
		return false;
	}
	return true;
}

bool DynLib::unload()
{
	if (_hInst == NULL)
		return false;
	zoo_debug("Unloading library %s.dll", _name.c_str());
	if (DYNLIB_UNLOAD(_hInst))
	{
		_hInst = NULL;
		zoo_error("Could not unload dynamic library %s.dll. System Error:%s", _name.c_str(), dynlibError().c_str());
		return false;
	}
	_hInst = NULL;
	return true;
}

string DynLib::dynlibError()
{
#ifdef _WIN32
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);
	string ret = (char*)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return ret;
#endif
}

}
