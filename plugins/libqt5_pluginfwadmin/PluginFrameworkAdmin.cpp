#include "PluginFrameworkAdmin.h"
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
		QDirIterator dirIter(pluginPath, _pluginLibFilter, QDir::Files);
		while (dirIter.hasNext())
		{
			dirIter.next();
			QString pluginName = dirIter.fileName().mid(3);   // strip the "lib" prefix.
			pluginName.truncate(pluginName.lastIndexOf('.')); // remove the suffix.
#ifdef _DEBUG
			pluginName.truncate(pluginName.lastIndexOf('d'));
#endif

			if (_installedPluginNames.find(pluginName) == _installedPluginNames.end())
			{
				try
				{
					QSharedPointer<ctkPlugin> plugin = _context->installPlugin(QUrl::fromLocalFile(dirIter.filePath()));
					plugin->start(ctkPlugin::START_TRANSIENT);
					_installedPluginNames.insert(pluginName, plugin->getPluginId());
				}
				catch (const ctkPluginException& e)
				{
					qCritical() << e.what();
				}
				catch (const ctkRuntimeException& e)
				{
					qCritical() << e.what();
				}
				catch (const ctkException& e)
				{
					qCritical() << e.what();
				}
				catch (const std::exception& e)
				{
					qCritical() << e.what();
				}
			}
			else
			{
				qDebug() << pluginName << " plug-in is already installed!";
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
				qCritical() << e.what();
			}
			catch (const ctkRuntimeException& e)
			{
				qCritical() << e.what();
			}
			catch (const ctkException& e)
			{
				qCritical() << e.what();
			}
			catch (const std::exception& e)
			{
				qCritical() << e.what();
			}
		}
		else
		{
			qDebug() << pluginName << " plug-in is cannot be installed!";
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
				qCritical() << e.what();
			}
			catch (const ctkRuntimeException& e)
			{
				qCritical() << e.what();
			}
			catch (const ctkException& e)
			{
				qCritical() << e.what();
			}
			catch (const std::exception& e)
			{
				qCritical() << e.what();
			}
		}
		else
		{
			qDebug() << pluginName << " plug-in is cannot be uninstalled!";
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
