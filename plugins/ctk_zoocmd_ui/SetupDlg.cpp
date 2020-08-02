#include "SetupDlg.h"
#include <QSettings>
#include <QFileDialog>
#include <QDirIterator>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include "ZooCmdUI.h"

// Qt5ÖÐÎÄÂÒÂë
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

class NoFocusDelegate : public QStyledItemDelegate
{
public:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyleOptionViewItem itemOption(option);
		if (itemOption.state & QStyle::State_HasFocus)
			itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
		QStyledItemDelegate::paint(painter, itemOption, index);
	}
};

SetupDlg::SetupDlg(QString inputAdaName, QWidget *parent /*= Q_NULLPTR*/)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(onOk()));

	QStringList cmdlist;
	QStringList pluginLibFilter;
	QString cmdName, cmdName_r, cmdName_d;
	pluginLibFilter << inputAdaName + "-*.dll" << inputAdaName + "-*.so" << inputAdaName + "-*.dylib";
	QDirIterator dirIter(QCoreApplication::applicationDirPath(), pluginLibFilter, QDir::Files);
	while (dirIter.hasNext())
	{
		dirIter.next();
		cmdName = dirIter.fileName().mid(inputAdaName.size() + 1);
		cmdName.truncate(cmdName.lastIndexOf('.'));

		bool existed = false;
		bool needReplace = false;
		auto it = cmdlist.begin();
		auto itEnd = cmdlist.end();
		for (; it != itEnd; ++it)
		{
			if (it->length() > cmdName.length())
			{
				cmdName_d = *it;
				cmdName_r = cmdName;
#ifdef _DEBUG
				needReplace = false;
#else
				needReplace = true;
#endif
			}
			else
			{
				cmdName_d = cmdName;
				cmdName_r = *it;
#ifdef _DEBUG
				needReplace = true;
#else
				needReplace = false;
#endif
			}

			if (cmdName_d.compare(cmdName_r + "d", Qt::CaseInsensitive) == 0)
			{
				existed = true;
				if (needReplace)
					*it = cmdName;
				break;
			}
		}

		if (!existed)
			cmdlist.push_back(cmdName);
	}

#ifdef _DEBUG
	int len = cmdlist.size();
	for (int i = 0; i < len; ++i)
	{
		cmdlist[i] = cmdlist[i].mid(0, cmdlist[i].length() - 1);
	}
#endif

	int rowCount = cmdlist.size();
	_model = new QStandardItemModel(this);
	_model->setColumnCount(2);
	_model->setHeaderData(0, Qt::Horizontal, "#");
	_model->setHeaderData(1, Qt::Horizontal, "ÃüÁî");

	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("ZOO_CMDSET");
	QStringList cmdset = settings.value("activecmd").toStringList();
	settings.endGroup();

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
		ui.tableView_cmdset->setColumnWidth(1, 269);
		ui.tableView_cmdset->setItemDelegate(new NoFocusDelegate());
		ui.tableView_cmdset->setEditTriggers(QAbstractItemView::NoEditTriggers);
		ui.tableView_cmdset->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
		ui.tableView_cmdset->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	}

	setFixedSize(width(), height());
	connect(ui.tableView_cmdset, &QTableView::doubleClicked, [this](const QModelIndex &index)
	{
		if (index.column() != 0)
		{
			QStandardItem* pStandardItem = _model->item(index.row());
			Qt::CheckState checkSta = pStandardItem->checkState();
			checkSta = checkSta == Qt::Unchecked ? Qt::Checked : Qt::Unchecked;
			pStandardItem->setCheckState(checkSta);
		}
	});
}

QStringList SetupDlg::getCmdset() const
{
	return _cmdset;
}

void SetupDlg::onOk()
{
	_cmdset.clear();
	int rowCount = _model->rowCount();
	for (int i = 0; i < rowCount; ++i)
	{
		if (_model->item(i, 0)->checkState() == Qt::Checked)
			_cmdset.push_back(_model->item(i, 1)->text());
	}
}
