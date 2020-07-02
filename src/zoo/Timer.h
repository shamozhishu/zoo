#ifndef __ZOO_TIMER_H__
#define __ZOO_TIMER_H__

#include <chrono>

using namespace std;
using namespace std::chrono;

namespace zoo {

class Timer
{
public:
	Timer() : _start(high_resolution_clock::now()) {}
	void reset() { _start = high_resolution_clock::now(); }

	// ∫¡√Î
	template<typename Duration = milliseconds>
	int64_t elapsed() const
	{
		return duration_cast<Duration>(high_resolution_clock::now() - _start).count();
	}
	// Œ¢√Î
	int64_t elapsed_micro() const
	{
		return elapsed<microseconds>();
	}
	// ƒ…√Î
	int64_t elapsed_nano()const
	{
		return elapsed<nanoseconds>();
	}

	int64_t elapsed_seconds()const
	{
		return elapsed<seconds>();
	}

	int64_t elapsed_minutes()const
	{
		return elapsed<minutes>();
	}

	int64_t elasped_hours()const
	{
		return elapsed<hours>();
	}

private:
	time_point<high_resolution_clock> _start;
};

}

#endif // __ZOO_TIMER_H__
