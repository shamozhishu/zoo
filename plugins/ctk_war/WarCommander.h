#ifndef __WAR_COMMANDER_H__
#define __WAR_COMMANDER_H__

#include <QTimer>
#include <zoo/Support.h>
#include <zoo/Singleton.h>

class Battlefield;
class WarCommander : public QObject, public zoo::Singleton<WarCommander>
{
	Q_OBJECT
public:
	WarCommander(string cmd, string table);
	~WarCommander();
	const char* getCmd() const;
	bool enterBattlefield(int id);
	void exitCurBattlefield();
	void saveCurBattlefield();
	Battlefield* getCurBattlefield();

protected slots:
	void tick();

private:
	string _cmd;
	QTimer _tickTimer;
	zoo::TableCSV* _battlefieldTable;
	Battlefield* _currentBattlefield;
};

#endif // __WAR_COMMANDER_H__
