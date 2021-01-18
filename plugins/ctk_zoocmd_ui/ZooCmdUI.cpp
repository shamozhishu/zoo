#include "ZooCmdUI.h"
#include <zoo/Log.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <ctk_service/WarService.h>
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

// Qt5中文乱码
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

	QLabel* label = new QLabel(tr(" 请输入命令："));
	_ui.statusBar->addWidget(label);
	_cmdlineEdit = new QLineEdit;
	_ui.statusBar->addWidget(_cmdlineEdit);
	_cmdlineEdit->setFixedWidth(500);
	QToolButton* toolBtn = new QToolButton;
	_ui.statusBar->addWidget(toolBtn);
	toolBtn->setAutoRaise(true);
	toolBtn->setIcon(QIcon(QPixmap(":/images/Resources/images/cmd.png")));
	connect(toolBtn, SIGNAL(clicked()), this, SLOT(onSetup()));

	if (zoo::Log::console())
	{
		QDockWidget* pLogPrintDock = addWidget(CTK_ZOOCMD_UI_LOG_PRINT_WGT, tr("日志打印"), new LogPrintWgt(this), QIcon(), Qt::BottomDockWidgetArea,
			Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea, false, false, false);
		toolBtn = new QToolButton;
		_ui.statusBar->addWidget(toolBtn);
		toolBtn->setAutoRaise(true);
		toolBtn->setCheckable(true);
		toolBtn->setChecked(false);
		toolBtn->setIcon(QIcon(QPixmap(":/images/Resources/images/log.png")));
		connect(toolBtn, &QToolButton::clicked, [pLogPrintDock](bool checked)
		{ pLogPrintDock->setVisible(checked); if (checked) pLogPrintDock->lower(); });
	}

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

	_mainWidget = new ZooCmdWgt(_inputAdaName, cmdset, datadir, this);
	_ui.gridLayout_center->addWidget(_mainWidget);
	setCentralWidget(_mainWidget);
	connect(_mainWidget, &ZooCmdWgt::cmdRegistered, [this]
	{
		_progressBar->setValue(0);
		_progressBar->setVisible(true);

		int i = 0;
		while (true)
		{
			if (_mainWidget->isCmdRegCompleted())
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

		_cmdlineEdit->setEnabled(true);
	});
}

ZooCmdUI::~ZooCmdUI()
{
	removeWidget(CTK_ZOOCMD_UI_LOG_PRINT_WGT);
}

bool ZooCmdUI::needSavedScene()
{
	QString title = QMainWindow::windowTitle();
	if (title.size() > 0 && title.right(1) == tr("*"))
		return true;
	return false;
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

QWidget* ZooCmdUI::getWidget(const QString& wgtID)
{
	QDockWidget* dock = _dockWgts.value(wgtID).first;
	if (dock)
		return dock->widget();
	return nullptr;
}

void ZooCmdUI::raiseWidget(const QString& wgtID)
{
	QDockWidget* dock = _dockWgts.value(wgtID).first;
	if (dock)
		dock->raise();
}

QDockWidget* ZooCmdUI::addWidget(const QString& wgtID, const QString& wgtTitle, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area,
	Qt::DockWidgetAreas areas /*= Qt::AllDockWidgetAreas*/, bool isShow /*= true*/, bool hasToolBtn /*= true*/, bool hasSeparator /*= false*/)
{
	QDockWidget* dock = _dockWgts.value(wgtID).first;
	if (!dock)
	{
		dock = new QDockWidget(wgtTitle);
		addDockWidget(area, dock);
		dock->setAllowedAreas(area | areas);
		dock->setWidget(pWidget);
		dock->setVisible(isShow);
		dock->setFeatures(QDockWidget::DockWidgetMovable);
		if (hasToolBtn)
		{
			QAction* pActionWnd = new QAction(icon, wgtTitle, this);
			pActionWnd->setCheckable(true);
			pActionWnd->setChecked(isShow);
			_ui.mainToolBar->addAction(pActionWnd);
			connect(pActionWnd, &QAction::triggered, [dock](bool isShow)
			{
				dock->setVisible(isShow);
				if (isShow)
					dock->raise();
			});
			_dockWgts.insert(wgtID, qMakePair(dock, pActionWnd));
			if (hasSeparator)
				_ui.mainToolBar->addSeparator();
		}
	}

	return dock;
}

void ZooCmdUI::removeWidget(const QString& wgtID)
{
	if (_dockWgts.contains(wgtID))
	{
		QDockWidget* dock = _dockWgts.value(wgtID).first;
		if (dock)
		{
			delete dock->widget();
			removeDockWidget(dock);
			dock->deleteLater();
			_dockWgts.remove(wgtID);
			_ui.mainToolBar->removeAction(_dockWgts.value(wgtID).second);
		}
	}
}

void ZooCmdUI::tabifyWidget(const QString& firstWgtID, const QString& secondWgtID)
{
	QDockWidget* firstDock = _dockWgts.value(firstWgtID).first;
	QDockWidget* secondDock = _dockWgts.value(secondWgtID).first;
	if (firstDock && firstDock)
		tabifyDockWidget(firstDock, secondDock);
}

void ZooCmdUI::addMenu(const QString &menuID, QMenu* pSubMenu, bool hasToolBtn /*= true*/)
{
	QMenu* subMenu = _subMenus.value(menuID).first;
	if (!subMenu && pSubMenu)
	{
		_ui.menuBar->addMenu(pSubMenu);
		_subMenus.insert(menuID, qMakePair(pSubMenu, hasToolBtn));
		if (hasToolBtn)
		{
			_ui.mainToolBar->addActions(pSubMenu->actions());
			_ui.mainToolBar->addSeparator();
		}
	}
}

void ZooCmdUI::removeMenu(const QString& menuID)
{
	QMenu* subMenu = _subMenus.value(menuID).first;
	if (subMenu)
	{
		if (_subMenus.value(menuID).second)
		{
			QList<QAction*> acts = subMenu->actions();
			auto it = acts.begin();
			for (; it != acts.end(); ++it)
			{
				QAction* pAct = *it;
				_ui.mainToolBar->removeAction(pAct);
			}
		}

		_subMenus.remove(menuID);
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
		QMessageBox::warning(this, tr("警告"), QString::fromLocal8Bit(zooCmd_TipMessage()));
		return;
	}

	const char* szTips = zooCmd_TipMessage();
	if (0 != strcmp(szTips, ""))
		QMessageBox::information(this, tr("提示"), QString::fromLocal8Bit(szTips));

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
			QMessageBox::warning(this, tr("警告"), tr("未选中任何命令模块！"));
		}
	}
}
