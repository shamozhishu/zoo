#include "WarSimulator.h"
#include "LuaScript.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>

SimState::SimState()
	: _script(nullptr)
	, _scriptValid(false)
{
}

SimState::~SimState()
{
	SAFE_DELETE(_script);
}

bool SimState::init(string scriptFile, WarSimulator* simulator)
{
	if (simulator && scriptFile != "")
	{
		SAFE_DELETE(_script);
		_script = new LuaScript;
		_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + scriptFile);
		if (_scriptValid)
		{
			_script->setVariable("this", simulator->typeName().c_str(), simulator);
			_script->executeGlobalFunction("Init");
		}
	}

	return _scriptValid;
}

void SimState::enter()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Enter");
}

void SimState::stepping()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Stepping");
}

void SimState::exit()
{
	if (_scriptValid)
		_script->executeGlobalFunction("Exit");
}

SimState WarSimulator::_simStates[count_];
WarSimulator::WarSimulator()
	: _currentState(&_simStates[uninited_])
{
}

void WarSimulator::stepping()
{
	if (_currentState && _currentState != &_simStates[uninited_])
	{
		_currentState->stepping();
		if (_currentState == &_simStates[running_])
		{
			auto it = _behaviors.begin();
			auto itEnd = _behaviors.end();
			for (; it != itEnd; ++it)
				(*it)->exec();
		}
	}
}

void WarSimulator::transition(ESimState simState)
{
	if (_currentState != &WarSimulator::_simStates[simState])
	{
		if (_currentState)
			_currentState->exit();
		_currentState = &WarSimulator::_simStates[simState];
		if (_currentState)
			_currentState->enter();
	}
}

void WarSimulator::addBehavior(Behavior* behavior)
{
	_behaviors.push_back(behavior);
}

void WarSimulator::removeBehavior(Behavior* behavior)
{
	auto it = std::find(_behaviors.begin(), _behaviors.end(), behavior);
	if (it != _behaviors.end())
		_behaviors.erase(it);
}
