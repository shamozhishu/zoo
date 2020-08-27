#ifndef __WAR_SIMULATOR_H__
#define __WAR_SIMULATOR_H__

#include "WarComponents.h"

enum ESimState { uninited_ = 0, ready_, running_, paused_, count_ };

class LuaScript;
class WarSimulator;
class SimState
{
	bool _scriptValid;
	LuaScript* _script;
public:
	SimState();
	~SimState();
	bool init(string scriptFile, WarSimulator* simulator);
	void enter();
	void stepping();
	void exit();
};

class WarSimulator : zoo::Type
{	
public:
	WarSimulator();
	virtual void stepping();
	void transition(ESimState simState);
	void addBehavior(Behavior* behavior);
	void removeBehavior(Behavior* behavior);

protected:
	friend class SimState;
	SimState* _currentState;
	list<Behavior*> _behaviors;
	static SimState _simStates[count_];
};

#endif // __WAR_SIMULATOR_H__
