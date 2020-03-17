#include "osgCmdUI.h"
#include <osgCmd.h>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include "SetupDlg.h"
#include "osgCmdWidget.h"

osgCmdUI::osgCmdUI(QWidget* parent /*= Q_NULLPTR*/)
	: QMainWindow(parent)
	, _mainWidget(nullptr)
{
	ui.setupUi(this);
	QLabel* label = new QLabel(QString::fromLocal8Bit(" 请输入命令："));
	ui.statusBar->addWidget(label);
	_cmdlineEdit = new QLineEdit;
	ui.statusBar->addWidget(_cmdlineEdit);
	_cmdlineEdit->setEnabled(false);
	connect(_cmdlineEdit, SIGNAL(returnPressed()), this, SLOT(onCmd()));
	connect(ui.action_setup, SIGNAL(triggered()), this, SLOT(onSetup()));
}

void osgCmdUI::onCmd()
{
	std::string cmdline = _cmdlineEdit->text().toStdString();
	if (cmdline.find("--exit") == -1)
	{
		if (osgCmd_Send(cmdline.c_str()))
		{
			const char* szMessage = osgCmd_ErrorMessage();
			if (szMessage != nullptr && 0 != strcmp(szMessage, ""))
				QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit(szMessage));
		}
		else
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("发送命令失败！"));
	}
	else
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("发送命令失败！"));
}

void osgCmdUI::onSetup()
{
	SetupDlg dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		QString datadir = dlg.getDataDir();
		QStringList cmdset = dlg.getCmdset();
		if (cmdset.size() > 0)
		{
			if (_mainWidget)
			{
				delete _mainWidget;
				_mainWidget = nullptr;
			}

			_mainWidget = new osgCmdWidget(cmdset, datadir, true, this);
			setCentralWidget(_mainWidget);
			_cmdlineEdit->setEnabled(true);
		}
		else
		{
			QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("未启用任何插件！"));
		}
	}
}
