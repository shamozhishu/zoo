#include <QPluginLoader>
#include <QDebug>
#include <QFile>
#include <QtWidgets/QApplication>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/PluginfwAdminInterface.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QString bootPluginPath = QCoreApplication::applicationDirPath() + "/plugins/";
	QString bootPluginFile = bootPluginPath;
	bootPluginFile += "ctk_pluginfwadmin.dll";

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
	fwAdmin->addPluginPath(bootPluginPath);
	int ret = QCoreApplication::exec();
	zooCmdL_Close();
	return ret;
}
