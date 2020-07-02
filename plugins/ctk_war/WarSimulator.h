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
	SimState* handle(ESimState simState);
	void refresh();
	void enter();
	void exit();
};

class WarSimulator
{
	friend class SimState;
	SimState* _state;
public:
	WarSimulator();
	virtual void refresh();
	virtual void handle(ESimState simState);

protected:
	static SimState _SimStates[count_];
};

#endif // __WAR_SIMULATOR_H__
