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
