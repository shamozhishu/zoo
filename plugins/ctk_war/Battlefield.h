#ifndef __BATTLEFIELD_H__
#define __BATTLEFIELD_H__

#include "WarSimulator.h"
#include "WarComponents.h"

using namespace zoo;

class Battlefield : public WarSimulator { // tolua_export
	TableCSV* _table;
	Spawner* _spawner;
	list<Behavior*> _behaviors;
	PROPERTY_R(int, _id, ID)

public:
	Battlefield(int id, TableCSV* table);
	~Battlefield();
	void enter();
	void save();
	void exit();
	void stepping();
	void addBehavior(Behavior* behavior);
	void removeBehavior(Behavior* behavior);

public:
	// tolua_begin
	Entity* createArmy(int id, int breed);
	Entity* getArmy(int id, int breed);
	void destroyArmy(int id, int breed);
};
// tolua_end

#endif // __BATTLEFIELD_H__
