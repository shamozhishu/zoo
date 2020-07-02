#ifndef __INSTANCE_RELEASER_H__
#define __INSTANCE_RELEASER_H__

class InstanceReleaser
{
public:
	static void addIns(void(*func)());
	static void destroyIns();
};

#endif // __INSTANCE_RELEASER_H__
