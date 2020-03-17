#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgCmdUI.h"

class QLineEdit;
class osgCmdWidget;
class osgCmdUI : public QMainWindow
{
	Q_OBJECT
public:
	osgCmdUI(QWidget *parent = Q_NULLPTR);

private Q_SLOTS:
	void onCmd();
	void onSetup();

private:
	Ui::osgCmdUIClass ui;
	QLineEdit* _cmdlineEdit;
	osgCmdWidget* _mainWidget;
};
