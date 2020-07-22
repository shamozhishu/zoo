#ifndef __ZOO_CMD_WIDGET_H__
#define __ZOO_CMD_WIDGET_H__

#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtOpenGL/QGL>

class QThread;
class QKeyEvent;
class QInputEvent;
class QMouseEvent;
class QWheelEvent;
class ZooCmdWidget : public QGLWidget
{
	Q_OBJECT
public:
	ZooCmdWidget(QString inputAdaName, QString datadir = "", bool mainThreadInit = true,
		QWidget* parent = Q_NULLPTR, const QGLWidget* shareWidget = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~ZooCmdWidget();
	void resgisterCmdset(QStringList cmdset);
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
	QTimer   _frameTimer;
	QThread* _initThread;
	QThread* _regCmdThread;
};

#endif // __ZOO_CMD_WIDGET_H__
