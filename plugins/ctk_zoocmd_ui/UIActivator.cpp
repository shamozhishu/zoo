#include "UIActivator.h"
#include "ZooCmdUI.h"
#include <QSurfaceFormat>

// Qt5ÖÐÎÄÂÒÂë
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

ctkPluginContext* UIActivator::_pluginContext = nullptr;
UIActivator::UIActivator()
	: _ui(nullptr)
{
}

void UIActivator::start(ctkPluginContext* context)
{
	_pluginContext = context;

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
	ctkDictionary props;
	props.insert(ctkPluginConstants::SERVICE_RANKING, 0);
	context->registerService<UIManagerService>(_ui, props);
}

void UIActivator::stop(ctkPluginContext* context)
{
	if (_ui)
	{
		delete _ui;
		_ui = nullptr;
	}
	_pluginContext = nullptr;
}

ctkPluginContext* UIActivator::getPluginContext()
{
	return _pluginContext;
}
