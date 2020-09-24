#ifndef __COM_PROPERTY_BOARD_H__
#define __COM_PROPERTY_BOARD_H__

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <component/war/WarComponents.h>

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

private:
	zoo::Entity* _curSelEnt;
	Ui::ComPropertyBoard* _ui;
	QHash<QString, PropertyWgt*> _comPropertyWgts;
};

#endif // __COM_PROPERTY_BOARD_H__
