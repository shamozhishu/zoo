#include "InstanceReleaser.h"
#include <vector>

static std::vector< void(*)()> s_destroy_funcs;
void InstanceReleaser::addIns(void(*func)())
{
	s_destroy_funcs.push_back(func);
}

void InstanceReleaser::destroyIns()
{
	auto it = s_destroy_funcs.begin();
	for (; it != s_destroy_funcs.end(); ++it)
		(*it)();
	s_destroy_funcs.clear();
}
