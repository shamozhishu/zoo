#pragma once

#include <zoo/Support.h>

#if defined(SHAREDOBJECT_NONCLIENT_BUILD)
#	define SharedObject_Export __declspec(dllexport)
#else
#	if defined(__MINGW32__)
#		define SharedObject_Export
#	else
#		define SharedObject_Export __declspec(dllimport)
#	endif
#endif

class SharedObject_Export SharedObject
{
	PROPERTY_R(int, _id, ID)
	PROPERTY_R(int, _group, Group)
	DISALLOW_COPY_AND_ASSIGN(SharedObject)
protected:
	SharedObject(int id, int group);
	virtual ~SharedObject();
};

class SharedObject_Export SharedObjectPool
{
	friend class SharedObject;
	static int const GroupCount = 10;
	static std::vector<SharedObject*> _objectList;
	static std::map<int, SharedObject*> _objectMap[GroupCount];

public:
	static std::vector<SharedObject*> getObjectList();
	static std::map<int, SharedObject*> getObjectMap(int group);
	static SharedObject* findObject(int id, int group);
	template<typename T>
	static T* findObject(int id, int group)
	{
		return dynamic_cast<T*>(findObject(id, group));
	}
};
