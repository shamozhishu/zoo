#include "osgCmdUI.h"
#include <QtWidgets/QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
	QSurfaceFormat format = QSurfaceFormat::defaultFormat();
	format.setVersion(2, 0);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setOption(QSurfaceFormat::DebugContext);

	format.setDepthBufferSize(24);
	format.setSamples(8);
	format.setStencilBufferSize(8);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

	QApplication a(argc, argv);
	osgCmdUI w;
	w.show();
	return a.exec();
}
