#include "ZooCmdWgt.h"
#include <zooCmdLoader/ZooCmdLoader.h>
#include <vector>
#include <string>
#include <QThread>
#include <QScreen>
#include <QWindow>
#include <QLayout>
#include <QSettings>
#include <QMessageBox>
#include <QMainWindow>
#include <QApplication>
#include <QInputDialog>

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

// 在单独的线程中启动渲染循环,目前会导致程序崩溃,所以暂时不启用这个类的run().
class RenderThread : public QThread
{
public:
	virtual ~RenderThread()
	{
		zooCmd_Destroy();
		wait();
	}

	void run()
	{
		zooCmd_Run();
	}
};

class CmdRegThread : public QThread
{
public:
	CmdRegThread(QStringList cmdset) : _cmdset(cmdset), _completed(false) {}
	bool isCompleted() const { return _completed; }
	QString getErrTip() const { return _errStr; }

	void init()
	{
		zooCmd_UnregisterAll();
		regCmdset();
		_completed = true;
	}

	void setCmdset(QStringList cmdset)
	{
		_completed = false;
		_cmdset = cmdset;
		_errStr = "";
	}

protected:
	void run() override
	{
		init();
	}

	void regCmdset()
	{
		int len = _cmdset.size();
		for (int i = 0; i < len; ++i)
		{
			if (!zooCmd_Register(_cmdset[i].toStdString().c_str()))
			{
				if (_errStr == "")
					_errStr = _cmdset[i];
				else
					_errStr += (QString(", ") + _cmdset[i]);
			}
		}

		if (_errStr != "")
			_errStr = QString("加载[") + _errStr + "]命令插件失败！";
	}

protected:
	bool _completed;
	QString _errStr;
	QStringList _cmdset;
};

ZooCmdWgt::ZooCmdWgt(QString renderAdaName, QStringList cmdset, QString datadir /*= ""*/, QWidget* parent /*= Q_NULLPTR*/)
	: QWidget(parent)
	, _renderThread(new RenderThread)
	, _cmdRegThread(new CmdRegThread(cmdset))
{
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	zooCmdL_Load();

	QScreen* screen = windowHandle() && windowHandle()->screen() ? windowHandle()->screen() : qApp->screens().front();
	zooCmd_Destroy();
	if (zooCmd_InitW(renderAdaName.toStdString().c_str(), datadir.toStdWString().c_str()))
	{
		QWidget* pGLWgt = (QWidget*)zooCmd_Setup(width(), height(), screen->devicePixelRatio(), ZOOCMDWGT);
		if (pGLWgt)
		{
			setLayout(new QHBoxLayout);
			layout()->setMargin(0);
			layout()->addWidget(pGLWgt);
			resgisterCmdset(cmdset);
		}
		else
			QMessageBox::warning(nullptr, tr("警告"), "初始化GLWidget窗体失败！");
	}
	else
		QMessageBox::warning(nullptr, tr("警告"), "初始化zooCmd框架失败！");
}

ZooCmdWgt::~ZooCmdWgt()
{
	while (_cmdRegThread->isRunning())
		QThread::sleep(1);

	delete _cmdRegThread;
	delete _renderThread;
}

HWND ZooCmdWgt::getWnd()
{
	return (HWND)winId();
}

bool ZooCmdWgt::isFocus()
{
	return isActiveWindow();
}

void ZooCmdWgt::resgisterCmdset(QStringList cmdset)
{
	_cmdRegThread->setCmdset(cmdset);
	_cmdRegThread->start();

	emit cmdRegistered();

	QString errTip = _cmdRegThread->getErrTip();
	if (errTip != "")
		QMessageBox::warning(this, tr("警告"), errTip);
	else
	{
		QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
		settings.beginGroup("ZOO_CMDSET");
		settings.setValue("activecmd", cmdset);
		settings.endGroup();
	}
}

bool ZooCmdWgt::isCmdRegCompleted() const
{
	return _cmdRegThread->isCompleted();
}
