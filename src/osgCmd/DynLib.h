#pragma once

#include <osgCmd/Support.h>

#ifdef _WIN32
#define DYNLIB_HANDLE       hInstance
#define DYNLIB_LOAD(a)      LoadLibrary(a)
#define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
#define DYNLIB_UNLOAD(a)    !FreeLibrary(a)
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;
#endif

namespace osgCmd {

class _osgCmdExport DynLib
{
public:
	DynLib(const string& name);
	~DynLib();
	bool  load();
	bool  unload();
	const string& getName(void) const { return _name; }
	void* getSymbol(const string& strName) const;

protected:
	string dynlibError();
protected:
	string        _name;
	DYNLIB_HANDLE _hInst;
};

}
