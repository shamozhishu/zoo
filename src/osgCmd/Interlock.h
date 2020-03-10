#pragma once

#include <osgCmd/Support.h>

namespace osgCmd {

struct Interlock
{
	bool canExchange;
	bool needExchange;
	Interlock() : canExchange(false), needExchange(false) {}

	inline void beginExchanging()
	{
		needExchange = true;
		while (!canExchange)
			OpenThreads::Thread::microSleep(1);
	}

	inline void endExchanging()
	{
		needExchange = false;
	}

	inline void waitExchanged()
	{
		while (needExchange && canExchange)
			OpenThreads::Thread::microSleep(1);
		canExchange = false;
	}

	inline void releaseWait()
	{
		if (needExchange)
			canExchange = true;
	}
};

}
