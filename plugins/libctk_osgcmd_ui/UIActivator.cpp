#include "UIActivator.h"
#include "osgCmdUI.h"
#include <QSurfaceFormat>

void UIActivator::start(ctkPluginContext* context)
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

	_ui = new osgCmdUI();
	_ui->showMaximized();
}

void UIActivator::stop(ctkPluginContext* context)
{
	delete _ui;
	_ui = nullptr;
}
