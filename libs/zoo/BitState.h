#ifndef __ZOO_BIT_STATE_H__
#define __ZOO_BIT_STATE_H__

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
	ESTATE_11 = 1 << 10,
	ESTATE_12 = 1 << 11,
	ESTATE_13 = 1 << 12,
	ESTATE_14 = 1 << 13,
	ESTATE_15 = 1 << 14,
	ESTATE_16 = 1 << 15,
	ESTATE_17 = 1 << 16,
	ESTATE_18 = 1 << 17,
	ESTATE_19 = 1 << 18,
	ESTATE_20 = 1 << 19,
	ESTATE_21 = 1 << 20,
	ESTATE_22 = 1 << 21,
	ESTATE_23 = 1 << 22,
	ESTATE_24 = 1 << 23,
	ESTATE_25 = 1 << 24,
	ESTATE_26 = 1 << 25,
	ESTATE_27 = 1 << 26,
	ESTATE_28 = 1 << 27,
	ESTATE_29 = 1 << 28,
	ESTATE_30 = 1 << 29,
	ESTATE_31 = 1 << 30,
	ESTATE_32 = 1 << 31
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

#endif // __ZOO_BIT_STATE_H__
