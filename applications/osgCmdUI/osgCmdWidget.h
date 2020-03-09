#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtCore/QTimer>
#include <QtOpenGL/QGL>

class QKeyEvent;
class QInputEvent;
class QMouseEvent;
class QWheelEvent;

class osgCmdWidget : public QGLWidget
{
	Q_OBJECT
public:
	osgCmdWidget(int cmdcount, const char* cmdset[], const char* workdir = Q_NULLPTR,
		QWidget* parent = Q_NULLPTR, const QGLWidget* shareWidget = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~osgCmdWidget();

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
	QTimer _timer;
};
