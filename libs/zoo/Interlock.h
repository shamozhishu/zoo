#ifndef __ZOO_INTERLOCK_H__
#define __ZOO_INTERLOCK_H__

#include <zoo/Support.h>

namespace zoo {

class Interlock
{
public:
	Interlock() : canExchange(false), needExchange(false) {}

	inline void beginExchanging()
	{
		needExchange = true;
		while (!canExchange)
			std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	inline void endExchanging()
	{
		needExchange = false;
	}

	inline void waitExchanged()
	{
		while (needExchange && canExchange)
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		canExchange = false;
	}

	inline void releaseWait()
	{
		if (needExchange)
			canExchange = true;
	}

	bool canExchange;
	bool needExchange;
};

}

#endif // __ZOO_INTERLOCK_H__
