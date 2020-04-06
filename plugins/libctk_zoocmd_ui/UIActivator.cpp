#include "UIActivator.h"
#include "ZooCmdUI.h"
#include <QSurfaceFormat>

ctkPluginContext* UIActivator::_pluginContext = nullptr;
UIActivator::UIActivator()
	: _ui(nullptr)
{
}

UIActivator::~UIActivator()
{
	if (_ui)
		delete _ui;
}

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

	_ui = new ZooCmdUI();
	_ui->showMaximized();
	_pluginContext = context;
}

void UIActivator::stop(ctkPluginContext* context)
{
	_pluginContext = nullptr;
	if (_ui)
	{
		delete _ui;
		_ui = nullptr;
	}
}

ctkPluginContext* UIActivator::getPluginContext()
{
	return _pluginContext;
}
