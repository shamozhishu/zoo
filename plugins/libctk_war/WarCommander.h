#pragma once

#include <QTimer>
#include <zoo/Support.h>
#include <zoo/Singleton.h>
#include <QtXml/QDomElement>
#include "PublicEnum.h"

class DoF;
class Battlefield;
class WarCommander : public QObject, public zoo::Singleton<WarCommander>
{
	Q_OBJECT
public:
	WarCommander(string relatedCmd, string resPath, string mainTable);
	~WarCommander();
	const string& getResPath() const;
	bool enterBattlefield(int id);
	void exitCurBattlefield();
	void saveCurBattlefield();
	const char* getRelatedCmd() const;
	const char* getEntTypeName(ENTITY_TYPE entType) const;

protected:
	void loadDoF(QDomElement& elem, DoF* parent);
	void saveDoF(QDomElement& elem, DoF* parent, QDomDocument& doc);
	void writeFile(ENTITY_TYPE entType, stringstream& ss, zoo::TableCSV* pTable);

protected slots:
	void tick();

private:
	string _relatedCmd;
	string _resPath;
	string _mainTable;
	QTimer _tickTimer;
	Battlefield* _currentBattlefield;
	const char* _entNames[ENTITY_COUNT];
	map<int, Battlefield*> _battlefields;
};
