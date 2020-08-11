#include "UIActivator.h"
#include "ArmyListWgt.h"
#include "MenuToolButtons.h"
#include "ComPropertyBoard.h"
#include <QMessageBox>
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include <zooCmdLoader/ZooCmdLoader.h>

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

ctkPluginContext* UIActivator::_pluginContext = nullptr;
UIActivator::UIActivator()
{
	zooCmdL_Load();
}

void UIActivator::start(ctkPluginContext* context)
{
	_pluginContext = context;

	UIManagerService* service = UIActivator::getService<UIManagerService>();
	if (service != Q_NULLPTR)
	{
		service->finishWindowLaunch();
		service->setWindowTitle(tr("战场编辑器"));
		ArmyListWgt* pArmyListWidget = new ArmyListWgt;
		MenuToolButtons::create(service, MenuToolButtons::file_, pArmyListWidget);
		MenuToolButtons::create(service, MenuToolButtons::edit_, pArmyListWidget);
		service->addWidget(CTK_WAR_UI_LIST_WIDGET, tr("实体导航"), pArmyListWidget, QIcon(QPixmap(":/images/Resources/images/entnavig.png")),
			Qt::LeftDockWidgetArea, Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		ComPropertyBoard* pComPropertyBoard = new ComPropertyBoard;
		service->addWidget(CTK_WAR_UI_PROPERTY_BOARD, tr("组件属性"), pComPropertyBoard, QIcon(QPixmap(":/images/Resources/images/entparam.png")),
			Qt::RightDockWidgetArea, Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	}
	else
	{
		zoo_warning(zoo::utf8ToAnsi("获取“pointer of UIManagerService”为空！").c_str());
	}
}

void UIActivator::stop(ctkPluginContext* context)
{
	UIManagerService* service = UIActivator::getService<UIManagerService>();
	if (service != Q_NULLPTR)
	{
		service->removeWidget(CTK_WAR_UI_PROPERTY_BOARD);
		service->removeWidget(CTK_WAR_UI_LIST_WIDGET);
		MenuToolButtons::destroy(service, MenuToolButtons::file_);
		MenuToolButtons::destroy(service, MenuToolButtons::edit_);
	}

	_pluginContext = nullptr;
}

void UIActivator::sendWarCmd(QString cmdline)
{
	if (!zooCmd_Send(cmdline.trimmed().toLocal8Bit()))
	{
		QMessageBox::warning(nullptr, tr("警告"), QString::fromLocal8Bit(zooCmd_TipMessage()));
		return;
	}

	const char* szTips = zooCmd_TipMessage();
	if (0 != strcmp(szTips, ""))
		QMessageBox::information(nullptr, tr("提示"), QString::fromLocal8Bit(szTips));
}
