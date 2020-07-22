#ifndef __ARMY_LIST_WGT_H__
#define __ARMY_LIST_WGT_H__

#include <QWidget>
#include <component/war/SharedComponents.h>

namespace Ui
{
	class ArmyListWgt;
}

class ArmyTreeItem;
class ArmyListWgt : public QWidget
{
	Q_OBJECT
public:
	ArmyListWgt();
	~ArmyListWgt();

Q_SIGNALS:
	void createItem(zoo::Entity* ent);

private Q_SLOTS:
	void onOpen();
	void onSave();
	void onSim(bool checked);
	void onCreate(zoo::Entity* ent);

private:
	void generateItemTree(zoo::Entity* ent, ArmyTreeItem* parentItem);

private:
	Ui::ArmyListWgt* _ui;
	zoo::Spawner* _spawner;
	ArmyTreeItem* _rootItem;
};

#endif // __ARMY_LIST_WGT_H__
