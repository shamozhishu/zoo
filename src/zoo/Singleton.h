#ifndef __ZOO_SINGLETON_H__
#define __ZOO_SINGLETON_H__

#include <zoo/Common.h>

namespace zoo {

template<typename T>
class Singleton
{
public:
	Singleton(void)
	{
		ZOO_ASSERT(!_singleton);
		_singleton = static_cast<T*>(this);
	}

	~Singleton(void)
	{
		ZOO_ASSERT(_singleton);
		_singleton = NULL;
	}

	static T& getSingleton(void)
	{
		ZOO_ASSERT(_singleton);
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

#endif // __ZOO_SINGLETON_H__
