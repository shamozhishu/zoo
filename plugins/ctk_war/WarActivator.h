#ifndef __WAR_ACTIVATOR_H__
#define __WAR_ACTIVATOR_H__

#include <zoo/Log.h>
#include <zoo/Utils.h>
#include <ctkPluginActivator.h>

class WarServiceImpl;
class WarActivator : public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "ctk_war")
public:
	WarActivator();
	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);
	template<typename T>
	static T* getService()
	{
		ctkServiceReference ref = _pluginContext->getServiceReference<T>();
		if (ref)
			return qobject_cast<T*>(_pluginContext->getService(ref));

		zoo_warning(zoo::utf8ToAnsi("»ñÈ¡¡°ctkServiceReference¡±Îª¿Õ£¡").c_str());
		return nullptr;
	}

private:
	WarServiceImpl* _warService;
	static ctkPluginContext* _pluginContext;
};

#endif // __WAR_ACTIVATOR_H__
