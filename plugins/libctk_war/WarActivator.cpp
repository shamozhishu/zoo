#include "WarActivator.h"
#include "WarCommander.h"
#include "WarServiceImpl.h"
#include <zoo/Log.h>
#include <zooCmdLoader/ZooCmdLoader.h>

WarActivator::WarActivator()
	: _warService(nullptr)
{
	zooCmdL_Load();
}

WarActivator::~WarActivator()
{
	SAFE_DELETE(_warService);
	if (WarCommander::getSingletonPtr())
		delete WarCommander::getSingletonPtr();
}

void WarActivator::start(ctkPluginContext* context)
{
	QString resPath = context->getProperty("ctk.war.respath").toString();
	QString mainTable = context->getProperty("ctk.war.maintable").toString();
	if (resPath.isEmpty())
	{
		const char* szTip = "Framework property \"ctk.war.respath\" containing the resource path of the CTK War not found!";
		zoo_error("%s", szTip);
		throw ctkRuntimeException(szTip);
	}

	if (mainTable.isEmpty())
	{
		const char* szTip = "Framework property \"ctk.war.maintable\" containing the main table name of the CTK War not found!";
		zoo_error("%s", szTip);
		throw ctkRuntimeException(szTip);
	}

	zoo_debug("ctk.war.respath=%s", resPath.toStdString().c_str());
	zoo_debug("ctk.war.maintable=%s", mainTable.toStdString().c_str());
	new WarCommander(resPath.toStdString(), mainTable.toStdString());
	ctkDictionary props;
	_warService = new WarServiceImpl();
	props.insert(ctkPluginConstants::SERVICE_RANKING, 1);
	context->registerService<WarService>(_warService, props);
}

void WarActivator::stop(ctkPluginContext* context)
{
	SAFE_DELETE(_warService);
	if (WarCommander::getSingletonPtr())
		delete WarCommander::getSingletonPtr();
}
