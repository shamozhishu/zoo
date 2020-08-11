#ifndef __UI_MANAGER_SERVICE_H__
#define __UI_MANAGER_SERVICE_H__

#include <QtPlugin>
#include <QActionGroup>

class UIManagerService
{
public:
	virtual void setWindowTitle(QString windowTitle) = 0;
	virtual void starWindowTitle() = 0;
	virtual void unstarWindowTitle() = 0;
	virtual void finishWindowLaunch() = 0;
	virtual void addWidget(const QString& strId, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area, Qt::DockWidgetAreas areas = Qt::AllDockWidgetAreas) = 0;
	virtual void removeWidget(const QString& strId) = 0;
	virtual QWidget* getWidget(const QString& strId) = 0;
	virtual void addMenu(const QString &strId, QMenu* pSubMenu) = 0;
	virtual void removeMenu(const QString& strId) = 0;
	virtual void addToolButton(const QString &strId, QActionGroup* pActionGroup) = 0;
	virtual void removeToolButton(const QString& strId) = 0;
};

#define UIManagerService_iid "com.Interface.UIManagerService"
Q_DECLARE_INTERFACE(UIManagerService, UIManagerService_iid)

#endif // __UI_MANAGER_SERVICE_H__
