#ifndef __BATTLEFIELD_H__
#define __BATTLEFIELD_H__

#include "WarSimulator.h"

using namespace zoo;

class Battlefield : public WarSimulator { // tolua_export
	TableCSV* _table;
	Spawner* _spawner;
	PROPERTY_R(int, _id, ID)

public:
	Battlefield(int id, TableCSV* table);
	~Battlefield();
	void stepping();
	bool load();
	void save();

public:
	// tolua_begin
	Entity* createArmy(int id, int breed);
	Entity* getArmy(int id, int breed);
	void destroyArmy(int id, int breed);
};
// tolua_end

#endif // __BATTLEFIELD_H__
