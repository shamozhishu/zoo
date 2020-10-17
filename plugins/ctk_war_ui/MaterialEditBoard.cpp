#include "MaterialEditBoard.h"
#include <QFileDialog>
#include <QDesktopServices>
#include "ui_MaterialEditBoard.h"
#include "MaterialDisplayWgt.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

MaterialEditBoard::MaterialEditBoard()
	: _ui(new Ui::MaterialEditBoard)
	, _matDisplayWgt(new MaterialDisplayWgt)
{
	_ui->setupUi(this);
	_ui->verticalLayout_mat->addWidget(_matDisplayWgt);
	connect(_ui->toolButton_open, &QToolButton::clicked, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("材质脚本打开"), ZOO_DATA_ROOT_DIR.c_str(), tr("材质脚本(*.material)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
		}
	});
	connect(_ui->toolButton_edit, &QToolButton::clicked, [this]
	{
		QString filename = _ui->lineEdit->text();
		if (!filename.isEmpty())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
	});
	connect(_ui->toolButton_compile, &QToolButton::clicked, [this]
	{
		
	});
}

MaterialEditBoard::~MaterialEditBoard()
{
	delete _ui;
}
