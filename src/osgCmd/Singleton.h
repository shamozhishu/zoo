#pragma once

#include <osgCmd/Common.h>

namespace osgCmd {

template<typename T>
class Singleton
{
public:
	Singleton(void)
	{
		OSGCMD_ASSERT(!_singleton);
		_singleton = static_cast<T*>(this);
	}

	~Singleton(void)
	{
		OSGCMD_ASSERT(_singleton);
		_singleton = NULL;
	}

	static T& getSingleton(void)
	{
		OSGCMD_ASSERT(_singleton);
		return (*_singleton);
	}

	static T* getSingletonPtr(void)
	{
		return _singleton;
	}

private:
	static T* _singleton;
	DISALLOW_COPY_AND_ASSIGN(Singleton<T>)
};

template<typename T>
T* Singleton<T>::_singleton = nullptr;

}
