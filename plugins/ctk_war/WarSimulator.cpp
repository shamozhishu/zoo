#include "WarSimulator.h"
#include <zoo/Utils.h>
#include <zoo/DatabaseCSV.h>
#include <luaScript/LuaScriptStack.h>

extern TOLUA_OPEN_FUNC g_func[];

// C++调用执行的lua脚本函数名称
static const string s_Init = "Init";             // 仿真初始化
static const string s_Start = "Start";           // 仿真开始
static const string s_Enter = "Enter";           // 进入仿真
static const string s_Update = "Update";         // 仿真轮询
static const string s_LateUpdate = "LateUpdate"; // 延迟轮询
static const string s_Exit = "Exit";             // 退出仿真
static const string s_Finish = "Finish";         // 仿真结束

WarSimulator::WarSimulator(string scriptFile)
	: _script(new LuaScriptStack(g_func))
	, _scriptValid(false)
	, _scriptInited(false)
	, _scriptFile(scriptFile)
	, _currentState(null_)
{
}

WarSimulator::~WarSimulator()
{
	SAFE_DELETE(_script);
}

void WarSimulator::resetSimuScene()
{
	_scriptInited = false;
}

void WarSimulator::stepping()
{
	if (_currentState == running_)
	{
		execBehaviors(s_Update.c_str());
		execSimuScene(s_Update.c_str());
		execBehaviors(s_LateUpdate.c_str());
		execSimuScene(s_LateUpdate.c_str());
	}
}

ESimState WarSimulator::getCurSimState() const
{
	return _currentState;
}

void WarSimulator::transition(ESimState simState)
{
	if (_currentState == simState)
		return;

	if (_currentState == running_)
	{
		execBehaviors(s_Exit.c_str());
		execSimuScene(s_Exit.c_str());
	}

	if ((simState == null_ || simState == ready_) && _currentState != null_ && _currentState != ready_)
	{
		execBehaviors(s_Finish.c_str());
		execSimuScene(s_Finish.c_str());
	}

	if ((_currentState == null_ || _currentState == ready_) && simState != null_ && simState != ready_)
	{
		execSimuScene(s_Start.c_str());
		execBehaviors(s_Start.c_str());
	}

	if (simState == running_)
	{
		execSimuScene(s_Enter.c_str());
		execBehaviors(s_Enter.c_str());
	}

	_currentState = simState;
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

void WarSimulator::execBehaviors(const char* func)
{
	auto it = _behaviors.begin();
	auto itEnd = _behaviors.end();
	for (; it != itEnd; ++it)
		(*it)->exec(func);
}

void WarSimulator::execSimuScene(const char* func)
{
	if (!_scriptInited)
	{
		_scriptInited = true;
		_scriptValid = false;
		if (_scriptFile != "")
		{
			LuaScriptStack::clearScriptFileName2ContentCache(ZOO_DATA_ROOT_DIR + _scriptFile);
			_scriptValid = _script->executeScriptFile(ZOO_DATA_ROOT_DIR + _scriptFile);
			if (_scriptValid)
			{
				static Log logger;
				_script->setVariable("logger", "Log", &logger);
				_script->setVariable("this", typeName().c_str(), this);
				_script->executeGlobalFunction(s_Init);
			}
		}
	}

	if (_scriptValid)
		_script->executeGlobalFunction(func);
}
