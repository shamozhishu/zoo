#include "WarActivator.h"
#include "WarManager.h"
#include "WarServiceImpl.h"
#include <zoo/Log.h>

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
	QString resPath = context->getProperty("ctk.war.scene.respath").toString();
	QString mainTable = context->getProperty("ctk.war.scene.maintable").toString();
	if (resPath.isEmpty())
	{
		const char* szTip = "Framework property \"ctk.war.scene.respath\" containing the resource path of the CTK War Scene not found!";
		zoo::Log::print(zoo::ELL_ERROR, szTip);
		throw ctkRuntimeException(szTip);
	}

	if (mainTable.isEmpty())
	{
		const char* szTip = "Framework property \"ctk.war.scene.maintable\" containing the main table name of the CTK War Scene not found!";
		zoo::Log::print(zoo::ELL_ERROR, szTip);
		throw ctkRuntimeException(szTip);
	}

	new WarManager(resPath.toStdString(), mainTable.toStdString());
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
