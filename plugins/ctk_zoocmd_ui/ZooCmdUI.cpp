#include "ZooCmdUI.h"
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/war/WarService.h>
#include <QLabel>
#include <QThread>
#include <QLineEdit>
#include <QKeyEvent>
#include <QSettings>
#include <QToolButton>
#include <QDockWidget>
#include <QMessageBox>
#include <QProgressBar>
#include "SetupDlg.h"
#include "ZooCmdWgt.h"
#include "UIActivator.h"
#include "LogPrintWgt.h"

// Qt5��������
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

#define CTK_ZOOCMD_UI_LOG_PRINT_WGT "ctk_zoocmd_ui_log_print_wgt"

ZooCmdUI::ZooCmdUI(QWidget* parent /*= Q_NULLPTR*/)
	: QMainWindow(parent)
	, _idx(0)
	, _mainWidget(nullptr)
	, _inputAdaName("zooCmd_osg")
{
	_ui.setupUi(this);

	QLabel* label = new QLabel(tr(" ���������"));
	_ui.statusBar->addWidget(label);
	_cmdlineEdit = new QLineEdit;
	_ui.statusBar->addWidget(_cmdlineEdit);
	_cmdlineEdit->setFixedWidth(500);
	QToolButton* toolBtn = new QToolButton;
	_ui.statusBar->addWidget(toolBtn);
	toolBtn->setAutoRaise(true);
	toolBtn->setIcon(QIcon(QPixmap(":/images/Resources/images/cmd.png")));
	connect(toolBtn, SIGNAL(clicked()), this, SLOT(onSetup()));
#ifdef _DEBUG
	QDockWidget* pLogPrintDock = addWidget(CTK_ZOOCMD_UI_LOG_PRINT_WGT, tr("��־��ӡ"), new LogPrintWgt(this), QIcon(), Qt::BottomDockWidgetArea,
		Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea, false, false, false);
	toolBtn = new QToolButton;
	_ui.statusBar->addWidget(toolBtn);
	toolBtn->setAutoRaise(true);
	toolBtn->setCheckable(true);
	toolBtn->setChecked(false);
	toolBtn->setIcon(QIcon(QPixmap(":/images/Resources/images/log.png")));
	connect(toolBtn, &QToolButton::clicked, [pLogPrintDock](bool checked)
	{ pLogPrintDock->setVisible(checked); });
#endif
	_progressBar = new QProgressBar;
	_ui.statusBar->addWidget(_progressBar);
	_progressBar->setMinimum(0);
	_progressBar->setMaximum(100);
	_progressBar->setFixedWidth(100);
	_progressBar->setVisible(false);
	_progressBar->setTextVisible(false);

	connect(_cmdlineEdit, SIGNAL(returnPressed()), this, SLOT(onCmd()));

	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("ZOO_CMDSET");
	QString datadir = settings.value("datadir").toString();
	QStringList cmdset = settings.value("activecmd").toStringList();
	settings.endGroup();

	bool mainThreadInit = false;
	_mainWidget = new ZooCmdWgt(_inputAdaName, cmdset, datadir, mainThreadInit, this);
	UIActivator::getPluginContext()->registerService<Win32Service>(_mainWidget);
	_ui.gridLayout_center->addWidget(_mainWidget);
	setCentralWidget(_mainWidget);
	connect(_mainWidget, &ZooCmdWgt::inited, [this, mainThreadInit]
	{
		if (!mainThreadInit)
		{
			_progressBar->setValue(0);
			_progressBar->setVisible(true);

			int i = 0;
			while (true)
			{
				if (_mainWidget->isInitCmdCompleted())
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

	connect(_mainWidget, &ZooCmdWgt::cmdRegistered, [this, mainThreadInit]
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
}

ZooCmdUI::~ZooCmdUI()
{
#ifdef _DEBUG
	removeWidget(CTK_ZOOCMD_UI_LOG_PRINT_WGT);
#endif
}

void ZooCmdUI::setWindowTitle(QString windowTitle)
{
	QMainWindow::setWindowTitle(windowTitle);
}

void ZooCmdUI::starWindowTitle()
{
	QString title = QMainWindow::windowTitle();
	if (title.size() > 0 && title.right(1) != tr("*"))
		QMainWindow::setWindowTitle(title + tr("*"));
}

void ZooCmdUI::unstarWindowTitle()
{
	QString title = QMainWindow::windowTitle();
	if (title.size() > 0 && title.right(1) == tr("*"))
		QMainWindow::setWindowTitle(title.left(title.size() - 1));
}

void ZooCmdUI::finishWindowLaunch()
{
	showMaximized();
}

QDockWidget* ZooCmdUI::addWidget(const QString& strId, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area,
	Qt::DockWidgetAreas areas /*= Qt::AllDockWidgetAreas*/, bool isShow /*= true*/, bool hasToolBtn /*= true*/, bool hasSeparator /*= false*/)
{
	QDockWidget* dock = _dockWgts.value(strId).first;
	if (!dock)
	{
		dock = new QDockWidget(strName);
		addDockWidget(area, dock);
		dock->setAllowedAreas(area | areas);
		dock->setWidget(pWidget);
		dock->setVisible(isShow);
		dock->setFeatures(QDockWidget::DockWidgetMovable);
		if (hasToolBtn)
		{
			QAction* pActionWnd = new QAction(icon, strName, this);
			pActionWnd->setCheckable(true);
			pActionWnd->setChecked(isShow);
			_ui.mainToolBar->addAction(pActionWnd);
			connect(pActionWnd, SIGNAL(triggered(bool)), dock, SLOT(setVisible(bool)));
			_dockWgts.insert(strId, qMakePair(dock, pActionWnd));
			if (hasSeparator)
				_ui.mainToolBar->addSeparator();
		}
	}

	return dock;
}

void ZooCmdUI::removeWidget(const QString& strId)
{
	QDockWidget* dock = _dockWgts.value(strId).first;
	if (dock)
	{
		delete dock->widget();
		removeDockWidget(dock);
		dock->deleteLater();
		_dockWgts.remove(strId);
		_ui.mainToolBar->removeAction(_dockWgts.value(strId).second);
	}
}

QWidget* ZooCmdUI::getWidget(const QString& strId)
{
	QDockWidget* dock = _dockWgts.value(strId).first;
	if (dock)
		return dock->widget();
	return nullptr;
}

void ZooCmdUI::addMenu(const QString &strId, QMenu* pSubMenu, bool hasToolButton /*= true*/)
{
	QMenu* subMenu = _subMenus.value(strId).first;
	if (!subMenu && pSubMenu)
	{
		_ui.menuBar->addMenu(pSubMenu);
		_subMenus.insert(strId, qMakePair(pSubMenu, hasToolButton));
		if (hasToolButton)
		{
			_ui.mainToolBar->addActions(pSubMenu->actions());
			_ui.mainToolBar->addSeparator();
		}
	}
}

void ZooCmdUI::removeMenu(const QString& strId)
{
	QMenu* subMenu = _subMenus.value(strId).first;
	if (subMenu)
	{
		if (_subMenus.value(strId).second)
		{
			QList<QAction*> acts = subMenu->actions();
			auto it = acts.begin();
			for (; it != acts.end(); ++it)
			{
				QAction* pAct = *it;
				_ui.mainToolBar->removeAction(pAct);
			}
		}

		_subMenus.remove(strId);
		subMenu->deleteLater();
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
	QString cmdline = _cmdlineEdit->text().trimmed();
	if (!zooCmd_Send(cmdline.toLocal8Bit()))
	{
		QMessageBox::warning(this, tr("����"), QString::fromLocal8Bit(zooCmd_TipMessage()));
		return;
	}

	const char* szTips = zooCmd_TipMessage();
	if (0 != strcmp(szTips, ""))
		QMessageBox::information(this, tr("��ʾ"), QString::fromLocal8Bit(szTips));

	QVector<QString>::iterator it = qFind(_cmdlines.begin(), _cmdlines.end(), cmdline);
	if (it == _cmdlines.end())
	{
		_cmdlines.push_back(cmdline);
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
		QStringList cmdset = dlg.getCmdset();
		if (cmdset.size() > 0)
		{
			_cmdlineEdit->setEnabled(false);
			_mainWidget->resgisterCmdset(cmdset);
		}
		else
		{
			QMessageBox::warning(this, tr("����"), tr("δѡ���κ�����ģ�飡"));
		}
	}
}