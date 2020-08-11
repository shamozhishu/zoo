#include "WarSimulator.h"
#include "LuaScript.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>

SimState::SimState()
	: _script(new LuaScript)
	, _scriptValid(false)
{
}

SimState::~SimState()
{
	SAFE_DELETE(_script);
}

bool SimState::init(string scriptFile)
{
	_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + scriptFile);
	return _scriptValid;
}

void SimState::stepping()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Stepping");
}

void SimState::enter()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Enter");
}

void SimState::exit()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Exit");
}

SimState WarSimulator::_simStates[count_];
WarSimulator::WarSimulator()
	: _currentState(nullptr)
{
}

void WarSimulator::stepping()
{
	if (_currentState)
		_currentState->stepping();
}

void WarSimulator::transition(ESimState simState)
{
	if (_currentState)
		_currentState->exit();
	_currentState = &WarSimulator::_simStates[simState];
	if (_currentState)
		_currentState->enter();
}
