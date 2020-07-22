#ifndef __COM_PROPERTY_BOARD_H__
#define __COM_PROPERTY_BOARD_H__

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <component/war/SharedComponents.h>

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
	QVBoxLayout* _rootLayout;
	QPushButton* _addComponentBtn;
	QPushButton* _removeComponentBtn;
	QHash<QString, PropertyWgt*> _comPropertyWgts;
};

#endif // __COM_PROPERTY_BOARD_H__
