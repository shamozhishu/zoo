#pragma once

#include <zoo/Support.h>

enum ESimState { Ready_ = 0, Running_, Paused_, FinishedSuccess_, FinishedFailed_, Count_ };

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
	void update();
	void enter();
	void exit();
};

class WarSimulator
{
	friend class SimState;
	SimState* _state;
public:
	WarSimulator();
	virtual void update();
	virtual void handle(ESimState simState);

protected:
	static SimState _SimStates[Count_];
};
