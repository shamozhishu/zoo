#ifndef __ZOO_CMD_UI_H__
#define __ZOO_CMD_UI_H__

#include "ui_ZooCmdUI.h"
#include <QtWidgets/QMainWindow>
#include <ctk_service/zoocmd_ui/UIManagerService.h>

class ZooCmdWgt;
class QLineEdit;
class QProgressBar;
class ZooCmdUI : public QMainWindow, public UIManagerService
{
	Q_OBJECT
	Q_INTERFACES(UIManagerService)
public:
	ZooCmdUI(QWidget *parent = Q_NULLPTR);
	~ZooCmdUI();
	bool needSavedScene();
	void setWindowTitle(QString windowTitle);
	void starWindowTitle();
	void unstarWindowTitle();
	void finishWindowLaunch();
	QWidget* getWidget(const QString& wgtID);
	void raiseWidget(const QString& wgtID);
	QDockWidget* addWidget(const QString& wgtID, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area,
		Qt::DockWidgetAreas areas = Qt::AllDockWidgetAreas, bool isShow = true, bool hasToolBtn = true, bool hasSeparator = false);
	void removeWidget(const QString& wgtID);
	void tabifyWidget(const QString& firstWgtID, const QString& secondWgtID);
	void addMenu(const QString &menuID, QMenu* pSubMenu, bool hasToolBtn = true);
	void removeMenu(const QString& menuID);

protected:
	void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
	void onCmd();
	void onSetup();

private:
	int _idx;
	Ui::ZooCmdUIClass _ui;
	QString _inputAdaName;
	ZooCmdWgt* _mainWidget;
	QLineEdit* _cmdlineEdit;
	QProgressBar* _progressBar;
	QVector<QString> _cmdlines;
	QMap<QString, QPair<QMenu*, bool>> _subMenus;
	QMap<QString, QPair<QDockWidget*, QAction*>> _dockWgts;
};

#endif // __ZOO_CMD_UI_H__
