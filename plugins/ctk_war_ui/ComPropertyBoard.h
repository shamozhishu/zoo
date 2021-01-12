#ifndef __COM_PROPERTY_BOARD_H__
#define __COM_PROPERTY_BOARD_H__

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <component/WarComponents.h>

namespace Ui {
	class ComPropertyBoard;
}

class PropertyWgt;
class ComPropertyBoard : public QWidget
{
public:
	ComPropertyBoard();
	~ComPropertyBoard();

public:
	void showCurEntComs(zoo::Entity* ent);
	void showCom(QString comTypeName, zoo::Component* pCom);
	void hideCom(QString comTypeName);

private:
	void hideAll();
	PropertyWgt* createPropertyWgtForTerrainScene(QString comTypeName);
	PropertyWgt* createPropertyWgtForTerrainEntity(QString comTypeName);
	PropertyWgt* createPropertyWgtForEarthScene(QString comTypeName);
	PropertyWgt* createPropertyWgtForEarthEntity(QString comTypeName);
	PropertyWgt* createPropertyWgtForMapScene(QString comTypeName);
	PropertyWgt* createPropertyWgtForMapEntity(QString comTypeName);

private:
	zoo::Entity* _curSelEnt;
	Ui::ComPropertyBoard* _ui;
	QHash<QString, PropertyWgt*> _comPropertyWgts;
};

#endif // __COM_PROPERTY_BOARD_H__
