#ifndef __UI_ACTIVATOR_H__
#define __UI_ACTIVATOR_H__

#include <ctkPluginActivator.h>

class UIActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "ctk_zoocmd_ui")
public:
	UIActivator();
	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);
	static ctkPluginContext* getPluginContext();

private:
	class ZooCmdUI* _ui;
	static ctkPluginContext* _pluginContext;
};

#endif // __UI_ACTIVATOR_H__
