#include "MenuToolButtons.h"
#include "ArmyListWgt.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include <QMenu>
#include <QMessageBox>

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

void MenuToolButtons::create(UIManagerService* service, EMenu menutype, QObject* receiver, bool hasToolButton)
{
	QMenu* pSubMenu = nullptr;
	switch (menutype)
	{
	case scene_:
	{
		pSubMenu = new QMenu("场景");
		QAction* act = new QAction(QIcon(QPixmap(":/images/Resources/images/open.png")), "打开");
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onOpen()));
		pSubMenu->addAction(act);
		act = new QAction(QIcon(QPixmap(":/images/Resources/images/close.png")), "关闭");
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onClose()));
		pSubMenu->addAction(act);
		act = new QAction(QIcon(QPixmap(":/images/Resources/images/save.png")), "保存");
		act->setEnabled(false);
		QObject::connect(act, SIGNAL(triggered()), receiver, SLOT(onSave()));
		pSubMenu->addAction(act);

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendSucceed, [act]
		{
			act->setEnabled(true);
		});
		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendFailed, [act]
		{
			act->setEnabled(false);
		});
	}
	break;
	case simulation_:
	{
		pSubMenu = new QMenu("仿真");
		QAction* actStart = new QAction(QIcon(QPixmap(":/images/Resources/images/start.png")), "开始");
		actStart->setEnabled(false);
		actStart->setCheckable(true);
		pSubMenu->addAction(actStart);
		QAction* actPause = new QAction(QIcon(QPixmap(":/images/Resources/images/pause.png")), "暂停");
		actPause->setEnabled(false);
		actPause->setCheckable(true);
		pSubMenu->addAction(actPause);

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendSucceed, [actStart, actPause]
		{
			actStart->setEnabled(true);
			actPause->setEnabled(false);

			actStart->setText("开始");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/start.png")));
			actStart->setChecked(false);
			actPause->setChecked(false);
		});

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::sceneOpendFailed, [actStart, actPause]
		{
			actStart->setEnabled(false);
			actPause->setEnabled(false);

			actStart->setText("开始");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/start.png")));
			actStart->setChecked(false);
			actPause->setChecked(false);
		});

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::simStarted, [actStart, actPause]
		{
			actStart->setText("终止");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/stop.png")));
			actPause->setEnabled(true);
		});

		QObject::connect(qobject_cast<ArmyListWgt*>(receiver), &ArmyListWgt::simStoped, [actStart, actPause]
		{
			actStart->setText("开始");
			actStart->setIcon(QIcon(QPixmap(":/images/Resources/images/start.png")));
			actPause->setChecked(false);
			actPause->setEnabled(false);
		});

		QObject::connect(actStart, SIGNAL(triggered(bool)), receiver, SLOT(onSim(bool)));
		QObject::connect(actPause, SIGNAL(triggered(bool)), receiver, SLOT(onPaused(bool)));
	}
	break;
	case help_:
	{
		pSubMenu = new QMenu("帮助");
		QAction* act = new QAction(QIcon(QPixmap(":/images/Resources/images/about.png")), "关于");
		pSubMenu->addAction(act);
		QObject::connect(act, &QAction::triggered, [service]
		{
			QMessageBox::about(dynamic_cast<QWidget*>(service), "关于", "战场编辑器 - 版本1.0");
		});
	}
	break;
	default:
		break;
	}

	if (pSubMenu)
		service->addMenu(QString("MENU_%1").arg(menutype), pSubMenu, hasToolButton);
}

void MenuToolButtons::destroy(UIManagerService* service, EMenu menutype)
{
	service->removeMenu(QString("MENU_%1").arg(menutype));
}
