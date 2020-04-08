#include "SetupDlg.h"
#include <QSettings>
#include <QFileDialog>
#include <QDirIterator>
#include <QStandardItemModel>
#include "ZooCmdUI.h"

SetupDlg::SetupDlg(QString inputAdaName, QWidget *parent /*= Q_NULLPTR*/)
	: QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(330, 200);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	connect(ui.pushButton_datadir, SIGNAL(clicked()), this, SLOT(onSetDataDir()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

	QStringList cmdlist;
	QStringList pluginLibFilter;
	pluginLibFilter << "*.dll" << "*.so" << "*.dylib";
	QDirIterator dirIter(QCoreApplication::applicationDirPath() + "/" + inputAdaName, pluginLibFilter, QDir::Files);
	while (dirIter.hasNext())
	{
		dirIter.next();
		QString cmdName = dirIter.fileName().mid(inputAdaName.size() + 1);
		cmdName.truncate(cmdName.lastIndexOf('.'));
#ifdef _DEBUG
		cmdName.truncate(cmdName.lastIndexOf('d'));
#endif
		cmdlist.push_back(cmdName);
	}

	int rowCount = cmdlist.size();
	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit(""));
	_model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("命令"));

	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("ZOO_CMDSET");

	QString datadir = settings.value("datadir").toString();
	if (!datadir.isEmpty())
		ui.lineEdit_datadir->setText(datadir);

	QStringList cmdset = settings.value("activecmd").toStringList();

	if (rowCount > 0)
	{
		_model->setRowCount(rowCount);

		for (int i = 0; i < rowCount; i++)
		{
			QStandardItem* pStandardItem = new QStandardItem();
			pStandardItem->setCheckable(true);
			pStandardItem->setCheckState(Qt::Unchecked);
			_model->setItem(i, pStandardItem);
			if (cmdset.contains(cmdlist[i], Qt::CaseInsensitive))
				pStandardItem->setCheckState(Qt::Checked);

			pStandardItem = new QStandardItem(cmdlist[i]);
			pStandardItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			_model->setItem(i, 1, pStandardItem);
		}

		ui.tableView_cmdset->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tableView_cmdset->verticalHeader()->setDefaultSectionSize(20);
		ui.tableView_cmdset->verticalHeader()->setVisible(false);
		ui.tableView_cmdset->setShowGrid(true);
		ui.tableView_cmdset->setModel(_model);
		ui.tableView_cmdset->setColumnWidth(0, 20);
		ui.tableView_cmdset->setColumnWidth(1, 289);
		ui.tableView_cmdset->setFocusPolicy(Qt::NoFocus);
		ui.tableView_cmdset->setEditTriggers(QAbstractItemView::NoEditTriggers);
		ui.tableView_cmdset->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	}

	settings.endGroup();
}

QString SetupDlg::getDataDir() const
{
	return _datadir;
}

QStringList SetupDlg::getCmdset() const
{
	return _cmdset;
}

void SetupDlg::onOk()
{
	_cmdset.clear();
	_datadir = ui.lineEdit_datadir->text();
	int rowCount = _model->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		if (_model->item(i, 0)->checkState() == Qt::Checked)
			_cmdset.push_back(_model->item(i, 1)->text());
	}
}

void SetupDlg::onSetDataDir()
{
	QString datadir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择资源路径"));
	if (!datadir.isEmpty())
		ui.lineEdit_datadir->setText(datadir);
}
