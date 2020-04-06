#pragma once

class InstanceReleaser
{
public:
	static void addIns(void(*func)());
	static void destroyIns();
};
