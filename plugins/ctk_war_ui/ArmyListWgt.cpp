#include "ArmyListWgt.h"
#include "UIActivator.h"
#include "ui_ArmyListWgt.h"
#include <QTreeWidgetItem>
#include <QDesktopServices>
#include <QMessageBox>
#include <QMimeData>
#include <QDrag>
#include <QMenu>
#include <QMouseEvent>
#include <ctk_service/WarService.h>
#include <ctk_service/UIManagerService.h>
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
		, _earthCom(nullptr)
		, _uiMgr(UIActivator::getService<UIManagerService>())
	{
		setText(0, QString::fromLocal8Bit(ent->desc().c_str()));

		_doF = ent->getComponent<DoF>();
		_mainCam = ent->getComponent<Camera>();
		if (!_mainCam)
		{
			_mainCam = ent->addComponent<Camera, int>("CameraImpl", MAIN_VIEW_WGT);
			_mainCam->getImp()->awake();
			_uiMgr->starWindowTitle();
		}

		if (ent->isSpawner() && ent->breed() != 0)
		{
			_earthCom = ent->getComponent<Earth>();
			if (!_earthCom)
			{
				_earthCom = ent->addComponent<Earth>("EarthImpl");
				_earthCom->getImp()->awake();
				_uiMgr->starWindowTitle();
			}
		}
	}

	ArmyTreeItem(zoo::Entity* ent, ArmyTreeItem* parent)
		: QTreeWidgetItem(parent)
		, _ent(ent)
		, _mainCam(nullptr)
		, _earthCom(nullptr)
		, _uiMgr(UIActivator::getService<UIManagerService>())
	{
		setText(0, QString::fromLocal8Bit(ent->desc().c_str()));
		setFlags(flags() | Qt::ItemIsEditable);

		_doF = ent->getComponent<DoF>();
		if (!_doF->_parent)
		{
			setParent(parent->_doF);
			_uiMgr->starWindowTitle();
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

		const_cast<BitState&>(_doF->dirtyBit()).addState(DoF::Parent_);
		_uiMgr->starWindowTitle();
	}

private:
	DoF* _doF;
	Camera* _mainCam;
	Earth* _earthCom;
	zoo::Entity* _ent;
	UIManagerService* _uiMgr;
};

ArmyTreeWgt::ArmyTreeWgt(QWidget* parent /*= Q_NULLPTR*/)
	: _rightClickMenu(nullptr)
	, _newParentItem(nullptr)
{
	_rightClickMenu = new QMenu(this);
	QAction* pAct = new QAction(tr("删除"), this);
	_rightClickMenu->addAction(pAct);
	setItemDelegate(new NoFocusDelegate());
	connect(pAct, &QAction::triggered, [this]
	{
		ArmyTreeItem* pItem = dynamic_cast<ArmyTreeItem*>(currentItem());
		if (pItem)
		{
			removeItemWidget(pItem, 0);
			delete pItem;
			UIActivator::getService<UIManagerService>()->starWindowTitle();
		}
	});

	connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(onTreeWgtItemPressed(QTreeWidgetItem*, int)));
	connect(this, &QTreeWidget::currentItemChanged, [this](QTreeWidgetItem *current, QTreeWidgetItem *previous)
	{
		UIManagerService* service = UIActivator::getService<UIManagerService>();
		ComPropertyBoard* pPropBoard = dynamic_cast<ComPropertyBoard*>(service->getWidget(CTK_WAR_UI_PROPERTY_BOARD));
		if (pPropBoard)
		{
			if (_newParentItem && previous)
			{
				previous->setExpanded(true);
				if (previous->parent())
					previous->parent()->setExpanded(true);

				ArmyTreeItem* pChildItem = dynamic_cast<ArmyTreeItem*>(previous);
				if (pChildItem)
				{
					pChildItem->setParent(_newParentItem->getEnt()->getComponent<DoF>());
					pPropBoard->showCom("DoF", pChildItem->getEnt()->getComponent<DoF>());
				}
			}

			ArmyTreeItem* pItem = dynamic_cast<ArmyTreeItem*>(current);
			if (pItem && pItem->getEnt())
			{
				pItem->setExpanded(true);
				if (pItem->parent())
					pItem->parent()->setExpanded(true);

				zoo::Entity* pEnt = pItem->getEnt();
				pPropBoard->showCurEntComs(pEnt);
				QString strId = pEnt->id() == -1 ? "null" : QString::number(pEnt->id());
				QString strBreed = "null";
				if (pEnt->isSpawner())
				{
					switch (pEnt->breed())
					{
					case 0: strBreed = "地形"; break;
					case 1: strBreed = "地球"; break;
					case 2: strBreed = "地图"; break;
					}
				}
				else
				{
					strBreed = pEnt->breed() == -1 ? "null" : QString::number(pEnt->breed());
				}
				QString strDesc = pEnt->desc() == "" ? "null" : QString::fromLocal8Bit(pEnt->desc().c_str());
				setHeaderLabels(QStringList() << QString(tr("编号：%1    类别：%2    描述：%3").arg(strId, strBreed, strDesc)));
			}
			else
			{
				pPropBoard->showCurEntComs(nullptr);
				setHeaderLabels(QStringList() << tr("编号：null    类别：null    描述：null"));
			}
		}
	});

	connect(this, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem *item, int column)
	{
		ArmyTreeItem* pItem = (ArmyTreeItem*)item;
		if (pItem && pItem->getEnt())
		{
			UIActivator::sendWarCmd(QString("focus(%1,%2,%3)").arg(
				pItem->getEnt()->id()).arg(pItem->getEnt()->breed()).arg(pItem->getEnt()->getSpawner()->id()));
		}
	});

	connect(this, &QTreeWidget::itemChanged, [this](QTreeWidgetItem *item, int column)
	{
		ArmyTreeItem* pItem = (ArmyTreeItem*)item;
		if (pItem && pItem->getEnt())
		{
			if (pItem->text(0).isEmpty())
				pItem->setText(0, QString::fromLocal8Bit(pItem->getEnt()->desc().c_str()));
			else
			{
				zoo::Entity* pEnt = pItem->getEnt();
				string strOldDesc = pEnt->desc();
				string strNewDesc = pItem->text(0).toLocal8Bit();
				if (strOldDesc != strNewDesc)
				{
					pEnt->desc() = strNewDesc;
					UIActivator::getService<UIManagerService>()->starWindowTitle();

					if (item == currentItem())
					{
						QString strId = pEnt->id() == -1 ? "null" : QString::number(pEnt->id());
						QString strBreed = pEnt->breed() == -1 ? "null" : QString::number(pEnt->breed());
						QString strDesc = pEnt->desc() == "" ? "null" : QString::fromLocal8Bit(pEnt->desc().c_str());
						setHeaderLabels(QStringList() << QString(tr("编号：%1    类别：%2    描述：%3").arg(strId, strBreed, strDesc)));
					}
				}
			}
		}
	});
}

void ArmyTreeWgt::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
		_beginDragPoint = ev->pos();
	QTreeWidget::mousePressEvent(ev);
}

void ArmyTreeWgt::mouseMoveEvent(QMouseEvent* ev)
{
	if (ev->buttons() & Qt::LeftButton)
	{
		int dragDistance = QPoint(0, ev->pos().ry() - _beginDragPoint.ry()).manhattanLength();
		if (dragDistance > QApplication::startDragDistance())
		{
		}
	}
	QTreeWidget::mouseMoveEvent(ev);
}

void ArmyTreeWgt::dragMoveEvent(QDragEnterEvent* ev)
{
	ArmyTreeWgt* source = qobject_cast<ArmyTreeWgt*>(ev->source());
	if (source && source == this && itemAt(ev->pos()))
	{
		ev->setDropAction(Qt::MoveAction);
		ev->accept();
	}
}

void ArmyTreeWgt::dragLeaveEvent(QDragEnterEvent* ev)
{
	ArmyTreeWgt* source = qobject_cast<ArmyTreeWgt*>(ev->source());
	if (source && source == this && itemAt(ev->pos()))
	{
		ev->setDropAction(Qt::MoveAction);
		ev->accept();
	}
}

void ArmyTreeWgt::dropEvent(QDropEvent* ev)
{
	ArmyTreeWgt* source = qobject_cast<ArmyTreeWgt*>(ev->source());
	if (source && source == this)
	{
		ev->setDropAction(Qt::MoveAction);
		_newParentItem = dynamic_cast<ArmyTreeItem*>(itemAt(ev->pos()));
		if (!_newParentItem)
			return;
	}
	QTreeWidget::dropEvent(ev);
	_newParentItem = nullptr;
}

void ArmyTreeWgt::onTreeWgtItemPressed(QTreeWidgetItem* item, int column)
{
	ArmyTreeItem* pItem = dynamic_cast<ArmyTreeItem*>(item);
	if (!pItem || pItem == ((ArmyListWgt*)parent())->_rootItem)
		return;
	if (qApp->mouseButtons() == Qt::RightButton)
		_rightClickMenu->exec(QCursor::pos());
}

ArmyListWgt::ArmyListWgt()
	: _ui(new Ui::ArmyListWgt)
	, _spawner(nullptr)
	, _rootItem(nullptr)
{
	_ui->setupUi(this);
	_ui->treeWidget->setHeaderLabels(QStringList() << tr("编号：null    类别：null    描述：null"));
	connect(this, SIGNAL(createItem(zoo::Entity*)), this, SLOT(onCreate(zoo::Entity*)));

	QIntValidator* pIntValidator = new QIntValidator;
	pIntValidator->setRange(0, 9999);
	_ui->lineEdit_id->setValidator(pIntValidator);
	_ui->lineEdit_breed->setValidator(pIntValidator);

	connect(_ui->toolButton_createEnt, &QPushButton::clicked, [this]
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
			QMessageBox::warning(this, tr("警告"), tr("实体类别不能为空！"));
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
	connect(_ui->toolButton_script, &QToolButton::clicked, [this]
	{
		QString filename = OpenBattlefieldDlg::getCurBattlefieldFile(ScriptFile_);
		if (filename.isEmpty())
		{
			QMessageBox::warning(this, tr("警告"), tr("仿真脚本不存在！"));
			return;
		}

		filename = QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()) + filename;
		if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filename)))
			QMessageBox::warning(this, tr("警告"), tr("打开仿真脚本文件%1失败！").arg(filename));
	});
}

ArmyListWgt::~ArmyListWgt()
{
	delete _ui;
}

QSize ArmyListWgt::sizeHint() const
{
	return QSize(300, -1); // 在这里定义dock的初始大小
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

		QString curscenefile = dlg.getCurBattlefieldFile(SceneFile_);
		if (curscenefile.isEmpty())
		{
			QMessageBox::warning(this, tr("警告"), tr("场景文件不存在！"));
			return;
		}

		WarService* service = UIActivator::getService<WarService>();
		UIManagerService* uiMgr = UIActivator::getService<UIManagerService>();
		if (service && uiMgr)
		{
			_rootItem = nullptr;
			_ui->treeWidget->clear();
			service->closeScene();
			if (service->openScene(curid))
			{
				emit sceneOpendSucceed();
				_spawner = Spawner::find(curid);
				generateItemTree(_spawner, nullptr);
				uiMgr->setWindowTitle(tr("战场编辑器 - ") + QString::fromLocal8Bit(ZOO_DATA_ROOT_DIR.c_str()) + curscenefile);
			}
			else
			{
				emit sceneOpendFailed();
				uiMgr->setWindowTitle(tr("战场编辑器"));
				QMessageBox::warning(this, tr("警告"), tr("打开场景失败！"));
			}
		}
	}
}

void ArmyListWgt::onClose()
{
	WarService* service = UIActivator::getService<WarService>();
	UIManagerService* uiMgr = UIActivator::getService<UIManagerService>();
	if (service && uiMgr)
	{
		if (uiMgr->needSavedScene())
		{
			if (QMessageBox::information(this, tr("提示"), tr("是否需要保存当前的场景？"), QMessageBox::Save, QMessageBox::Discard) == QMessageBox::Save)
				service->saveScene();
		}

		_rootItem = nullptr;
		_ui->treeWidget->clear();
		service->closeScene();
		emit sceneClosed();
		uiMgr->setWindowTitle(tr("战场编辑器"));
	}
}

void ArmyListWgt::onSave()
{
	WarService* service = UIActivator::getService<WarService>();
	UIManagerService* uiMgr = UIActivator::getService<UIManagerService>();
	if (service && uiMgr)
	{
		service->saveScene();
		uiMgr->unstarWindowTitle();
	}
}

void ArmyListWgt::onSim(bool started)
{
	WarService* service = UIActivator::getService<WarService>();
	if (service)
	{
		if (started)
		{
			if (service->startSimulation())
				emit simStarted();
		}
		else
		{
			if (service->stopSimulation())
				emit simStoped();
		}
	}
}

void ArmyListWgt::onPaused(bool paused)
{
	WarService* service = UIActivator::getService<WarService>();
	if (service)
	{
		if (paused)
		{
			if (service->pauseSimulation())
				emit simPaused();
		}
		else
		{
			if (service->startSimulation())
				emit simStarted();
		}
	}
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
