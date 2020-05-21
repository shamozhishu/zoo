#include "WarServiceImpl.h"
#include "WarCommander.h"

WarServiceImpl::WarServiceImpl()
{
}

WarServiceImpl::~WarServiceImpl()
{
}

bool WarServiceImpl::openScene(int id)
{
	return WarCommander::getSingleton().enterBattlefield(id);
}

void WarServiceImpl::closeScene()
{
	WarCommander::getSingleton().exitCurBattlefield();
}

void WarServiceImpl:: saveScene()
{
	WarCommander::getSingleton().saveCurBattlefield();
}

void WarServiceImpl::startSimulation()
{

}

void WarServiceImpl::stopSimulation()
{

}
