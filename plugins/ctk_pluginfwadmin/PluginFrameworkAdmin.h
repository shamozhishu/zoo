#ifndef __PLUGIN_FRAMEWORK_ADMIN_H__
#define __PLUGIN_FRAMEWORK_ADMIN_H__

#include <ctkPluginFramework_global.h>
#include <ctk_service/PluginfwAdminInterface.h>

class PluginFrameworkAdminPrivate;
class PluginFrameworkAdmin : public QObject, public PluginfwAdminInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginfwAdminInterface)
	Q_PLUGIN_METADATA(IID PluginfwAdminInterface_iid)
public:
	PluginFrameworkAdmin();
	~PluginFrameworkAdmin();

public:
	void addPluginPath(const QString& pluginPath);
	void installPlugin(const QString& pluginName);
	void uninstallPlugin(const QString& pluginName);

private:
	Q_DECLARE_PRIVATE(PluginFrameworkAdmin)
	const QScopedPointer<PluginFrameworkAdminPrivate> d_ptr;
};

#endif // __PLUGIN_FRAMEWORK_ADMIN_H__
