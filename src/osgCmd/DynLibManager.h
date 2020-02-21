#pragma once

#include <osgCmd/Support.h>

namespace osgCmd {

class _osgCmdExport DynLibManager
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
