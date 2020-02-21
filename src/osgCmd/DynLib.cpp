#include <osgCmd/DynLib.h>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace osgCmd {

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
	OSG_NOTICE << "Loading library " << _name.c_str() << ".dll" << std::endl;
	_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(_name.c_str());
	if (!_hInst)
	{
		OSG_FATAL << "Could not load dynamic library " << _name.c_str() << ".dll. System Error:" << dynlibError().c_str() << "(DynLib::load)" << std::endl;
		return false;
	}
	return true;
}

bool DynLib::unload()
{
	if (_hInst == NULL)
		return false;
	OSG_NOTICE << "Unloading library " << _name.c_str() << ".dll" << std::endl;
	if (DYNLIB_UNLOAD(_hInst))
	{
		_hInst = NULL;
		OSG_FATAL << "Could not unload dynamic library " << _name.c_str() << ".dll. System Error:" << dynlibError().c_str() << "(DynLib::unload)" << std::endl;
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
