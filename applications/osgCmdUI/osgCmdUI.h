#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgCmdUI.h"
#include "osgCmdWidget.h"

class QLineEdit;
class osgCmdUI : public QMainWindow
{
	Q_OBJECT
public:
	osgCmdUI(QWidget *parent = Q_NULLPTR);

private Q_SLOTS:
	void testCommand();

private:
	osgCmdWidget _widget;
	Ui::osgCmdUIClass ui;
	QLineEdit* _cmdlineEdit;
};
