#include <QApplication>
#include <QPluginLoader>
#include <QDebug>
#include <QFile>
#include "../../plugins/libqt5_pluginfw_admin/PluginfwAdminInterface.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString bootPluginPath = QCoreApplication::applicationDirPath() + "/plugins/";
	QString bootPluginFile = bootPluginPath;
#ifdef _DEBUG
	bootPluginFile += "libqt5_pluginfw_admind.dll";
#else
	bootPluginFile += "libqt5_pluginfw_admin.dll";
#endif

	if (!QFile(bootPluginFile).exists())
	{
		qDebug() << "[Error] boot plugin does not exists!";
		return -1;
	}

	QPluginLoader pluginLoader(bootPluginFile);
	PluginfwAdminInterface* fwAdmin = qobject_cast<PluginfwAdminInterface*>(pluginLoader.instance());
	fwAdmin->addPluginPath(bootPluginPath);
	return QCoreApplication::exec();
}
