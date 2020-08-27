#ifndef __UI_MANAGER_SERVICE_H__
#define __UI_MANAGER_SERVICE_H__

#include <QMenu>
#include <QtPlugin>
#include <QDockWidget>

class UIManagerService
{
public:
	virtual void setWindowTitle(QString windowTitle) = 0;
	virtual void starWindowTitle() = 0;
	virtual void unstarWindowTitle() = 0;
	virtual void finishWindowLaunch() = 0;
	virtual QWidget* getWidget(const QString& strId) = 0;
	virtual QDockWidget* addWidget(const QString& strId, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area,
		Qt::DockWidgetAreas areas = Qt::AllDockWidgetAreas, bool isShow = true, bool hasToolBtn = true, bool hasSeparator = false) = 0;
	virtual void removeWidget(const QString& strId) = 0;
	virtual void addMenu(const QString &strId, QMenu* pSubMenu, bool hasToolButton = true) = 0;
	virtual void removeMenu(const QString& strId) = 0;
};

#define UIManagerService_iid "com.Interface.UIManagerService"
Q_DECLARE_INTERFACE(UIManagerService, UIManagerService_iid)

#endif // __UI_MANAGER_SERVICE_H__
