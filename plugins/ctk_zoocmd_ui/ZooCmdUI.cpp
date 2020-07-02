#include "ZooCmdUI.h"
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/war/WarService.h>
#include <QLabel>
#include <QThread>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSettings>
#include <QMessageBox>
#include <QProgressBar>
#include "SetupDlg.h"
#include "ZooCmdWidget.h"
#include "UIActivator.h"

ZooCmdUI::ZooCmdUI(QWidget* parent /*= Q_NULLPTR*/)
	: QMainWindow(parent)
	, _idx(0)
	, _mainWidget(nullptr)
	, _inputAdaName("zooCmd_osg")
{
	_ui.setupUi(this);
	QLabel* label = new QLabel(QString::fromLocal8Bit(" 请输入命令："));
	_ui.statusBar->addWidget(label);
	_cmdlineEdit = new QLineEdit;
	_ui.statusBar->addWidget(_cmdlineEdit);
	_cmdlineEdit->setFixedWidth(500);
	_progressBar = new QProgressBar;
	_ui.statusBar->addWidget(_progressBar);
	_progressBar->setMinimum(0);
	_progressBar->setMaximum(100);
	_progressBar->setFixedWidth(100);
	_progressBar->setVisible(false);
	_progressBar->setTextVisible(false);

	connect(_cmdlineEdit, SIGNAL(returnPressed()), this, SLOT(onCmd()));
	connect(_ui.action_open, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(_ui.action_save, SIGNAL(triggered()), this, SLOT(onSave()));
	connect(_ui.action_sim, SIGNAL(triggered(bool)), this, SLOT(onSim(bool)));
	connect(_ui.action_setup, SIGNAL(triggered()), this, SLOT(onSetup()));

	showMaximized();

	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("ZOO_CMDSET");
	QString datadir = settings.value("datadir").toString();
	QStringList cmdset = settings.value("activecmd").toStringList();
	settings.endGroup();
	createZooCmdWidget(cmdset, datadir);
}

void ZooCmdUI::keyPressEvent(QKeyEvent *event)
{
	if (_cmdlineEdit->hasFocus())
	{
		switch (event->key())
		{
		case Qt::Key_Up:
			if (_idx < _cmdlines.size())
			{
				if (_idx > 0)
					--_idx;
				_cmdlineEdit->setText(_cmdlines[_idx]);
				return;
			}
			break;
		case Qt::Key_Down:
			if (_idx < _cmdlines.size())
			{
				if (_idx != _cmdlines.size() - 1)
					++_idx;
				_cmdlineEdit->setText(_cmdlines[_idx]);
				return;
			}
			break;
		default:
			break;
		}
	}

	QMainWindow::keyPressEvent(event);
}

void ZooCmdUI::onCmd()
{
	std::string cmdline = _cmdlineEdit->text().toStdString();
	if (cmdline.find("exit") != -1 || !zooCmd_Send(cmdline.c_str()))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("发送命令失败！"));
		return;
	}

	const char* szMessage = zooCmd_ErrorMessage();
	if (0 != strcmp(szMessage, ""))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit(szMessage));
		return;
	}

	QVector<QString>::iterator it = qFind(_cmdlines.begin(), _cmdlines.end(), cmdline.c_str());
	if (it == _cmdlines.end())
	{
		_cmdlines.push_back(cmdline.c_str());
		_idx = _cmdlines.size() - 1;
		return;
	}
	
	QString tmp = *it;
	_cmdlines.erase(it);
	_cmdlines.push_back(tmp);
	_idx = _cmdlines.size() - 1;
}

void ZooCmdUI::onOpen()
{
	ctkServiceReference ref = UIActivator::getPluginContext()->getServiceReference<WarService>();
	if (ref)
	{
		WarService* service = qobject_cast<WarService*>(UIActivator::getPluginContext()->getService(ref));
		if (service != Q_NULLPTR)
		{
			service->closeScene();
			service->openScene(1);
		}
	}
}

void ZooCmdUI::onSave()
{
	ctkServiceReference ref = UIActivator::getPluginContext()->getServiceReference<WarService>();
	if (ref)
	{
		WarService* service = qobject_cast<WarService*>(UIActivator::getPluginContext()->getService(ref));
		if (service != Q_NULLPTR)
			service->saveScene();
	}
}

void ZooCmdUI::onSim(bool checked)
{
	
}

void ZooCmdUI::onSetup()
{
	SetupDlg dlg(_inputAdaName);
	if (dlg.exec() == QDialog::Accepted)
		createZooCmdWidget(dlg.getCmdset(), dlg.getDataDir());
}

void ZooCmdUI::createZooCmdWidget(QStringList cmdset, QString datadir)
{
	if (_mainWidget)
	{
		delete _mainWidget;
		_mainWidget = nullptr;
	}

	bool mainThreadInit = false;
	_cmdlineEdit->setEnabled(false);
	_mainWidget = new ZooCmdWidget(cmdset, _inputAdaName, datadir, mainThreadInit, this);
	setCentralWidget(_mainWidget);
	connect(_mainWidget, &ZooCmdWidget::inited, [this, cmdset, datadir, mainThreadInit]
	{
		if (!mainThreadInit)
		{
			_progressBar->setValue(0);
			_progressBar->setVisible(true);

			int i = 0;
			while (true)
			{
				if (zooCmd_IsInited())
				{
					_progressBar->setValue(100);
					_progressBar->setVisible(false);
					break;
				}

				QThread::msleep(5);
				_progressBar->setValue(i++);
				if (i > 100)
					i = 0;
			}
		}

		const char* szMessage = zooCmd_ErrorMessage();
		if (0 != strcmp(szMessage, ""))
		{
			QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit(szMessage));
		}
		else
		{
			QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
			settings.beginGroup("ZOO_CMDSET");
			settings.setValue("datadir", datadir);
			settings.setValue("activecmd", cmdset);
			settings.endGroup();
		}

		_cmdlineEdit->setEnabled(true);
	});
}
