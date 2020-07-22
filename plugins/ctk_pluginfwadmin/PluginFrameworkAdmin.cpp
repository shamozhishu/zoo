#include "PluginFrameworkAdmin.h"
#include <zoo/Log.h>
#include <QPair>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>
#include <QCoreApplication>
#include <ctkPluginContext.h>
#include <ctkPluginFramework.h>
#include <ctkPluginException.h>
#include <ctkPluginFrameworkFactory.h>
#include <ctkPluginFrameworkLauncher.h>

#define MAIN_PLUGIN "ctk_zoocmd_ui"

class PluginFrameworkAdminPrivate
{
	QStringList _pluginLibFilter;
public:
	long _mainPluginId;
	QString _mainPluginName;
	ctkPluginContext* _context;
	ctkPluginFrameworkFactory* _fwFactory;
	QVector<long> _dependencyPlugins;
	QHash<QString, QString> _pluginList;
	QHash<QString, long> _installedPlugins;

	PluginFrameworkAdminPrivate()
		: _context(nullptr)
		, _fwFactory(nullptr)
		, _mainPluginId(-1)
	{
		_pluginLibFilter << "*.dll" << "*.so" << "*.dylib";
	}

	void installPlugins(const QString& pluginPath)
	{
		QString pluginName, pluginFile;
		QDirIterator dirIter(pluginPath, _pluginLibFilter, QDir::Files);

		while (dirIter.hasNext())
		{
			dirIter.next();
			pluginName = dirIter.fileName().mid(3);   // strip the "lib" prefix.
			pluginName.truncate(pluginName.lastIndexOf('.')); // remove the suffix.
			_pluginList.insert(pluginName, dirIter.filePath());
		}

		if (_mainPluginId == -1)
		{
			pluginFile = _pluginList.value(MAIN_PLUGIN);
			if (pluginFile.isEmpty())
			{
				zoo_error("The main UI plug-in \"ctk_zoocmd_ui\" does not exist!");
				exit(-1);
			}

			QSharedPointer<ctkPlugin> plugin = installThenStart(pluginName);
			_mainPluginName = pluginName;
			_mainPluginId = plugin->getPluginId();
			zoo_debug("Install main plug-in %s: %s", pluginName.toStdString().c_str(), pluginFile.toStdString().c_str());
		}

		auto it = _pluginList.begin();
		auto itEnd = _pluginList.end();
		for (; it != itEnd; ++it)
		{
			pluginName = it.key();
			pluginFile = it.value();

			if (pluginName.compare(MAIN_PLUGIN, Qt::CaseInsensitive) == 0)
				continue;

			if (_installedPlugins.find(pluginName) == _installedPlugins.end())
			{
				QSharedPointer<ctkPlugin> plugin = installThenStart(pluginName);
				_installedPlugins.insert(pluginName, plugin->getPluginId());
				zoo_debug("Install plug-in %s: %s", pluginName.toStdString().c_str(), pluginFile.toStdString().c_str());
			}
			else
			{
				zoo_debug("%s plug-in is already installed!", pluginName.toStdString().c_str());
			}
		}
	}

	void installPlugin(const QString& pluginName)
	{
		auto it = _installedPlugins.find(pluginName);
		if (it != _installedPlugins.end() && it.value() == -1)
		{
			QSharedPointer<ctkPlugin> plugin = installThenStart(pluginName);
			it.value() = plugin->getPluginId();
		}
		else
		{
			zoo_debug("%s plug-in is cannot be installed!", pluginName.toStdString().c_str());
		}
	}

	void uninstallPlugin(const QString& pluginName)
	{
		auto it = _installedPlugins.find(pluginName);
		if (it != _installedPlugins.end() && it.value() != -1)
		{
			stopThenUninstall(it.value());
			it.value() = -1;
		}
		else
		{
			zoo_debug("%s plug-in is cannot be uninstalled!", pluginName.toStdString().c_str());
		}
	}

	void uninstallPlugins()
	{
		int len = _dependencyPlugins.size();
		for (int i = len - 1; i >= 0; --i)
		{
			if (_dependencyPlugins[i] != -1)
			{
				stopThenUninstall(_dependencyPlugins[i]);
				_dependencyPlugins[i] = -1;
			}
		}

		stopThenUninstall(_mainPluginId);
	}

private:
	QSharedPointer<ctkPlugin> installThenStart(QString pluginName)
	{
		QSharedPointer<ctkPlugin> plugin;
		try
		{
			plugin = _context->installPlugin(QUrl::fromLocalFile(_pluginList.value(pluginName)));
			QString dependPlugin = plugin->getHeaders().value(ctkPluginConstants::REQUIRE_PLUGIN);
			if (!dependPlugin.isEmpty())
				installThenStart(dependPlugin.replace(".", "_"));
			plugin->start();
		}
		catch (const ctkPluginException& e)
		{
			zoo_error("%s", e.what());
			return QSharedPointer<ctkPlugin>();
		}
		catch (const ctkRuntimeException& e)
		{
			zoo_error("%s", e.what());
			return QSharedPointer<ctkPlugin>();
		}
		catch (const ctkException& e)
		{
			zoo_error("%s", e.what());
			return QSharedPointer<ctkPlugin>();
		}
		catch (const std::exception& e)
		{
			zoo_error("%s", e.what());
			return QSharedPointer<ctkPlugin>();
		}

		return plugin;
	}

	void stopThenUninstall(long pluginID)
	{
		try
		{
			QSharedPointer<ctkPlugin> plugin = _context->getPlugin(pluginID);
			plugin->uninstall();
		}
		catch (const ctkPluginException& e)
		{
			zoo_error("%s", e.what());
		}
		catch (const ctkRuntimeException& e)
		{
			zoo_error("%s", e.what());
		}
		catch (const ctkException& e)
		{
			zoo_error("%s", e.what());
		}
		catch (const std::exception& e)
		{
			zoo_error("%s", e.what());
		}
	}
};

PluginFrameworkAdmin::PluginFrameworkAdmin()
	: d_ptr(new PluginFrameworkAdminPrivate())
{
	ctkProperties fwProps;
	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("CTK_PLUGIN_FRAMEWORK_PROPS");
	QStringList keys = settings.childKeys();
	int len = keys.size();
	for (int i = 0; i < len; ++i)
		fwProps.insert(keys[i], settings.value(keys[i]));
	settings.endGroup();

	Q_D(PluginFrameworkAdmin);
	d->_fwFactory = new ctkPluginFrameworkFactory(fwProps);
	QSharedPointer<ctkPluginFramework> framework = d->_fwFactory->getFramework();
	framework->init();
	d->_context = framework->getPluginContext();
	d->_context->connectPluginListener(this, SLOT(onPluginEvent(ctkPluginEvent)));
}

PluginFrameworkAdmin::~PluginFrameworkAdmin()
{
	Q_D(PluginFrameworkAdmin);
	delete d->_fwFactory;
}

void PluginFrameworkAdmin::addPluginPath(const QString& pluginPath)
{
	Q_D(PluginFrameworkAdmin);
	QCoreApplication::addLibraryPath(pluginPath);
	ctkPluginFrameworkLauncher::appendPathEnv(pluginPath);
	d->installPlugins(pluginPath);
}

void PluginFrameworkAdmin::installPlugin(const QString& pluginName)
{
	Q_D(PluginFrameworkAdmin);
	d->installPlugin(pluginName);
}

void PluginFrameworkAdmin::uninstallPlugin(const QString& pluginName)
{
	Q_D(PluginFrameworkAdmin);
	d->uninstallPlugin(pluginName);
}

void PluginFrameworkAdmin::uninstallAllPlugins()
{
	Q_D(PluginFrameworkAdmin);
	d->uninstallPlugins();
}

void PluginFrameworkAdmin::onPluginEvent(ctkPluginEvent pluginEvt)
{
	if (pluginEvt.getType() == ctkPluginEvent::STARTED)
	{
		Q_D(PluginFrameworkAdmin);
		if (pluginEvt.getPlugin()->getSymbolicName() != "ctk.zoocmd.ui")
			d->_dependencyPlugins.push_back(pluginEvt.getPlugin()->getPluginId());
	}
}
