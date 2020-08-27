#ifndef __PROPERTY_WGTS_H__
#define __PROPERTY_WGTS_H__

#include <QWidget>
#include <QHBoxLayout>
#include <component/war/WarComponents.h>

namespace Ui
{
	class ComListWgt;
	class DoFPropertyWgt;
	class ModelPropertyWgt;
	class CameraPropertyWgt;
	class EarthPropertyWgt;
	class BehaviorPropertyWgt;
	class EnvirPropertyWgt;
}

class UIManagerService;
class ComPropertyBoard;
class ComListWgt : public QWidget
{
public:
	ComListWgt(ComPropertyBoard* propBoard);
	~ComListWgt();
	void refreshComList(zoo::Entity* ent, bool bAddComponentBtn, const QStringList& comlist);
protected:
	bool eventFilter(QObject *o, QEvent *e);
private:
	Ui::ComListWgt* _ui;
	bool _addComponentBtn;
	zoo::Entity* _operateEnt;
	ComPropertyBoard* _propBoard;
};

class PropertyWgt : public QWidget
{
public:
	PropertyWgt(QWidget *parent);
	QSize sizeHint() const;
	virtual void resetCom(zoo::Component* pCom);

protected:
	zoo::Component* _com;
	UIManagerService* _uiMgr;
};

class DoFPropertyWgt : public PropertyWgt
{
public:
	DoFPropertyWgt(QWidget *parent);
	~DoFPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::DoFPropertyWgt* _ui;
};

class ModelPropertyWgt : public PropertyWgt
{
public:
	ModelPropertyWgt(QWidget* parent);
	~ModelPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::ModelPropertyWgt* _ui;
};

class CameraPropertyWgt : public PropertyWgt
{
public:
	CameraPropertyWgt(QWidget* parent);
	~CameraPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::CameraPropertyWgt* _ui;
};

class EarthPropertyWgt : public PropertyWgt
{
public:
	EarthPropertyWgt(QWidget* parent);
	~EarthPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::EarthPropertyWgt* _ui;
};

class BehaviorPropertyWgt : public PropertyWgt
{
public:
	BehaviorPropertyWgt(QWidget* parent);
	~BehaviorPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::BehaviorPropertyWgt* _ui;
};

class EnvirPropertyWgt : public PropertyWgt
{
public:
	EnvirPropertyWgt(QWidget* parent);
	~EnvirPropertyWgt();
	void resetCom(zoo::Component* pCom);

private:
	Ui::EnvirPropertyWgt* _ui;
};

#endif // __PROPERTY_WGTS_H__
