#pragma once

#include <zoo/Support.h>

namespace zoo {

class Block
{
public:
	Block() : _released(false) {}
	~Block() { release(); }

	inline void block()
	{
		std::unique_lock<std::mutex> mutlock(_mut);
		if (!_released)
			_cond.wait(mutlock);
	}

	inline void block(unsigned long timeout)
	{
		std::unique_lock<std::mutex> mutlock(_mut);
		if (!_released)
			_cond.wait_for(mutlock, std::chrono::seconds(timeout));
	}

	inline void release()
	{
		std::unique_lock<std::mutex> mutlock(_mut);
		if (!_released)
		{
			_released = true;
			_cond.notify_all();
		}
	}

	inline void reset()
	{
		std::unique_lock<std::mutex> mutlock(_mut);
		_released = false;
	}

	inline void set(bool doRelease)
	{
		if (doRelease != _released)
		{
			if (doRelease)
				release();
			else
				reset();
		}
	}

protected:
	bool _released;
	std::mutex _mut;
	std::condition_variable _cond;

private:
	Block(const Block&) = delete;
};

}
