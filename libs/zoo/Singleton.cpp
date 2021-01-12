#include <zoo/Singleton.h>

namespace zoo {

void InstanceReleaser::addIns(std::function<void()> func)
{
	_destroyFuncs.push_back(func);
}

void InstanceReleaser::destroyIns()
{
	auto it = _destroyFuncs.begin();
	for (; it != _destroyFuncs.end(); ++it)
		(*it)();
	_destroyFuncs.clear();
}

}
