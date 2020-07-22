#include <QPluginLoader>
#include <QDebug>
#include <QFile>
#include <QtWidgets/QApplication>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/PluginfwAdminInterface.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString bootPluginPath = QCoreApplication::applicationDirPath();
	QString bootPluginFile = bootPluginPath;
	bootPluginFile += "/ctk_pluginfwadmin.dll";

	if (!QFile(bootPluginFile).exists())
	{
		qCritical() << "[Error] Boot plugin does not exists!";
		return -1;
	}

	if (!zooCmdL_Open())
	{
		qCritical() << "[Error] Open zooCmd module failed!";
		return -1;
	}

	QPluginLoader pluginLoader(bootPluginFile);
	PluginfwAdminInterface* fwAdmin = qobject_cast<PluginfwAdminInterface*>(pluginLoader.instance());
	if (!fwAdmin)
	{
		qCritical() << pluginLoader.errorString();
		return -1;
	}

	fwAdmin->addPluginPath(bootPluginPath + "/plugins");
	int ret = QCoreApplication::exec();
	fwAdmin->uninstallAllPlugins();
	zooCmdL_Close();
	return ret;
}
