#include "ZooCmdWidget.h"
#include <zooCmd/zooCmd.h>
#include <vector>
#include <string>
#include <QThread>
#include <QScreen>
#include <QWindow>
#include <QLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMainWindow>
#include <QApplication>
#include <QInputDialog>

class ZooCmdInitThread : public QThread
{
public:
	ZooCmdInitThread(ZooCmdWidget* cmdWidget, QStringList cmdset, QString inputAdaName, QString datadir = "")
		: _zooCmdWidget(cmdWidget)
		, _cmdset(cmdset)
		, _datadir(datadir)
		, _inputAdaName(inputAdaName) {}

	void init()
	{
		QScreen* screen = _zooCmdWidget->windowHandle() && _zooCmdWidget->windowHandle()->screen() ? _zooCmdWidget->windowHandle()->screen() : qApp->screens().front();

		char** cmdset = nullptr;
		int cmdcount = _cmdset.size();
		std::vector<std::string> vecCmdset;
		if (cmdcount > 0)
		{
			vecCmdset.resize(cmdcount);
			cmdset = new char*[cmdcount];
			for (int i = 0; i < cmdcount; ++i)
			{
				vecCmdset[i] = _cmdset[i].toStdString();
				cmdset[i] = const_cast<char*>(vecCmdset[i].c_str());
			}
		}

		zooCmd_Destroy();
		zooCmd_InitW(cmdcount, (const char**)cmdset, _inputAdaName.toStdString().c_str(), _datadir.toStdWString().c_str(), _zooCmdWidget->width(), _zooCmdWidget->height(), screen->devicePixelRatio());

		if (cmdset)
			delete[] cmdset;
	}

protected:
	void run() override
	{
		init();
	}

private:
	QStringList   _cmdset;
	QString       _datadir;
	QString       _inputAdaName;
	ZooCmdWidget* _zooCmdWidget;
};

ZooCmdWidget::ZooCmdWidget(QStringList cmdset, QString inputAdaName, QString datadir /*= ""*/, bool mainThreadInit /*= true*/,
	QWidget* parent /*= Q_NULLPTR*/, const QGLWidget* shareWidget /*= Q_NULLPTR*/, Qt::WindowFlags f /*= Qt::WindowFlags()*/)
	: QGLWidget(parent, shareWidget, f)
	, _initThread(new ZooCmdInitThread(this, cmdset, inputAdaName, datadir))
	, _mainThreadInit(mainThreadInit)
	, _isInited(false)
{
	setFocusPolicy(Qt::ClickFocus);

	zooCmd_RemapKeyboard(zooCmd_Key_Escape, Qt::Key_Escape);
	zooCmd_RemapKeyboard(zooCmd_Key_Delete, Qt::Key_Delete);
	zooCmd_RemapKeyboard(zooCmd_Key_Home, Qt::Key_Home);
	zooCmd_RemapKeyboard(zooCmd_Key_Enter, Qt::Key_Enter);
	zooCmd_RemapKeyboard(zooCmd_Key_End, Qt::Key_End);
	zooCmd_RemapKeyboard(zooCmd_Key_Return, Qt::Key_Return);
	zooCmd_RemapKeyboard(zooCmd_Key_PageUp, Qt::Key_PageUp);
	zooCmd_RemapKeyboard(zooCmd_Key_PageDown, Qt::Key_PageDown);
	zooCmd_RemapKeyboard(zooCmd_Key_Left, Qt::Key_Left);
	zooCmd_RemapKeyboard(zooCmd_Key_Right, Qt::Key_Right);
	zooCmd_RemapKeyboard(zooCmd_Key_Up, Qt::Key_Up);
	zooCmd_RemapKeyboard(zooCmd_Key_Down, Qt::Key_Down);
	zooCmd_RemapKeyboard(zooCmd_Key_Backspace, Qt::Key_Backspace);
	zooCmd_RemapKeyboard(zooCmd_Key_Tab, Qt::Key_Tab);
	zooCmd_RemapKeyboard(zooCmd_Key_Space, Qt::Key_Space);
	zooCmd_RemapKeyboard(zooCmd_Key_Alt, Qt::Key_Alt);
	zooCmd_RemapKeyboard(zooCmd_Key_Shift, Qt::Key_Shift);
	zooCmd_RemapKeyboard(zooCmd_Key_Control, Qt::Key_Control);
	zooCmd_RemapKeyboard(zooCmd_Key_Meta, Qt::Key_Meta);
	zooCmd_RemapKeyboard(zooCmd_Key_F1, Qt::Key_F1);
	zooCmd_RemapKeyboard(zooCmd_Key_F2, Qt::Key_F2);
	zooCmd_RemapKeyboard(zooCmd_Key_F3, Qt::Key_F3);
	zooCmd_RemapKeyboard(zooCmd_Key_F4, Qt::Key_F4);
	zooCmd_RemapKeyboard(zooCmd_Key_F5, Qt::Key_F5);
	zooCmd_RemapKeyboard(zooCmd_Key_F6, Qt::Key_F6);
	zooCmd_RemapKeyboard(zooCmd_Key_F7, Qt::Key_F7);
	zooCmd_RemapKeyboard(zooCmd_Key_F8, Qt::Key_F8);
	zooCmd_RemapKeyboard(zooCmd_Key_F9, Qt::Key_F9);
	zooCmd_RemapKeyboard(zooCmd_Key_F10, Qt::Key_F10);
	zooCmd_RemapKeyboard(zooCmd_Key_F11, Qt::Key_F11);
	zooCmd_RemapKeyboard(zooCmd_Key_F12, Qt::Key_F12);
	zooCmd_RemapKeyboard(zooCmd_Key_F13, Qt::Key_F13);
	zooCmd_RemapKeyboard(zooCmd_Key_F14, Qt::Key_F14);
	zooCmd_RemapKeyboard(zooCmd_Key_F15, Qt::Key_F15);
	zooCmd_RemapKeyboard(zooCmd_Key_F16, Qt::Key_F16);
	zooCmd_RemapKeyboard(zooCmd_Key_F17, Qt::Key_F17);
	zooCmd_RemapKeyboard(zooCmd_Key_F18, Qt::Key_F18);
	zooCmd_RemapKeyboard(zooCmd_Key_F19, Qt::Key_F19);
	zooCmd_RemapKeyboard(zooCmd_Key_F20, Qt::Key_F20);
	zooCmd_RemapKeyboard(zooCmd_Key_Hyphen, Qt::Key_hyphen);
	zooCmd_RemapKeyboard(zooCmd_Key_Equal, Qt::Key_Equal);
	zooCmd_RemapKeyboard(zooCmd_Key_Division, Qt::Key_division);
	zooCmd_RemapKeyboard(zooCmd_Key_Multiply, Qt::Key_multiply);
	zooCmd_RemapKeyboard(zooCmd_Key_Minus, Qt::Key_Minus);
	zooCmd_RemapKeyboard(zooCmd_Key_Plus, Qt::Key_Plus);
	zooCmd_RemapKeyboard(zooCmd_Key_Insert, Qt::Key_Insert);
}

ZooCmdWidget::~ZooCmdWidget()
{
	while (_initThread->isRunning())
		QThread::sleep(1);

	delete _initThread;
	zooCmd_Destroy();
}

void ZooCmdWidget::initializeGL()
{
	if (!_isInited)
	{
		_isInited = true;

		if (_mainThreadInit)
			((ZooCmdInitThread*)_initThread)->init();
		else
			_initThread->start();

		emit inited();
		connect(&_frameTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
		_frameTimer.start(20);
	}
}

void ZooCmdWidget::resizeGL(int w, int h)
{
	QScreen* screen = windowHandle() && windowHandle()->screen() ? windowHandle()->screen() : qApp->screens().front();
	zooCmd_Resize(w, h, screen->devicePixelRatio());
}

void ZooCmdWidget::paintGL()
{
	zooCmd_Tick();
	zooCmd_Render();
}

void ZooCmdWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat())
		event->ignore();
	else
		zooCmd_KeyPress(int(*(event->text().toLatin1().data())), getKeyboardModifiers(event));
}

void ZooCmdWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat())
		event->ignore();
	else
		zooCmd_KeyRelease(int(*(event->text().toLatin1().data())), getKeyboardModifiers(event));
}

void ZooCmdWidget::mousePressEvent(QMouseEvent* event)
{
	zooCmd_MousePress(event->x(), event->y(), getKeyboardModifiers(event), (zooCmd_MouseButton)getMouseButton(event));
}

void ZooCmdWidget::mouseReleaseEvent(QMouseEvent* event)
{
	zooCmd_MouseRelease(event->x(), event->y(), getKeyboardModifiers(event), (zooCmd_MouseButton)getMouseButton(event));
}

void ZooCmdWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	zooCmd_MouseDoubleClick(event->x(), event->y(), getKeyboardModifiers(event), (zooCmd_MouseButton)getMouseButton(event));
}

void ZooCmdWidget::mouseMoveEvent(QMouseEvent* event)
{
	zooCmd_MouseMove(event->x(), event->y(), getKeyboardModifiers(event));
}

void ZooCmdWidget::wheelEvent(QWheelEvent* event)
{
	zooCmd_Wheel(event->x(), event->y(), getKeyboardModifiers(event), (event->orientation() == Qt::Vertical
		? (event->delta() > 0 ? zooCmd_Scroll_Up : zooCmd_Scroll_Down)
		: (event->delta() > 0 ? zooCmd_Scroll_Left : zooCmd_Scroll_Right)));
}

unsigned int ZooCmdWidget::getMouseButton(QMouseEvent* event)
{
	zooCmd_MouseButton button = zooCmd_Button_No;
	switch (event->button())
	{
	case Qt::LeftButton:
		button = zooCmd_Button_Left;
		break;
	case Qt::MidButton:
		button = zooCmd_Button_Mid;
		break;
	case Qt::RightButton:
		button = zooCmd_Button_Right;
		break;
	case Qt::NoButton:
		button = zooCmd_Button_No;
		break;
	default:
		button = zooCmd_Button_No;
		break;
	}

	return button;
}

unsigned int ZooCmdWidget::getKeyboardModifiers(QInputEvent* event)
{
	unsigned int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
	unsigned int modkeymask = 0;

	if (modkey & Qt::ShiftModifier)
		modkeymask |= zooCmd_Modkey_Shist;

	if (modkey & Qt::ControlModifier)
		modkeymask |= zooCmd_Modkey_Ctrl;

	if (modkey & Qt::AltModifier)
		modkeymask |= zooCmd_Modkey_Alt;

	return modkeymask;
}
