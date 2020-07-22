#ifndef __ZOO_CMD_UI_H__
#define __ZOO_CMD_UI_H__

#include <QtWidgets/QMainWindow>
#include "ui_ZooCmdUI.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>

class QLineEdit;
class QProgressBar;
class ZooCmdWidget;
class ZooCmdUI : public QMainWindow, public UIManagerService
{
	Q_OBJECT
	Q_INTERFACES(UIManagerService)
public:
	ZooCmdUI(QWidget *parent = Q_NULLPTR);
	void addWidget(const QString& strId, const QString& strName, QWidget* pWidget, const QIcon& icon, Qt::DockWidgetArea area, Qt::DockWidgetAreas areas = Qt::AllDockWidgetAreas);
	void removeWidget(const QString& strId);
	QWidget* getWidget(const QString& strId);
	void addMenu(const QString &strId, QMenu* pSubMenu);
	void removeMenu(const QString& strId);
	void addToolButton(const QString &strId, QActionGroup* pActionGroup);
	void removeToolButton(const QString& strId);

protected:
	void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
	void onCmd();
	void onSetup();

private:
	int _idx;
	Ui::ZooCmdUIClass _ui;
	QString _inputAdaName;
	QLineEdit* _cmdlineEdit;
	QProgressBar* _progressBar;
	ZooCmdWidget* _mainWidget;
	QVector<QString> _cmdlines;
	QMap<QString, QMenu*> _subMenus;
	QMap<QString, QDockWidget*> _dockWgts;
	QMap<QString, QActionGroup*> _actionItems;
};

#endif // __ZOO_CMD_UI_H__
