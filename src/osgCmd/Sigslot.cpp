#include <osgCmd/Sigslot.h>

namespace osgCmd {

bool SignalTrigger::hasSignal(const Signal& sig)
{
	return !sig._slotset.empty();
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
	vector<Slot*>::const_iterator itSlot = sig._slotset.cbegin();
	for (; itSlot != sig._slotset.cend(); ++itSlot)
	{
		Slot* const &item = *itSlot;
		if (item)
			(*item)(sig);
	}
}

void SignalTrigger::connect(const Signal& sig, SlotFunction::SLOT_FUNCTION func)
{
	OSGCMD_ASSERT(func);
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
	{
		SlotFunction* item = dynamic_cast<SlotFunction*>(*it);
		if (NULL != item && func == item->_function)
		{
			OSG_WARN << "The slot don't support same function in same signal id " << sig.getSignalID() << std::endl;
			return;
		}
	}
	sig._slotset.push_back(new SlotFunction(func));
}

void SignalTrigger::disconnect(const Signal& sig, SlotFunction::SLOT_FUNCTION func)
{
	OSGCMD_ASSERT(func);
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
	{
		SlotFunction* item = dynamic_cast<SlotFunction*>(*it);
		if (NULL != item && func == item->_function)
		{
			SAFE_DELETE(item);
			sig._slotset.erase(it);
			break;
		}
	}
}

void SignalTrigger::disconnect(const Signal& sig)
{
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
		delete *it;
	sig._slotset.clear();
}

}
