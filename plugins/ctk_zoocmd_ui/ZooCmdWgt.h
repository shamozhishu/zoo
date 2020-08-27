#ifndef __ZOO_CMD_WGT_H__
#define __ZOO_CMD_WGT_H__

#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtOpenGL/QGL>
#include <ctk_service/zoocmd_ui/Win32Service.h>

class CmdThread;
class QKeyEvent;
class QInputEvent;
class QMouseEvent;
class QWheelEvent;

class ZooCmdWgt : public QGLWidget, public Win32Service
{
	Q_OBJECT
	Q_INTERFACES(Win32Service)
public:
	ZooCmdWgt(QString inputAdaName, QStringList cmdset, QString datadir = "", bool mainThreadInit = true,
		QWidget* parent = Q_NULLPTR, const QGLWidget* shareWidget = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~ZooCmdWgt();
	HWND getWnd();
	bool isFocus();
	void resgisterCmdset(QStringList cmdset);
	bool isInitCmdCompleted() const;
	bool isRegCmdCompleted() const;

signals:
	void inited();
	void cmdRegistered();

protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	unsigned int getMouseButton(QMouseEvent* event);
	unsigned int getKeyboardModifiers(QInputEvent* event);

private:
	bool _isInited;
	bool _mainThreadInit;
	QTimer _frameTimer;
	CmdThread* _initThread;
	CmdThread* _regCmdThread;
};

#endif // __ZOO_CMD_WGT_H__
