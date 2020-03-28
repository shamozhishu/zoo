#pragma once

#include <QtPlugin>

class PluginfwAdminInterface
{
public:
	virtual void addPluginPath(const QString& pluginPath) = 0;
	virtual void installPlugin(const QString& pluginName) = 0;
	virtual void uninstallPlugin(const QString& pluginName) = 0;
};

#define PluginfwAdminInterface_iid "com.Interface.PluginfwAdminInterface"
Q_DECLARE_INTERFACE(PluginfwAdminInterface, PluginfwAdminInterface_iid)
