#ifndef __SETUP_DLG_H__
#define __SETUP_DLG_H__

#include <qdialog.h>
#include "ui_SetupDlg.h"

class QStandardItemModel;
class SetupDlg : public QDialog
{
	Q_OBJECT
public:
	SetupDlg(QString inputAdaName, QWidget *parent = Q_NULLPTR);
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

#endif // __SETUP_DLG_H__
