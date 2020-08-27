#include "PropertyWgts.h"
#include "UIActivator.h"
#include <zoo/Utils.h>
#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthUtils.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QDesktopServices>
#include <QStandardItemModel>
#include "ComPropertyBoard.h"
#include "ui_ComListWgt.h"
#include "ui_DoFPropertyWgt.h"
#include "ui_ModelPropertyWgt.h"
#include "ui_CameraPropertyWgt.h"
#include "ui_EarthPropertyWgt.h"
#include "ui_BehaviorPropertyWgt.h"
#include "ui_EnvirPropertyWgt.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

#define COM_CAST(TYPE) ((TYPE*)_com)
#define BEHAVIOR_COMPONENT_NAME "Behavior"
#define FLOAT_TO_STRING(VAR) QString::number((float)VAR, 'f', 6).replace(QRegExp("(\\.){0,1}0+$"), "").replace(QRegExp("[.]$"), "")
#define DOUBLE_TO_STRING(VAR) QString::number((double)VAR, 'f', 8).replace(QRegExp("(\\.){0,1}0+$"), "").replace(QRegExp("[.]$"), "")

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

	UIManagerService* pUIMgr = UIActivator::getService<UIManagerService>();
	connect(_ui->tableWidget, &QTableWidget::itemDoubleClicked, [this, pUIMgr](QTableWidgetItem *item)
	{
		if (item->flags().testFlag(Qt::ItemIsEditable))
			return;

		if (_operateEnt)
		{
			QString comName = _ui->tableWidget->item(item->row(), 0)->data(Qt::DisplayRole).toString();
			QString comImpl = _ui->tableWidget->item(item->row(), 1)->data(Qt::DisplayRole).toString();
			if (_addComponentBtn) // 添加组件
			{
				zoo::Component* pCom = _operateEnt->addComponent(comName.toStdString(), comImpl.toStdString());
				if (pCom)
				{
					_propBoard->showCom(comName, pCom);
					if (pCom->getImp())
					{
						pCom->getImp()->awake();
						pCom->getEntity()->notifyComponents();
						pUIMgr->starWindowTitle();
					}
					else if (comName != BEHAVIOR_COMPONENT_NAME)
					{
						_operateEnt->removeComponent(pCom);
						QMessageBox::warning(this, tr("警告"), comName + tr("组件实现不存在！"));
					}
					else
						pUIMgr->starWindowTitle();
				}
				else
				{
					QMessageBox::warning(this, tr("警告"), comName + tr("组件不存在！"));
				}
			}
			else // 移除组件
			{
				_operateEnt->removeComponent(comName.toStdString());
				_propBoard->hideCom(comName);
				pUIMgr->starWindowTitle();
			}

			hide();
		}
	});
}

ComListWgt::~ComListWgt()
{
	delete _ui;
}

void ComListWgt::refreshComList(zoo::Entity* ent, bool bAddComponentBtn, const QStringList& comlist)
{
	if (!ent)
		return;

	_operateEnt = ent;
	_addComponentBtn = bAddComponentBtn;
	QStringList strlist, addlist, removelist;

	for (int i = 0; i < comlist.size(); ++i)
	{
		if (ent->getComponent(comlist[i].toLocal8Bit().data()))
			removelist << comlist[i];
		else
			addlist << comlist[i];
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
		if (strlist[i] == BEHAVIOR_COMPONENT_NAME)
		{
			pItem = new QTableWidgetItem();
			pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
		}
		else
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
	_uiMgr = UIActivator::getService<UIManagerService>();
}

QSize PropertyWgt::sizeHint() const
{
	return QSize(300, -1); // 在这里定义dock的初始大小
}

void PropertyWgt::resetCom(zoo::Component* pCom)
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

	connect(_ui->lineEdit_posx, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		double tmp = _ui->lineEdit_posx->text().toDouble();
		if (!zoo::equals(COM_CAST(DoF)->_x, tmp))
		{
			COM_CAST(DoF)->_x = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_posy, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		double tmp = _ui->lineEdit_posy->text().toDouble();
		if (!zoo::equals(COM_CAST(DoF)->_y, tmp))
		{
			COM_CAST(DoF)->_y = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_posz, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		double tmp = _ui->lineEdit_posz->text().toDouble();
		if (!zoo::equals(COM_CAST(DoF)->_z, tmp))
		{
			COM_CAST(DoF)->_z = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_rotx, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_rotx->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_pitch, tmp))
		{
			COM_CAST(DoF)->_pitch = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_roty, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_roty->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_roll, tmp))
		{
			COM_CAST(DoF)->_roll = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_rotz, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_rotz->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_heading, tmp))
		{
			COM_CAST(DoF)->_heading = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_scalex, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_scalex->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_sx, tmp))
		{
			COM_CAST(DoF)->_sx = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_scaley, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_scaley->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_sy, tmp))
		{
			COM_CAST(DoF)->_sy = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->lineEdit_scalez, static_cast<void(QLineEdit::*)(const QString&)>(&QLineEdit::textChanged), [this]
	{
		float tmp = _ui->lineEdit_scalez->text().toFloat();
		if (!zoo::equals(COM_CAST(DoF)->_sz, tmp))
		{
			COM_CAST(DoF)->_sz = tmp;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->doubleSpinBox_lon, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val)
	{
		if (!zoo::equals(COM_CAST(DoF)->_x, val))
		{
			COM_CAST(DoF)->_x = val;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->doubleSpinBox_lat, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val)
	{
		if (!zoo::equals(COM_CAST(DoF)->_y, val))
		{
			COM_CAST(DoF)->_y = val;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->doubleSpinBox_height, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this](double val)
	{
		if (!zoo::equals(COM_CAST(DoF)->_z, val))
		{
			COM_CAST(DoF)->_z = val;
			_com->dirtyBit().addState(DoF::Dof_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->radioButton, static_cast<void(QRadioButton::*)(bool)>(&QRadioButton::toggled), [this](bool checked)
	{
		DoF* pDoF = COM_CAST(DoF);
		if (pDoF->_lonLatHeight == checked)
			return;

		_uiMgr->starWindowTitle();
		pDoF->_lonLatHeight = checked;
		_ui->stackedWidget->setCurrentIndex(checked ? 1 : 0);
		if (checked)
		{
			double lon, lat, height;
			if (zoo::ServiceLocator<OsgEarthUtils>::getService()->convertXYZToLatLongHeight(pDoF->_x, pDoF->_y, pDoF->_z, lat, lon, height))
			{
				_ui->doubleSpinBox_lon->setValue(lon);
				_ui->doubleSpinBox_lat->setValue(lat);
				_ui->doubleSpinBox_height->setValue(height);
				pDoF->_x = lon;
				pDoF->_y = lat;
				pDoF->_z = height;
				_com->dirtyBit().addState(DoF::Dof_);
				_com->dirtyBit().addState(DoF::Parent_);
			}
		}
		else
		{
			double X, Y, Z;
			if (zoo::ServiceLocator<OsgEarthUtils>::getService()->convertLatLongHeightToXYZ(pDoF->_y, pDoF->_x, pDoF->_z, X, Y, Z))
			{
				_ui->lineEdit_posx->setText(DOUBLE_TO_STRING(X));
				_ui->lineEdit_posy->setText(DOUBLE_TO_STRING(Y));
				_ui->lineEdit_posz->setText(DOUBLE_TO_STRING(Z));
				pDoF->_x = X;
				pDoF->_y = Y;
				pDoF->_z = Z;
				_com->dirtyBit().addState(DoF::Dof_);
				_com->dirtyBit().addState(DoF::Parent_);
			}
		}
	});
}

DoFPropertyWgt::~DoFPropertyWgt()
{
	delete _ui;
}

void DoFPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	DoF* pDoF = COM_CAST(DoF);
	_ui->lineEdit_rotx->setText(FLOAT_TO_STRING(pDoF->_pitch));
	_ui->lineEdit_roty->setText(FLOAT_TO_STRING(pDoF->_roll));
	_ui->lineEdit_rotz->setText(FLOAT_TO_STRING(pDoF->_heading));
	_ui->lineEdit_scalex->setText(FLOAT_TO_STRING(pDoF->_sx));
	_ui->lineEdit_scaley->setText(FLOAT_TO_STRING(pDoF->_sy));
	_ui->lineEdit_scalez->setText(FLOAT_TO_STRING(pDoF->_sz));

	do 
	{
		DoF* pParent = pCom->getEntity()->getComponent<DoF>()->_parent;
		if (!pCom->getEntity()->getSpawner()->getComponent<Earth>() ||
			(pParent && !pParent->getEntity()->isSpawner()))
		{
			_ui->radioButton->hide();
			_ui->radioButton->setChecked(Qt::Unchecked);
		}
		else
		{
			_ui->radioButton->show();
			_ui->radioButton->setChecked(pDoF->_lonLatHeight ? Qt::Checked : Qt::Unchecked);
			if (pDoF->_lonLatHeight)
			{
				_ui->stackedWidget->setCurrentIndex(1);
				_ui->doubleSpinBox_lon->setValue(pDoF->_x);
				_ui->doubleSpinBox_lat->setValue(pDoF->_y);
				_ui->doubleSpinBox_height->setValue(pDoF->_z);
				break;
			}
		}

		_ui->stackedWidget->setCurrentIndex(0);
		_ui->lineEdit_posx->setText(DOUBLE_TO_STRING(pDoF->_x));
		_ui->lineEdit_posy->setText(DOUBLE_TO_STRING(pDoF->_y));
		_ui->lineEdit_posz->setText(DOUBLE_TO_STRING(pDoF->_z));

	} while (0);
}

ModelPropertyWgt::ModelPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::ModelPropertyWgt)
{
	_ui->setupUi(this);
	connect(_ui->checkBox, &QCheckBox::stateChanged, [this](int boxState)
	{
		bool isShow = (boxState == Qt::Checked ? true : false);
		if (COM_CAST(Model)->_visible != isShow)
		{
			COM_CAST(Model)->_visible = isShow;
			_com->dirtyBit().addState(Model::Visible_);
			_uiMgr->starWindowTitle();
		}
	});

	_ui->toolButton->setDefaultAction(_ui->modelFile);
	connect(_ui->modelFile, &QAction::triggered, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("模型文件打开"), ZOO_DATA_ROOT_DIR.c_str(), tr("models(*.osg *.ive *.flt)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			COM_CAST(Model)->_modelFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			_com->getImp()->awake();
			_com->getEntity()->notifyComponents();
			_uiMgr->starWindowTitle();
		}
	});
}

ModelPropertyWgt::~ModelPropertyWgt()
{
	delete _ui;
}

void ModelPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Model* pModel = COM_CAST(Model);
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

	QStringList manipulatorType;
	manipulatorType << tr("地球") << tr("节点跟踪器") << tr("轨迹球") << tr("飞行") << tr("驾驶")
		<< tr("地形") << tr("轨道") << tr("第一人称视角") << tr("球面") << tr("定制");
	_ui->comboBox->addItems(manipulatorType);

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
			float tmp = leftPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_lRatio, tmp))
			{
				_ui->lineEdit_left->setStyleSheet(leftPercentSS);
				COM_CAST(Camera)->_lRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
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
			float tmp = rightPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_rRatio, tmp))
			{
				_ui->lineEdit_right->setStyleSheet(rightPercentSS);
				COM_CAST(Camera)->_rRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
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
			float tmp = bottomPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_bRatio, tmp))
			{
				_ui->lineEdit_bottom->setStyleSheet(bottomPercentSS);
				COM_CAST(Camera)->_bRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
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
			float tmp = topPercent / 100.0f;
			if (!zoo::equals(COM_CAST(Camera)->_tRatio, tmp))
			{
				_ui->lineEdit_top->setStyleSheet(topPercentSS);
				COM_CAST(Camera)->_tRatio = tmp;
				_com->dirtyBit().addState(Camera::Viewport_);
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->pushButton_color, &QPushButton::clicked, [this, bgColorSS]()
	{
		Camera* pCam = COM_CAST(Camera);
		QColor initclr = QColor(pCam->_red, pCam->_green, pCam->_blue, pCam->_alpha);
		QColorDialog dlg(initclr, this);
		if (dlg.exec() == QDialog::Accepted)
		{
			QColor clr = dlg.currentColor();
			if (clr != initclr)
			{
				pCam->_red = clr.red();
				pCam->_green = clr.green();
				pCam->_blue = clr.blue();
				pCam->_alpha = clr.alpha();
				_com->dirtyBit().addState(Camera::Bgcolour_);
				_ui->pushButton_color->setStyleSheet(QString(tr("background-color:rgba(%1,%2,%3,%4)").arg(
					QString::number(pCam->_red), QString::number(pCam->_green),
					QString::number(pCam->_blue), QString::number(pCam->_alpha))));
				_uiMgr->starWindowTitle();
			}
		}
	});
	connect(_ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
	{
		if (COM_CAST(Camera)->_manipulatorKey != index)
		{
			COM_CAST(Camera)->_manipulatorKey = index;
			_com->dirtyBit().addState(Camera::Manipulator_);
			_uiMgr->starWindowTitle();
		}
	});
}

CameraPropertyWgt::~CameraPropertyWgt()
{
	delete _ui;
}

void CameraPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Camera* pCam = COM_CAST(Camera);
	_ui->comboBox->setCurrentIndex(pCam->_manipulatorKey);
	_ui->lineEdit_left->setText(FLOAT_TO_STRING(pCam->_lRatio * 100.0f));
	_ui->lineEdit_right->setText(FLOAT_TO_STRING(pCam->_rRatio * 100.0f));
	_ui->lineEdit_bottom->setText(FLOAT_TO_STRING(pCam->_bRatio * 100.0f));
	_ui->lineEdit_top->setText(FLOAT_TO_STRING(pCam->_tRatio * 100.0f));
	_ui->pushButton_color->setStyleSheet(QString(tr("background-color:rgba(%1,%2,%3,%4)").arg(
		QString::number(pCam->_red), QString::number(pCam->_green),
		QString::number(pCam->_blue), QString::number(pCam->_alpha))));
}

EarthPropertyWgt::EarthPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::EarthPropertyWgt)
{
	_ui->setupUi(this);
	connect(_ui->toolButton_open, &QToolButton::clicked, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Earth文件打开"), ZOO_DATA_ROOT_DIR.c_str(), tr("地球(*.earth)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			COM_CAST(Earth)->_earthFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			_com->getImp()->awake();
			_com->getEntity()->notifyComponents();
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->toolButton_edit, &QToolButton::clicked, [this]
	{
		QString filename = _ui->lineEdit->text();
		if (!filename.isEmpty())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
	});
	connect(_ui->toolButton_reset, &QToolButton::clicked, [this]
	{
		_com->getImp()->awake();
		_com->getEntity()->notifyComponents();
	});
	connect(_ui->checkBox_sun, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_sun->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::sun_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::sun_] = isVisibility;
			_com->dirtyBit().addState(Earth::SunVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_moon, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_moon->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::moon_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::moon_] = isVisibility;
			_com->dirtyBit().addState(Earth::MoonVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_star, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_star->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::star_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::star_] = isVisibility;
			_com->dirtyBit().addState(Earth::StarVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_nebula, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_nebula->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::nebula_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::nebula_] = isVisibility;
			_com->dirtyBit().addState(Earth::NebulaVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->checkBox_atmosphere, &QCheckBox::stateChanged, [this]
	{
		bool isVisibility = _ui->checkBox_atmosphere->checkState() == Qt::Unchecked ? false : true;
		if (COM_CAST(Earth)->_skyVisibility[Earth::atmosphere_] != isVisibility)
		{
			COM_CAST(Earth)->_skyVisibility[Earth::atmosphere_] = isVisibility;
			_com->dirtyBit().addState(Earth::AtmosphereVisible_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->horizontalSlider, &QSlider::valueChanged, [this](int val)
	{
		float intensity = (float)val / _ui->horizontalSlider->maximum();
		if (!zoo::equals(COM_CAST(Earth)->_sunlightIntensity, intensity))
		{
			COM_CAST(Earth)->_sunlightIntensity = intensity;
			_com->dirtyBit().addState(Earth::SunlightIntensity_);
			_uiMgr->starWindowTitle();
		}
	});
}

EarthPropertyWgt::~EarthPropertyWgt()
{
	delete _ui;
}

void EarthPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Earth* pEarth = COM_CAST(Earth);
	if (pEarth->_earthFile == "")
		_ui->lineEdit->setText("");
	else
		_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + pEarth->_earthFile).c_str()));

	_ui->checkBox_sun->setCheckState(pEarth->_skyVisibility[Earth::sun_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_moon->setCheckState(pEarth->_skyVisibility[Earth::moon_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_star->setCheckState(pEarth->_skyVisibility[Earth::star_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_nebula->setCheckState(pEarth->_skyVisibility[Earth::nebula_] ? Qt::Checked : Qt::Unchecked);
	_ui->checkBox_atmosphere->setCheckState(pEarth->_skyVisibility[Earth::atmosphere_] ? Qt::Checked : Qt::Unchecked);

	int pos = pEarth->_sunlightIntensity * _ui->horizontalSlider->maximum();
	if (pos < _ui->horizontalSlider->minimum())
		pos = _ui->horizontalSlider->minimum();
	if (pos > _ui->horizontalSlider->maximum())
		pos = _ui->horizontalSlider->maximum();
	_ui->horizontalSlider->setSliderPosition(pos);
}

BehaviorPropertyWgt::BehaviorPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::BehaviorPropertyWgt)
{
	_ui->setupUi(this);
	connect(_ui->toolButton_open, &QToolButton::clicked, [this]
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("脚本文件打开"), ZOO_DATA_ROOT_DIR.c_str(), tr("脚本(*.lua)"));
		if (!fileName.isEmpty())
		{
			_ui->lineEdit->setText(fileName);
			COM_CAST(Behavior)->_scriptFile = fileName.replace(QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()), tr("")).toLocal8Bit();
			COM_CAST(Behavior)->_scriptInited = false;
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->toolButton_edit, &QToolButton::clicked, [this]
	{
		QString filename = _ui->lineEdit->text();
		if (!filename.isEmpty())
			QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
	});
	connect(_ui->toolButton_reset, &QToolButton::clicked, [this]
	{
		COM_CAST(Behavior)->_scriptInited = false;
	});
}

BehaviorPropertyWgt::~BehaviorPropertyWgt()
{
	delete _ui;
}

void BehaviorPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	Behavior* pBehavior = COM_CAST(Behavior);
	if (pBehavior->_scriptFile == "")
		_ui->lineEdit->setText("");
	else
		_ui->lineEdit->setText(QString::fromLocal8Bit((ZOO_DATA_ROOT_DIR + pBehavior->_scriptFile).c_str()));
}

EnvirPropertyWgt::EnvirPropertyWgt(QWidget* parent)
	: PropertyWgt(parent)
	, _ui(new Ui::EnvirPropertyWgt)
{
	_ui->setupUi(this);
	QStringList weatherType;
	weatherType << tr("晴") << tr("雨") << tr("雪");
	_ui->comboBox->addItems(weatherType);
	connect(_ui->comboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
	{
		if (COM_CAST(Environment)->_type != index)
		{
			COM_CAST(Environment)->_type = (Environment::EWeather)index;
			_com->dirtyBit().addState(Environment::Weather_);
			_uiMgr->starWindowTitle();
		}
	});
	connect(_ui->horizontalSlider, &QSlider::valueChanged, [this](int val)
	{
		float intensity = (float)val / _ui->horizontalSlider->maximum();
		if (!zoo::equals(COM_CAST(Environment)->_intensity, intensity))
		{
			COM_CAST(Environment)->_intensity = intensity;
			_com->dirtyBit().addState(Environment::Weather_);
			_uiMgr->starWindowTitle();
		}
	});
}

EnvirPropertyWgt::~EnvirPropertyWgt()
{
	delete _ui;
}

void EnvirPropertyWgt::resetCom(zoo::Component* pCom)
{
	PropertyWgt::resetCom(pCom);
	_ui->comboBox->setCurrentIndex(COM_CAST(Environment)->_type);
	int pos = COM_CAST(Environment)->_intensity * _ui->horizontalSlider->maximum();
	if (pos < _ui->horizontalSlider->minimum())
		pos = _ui->horizontalSlider->minimum();
	if (pos > _ui->horizontalSlider->maximum())
		pos = _ui->horizontalSlider->maximum();
	_ui->horizontalSlider->setSliderPosition(pos);
}
