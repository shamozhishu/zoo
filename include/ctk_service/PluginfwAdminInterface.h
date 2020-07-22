#ifndef __PLUGIN_FRAMEWORK_ADMIN_INTERFACE_H__
#define __PLUGIN_FRAMEWORK_ADMIN_INTERFACE_H__

#include <QtPlugin>

class PluginfwAdminInterface
{
public:
	virtual void addPluginPath(const QString& pluginPath) = 0;
	virtual void installPlugin(const QString& pluginName) = 0;
	virtual void uninstallPlugin(const QString& pluginName) = 0;
	virtual void uninstallAllPlugins() = 0;
};

#define PluginfwAdminInterface_iid "com.Interface.PluginfwAdminInterface"
Q_DECLARE_INTERFACE(PluginfwAdminInterface, PluginfwAdminInterface_iid)

#endif // __PLUGIN_FRAMEWORK_ADMIN_INTERFACE_H__
