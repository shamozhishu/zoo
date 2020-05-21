#pragma once

#include <QTimer>
#include <zoo/Support.h>
#include <zoo/Singleton.h>
#include "PublicEnum.h"
#include "WarSimulator.h"

class DoF;
class Battlefield;
class WarCommander : public QObject, public WarSimulator, public zoo::Singleton<WarCommander>
{
	Q_OBJECT
public:
	WarCommander(string relatedCmd, string mainTable);
	~WarCommander();
	bool enterBattlefield(int id);
	void exitCurBattlefield();
	void saveCurBattlefield();
	const char* getRelatedCmd() const;
	const char* getEntTypeName(ENTITY_TYPE entType) const;

protected slots:
	void tick();

private:
	string _relatedCmd;
	string _mainTable;
	QTimer _tickTimer;
	Battlefield* _currentBattlefield;
	const char* _entNames[ENTITY_COUNT];
	map<int, Battlefield*> _battlefields;
};
