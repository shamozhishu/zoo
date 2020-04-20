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

void WarServiceImpl:: saveScene()
{
	WarCommander::getSingleton().saveCurBattlefield();
}
