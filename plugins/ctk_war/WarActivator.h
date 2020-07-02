#ifndef __WAR_ACTIVATOR_H__
#define __WAR_ACTIVATOR_H__

#include <ctkPluginActivator.h>

class WarServiceImpl;
class WarActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "WarActivator")
public:
	WarActivator();
	~WarActivator();
	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	WarServiceImpl* _warService;
};

#endif // __WAR_ACTIVATOR_H__
