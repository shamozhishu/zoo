#include <zoo/Sigslot.h>

namespace zoo {

bool SignalTrigger::hasSignal(const Signal& sig)
{
	return !sig._slotList.empty();
}

Signal::Signal()
{
	static int idx = 0;
	_sigid = idx++;
}

Signal::~Signal()
{
	SignalTrigger::disconnect(*this);
}

void SignalTrigger::trigger(const Signal& sig)
{
	vector<Slot*>::const_iterator itSlot = sig._slotList.cbegin();
	for (; itSlot != sig._slotList.cend(); ++itSlot)
	{
		Slot* const &item = *itSlot;
		if (item)
			(*item)(sig);
	}
}

void SignalTrigger::connect(const Signal& sig, std::function<void()> lamb)
{
	sig._slotList.push_back(new SlotLambda(lamb));
}

void SignalTrigger::connect(const Signal& sig, SlotFunction::SLOT_FUNCTION func)
{
	ZOO_ASSERT(func);
	vector<Slot*>::const_iterator it = sig._slotList.cbegin();
	for (; it != sig._slotList.cend(); ++it)
	{
		SlotFunction* item = dynamic_cast<SlotFunction*>(*it);
		if (NULL != item && func == item->_function)
		{
			zoo_warning("The slot don't support same function in same signal id %d", sig.getSignalID());
			return;
		}
	}
	sig._slotList.push_back(new SlotFunction(func));
}

void SignalTrigger::disconnect(const Signal& sig, SlotFunction::SLOT_FUNCTION func)
{
	ZOO_ASSERT(func);
	vector<Slot*>::const_iterator it = sig._slotList.cbegin();
	for (; it != sig._slotList.cend(); ++it)
	{
		SlotFunction* item = dynamic_cast<SlotFunction*>(*it);
		if (NULL != item && func == item->_function)
		{
			SAFE_DELETE(item);
			sig._slotList.erase(it);
			break;
		}
	}
}

void SignalTrigger::disconnect(const Signal& sig)
{
	vector<Slot*>::const_iterator it = sig._slotList.cbegin();
	for (; it != sig._slotList.cend(); ++it)
		delete *it;
	sig._slotList.clear();
}

}
