#pragma once

namespace zoo {

enum
{
	ESTATE_00 = 0,
	ESTATE_01 = 1 << 0,
	ESTATE_02 = 1 << 1,
	ESTATE_03 = 1 << 2,
	ESTATE_04 = 1 << 3,
	ESTATE_05 = 1 << 4,
	ESTATE_06 = 1 << 5,
	ESTATE_07 = 1 << 6,
	ESTATE_08 = 1 << 7,
	ESTATE_09 = 1 << 8,
	ESTATE_10 = 1 << 9,
};

class BitState
{
public:
	BitState() :_bitSet(0u) {}
	BitState(unsigned int state) :_bitSet(state) {}
	~BitState() {}

	unsigned int operator()() const
	{
		return _bitSet;
	}

	void operator()(unsigned int state)
	{
		_bitSet = state;
	}

	void addState(unsigned int state)
	{
		_bitSet |= state;
	}

	bool checkState(unsigned int state) const
	{
		return (_bitSet & state) == state;
	}

	void eraseState(unsigned int state)
	{
		_bitSet &= ~state;
	}

	bool isEmptyState() const
	{
		return 0u == _bitSet;
	}

	void clearState()
	{
		_bitSet = 0u;
	}

private:
	unsigned int _bitSet;
};

}
