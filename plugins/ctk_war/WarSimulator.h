#ifndef __WAR_SIMULATOR_H__
#define __WAR_SIMULATOR_H__

#include <zoo/Support.h>

enum ESimState { ready_ = 0, running_, paused_, count_ };

class LuaScript;
class SimState
{
	bool _scriptValid;
	LuaScript* _script;
public:
	SimState();
	~SimState();
	bool init(string scriptFile);
	void stepping();
	void enter();
	void exit();
};

class WarSimulator
{	
public:
	WarSimulator();
	virtual void stepping();
	void transition(ESimState simState);

protected:
	friend class SimState;
	SimState* _currentState;
	static SimState _simStates[count_];
};

#endif // __WAR_SIMULATOR_H__
