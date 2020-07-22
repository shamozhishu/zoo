#include "ArmyListWgt.h"
#include "UIActivator.h"
#include "ui_ArmyListWgt.h"
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <ctk_service/war/WarService.h>
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include "ComPropertyBoard.h"
#include "OpenBattlefieldDlg.h"

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

class ArmyTreeItem : public QTreeWidgetItem
{
public:
	ArmyTreeItem(zoo::Entity* ent, QTreeWidget* parent)
		: QTreeWidgetItem(parent)
		, _ent(ent)
	{
		setText(0, QString::fromLocal8Bit(ent->desc().c_str()));

		_doF = ent->getComponent<DoF>();
		if (!_doF)
		{
			_doF = ent->addComponent<DoF>("DoFImpl");
			_doF->getImp()->awake();
		}

		_mainCam = ent->getComponent<Camera>();
		if (!_mainCam)
		{
			_mainCam = ent->addComponent<Camera>("CameraImpl");
			_mainCam->getImp()->awake();
		}
	}

	ArmyTreeItem(zoo::Entity* ent, ArmyTreeItem* parent)
		: QTreeWidgetItem(parent)
		, _ent(ent)
		, _mainCam(nullptr)
	{
		setText(0, QString::fromLocal8Bit(ent->desc().c_str()));
		setFlags(flags() | Qt::ItemIsEditable);

		_doF = ent->getComponent<DoF>();
		if (!_doF)
		{
			_doF = ent->addComponent<DoF>("DoFImpl");
			_doF->getImp()->awake();
			setParent(parent->_doF);
		}
	}

	~ArmyTreeItem()
	{
		if (_ent)
			_ent->getSpawner()->remove(_ent, true);
	}

	zoo::Entity* getEnt()
	{
		return _ent;
	}

private:
	void setParent(DoF* parent)
	{
		if (_doF->_parent == parent)
			return;

		if (_doF->_parent)
		{
			auto it = std::find(_doF->_parent->_children.begin(), _doF->_parent->_children.end(), _doF);
			if (it != _doF->_parent->_children.end())
				_doF->_parent->_children.erase(it);
		}

		_doF->_parent = parent;

		if (parent)
			parent->_children.push_back(_doF);

		const_cast<BitState&>(_doF->dirtyBit()).addState(DoF::parent_);
	}

private:
	DoF* _doF;
	Camera* _mainCam;
	zoo::Entity* _ent;
};

ArmyListWgt::ArmyListWgt()
	: _ui(new Ui::ArmyListWgt)
	, _spawner(nullptr)
	, _rootItem(nullptr)
{
	_ui->setupUi(this);
	setFixedWidth(300);
	connect(this, SIGNAL(createItem(zoo::Entity*)), this, SLOT(onCreate(zoo::Entity*)));

	QIntValidator* pIntValidator = new QIntValidator;
	pIntValidator->setRange(0, 9999);
	_ui->lineEdit_id->setValidator(pIntValidator);
	_ui->lineEdit_breed->setValidator(pIntValidator);
	_ui->treeWidget->setHeaderLabels(QStringList() << tr("编号：null    种类：null"));

	connect(_ui->pushButton_create, &QPushButton::clicked, [this]
	{
		if (!_spawner || !_rootItem)
		{
			QMessageBox::warning(this, tr("警告"), tr("请先打开一个场景！"));
			return;
		}

		if (_ui->lineEdit_id->text() == "")
		{
			QMessageBox::warning(this, tr("警告"), tr("实体编号不能为空！"));
			return;
		}

		if (_ui->lineEdit_breed->text() == "")
		{
			QMessageBox::warning(this, tr("警告"), tr("实体种类不能为空！"));
			return;
		}

		if (_ui->lineEdit_desc->text() == "")
		{
			QMessageBox::warning(this, tr("警告"), tr("实体描述不能为空！"));
			return;
		}

		int id = _ui->lineEdit_id->text().toInt();
		int breed = _ui->lineEdit_breed->text().toInt();
		zoo::Entity* ent = _spawner->born(id, breed);
		if (!ent)
		{
			QMessageBox::warning(this, tr("警告"), QString("实体[id:%1,breed:%2]已经存在！").arg(QString::number(id), QString::number(breed)));
			return;
		}

		ent->desc() = _ui->lineEdit_desc->text().toLocal8Bit();
		emit createItem(ent);
	});

	connect(_ui->pushButton_destroy, &QPushButton::clicked, [this]
	{
		ArmyTreeItem* pItem = dynamic_cast<ArmyTreeItem*>(_ui->treeWidget->currentItem());
		if (pItem && pItem != _rootItem)
		{
			_ui->treeWidget->removeItemWidget(pItem, 0);
			delete pItem;
		}
	});

	connect(_ui->treeWidget, &QTreeWidget::currentItemChanged, [this](QTreeWidgetItem *current, QTreeWidgetItem *previous)
	{
		UIManagerService* service = UIActivator::getService<UIManagerService>();
		ComPropertyBoard* pPropBoard = dynamic_cast<ComPropertyBoard*>(service->getWidget(CTK_WAR_UI_PROPERTY_BOARD));
		if (pPropBoard)
		{
			ArmyTreeItem* pItem = (ArmyTreeItem*)current;
			if (pItem && pItem->getEnt())
			{
				zoo::Entity* pEnt = pItem->getEnt();
				pPropBoard->showCurEntComs(pEnt);
				QString strId = pEnt->id() == -1 ? "null" : QString::number(pEnt->id());
				QString strBreed = pEnt->breed() == -1 ? "null" : QString::number(pEnt->breed());
				_ui->treeWidget->setHeaderLabels(QStringList() << QString(tr("编号：%1    种类：%2").arg(strId, strBreed)));
			}
			else
			{
				pPropBoard->showCurEntComs(nullptr);
				_ui->treeWidget->setHeaderLabels(QStringList() << tr("编号：null    种类：null"));
			}
		}
	});

	connect(_ui->treeWidget, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem *item, int column)
	{
		ArmyTreeItem* pItem = (ArmyTreeItem*)item;
		if (pItem && pItem->getEnt())
		{
			Camera* pCam = pItem->getEnt()->getSpawner()->getComponent<Camera>();
			pCam->_trackEntID = pItem->getEnt()->id();
			pCam->_trackEntBreed = pItem->getEnt()->breed();
			const_cast<BitState&>(pCam->dirtyBit()).addState(Camera::trackEnt_);
		}
	});

	connect(_ui->treeWidget, &QTreeWidget::itemChanged, [this](QTreeWidgetItem *item, int column)
	{
		ArmyTreeItem* pItem = (ArmyTreeItem*)item;
		if (pItem && pItem->getEnt())
		{
			if (pItem->text(0).isEmpty())
				pItem->setText(0, QString::fromLocal8Bit(pItem->getEnt()->desc().c_str()));
			else
				pItem->getEnt()->desc() = pItem->text(0).toLocal8Bit();
		}
	});
}

ArmyListWgt::~ArmyListWgt()
{
	delete _ui;
}

void ArmyListWgt::onOpen()
{
	OpenBattlefieldDlg dlg;
	if (dlg.exec() == QDialog::Accepted)
	{
		int curid = dlg.getCurBattlefieldID();
		if (curid == -1)
		{
			QMessageBox::warning(this, tr("警告"), tr("没有要打开的场景！"));
			return;
		}

		WarService* service = UIActivator::getService<WarService>();
		if (service != Q_NULLPTR)
		{
			_rootItem = nullptr;
			_ui->treeWidget->clear();
			service->closeScene();
			service->openScene(curid);
			_spawner = Spawner::find(curid);
			generateItemTree(_spawner, nullptr);
		}
	}
}

void ArmyListWgt::onSave()
{
	WarService* service = UIActivator::getService<WarService>();
	if (service != Q_NULLPTR)
		service->saveScene();
}

void ArmyListWgt::onSim(bool checked)
{
}

void ArmyListWgt::onCreate(zoo::Entity* ent)
{
	ArmyTreeItem* pNewItem = nullptr;
	ArmyTreeItem* pCurSelectItem = dynamic_cast<ArmyTreeItem*>(_ui->treeWidget->currentItem());
	if (pCurSelectItem)
		pNewItem = new ArmyTreeItem(ent, pCurSelectItem);
	else
		pNewItem = new ArmyTreeItem(ent, _rootItem);

	if (pNewItem)
		pNewItem->setExpanded(true);

	_rootItem->setExpanded(true);
}

void ArmyListWgt::generateItemTree(zoo::Entity* ent, ArmyTreeItem* parentItem)
{
	ArmyTreeItem* pNewItem = nullptr;
	if (!parentItem)
		pNewItem = _rootItem = new ArmyTreeItem(ent, _ui->treeWidget);
	else
		pNewItem = new ArmyTreeItem(ent, parentItem);

	DoF* pDof = ent->getComponent<DoF>();
	if (pDof)
	{
		size_t len = pDof->_children.size();
		for (size_t i = 0; i < len; ++i)
		{
			generateItemTree(pDof->_children[i]->getEntity(), pNewItem);
		}
	}

	pNewItem->setExpanded(true);
}
