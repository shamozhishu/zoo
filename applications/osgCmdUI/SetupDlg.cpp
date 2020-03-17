#include "SetupDlg.h"
#include <QFileDialog>
#include <QStandardItemModel>
#include "osgCmdUI.h"

static QString s_datadir;
SetupDlg::SetupDlg(QWidget *parent /*= Q_NULLPTR*/)
	: QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(330, 200);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	connect(ui.pushButton_datadir, SIGNAL(clicked()), this, SLOT(onSetDataDir()));
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

	if (!s_datadir.isEmpty())
		ui.lineEdit_datadir->setText(s_datadir);

	QDir dir;
	dir.setPath(QCoreApplication::applicationDirPath());
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Size | QDir::Reversed);

	QStringList filelist;
	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i)
	{
		QString filename = list.at(i).fileName();
		if (filename.contains("osgCmd-", Qt::CaseInsensitive))
		{
#ifdef _DEBUG
			if (filename.contains("d.dll", Qt::CaseInsensitive))
#else
			if (filename.contains(".dll", Qt::CaseInsensitive))
#endif
			{
				filelist.push_back(filename);
			}
		}
	}

	int rowCount = filelist.size();
	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit(""));
	_model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("插件"));

	if (rowCount > 0)
	{
		_model->setRowCount(rowCount);

		for (int i = 0; i < rowCount; i++)
		{
			QStandardItem* pStandardItem = new QStandardItem();
			pStandardItem->setCheckable(true);
			pStandardItem->setCheckState(Qt::Unchecked);
			_model->setItem(i, pStandardItem);
			pStandardItem = new QStandardItem(filelist[i]);
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
		{
			QString pluginName = _model->item(i, 1)->text();
			pluginName.replace(QString("osgCmd-"), QString(""), Qt::CaseInsensitive);
#ifdef _DEBUG
			pluginName.replace(QString("d.dll"), QString(""), Qt::CaseInsensitive);
#else
			pluginName.replace(QString(".dll"), QString(""), Qt::CaseInsensitive);
#endif
			_cmdset.push_back(pluginName);
		}
	}
}

void SetupDlg::onSetDataDir()
{
	QString datadir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择资源路径"));
	if (!datadir.isEmpty())
	{
		s_datadir = datadir;
		ui.lineEdit_datadir->setText(datadir);
	}
}
