#pragma once

#include <ctkPluginActivator.h>

class UIActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "UIActivator")

public:
	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	class osgCmdUI* _ui;
};
