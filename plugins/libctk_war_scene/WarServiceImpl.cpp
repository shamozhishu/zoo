#include "WarServiceImpl.h"
#include "WarManager.h"

WarServiceImpl::WarServiceImpl()
{
}

WarServiceImpl::~WarServiceImpl()
{
}

bool WarServiceImpl::openScene(int sceneId)
{
	return WarManager::getSingleton().enterScene(sceneId);
}

void WarServiceImpl::closeScene(int sceneId)
{
	WarManager::getSingleton().exitScene(sceneId);
}