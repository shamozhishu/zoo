#include "osgCmdUI.h"
#include <osgCmd.h>
#include <QLabel>
#include <QThread>
#include <QLineEdit>
#include <QKeyEvent>
#include <QMessageBox>
#include <QProgressBar>
#include "SetupDlg.h"
#include "osgCmdWidget.h"

osgCmdUI::osgCmdUI(QWidget* parent /*= Q_NULLPTR*/)
	: QMainWindow(parent)
	, _idx(0)
{
	ui.setupUi(this);
	QLabel* label = new QLabel(QString::fromLocal8Bit(" ���������"));
	ui.statusBar->addWidget(label);
	_cmdlineEdit = new QLineEdit;
	ui.statusBar->addWidget(_cmdlineEdit);
	_cmdlineEdit->setFixedWidth(500);
	_progressBar = new QProgressBar;
	ui.statusBar->addWidget(_progressBar);
	_progressBar->setMinimum(0);
	_progressBar->setMaximum(100);
	_progressBar->setFixedWidth(100);
	_progressBar->setVisible(false);
	_progressBar->setTextVisible(false);
	
	_mainWidget = new osgCmdWidget(QStringList(), "", true, this);
	setCentralWidget(_mainWidget);

	connect(_cmdlineEdit, SIGNAL(returnPressed()), this, SLOT(onCmd()));
	connect(ui.action_setup, SIGNAL(triggered()), this, SLOT(onSetup()));
}

void osgCmdUI::keyPressEvent(QKeyEvent *event)
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

void osgCmdUI::onCmd()
{
	std::string cmdline = _cmdlineEdit->text().toStdString();
	if (cmdline.find("-exit") != -1 || !osgCmd_Send(cmdline.c_str()))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��������ʧ�ܣ�"));
		return;
	}

	const char* szMessage = osgCmd_ErrorMessage();
	if (0 != strcmp(szMessage, ""))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit(szMessage));
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

void osgCmdUI::onSetup()
{
	SetupDlg dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		if (_mainWidget)
		{
			delete _mainWidget;
			_mainWidget = nullptr;
		}

		bool mainThreadInit = false;
		_cmdlineEdit->setEnabled(false);
		_mainWidget = new osgCmdWidget(dlg.getCmdset(), dlg.getDataDir(), mainThreadInit, this);
		setCentralWidget(_mainWidget);
		connect(_mainWidget, &osgCmdWidget::inited, [this, mainThreadInit]
		{
			if (!mainThreadInit)
			{
				_progressBar->setValue(0);
				_progressBar->setVisible(true);

				int i = 0;
				while (true)
				{
					if (osgCmd_IsInited())
					{
						_progressBar->setValue(100);
						_progressBar->setVisible(false);
						break;
					}

					QThread::msleep(10);
					_progressBar->setValue(i++);
					if (i > 100)
						i = 0;
				}
			}

			const char* szMessage = osgCmd_ErrorMessage();
			if (0 != strcmp(szMessage, ""))
				QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit(szMessage));
			_cmdlineEdit->setEnabled(true);
		});
	}
}
