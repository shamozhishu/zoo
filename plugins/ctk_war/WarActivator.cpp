#include "WarActivator.h"
#include "WarCommander.h"
#include "WarServiceImpl.h"
#include <zoo/Log.h>
#include <zooCmdLoader/ZooCmdLoader.h>

ctkPluginContext* WarActivator::_pluginContext = nullptr;
WarActivator::WarActivator()
	: _warService(nullptr)
{
	zooCmdL_Load();
}

void WarActivator::start(ctkPluginContext* context)
{
	_pluginContext = context;

	QString cmdName = context->getProperty("ctk.war.cmd").toString();
	QString tableName = context->getProperty("ctk.war.table").toString();

	if (tableName.isEmpty())
	{
		const char* szTip = "Framework property \"ctk.war.table\" containing the table name of the CTK War not found!";
		zoo_error("%s", szTip);
		throw ctkRuntimeException(szTip);
	}

	zoo_debug("ctk.war.cmd=%s", cmdName.toStdString().c_str());
	zoo_debug("ctk.war.table=%s", tableName.toStdString().c_str());
	new WarCommander(cmdName.toStdString(), tableName.toStdString());
	_warService = new WarServiceImpl();
	ctkDictionary props;
	props.insert(ctkPluginConstants::SERVICE_RANKING, 1);
	context->registerService<WarService>(_warService, props);
}

void WarActivator::stop(ctkPluginContext* context)
{
	SAFE_DELETE(_warService);
	if (WarCommander::getSingletonPtr())
		delete WarCommander::getSingletonPtr();

	_pluginContext = nullptr;
}
