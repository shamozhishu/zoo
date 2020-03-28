#pragma once
#include <qdialog.h>
#include "ui_SetupDlg.h"

class QStandardItemModel;
class SetupDlg : public QDialog
{
	Q_OBJECT
public:
	SetupDlg(QWidget *parent = Q_NULLPTR);
	QString getDataDir() const;
	QStringList getCmdset() const;

private Q_SLOTS:
	void onOk();
	void onSetDataDir();

private:
	QString _datadir;
	QStringList _cmdset;
	Ui::SetupDlgClass ui;
	QStandardItemModel* _model;
};
