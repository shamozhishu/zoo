#include "WarServiceImpl.h"
#include "WarManager.h"

WarServiceImpl::WarServiceImpl()
{
}

WarServiceImpl::~WarServiceImpl()
{
}

bool WarServiceImpl::enterScene(int sceneId)
{
	return WarManager::getSingleton().enterScene(sceneId);
}

void WarServiceImpl::exitScene(int sceneId)
{
	WarManager::getSingleton().exitScene(sceneId);
}
