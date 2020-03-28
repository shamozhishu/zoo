#include <QApplication>
#include <QPluginLoader>
#include <QDebug>
#include <QFile>
#include "../../plugins/libqt5_pluginfw_admin/PluginfwAdminInterface.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString bootPluginPath = QCoreApplication::applicationDirPath();
#ifdef _DEBUG
	bootPluginPath += "/libqt5_pluginfw_admind.dll";
#else
	bootPluginPath += "/libqt5_pluginfw_admin.dll";
#endif

	if (!QFile(bootPluginPath).exists())
	{
		qDebug() << "[Error] boot plugin does not exists!";
		return -1;
	}

	QPluginLoader pluginLoader(bootPluginPath);
	PluginfwAdminInterface* fwAdmin = qobject_cast<PluginfwAdminInterface*>(pluginLoader.instance());
	fwAdmin->addPluginPath(QCoreApplication::applicationDirPath() + "/plugins/");
	return QCoreApplication::exec();
}
