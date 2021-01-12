#ifndef __ZOO_DYN_LIB_MANAGER_H__
#define __ZOO_DYN_LIB_MANAGER_H__

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

#endif // __ZOO_DYN_LIB_MANAGER_H__
