#include "WarActivator.h"
#include "WarManager.h"
#include "WarServiceImpl.h"

WarActivator::WarActivator()
	: _warService(nullptr)
{
}

WarActivator::~WarActivator()
{
	SAFE_DELETE(_warService);
	if (WarManager::getSingletonPtr())
		delete WarManager::getSingletonPtr();
}

void WarActivator::start(ctkPluginContext* context)
{
	QString resPath = context->getProperty("PROPS/ctk.war.scene.respath").toString();
	if (resPath.isEmpty())
	{
		throw ctkRuntimeException("Framework property 'ctk.war.scene.respath' containing the resource path of the CTK War Scene not found!");
	}

	new WarManager(resPath.toStdString());
	ctkDictionary props;
	_warService = new WarServiceImpl();
	props.insert(ctkPluginConstants::SERVICE_RANKING, 1);
	context->registerService<WarService>(_warService, props);
}

void WarActivator::stop(ctkPluginContext* context)
{
	SAFE_DELETE(_warService);
	if (WarManager::getSingletonPtr())
		delete WarManager::getSingletonPtr();
}
