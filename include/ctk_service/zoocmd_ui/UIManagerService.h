#ifndef __UI_MANAGER_SERVICE_H__
#define __UI_MANAGER_SERVICE_H__

#include <QMenu>
#include <QtPlugin>
#include <QDockWidget>

class UIManagerService
{
public:
	virtual bool needSavedScene() = 0;
	virtual void setWindowTitle(QString windowTitle) = 0;
	virtual void starWindowTitle() = 0;
	virtual void unstarWindowTitle() = 0;
	virtual void finishWindowLaunch() = 0;
	virtual QWidget* getWidget(const QString& wgtID) = 0;
	virtual void raiseWidget(const QString& wgtID) = 0;
	virtual QDockWidget* addWidget(const QString& wgtID, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area,
		Qt::DockWidgetAreas areas = Qt::AllDockWidgetAreas, bool isShow = true, bool hasToolBtn = true, bool hasSeparator = false) = 0;
	virtual void removeWidget(const QString& wgtID) = 0;
	virtual void tabifyWidget(const QString& firstWgtID, const QString& secondWgtID) = 0;
	virtual void addMenu(const QString &menuID, QMenu* pSubMenu, bool hasToolBtn = true) = 0;
	virtual void removeMenu(const QString& menuID) = 0;
};

#define UIManagerService_iid "com.Interface.UIManagerService"
Q_DECLARE_INTERFACE(UIManagerService, UIManagerService_iid)

#endif // __UI_MANAGER_SERVICE_H__
