#include "ZooCmdUI.h"
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/war/WarService.h>
#include <QLabel>
#include <QThread>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSettings>
#include <QDockWidget>
#include <QMessageBox>
#include <QProgressBar>
#include "SetupDlg.h"
#include "ZooCmdWidget.h"
#include "UIActivator.h"

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

ZooCmdUI::ZooCmdUI(QWidget* parent /*= Q_NULLPTR*/)
	: QMainWindow(parent)
	, _idx(0)
	, _mainWidget(nullptr)
	, _inputAdaName("zooCmd_osg")
{
	_ui.setupUi(this);
	QLabel* label = new QLabel(tr(" 请输入命令："));
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
	connect(_ui.action_setup, SIGNAL(triggered()), this, SLOT(onSetup()));

	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("ZOO_CMDSET");
	QString datadir = settings.value("datadir").toString();
	QStringList cmdset = settings.value("activecmd").toStringList();
	settings.endGroup();

	bool mainThreadInit = false;
	_mainWidget = new ZooCmdWidget(_inputAdaName, datadir, mainThreadInit, this);
	_ui.gridLayout_center->addWidget(_mainWidget);
	setCentralWidget(_mainWidget);
	connect(_mainWidget, &ZooCmdWidget::inited, [this, mainThreadInit]
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

		_cmdlineEdit->setEnabled(true);
	});

	connect(_mainWidget, &ZooCmdWidget::cmdRegistered, [this, mainThreadInit]
	{
		if (!mainThreadInit)
		{
			_progressBar->setValue(0);
			_progressBar->setVisible(true);

			int i = 0;
			while (true)
			{
				if (_mainWidget->isRegCmdCompleted())
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

		_cmdlineEdit->setEnabled(true);
	});

	showMaximized();
}

void ZooCmdUI::addWidget(const QString& strId, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area, Qt::DockWidgetAreas areas /*= Qt::AllDockWidgetAreas*/)
{
	QDockWidget* dock = _dockWgts.value(strId);
	if (!dock)
	{
		dock = new QDockWidget(strName);
		addDockWidget(area, dock);
		dock->setAllowedAreas(area | areas);
		dock->setWidget(pWidget);
		QAction* pActionWnd = new QAction(icon, strName, this);
		pActionWnd->setCheckable(true);
		pActionWnd->setChecked(true);
		_ui.mainToolBar->addAction(pActionWnd);
		connect(pActionWnd, SIGNAL(triggered(bool)), dock, SLOT(setVisible(bool)));
		_dockWgts.insert(strId, dock);
	}
}

void ZooCmdUI::removeWidget(const QString& strId)
{
	QDockWidget* dock = _dockWgts.value(strId);
	if (dock)
	{
		delete dock->widget();
		removeDockWidget(dock);
		dock->deleteLater();
		_dockWgts.remove(strId);
	}
}

QWidget* ZooCmdUI::getWidget(const QString& strId)
{
	QDockWidget* dock = _dockWgts.value(strId);
	if (dock)
		return dock->widget();
	return nullptr;
}

void ZooCmdUI::addMenu(const QString &strId, QMenu* pSubMenu)
{
	QMenu* subMenu = _subMenus.value(strId);
	if (!subMenu && pSubMenu)
	{
		_ui.menuBar->addMenu(pSubMenu);
		_subMenus.insert(strId, pSubMenu);
	}
}

void ZooCmdUI::removeMenu(const QString& strId)
{
	QMenu* subMenu = _subMenus.value(strId);
	if (subMenu)
	{
		subMenu->deleteLater();
		_subMenus.remove(strId);
	}
}

void ZooCmdUI::addToolButton(const QString &strId, QActionGroup* pActionGroup)
{
	QActionGroup* actGroup = _actionItems.value(strId);
	if (!actGroup && pActionGroup)
	{
		_ui.mainToolBar->addActions(pActionGroup->actions());
		_actionItems.insert(strId, pActionGroup);
	}
}

void ZooCmdUI::removeToolButton(const QString& strId)
{
	QActionGroup* actGroup = _actionItems.value(strId);
	if (actGroup)
	{
		QList<QAction*> acts = actGroup->actions();
		auto it = acts.begin();
		for (; it != acts.end(); ++it)
		{
			QAction* pAct = *it;
			_ui.mainToolBar->removeAction(pAct);
			pAct->deleteLater();
		}

		_actionItems.remove(strId);
	}
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
		QMessageBox::information(this, tr("提示"), tr("发送命令失败！"));
		return;
	}

	const char* szMessage = zooCmd_ErrorMessage();
	if (0 != strcmp(szMessage, ""))
	{
		QMessageBox::information(this, tr("提示"), tr(szMessage));
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

void ZooCmdUI::onSetup()
{
	SetupDlg dlg(_inputAdaName);
	if (dlg.exec() == QDialog::Accepted)
	{
		_cmdlineEdit->setEnabled(false);
		QStringList cmdset = dlg.getCmdset();
		if (cmdset.size() > 0)
			_mainWidget->resgisterCmdset(cmdset);
		else
			QMessageBox::warning(this, tr("警告"), tr("未选中任何命令模块！"));
	}
}
