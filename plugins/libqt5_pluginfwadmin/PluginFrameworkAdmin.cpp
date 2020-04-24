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

class PluginFrameworkAdminPrivate
{
	QStringList _pluginLibFilter;
public:
	ctkPluginContext* _context;
	ctkPluginFrameworkFactory* _fwFactory;
	QHash<QString, long> _installedPluginNames;

	PluginFrameworkAdminPrivate()
		: _context(nullptr)
		, _fwFactory(nullptr)
	{
		_pluginLibFilter << "*.dll" << "*.so" << "*.dylib";
	}

	void installPlugins(const QString& pluginPath)
	{
		QHash<QString, QString> pluginList;
		QString pluginName, pluginName_r, pluginName_d;
		QDirIterator dirIter(pluginPath, _pluginLibFilter, QDir::Files);

		while (dirIter.hasNext())
		{
			dirIter.next();
			pluginName = dirIter.fileName().mid(3);   // strip the "lib" prefix.
			pluginName.truncate(pluginName.lastIndexOf('.')); // remove the suffix.

			bool existed = false;
			bool needReplace = false;
			auto it = pluginList.begin();
			auto itEnd = pluginList.end();
			for (; it != itEnd; ++it)
			{
				if (it.key().length() > pluginName)
				{
					pluginName_d = it.key();
					pluginName_r = pluginName;
#ifdef _DEBUG
					needReplace = false;
#else
					needReplace = true;
#endif
				}
				else
				{
					pluginName_d = pluginName;
					pluginName_r = it.key();
#ifdef _DEBUG
					needReplace = true;
#else
					needReplace = false;
#endif
				}

				if (pluginName_d.compare(pluginName_r + "d", Qt::CaseInsensitive) == 0)
				{
					existed = true;
					if (needReplace)
					{
						pluginList.erase(it);
						pluginList.insert(pluginName, dirIter.filePath());
					}

					break;
				}
			}

			if (!existed)
				pluginList.insert(pluginName, dirIter.filePath());
		}

		auto it = pluginList.begin();
		auto itEnd = pluginList.end();
		for (; it != itEnd; ++it)
		{
			pluginName = it.key();
			QString pluginFile = it.value();
			if (pluginName.contains("qt5_pluginfwadmin", Qt::CaseInsensitive))
				continue;

			zoo_debug("%s: %s", pluginName.toStdString().c_str(), pluginFile.toStdString().c_str());
			if (_installedPluginNames.find(pluginName) == _installedPluginNames.end())
			{
				try
				{
					QSharedPointer<ctkPlugin> plugin = _context->installPlugin(QUrl::fromLocalFile(pluginFile));
					plugin->start(ctkPlugin::START_TRANSIENT);
					_installedPluginNames.insert(pluginName, plugin->getPluginId());
				}
				catch (const ctkPluginException& e)
				{
					zoo_error("%s", e.what());
					throw e.what();
					exit(-1);
				}
				catch (const ctkRuntimeException& e)
				{
					zoo_error("%s", e.what());
					throw e.what();
					exit(-1);
				}
				catch (const ctkException& e)
				{
					zoo_error("%s", e.what());
					throw e.what();
					exit(-1);
				}
				catch (const std::exception& e)
				{
					zoo_error("%s", e.what());
					throw e.what();
					exit(-1);
				}
			}
			else
			{
				zoo_debug("%s plug-in is already installed!", pluginName.toStdString().c_str());
			}
		}
	}

	void installPlugin(const QString& pluginName)
	{
		auto it = _installedPluginNames.find(pluginName);
		if (it != _installedPluginNames.end() && it.value() == -1)
		{
			try
			{
				QString pluginFileName = "lib" + pluginName;
#ifdef _DEBUG
				pluginFileName += "d.dll";
#else
				pluginFileName += ".dll";
#endif
				QSharedPointer<ctkPlugin> plugin = _context->installPlugin(QUrl::fromLocalFile(pluginFileName));
				plugin->start();
				it.value() = plugin->getPluginId();
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
		else
		{
			zoo_debug("%s plug-in is cannot be installed!", pluginName.toStdString().c_str());
		}
	}

	void uninstallPlugin(const QString& pluginName)
	{
		auto it = _installedPluginNames.find(pluginName);
		if (it != _installedPluginNames.end() && it.value() != -1)
		{
			try
			{
				QSharedPointer<ctkPlugin> plugin = _context->getPlugin(it.value());
				plugin->uninstall();
				it.value() = -1;
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
		else
		{
			zoo_debug("%s plug-in is cannot be uninstalled!", pluginName.toStdString().c_str());
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
	framework->start();
	d->_context = framework->getPluginContext();
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
