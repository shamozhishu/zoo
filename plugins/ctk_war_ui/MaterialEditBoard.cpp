#include "MaterialEditBoard.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <UniversalGlobalServices.h>
#include <component/war/WarComponents.h>
#include "PropertyWgts.h"
#include "MaterialDisplayWgt.h"
#include "ui_MaterialEditBoard.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

MaterialEditBoard::MaterialEditBoard()
	: _ui(new Ui::MaterialEditBoard)
	, _matDisplayWgt(new MaterialDisplayWgt)
{
	_ui->setupUi(this);
	_matWgt = new MaterialWgt(this, nullptr);
	_matWgt->resetMat(_matDisplayWgt->getDisplayedMat());
	_ui->verticalLayout->insertWidget(_ui->verticalLayout->count() - 1, _matWgt);
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
		QString str = _ui->lineEdit->text();
		if (!str.isEmpty())
		{
			_matDisplayWgt->generateMaterialSphere(str.toLocal8Bit().data());
			_matWgt->resetMat(_matDisplayWgt->getDisplayedMat());
		}
	});
	connect(_matWgt, &MaterialWgt::materialChanged, [this]
	{
		_matDisplayWgt->refreshMaterialSphere(_ui->toolButton_light->isChecked());
	});
	connect(_ui->toolButton_light, &QToolButton::clicked, [this](bool checked)
	{
		_matDisplayWgt->refreshMaterialSphere(checked);
		if (checked)
			_ui->toolButton_light->setToolTip(tr("关灯"));
		else
			_ui->toolButton_light->setToolTip(tr("开灯"));
	});
}

MaterialEditBoard::~MaterialEditBoard()
{
	delete _ui;
}
