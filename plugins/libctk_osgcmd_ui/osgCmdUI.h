#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_osgCmdUI.h"

class QLineEdit;
class QProgressBar;
class osgCmdWidget;
class osgCmdUI : public QMainWindow
{
	Q_OBJECT
public:
	osgCmdUI(QWidget *parent = Q_NULLPTR);

protected:
	void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
	void onCmd();
	void onSetup();

private:
	int _idx;
	Ui::osgCmdUIClass ui;
	QLineEdit* _cmdlineEdit;
	QProgressBar* _progressBar;
	osgCmdWidget* _mainWidget;
	QVector<QString> _cmdlines;
};
