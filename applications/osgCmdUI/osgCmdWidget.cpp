#include "osgCmdWidget.h"
#include <vector>
#include <string>
#include <osgCmd.h>
#include <QScreen>
#include <QWindow>
#include <QLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMainWindow>
#include <QApplication>
#include <QInputDialog>
#include <QtCore/QThread>

class osgCmdInitThread : public QThread
{
public:
	osgCmdInitThread(osgCmdWidget* cmdWidget, QStringList cmdset, QString datadir = "")
		: _osgCmdWidget(cmdWidget)
		, _cmdset(cmdset)
		, _datadir(datadir) {}

	void init()
	{
		QScreen* screen = _osgCmdWidget->windowHandle() && _osgCmdWidget->windowHandle()->screen() ? _osgCmdWidget->windowHandle()->screen() : qApp->screens().front();

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

		osgCmd_Destroy();
		osgCmd_InitW(cmdcount, (const char**)cmdset, _datadir.toStdWString().c_str(), _osgCmdWidget->width(), _osgCmdWidget->height(), screen->devicePixelRatio());

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
	osgCmdWidget* _osgCmdWidget;
};

osgCmdWidget::osgCmdWidget(QStringList cmdset, QString datadir /*= ""*/, bool mainThreadInit /*= true*/, QWidget* parent /*= Q_NULLPTR*/
	, const QGLWidget* shareWidget /*= Q_NULLPTR*/, Qt::WindowFlags f /*= Qt::WindowFlags()*/)
	: QGLWidget(parent, shareWidget, f)
	, _initThread(new osgCmdInitThread(this, cmdset, datadir))
	, _mainThreadInit(mainThreadInit)
{
	setFocusPolicy(Qt::ClickFocus);

	osgCmd_RemapKeyboard(osgCmd_Key_Escape, Qt::Key_Escape);
	osgCmd_RemapKeyboard(osgCmd_Key_Delete, Qt::Key_Delete);
	osgCmd_RemapKeyboard(osgCmd_Key_Home, Qt::Key_Home);
	osgCmd_RemapKeyboard(osgCmd_Key_Enter, Qt::Key_Enter);
	osgCmd_RemapKeyboard(osgCmd_Key_End, Qt::Key_End);
	osgCmd_RemapKeyboard(osgCmd_Key_Return, Qt::Key_Return);
	osgCmd_RemapKeyboard(osgCmd_Key_PageUp, Qt::Key_PageUp);
	osgCmd_RemapKeyboard(osgCmd_Key_PageDown, Qt::Key_PageDown);
	osgCmd_RemapKeyboard(osgCmd_Key_Left, Qt::Key_Left);
	osgCmd_RemapKeyboard(osgCmd_Key_Right, Qt::Key_Right);
	osgCmd_RemapKeyboard(osgCmd_Key_Up, Qt::Key_Up);
	osgCmd_RemapKeyboard(osgCmd_Key_Down, Qt::Key_Down);
	osgCmd_RemapKeyboard(osgCmd_Key_Backspace, Qt::Key_Backspace);
	osgCmd_RemapKeyboard(osgCmd_Key_Tab, Qt::Key_Tab);
	osgCmd_RemapKeyboard(osgCmd_Key_Space, Qt::Key_Space);
	osgCmd_RemapKeyboard(osgCmd_Key_Alt, Qt::Key_Alt);
	osgCmd_RemapKeyboard(osgCmd_Key_Shift, Qt::Key_Shift);
	osgCmd_RemapKeyboard(osgCmd_Key_Control, Qt::Key_Control);
	osgCmd_RemapKeyboard(osgCmd_Key_Meta, Qt::Key_Meta);
	osgCmd_RemapKeyboard(osgCmd_Key_F1, Qt::Key_F1);
	osgCmd_RemapKeyboard(osgCmd_Key_F2, Qt::Key_F2);
	osgCmd_RemapKeyboard(osgCmd_Key_F3, Qt::Key_F3);
	osgCmd_RemapKeyboard(osgCmd_Key_F4, Qt::Key_F4);
	osgCmd_RemapKeyboard(osgCmd_Key_F5, Qt::Key_F5);
	osgCmd_RemapKeyboard(osgCmd_Key_F6, Qt::Key_F6);
	osgCmd_RemapKeyboard(osgCmd_Key_F7, Qt::Key_F7);
	osgCmd_RemapKeyboard(osgCmd_Key_F8, Qt::Key_F8);
	osgCmd_RemapKeyboard(osgCmd_Key_F9, Qt::Key_F9);
	osgCmd_RemapKeyboard(osgCmd_Key_F10, Qt::Key_F10);
	osgCmd_RemapKeyboard(osgCmd_Key_F11, Qt::Key_F11);
	osgCmd_RemapKeyboard(osgCmd_Key_F12, Qt::Key_F12);
	osgCmd_RemapKeyboard(osgCmd_Key_F13, Qt::Key_F13);
	osgCmd_RemapKeyboard(osgCmd_Key_F14, Qt::Key_F14);
	osgCmd_RemapKeyboard(osgCmd_Key_F15, Qt::Key_F15);
	osgCmd_RemapKeyboard(osgCmd_Key_F16, Qt::Key_F16);
	osgCmd_RemapKeyboard(osgCmd_Key_F17, Qt::Key_F17);
	osgCmd_RemapKeyboard(osgCmd_Key_F18, Qt::Key_F18);
	osgCmd_RemapKeyboard(osgCmd_Key_F19, Qt::Key_F19);
	osgCmd_RemapKeyboard(osgCmd_Key_F20, Qt::Key_F20);
	osgCmd_RemapKeyboard(osgCmd_Key_Hyphen, Qt::Key_hyphen);
	osgCmd_RemapKeyboard(osgCmd_Key_Equal, Qt::Key_Equal);
	osgCmd_RemapKeyboard(osgCmd_Key_Division, Qt::Key_division);
	osgCmd_RemapKeyboard(osgCmd_Key_Multiply, Qt::Key_multiply);
	osgCmd_RemapKeyboard(osgCmd_Key_Minus, Qt::Key_Minus);
	osgCmd_RemapKeyboard(osgCmd_Key_Plus, Qt::Key_Plus);
	osgCmd_RemapKeyboard(osgCmd_Key_Insert, Qt::Key_Insert);
}

osgCmdWidget::~osgCmdWidget()
{
	while (_initThread->isRunning())
		QThread::sleep(1);

	delete _initThread;
	osgCmd_Destroy();
}

void osgCmdWidget::initializeGL()
{
	if (_mainThreadInit)
		((osgCmdInitThread*)_initThread)->init();
	else
		_initThread->start();

	connect(&_frameTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
	_frameTimer.start(20);
}

void osgCmdWidget::resizeGL(int w, int h)
{
	QScreen* screen = windowHandle() && windowHandle()->screen() ? windowHandle()->screen() : qApp->screens().front();
	osgCmd_Resize(w, h, screen->devicePixelRatio());
}

void osgCmdWidget::paintGL()
{
	osgCmd_Render();
}

void osgCmdWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat())
		event->ignore();
	else
		osgCmd_KeyPressEvent(int(*(event->text().toLatin1().data())), getKeyboardModifiers(event));
}

void osgCmdWidget::keyReleaseEvent(QKeyEvent* event)
{
	if (event->isAutoRepeat())
		event->ignore();
	else
		osgCmd_KeyReleaseEvent(int(*(event->text().toLatin1().data())), getKeyboardModifiers(event));
}

void osgCmdWidget::mousePressEvent(QMouseEvent* event)
{
	osgCmd_MousePressEvent(event->x(), event->y(), getKeyboardModifiers(event), (osgCmd_MouseButton)getMouseButton(event));
}

void osgCmdWidget::mouseReleaseEvent(QMouseEvent* event)
{
	osgCmd_MouseReleaseEvent(event->x(), event->y(), getKeyboardModifiers(event), (osgCmd_MouseButton)getMouseButton(event));
}

void osgCmdWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	osgCmd_MouseDoubleClickEvent(event->x(), event->y(), getKeyboardModifiers(event), (osgCmd_MouseButton)getMouseButton(event));
}

void osgCmdWidget::mouseMoveEvent(QMouseEvent* event)
{
	osgCmd_MouseMoveEvent(event->x(), event->y(), getKeyboardModifiers(event));
}

void osgCmdWidget::wheelEvent(QWheelEvent* event)
{
	osgCmd_WheelEvent(event->x(), event->y(), getKeyboardModifiers(event), (event->orientation() == Qt::Vertical
		? (event->delta() > 0 ? osgCmd_Scroll_Up : osgCmd_Scroll_Down)
		: (event->delta() > 0 ? osgCmd_Scroll_Left : osgCmd_Scroll_Right)));
}

unsigned int osgCmdWidget::getMouseButton(QMouseEvent* event)
{
	osgCmd_MouseButton button = osgCmd_Button_No;
	switch (event->button())
	{
	case Qt::LeftButton:
		button = osgCmd_Button_Left;
		break;
	case Qt::MidButton:
		button = osgCmd_Button_Mid;
		break;
	case Qt::RightButton:
		button = osgCmd_Button_Right;
		break;
	case Qt::NoButton:
		button = osgCmd_Button_No;
		break;
	default:
		button = osgCmd_Button_No;
		break;
	}

	return button;
}

unsigned int osgCmdWidget::getKeyboardModifiers(QInputEvent* event)
{
	unsigned int modkey = event->modifiers() & (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier);
	unsigned int modkeymask = 0;

	if (modkey & Qt::ShiftModifier)
		modkeymask |= osgCmd_Modkey_Shist;

	if (modkey & Qt::ControlModifier)
		modkeymask |= osgCmd_Modkey_Ctrl;

	if (modkey & Qt::AltModifier)
		modkeymask |= osgCmd_Modkey_Alt;

	return modkeymask;
}
