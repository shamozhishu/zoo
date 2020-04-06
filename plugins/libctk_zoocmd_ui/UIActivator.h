#pragma once

#include <ctkPluginActivator.h>

class UIActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "UIActivator")
public:
	UIActivator();
	~UIActivator();
	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);
	static ctkPluginContext* getPluginContext();

private:
	class ZooCmdUI* _ui;
	static ctkPluginContext* _pluginContext;
};
