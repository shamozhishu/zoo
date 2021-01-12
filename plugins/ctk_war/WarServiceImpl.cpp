#include "WarServiceImpl.h"
#include "WarCommander.h"

WarServiceImpl::WarServiceImpl()
	: _simuState(null_)
{
}

bool WarServiceImpl::openScene(int id)
{
	bool succ = WarCommander::getSingleton().enterBattlefield(id);
	if (succ)
	{
		WarSimulator* pWarSimulator = WarCommander::getSingleton().getCurBattlefield();
		if (pWarSimulator)
		{
			_simuState = ready_;
			pWarSimulator->transition(ready_);
		}
	}
	else
	{
		WarSimulator* pWarSimulator = WarCommander::getSingleton().getCurBattlefield();
		if (pWarSimulator)
		{
			_simuState = null_;
			pWarSimulator->transition(null_);
		}
	}

	return succ;
}

void WarServiceImpl::closeScene()
{
	WarSimulator* pWarSimulator = WarCommander::getSingleton().getCurBattlefield();
	if (pWarSimulator)
	{
		_simuState = null_;
		pWarSimulator->transition(null_);
	}

	WarCommander::getSingleton().exitCurBattlefield();
}

void WarServiceImpl:: saveScene()
{
	WarCommander::getSingleton().saveCurBattlefield();
}

bool WarServiceImpl::startSimulation()
{
	WarSimulator* pWarSimulator = WarCommander::getSingleton().getCurBattlefield();
	if (pWarSimulator)
	{
		switch (_simuState)
		{
		case ready_:
		case paused_:
			_simuState = running_;
			pWarSimulator->transition(running_);
			return true;
		default:
			break;
		}
	}

	return false;
}

bool WarServiceImpl::pauseSimulation()
{
	WarSimulator* pWarSimulator = WarCommander::getSingleton().getCurBattlefield();
	if (pWarSimulator)
	{
		switch (_simuState)
		{
		case running_:
			_simuState = paused_;
			pWarSimulator->transition(paused_);
			return true;
		default:
			break;
		}
	}

	return false;
}

bool WarServiceImpl::stopSimulation()
{
	WarSimulator* pWarSimulator = WarCommander::getSingleton().getCurBattlefield();
	if (pWarSimulator)
	{
		switch (_simuState)
		{
		case running_:
		case paused_:
			_simuState = ready_;
			pWarSimulator->transition(ready_);
			return true;
		default:
			break;
		}
	}

	return false;
}
