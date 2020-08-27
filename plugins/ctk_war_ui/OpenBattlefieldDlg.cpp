#include "OpenBattlefieldDlg.h"
#include <zoo/DatabaseCSV.h>
#include <QSettings>
#include "ui_OpenBattlefieldDlg.h"
#include "UIActivator.h"

// Qt5÷–Œƒ¬“¬Î
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

using namespace zoo;

QString OpenBattlefieldDlg::_curBattlefieldFile[FileCount_];
OpenBattlefieldDlg::OpenBattlefieldDlg()
	: _ui(new Ui::OpenBattlefieldDlg)
	, _curBattlefieldID(-1)
{
	_ui->setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	_ui->tableWidget->verticalHeader()->hide();
	_ui->tableWidget->setItemDelegate(new NoFocusDelegate());
	_ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	_ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("±‡∫≈")));
	_ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("√Ë ˆ")));

	QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("CTK_PLUGIN_FRAMEWORK_PROPS");
	TableCSV* pBattlefieldTable = DatabaseCSV::getSingleton().getTable(settings.value("ctk.war.table").toString().toLocal8Bit().data());
	settings.endGroup();

	std::set<string> allkeys = pBattlefieldTable->getMajorKeys();
	int len = allkeys.size();
	if (len > 0)
	{
		_ui->tableWidget->setRowCount(len);
		std::vector<string> sortedKeys(len);
		std::copy(allkeys.begin(), allkeys.end(), sortedKeys.begin());
		len = sortedKeys.size();
		for (int i = 0; i < len; ++i)
		{
			QTableWidgetItem* pItem = new QTableWidgetItem(QString::fromLocal8Bit(sortedKeys[i].c_str()));
			pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
			pItem->setTextAlignment(Qt::AlignCenter);
			_ui->tableWidget->setItem(i, 0, pItem);
			pItem = new QTableWidgetItem(QString::fromLocal8Bit(pBattlefieldTable->item2str(sortedKeys[i].c_str(), "description")));
			pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
			pItem->setTextAlignment(Qt::AlignCenter);
			_ui->tableWidget->setItem(i, 1, pItem);
		}
	}

	connect(_ui->okButton, &QPushButton::clicked, [this, pBattlefieldTable]
	{
		_curBattlefieldID = _ui->tableWidget->item(_ui->tableWidget->currentRow(), 0)->data(Qt::DisplayRole).toInt();
		_curBattlefieldFile[SceneFile_] = pBattlefieldTable->item2str(_curBattlefieldID, "scene_file");
		_curBattlefieldFile[ReadyScript_] = pBattlefieldTable->item2str(_curBattlefieldID, "ready_script");
		_curBattlefieldFile[RunningScript_] = pBattlefieldTable->item2str(_curBattlefieldID, "running_script");
		_curBattlefieldFile[PausedScript_] = pBattlefieldTable->item2str(_curBattlefieldID, "paused_script");
	});
}

OpenBattlefieldDlg::~OpenBattlefieldDlg()
{
	delete _ui;
}

int OpenBattlefieldDlg::getCurBattlefieldID() const
{
	return _curBattlefieldID;
}

QString OpenBattlefieldDlg::getCurBattlefieldFile(EBattlefieldFileType fileType)
{
	if (fileType >= FileCount_)
		return "";
	return _curBattlefieldFile[fileType];
}
