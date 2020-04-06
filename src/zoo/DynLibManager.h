#pragma once

#include <zoo/Support.h>

namespace zoo {

class _zooExport DynLibManager
{
	DISALLOW_COPY_AND_ASSIGN(DynLibManager)
public:
	DynLibManager();
	virtual ~DynLibManager();
	DynLib* load(const string& filename);
	void    unload(const string& filename);

protected:
	map<string, DynLib*> _dynLibList;
};

}
