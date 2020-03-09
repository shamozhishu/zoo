#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgCmdUI.h"
#include "osgCmdWidget.h"

class osgCmdUI : public QMainWindow
{
	Q_OBJECT
public:
	osgCmdUI(QWidget *parent = Q_NULLPTR);

private:
	osgCmdWidget _widget;
	Ui::osgCmdUIClass ui;
};
