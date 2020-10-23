#ifndef __ARMY_LIST_WGT_H__
#define __ARMY_LIST_WGT_H__

#include <QTreeWidget>
#include <component/war/WarComponents.h>

namespace Ui
{
	class ArmyListWgt;
}

class ArmyTreeItem;
class ArmyTreeWgt : public QTreeWidget
{
	Q_OBJECT
public:
	ArmyTreeWgt(QWidget* parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent* ev);
	void mouseMoveEvent(QMouseEvent* ev);
	void dragMoveEvent(QDragEnterEvent* ev);
	void dragLeaveEvent(QDragEnterEvent* ev);
	void dropEvent(QDropEvent* ev);

private Q_SLOTS:
	void onTreeWgtItemPressed(QTreeWidgetItem* item, int column);

private:
	QMenu* _rightClickMenu;
	QPoint _beginDragPoint;
	ArmyTreeItem* _newParentItem;
};

class ArmyListWgt : public QWidget
{
	Q_OBJECT
public:
	ArmyListWgt();
	~ArmyListWgt();
	QSize sizeHint() const;

Q_SIGNALS:
	void createItem(zoo::Entity* ent);
	void sceneOpendSucceed();
	void sceneOpendFailed();
	void sceneClosed();
	void simStarted();
	void simPaused();
	void simStoped();

private Q_SLOTS:
	void onOpen();
	void onClose();
	void onSave();
	void onSim(bool started);
	void onPaused(bool paused);
	void onCreate(zoo::Entity* ent);

private:
	void generateItemTree(zoo::Entity* ent, ArmyTreeItem* parentItem);

private:
	Ui::ArmyListWgt* _ui;
	zoo::Spawner* _spawner;
	ArmyTreeItem* _rootItem;
	friend class ArmyTreeWgt;
};

#endif // __ARMY_LIST_WGT_H__
