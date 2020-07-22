#include "PropertyWgts.h"
#include "UIActivator.h"
#include <zoo/Utils.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QStandardItemModel>
#include "ComPropertyBoard.h"
#include "ui_ComListWgt.h"
#include "ui_DoFPropertyWgt.h"
#include "ui_ModelPropertyWgt.h"
#include "ui_CameraPropertyWgt.h"

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

extern const char* g_comTypeName[];

ComListWgt::ComListWgt(ComPropertyBoard* propBoard)
	: _ui(new Ui::ComListWgt)
	, _propBoard(propBoard)
	, _operateEnt(nullptr)
	, _addComponentBtn(false)
{
	QWidget::installEventFilter(this);
	setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
	setFixedSize(300, 300);
	setVisible(false);
	_ui->setupUi(this);

	_ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	_ui->tableWidget->verticalHeader()->setDefaultSectionSize(20);
	_ui->tableWidget->verticalHeader()->setVisible(false);
	_ui->tableWidget->setShowGrid(true);
	_ui->tableWidget->setItemDelegate(new NoFocusDelegate());
	_ui->tableWidget->setColumnCount(2);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	_ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	_ui->tableWidget->setColumnWidth(0, 138);
	_ui->tableWidget->setColumnWidth(1, 138);
	_ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("组件")));
	_ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("实现")));

	connect(_ui->tableWidget, &QTableWidget::itemDoubleClicked, [this](QTableWidgetItem *item)
	{
		if (item->column() != 0)
			return;

		if (_operateEnt)
		{
			QString comName = item->data(Qt::DisplayRole).toString();
			QString comImpl = _ui->tableWidget->item(item->row(), 1)->data(Qt::DisplayRole).toString();
			if (_addComponentBtn)
			{
				zoo::Component* pCom = _operateEnt->addComponent(comName.toStdString(), comImpl.toStdString());
				if (pCom)
					_propBoard->showCom(comName, pCom);
			}
			else
			{
				_operateEnt->removeComponent(comName.toStdString());
				_propBoard->hideCom(comName);
			}

			hide();
		}
	});
}

ComListWgt::~ComListWgt()
{
	delete _ui;
}

void ComListWgt::refreshComList(zoo::Entity* ent, bool bAddComponentBtn)
{
	if (!ent)
		return;

	_operateEnt = ent;
	_addComponentBtn = bAddComponentBtn;
	QStringList strlist, addlist, removelist;

	int i = 1;
	while (g_comTypeName[i] != nullptr)
	{
		if (ent->getComponent(g_comTypeName[i]))
			removelist << g_comTypeName[i];
		else
			addlist << g_comTypeName[i];
		++i;
	}

	if (bAddComponentBtn)
	{
		strlist = addlist;
		setWindowTitle(tr("添加组件"));
	}
	else
	{
		strlist = removelist;
		setWindowTitle(tr("移除组件"));
	}

	int rowCount = strlist.size();
	_ui->tableWidget->setRowCount(rowCount);

	for (int i = 0; i < rowCount; i++)
	{
		QTableWidgetItem* pItem = new QTableWidgetItem(strlist[i]);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		_ui->tableWidget->setItem(i, 0, pItem);
		pItem = new QTableWidgetItem(strlist[i] + "Impl");
		_ui->tableWidget->setItem(i, 1, pItem);
	}
}

bool ComListWgt::eventFilter(QObject *o, QEvent *e)
{
	if (QEvent::WindowDeactivate == e->type())
		hide();
	return false;
}

PropertyWgt::PropertyWgt(QWidget *parent)
	: QWidget(parent)
	, _com(nullptr)
{
}

void PropertyWgt::setCom(zoo::Component* pCom)
{
	_com = pCom;
}

DoFPropertyWgt::DoFPropertyWgt(QWidget *parent)
	: PropertyWgt(parent)
	, _ui(new Ui::DoFPropertyWgt)
{
	_ui->setupUi(this);

	QDoubleValidator* pDoubleValidator = new QDoubleValidator;
	_ui->lineEdit_posx->setValidator(pDoubleValidator);
	_ui->lineEdit_posy->setValidator(pDoubleValidator);
	_ui->lineEdit_posz->setValidator(pDoubleValidator);
	_ui->lineEdit_rotx->setValidator(pDoubleValidator);
	_ui->lineEdit_roty->setValidator(pDoubleValidator);
	_ui->lineEdit_rotz->setValidator(pDoubleValidator);
	_ui->lineEdit_scalex->setValidator(pDoubleValidator);
	_ui->lineEdit_scaley->setValidator(pDoubleValidator);
	_ui->lineEdit_scalez->setValidator(pDoubleValidator);

	connect(_ui->lineEdit_posx, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_x = _ui->lineEdit_posx->text().toDouble();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_posy, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_y = _ui->lineEdit_posy->text().toDouble();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_posz, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_z = _ui->lineEdit_posz->text().toDouble();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_rotx, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_pitch = _ui->lineEdit_rotx->text().toFloat();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_roty, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_roll = _ui->lineEdit_roty->text().toFloat();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_rotz, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_heading = _ui->lineEdit_rotz->text().toFloat();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_scalex, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_sx = _ui->lineEdit_scalex->text().toFloat();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_scaley, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_sy = _ui->lineEdit_scaley->text().toFloat();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
	connect(_ui->lineEdit_scalez, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]()
	{
		((DoF*)_com)->_sz = _ui->lineEdit_scalez->text().toFloat();
		const_cast<BitState&>(_com->dirtyBit()).addState(DoF::dof_);
	});
}

DoFPropertyWgt::~DoFPropertyWgt()
{
	delete _ui;
}

void DoFPropertyWgt::setCom(zoo::Component* pCom)
{
	PropertyWgt::setCom(pCom);
	DoF* pDoF = (DoF*)pCom;
	_ui->lineEdit_posx->setText(QString::number(pDoF->_x));
	_ui->lineEdit_posy->setText(QString::number(pDoF->_y));
	_ui->lineEdit_posz->setText(QString::number(pDoF->_z));
	_ui->lineEdit_rotx->setText(QString::number(pDoF->_pitch));
	_ui->lineEdit_roty->setText(QString::number(pDoF->_roll));
	_ui->lineEdit_rotz->setText(QString::number(pDoF->_heading));
	_ui->lineEdit_scalex->setText(QString::number(pDoF->_sx));
	_ui->lineEdit_scaley->setText(QString::number(pDoF->_sy));
	_ui->lineEdit_scalez->setText(QString::number(pDoF->_sz));
}

ModelPropertyWgt::ModelPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::ModelPropertyWgt)
{
	_ui->setupUi(this);
	connect(_ui->checkBox, &QCheckBox::stateChanged, [this](int boxState)
	{
		((Model*)_com)->_visible = (boxState == Qt::Checked ? true : false);
		const_cast<BitState&>(_com->dirtyBit()).addState(Model::visible_);
	});

	_ui->toolButton->setDefaultAction(_ui->modelPath);
	connect(_ui->modelPath, &QAction::triggered, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("模型文件打开"), ZOO_DATA_ROOT_DIR.c_str(), tr("models(*.osg *ive *flt)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			((Model*)_com)->_modelFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			_com->getImp()->awake();
			_com->getEntity()->notifyComponents();
		}
	});
}

ModelPropertyWgt::~ModelPropertyWgt()
{
	delete _ui;
}

void ModelPropertyWgt::setCom(zoo::Component* pCom)
{
	PropertyWgt::setCom(pCom);
	Model* pModel = (Model*)pCom;
	_ui->checkBox->setCheckState(pModel->_visible ? Qt::Checked : Qt::Unchecked);
	if (pModel->_modelFile == "")
		_ui->lineEdit->setText("");
	else
		_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + pModel->_modelFile).c_str()));
}

CameraPropertyWgt::CameraPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::CameraPropertyWgt)
{
	_ui->setupUi(this);
	
	QIntValidator*pValidator = new QIntValidator;
	pValidator->setRange(0, 100);
	_ui->lineEdit_left->setValidator(pValidator);
	_ui->lineEdit_right->setValidator(pValidator);
	_ui->lineEdit_bottom->setValidator(pValidator);
	_ui->lineEdit_top->setValidator(pValidator);

	QString leftPercentSS = _ui->lineEdit_left->styleSheet();
	QString rightPercentSS = _ui->lineEdit_right->styleSheet();
	QString bottomPercentSS = _ui->lineEdit_bottom->styleSheet();
	QString topPercentSS = _ui->lineEdit_top->styleSheet();
	QString bgColorSS = _ui->pushButton_color->styleSheet();

	connect(_ui->lineEdit_left, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, leftPercentSS]()
	{
		int leftPercent = _ui->lineEdit_left->text().toInt();
		int rightPercent = _ui->lineEdit_right->text().toInt();

		if (leftPercent >= rightPercent)
		{
			_ui->lineEdit_left->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			_ui->lineEdit_left->setStyleSheet(leftPercentSS);
			((Camera*)_com)->_lRatio = leftPercent / 100.0f;
			const_cast<BitState&>(_com->dirtyBit()).addState(Camera::viewport_);
		}
	});
	connect(_ui->lineEdit_right, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, rightPercentSS]()
	{
		int leftPercent = _ui->lineEdit_left->text().toInt();
		int rightPercent = _ui->lineEdit_right->text().toInt();

		if (leftPercent >= rightPercent)
		{
			_ui->lineEdit_right->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			_ui->lineEdit_right->setStyleSheet(rightPercentSS);
			((Camera*)_com)->_rRatio = rightPercent / 100.0f;
			const_cast<BitState&>(_com->dirtyBit()).addState(Camera::viewport_);
		}
	});
	connect(_ui->lineEdit_bottom, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, bottomPercentSS]()
	{
		int bottomPercent = _ui->lineEdit_bottom->text().toInt();
		int topPercent = _ui->lineEdit_top->text().toInt();

		if (bottomPercent >= topPercent)
		{
			_ui->lineEdit_bottom->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			_ui->lineEdit_bottom->setStyleSheet(bottomPercentSS);
			((Camera*)_com)->_bRatio = bottomPercent / 100.0f;
			const_cast<BitState&>(_com->dirtyBit()).addState(Camera::viewport_);
		}
	});
	connect(_ui->lineEdit_top, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this, topPercentSS]()
	{
		int bottomPercent = _ui->lineEdit_bottom->text().toInt();
		int topPercent = _ui->lineEdit_top->text().toInt();

		if (bottomPercent >= topPercent)
		{
			_ui->lineEdit_top->setStyleSheet("background-color:rgba(255,0,0,255)");
		}
		else
		{
			_ui->lineEdit_top->setStyleSheet(topPercentSS);
			((Camera*)_com)->_tRatio = topPercent / 100.0f;
			const_cast<BitState&>(_com->dirtyBit()).addState(Camera::viewport_);
		}
	});
	connect(_ui->pushButton_color, &QPushButton::clicked, [this, bgColorSS]()
	{
		Camera* pCam = (Camera*)_com;
		QColor initclr = QColor(pCam->_red, pCam->_green, pCam->_blue, pCam->_alpha);
		QColorDialog dlg(initclr);
		if (dlg.exec() == QDialog::Accepted)
		{
			QColor clr = dlg.currentColor();
			if (clr != initclr)
			{
				pCam->_red = clr.red();
				pCam->_green = clr.green();
				pCam->_blue = clr.blue();
				pCam->_alpha = clr.alpha();
				const_cast<BitState&>(_com->dirtyBit()).addState(Camera::bgcolour_);
				_ui->pushButton_color->setStyleSheet(QString(tr("background-color:rgba(%1,%2,%3,%4)").arg(
					QString::number(pCam->_red), QString::number(pCam->_green),
					QString::number(pCam->_blue), QString::number(pCam->_alpha))));
			}
		}
	});
}

CameraPropertyWgt::~CameraPropertyWgt()
{
	delete _ui;
}

void CameraPropertyWgt::setCom(zoo::Component* pCom)
{
	PropertyWgt::setCom(pCom);
	Camera* pCam = (Camera*)pCom;
	_ui->lineEdit_left->setText(QString::number(pCam->_lRatio * 100.0f));
	_ui->lineEdit_right->setText(QString::number(pCam->_rRatio * 100.0f));
	_ui->lineEdit_bottom->setText(QString::number(pCam->_bRatio * 100.0f));
	_ui->lineEdit_top->setText(QString::number(pCam->_tRatio * 100.0f));
	_ui->pushButton_color->setStyleSheet(QString(tr("background-color:rgba(%1,%2,%3,%4)").arg(
		QString::number(pCam->_red), QString::number(pCam->_green),
		QString::number(pCam->_blue), QString::number(pCam->_alpha))));
}
