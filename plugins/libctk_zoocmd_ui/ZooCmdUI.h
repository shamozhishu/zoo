#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ZooCmdUI.h"

class QLineEdit;
class QProgressBar;
class ZooCmdWidget;
class ZooCmdUI : public QMainWindow
{
	Q_OBJECT
public:
	ZooCmdUI(QWidget *parent = Q_NULLPTR);

protected:
	void keyPressEvent(QKeyEvent *event);

private Q_SLOTS:
	void onCmd();
	void onOpen();
	void onSave();
	void onSim(bool checked);
	void onSetup();

private:
	void createZooCmdWidget(QStringList cmdset, QString datadir);

private:
	int _idx;
	Ui::ZooCmdUIClass _ui;
	QString _inputAdaName;
	QLineEdit* _cmdlineEdit;
	QProgressBar* _progressBar;
	ZooCmdWidget* _mainWidget;
	QVector<QString> _cmdlines;
};
