#include <zooCmd/CmdManager.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd/BuiltinCmd.h>
#include <zooCmd/CmdParser.h>
#include <zoo/Reflection.h>
#include <zoo/Interlock.h>
#include <zoo/Utils.h>

#define ZOO_ERROR_MESSAGE "ZOO_ERROR_MESSAGE"

namespace zooCmd {

zoo::Interlock g_interlock;
std::thread::id g_renderThreadID;
InputAdapter* g_inputAdapter = nullptr;

static UserData s_retValue;
static std::mutex s_mutex;
static bool s_blockWhenWaitReturnValue = true;
static map<string, std::pair<bool, shared_ptr<promise<Any>>>> s_promises;

static BuiltinCmd* s_builtinCmd = nullptr;
CmdManager::CmdManager()
	: _curCmd(nullptr)
	, _thread(nullptr)
	, _busying(false)
	, _running(false)
{
	s_blockWhenWaitReturnValue = true;
}

CmdManager::~CmdManager()
{
	if (g_inputAdapter)
		g_inputAdapter->setDone(true);

	g_interlock.canExchange = true;
	_block[0].release();

	if (!_busying)
		_curCmd = nullptr;

	_block[1].release();

	if (_thread)
		_thread->join();

	delete _thread;
	_thread = nullptr;
	_commands.clear();
	delete g_inputAdapter;
	g_inputAdapter = nullptr;
	s_promises.clear();
	s_retValue.clear();
	s_blockWhenWaitReturnValue = true;
}

void CmdManager::start()
{
	if (!_thread)
		_thread = new thread(&CmdManager::run, this);
}

void CmdManager::initBuiltinCmd()
{
	s_builtinCmd = ReflexFactory<>::getInstance().create<BuiltinCmd>("zooCmd::BuiltinCmd");
	addCmd("__BUILTIN__", s_builtinCmd);
}

void CmdManager::block(bool isBlock)
{
	if (isBlock)
	{
		if (_thread->get_id() == std::this_thread::get_id())
		{
			g_interlock.endExchanging();
			cancelRetValueBlock();
			_block[0].reset();
			_block[0].block();
			g_interlock.beginExchanging();
		}
	}
	else
	{
		_block[0].release();
	}
}

bool CmdManager::addCmd(const string& cmd, Cmd* pCmd)
{
	if (!pCmd)
		return false;

	std::shared_ptr<Cmd> cmdPtr(pCmd);
	if (!cmdPtr->init())
		return false;

	_commands[strToUpper(cmd)] = cmdPtr;
	return true;
}

void CmdManager::removeCmd(const string& cmd)
{
	auto it = _commands.find(strToUpper(cmd));
	if (it != _commands.end())
		_commands.erase(it);
}

bool CmdManager::sendCmd(vector<string> arglist)
{
	if (!_running)
	{
		Log::print(ELL_ERROR, "Command manager not started!");
		return false;
	}

	if (_busying)
	{
		Log::print(ELL_ERROR, "Last command not completed!");
		return false;
	}

	int argc = arglist.size();
	if (argc <= 0)
	{
		Log::print(ELL_ERROR, "Please enter a command!");
		return false;
	}

	Cmd* pCmd = nullptr;
	string cmdname = "";
	do
	{
		if (arglist[0][0] == '-')
		{
			argc += 1;
			arglist.insert(arglist.begin(), "__BUILTIN__");
			pCmd = s_builtinCmd;
			cmdname = "__BUILTIN__";
			break;
		}

		cmdname = strToUpper(arglist[0]);
		if (cmdname == _cmdName)
		{
			pCmd = _curCmd;
			break;
		}

		auto it = _commands.find(cmdname);
		if (it != _commands.end())
		{
			pCmd = it->second.get();
			break;
		}

		Log::print(ELL_ERROR, "Non-existent command!");
		return false;

	} while (0);

	if (argc <= 1 || arglist[1][0] != '-')
	{
		Log::print(ELL_ERROR, "Please enter valid command parameters!");
		return false;
	}

	char** argv = new char*[argc];
	for (int i = 0; i < argc; ++i)
		argv[i] = const_cast<char*>(arglist[i].c_str());

	CmdParser cmdArg(&argc, argv);
	if (_curCmd != pCmd)
	{
		cmdArg.getCmdUsage()->setCommandLineOptions(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setCommandLineOptionsDefaults(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setEnvironmentalVariables(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setEnvironmentalVariablesDefaults(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setKeyboardMouseBindings(CmdUsage::UsageMap());
	}

	unsigned int helpType = 0;
	if ((helpType = cmdArg.readHelpType()))
	{
		_curCmd = pCmd;
		_cmdName = cmdname;
		cmdArg.getCmdUsage()->setCommandLineUsage(cmdArg.getCmdName() + " --options [<input-args...>] [(retrun-value...)], [] means default.");
		pCmd->helpInformation(cmdArg.getCmdUsage());
		cmdArg.getCmdUsage()->write(std::cout, helpType);
		SAFE_DELETE_ARRAY(argv);
		return true;
	}

	SignalTrigger::disconnect(pCmd->_subCommands);
	s_promises.clear();
	s_retValue.clear();
	pCmd->parseCmdArg(cmdArg, s_retValue);
	cmdArg.reportRemainingOptionsAsUnrecognized();
	if (cmdArg.errors())
	{
		SignalTrigger::disconnect(pCmd->_subCommands);
		cmdArg.writeErrorMessages(std::cout);
		SAFE_DELETE_ARRAY(argv);
		return false;
	}

	_curCmd = pCmd;
	_cmdName = cmdname;
	s_blockWhenWaitReturnValue = true;
	SAFE_DELETE_ARRAY(argv);
	_block[1].release();
	return true;
}

Cmd* CmdManager::findCmd(const char* cmd)
{
	string strcmd = strToUpper(cmd);
	auto it = _commands.find(strcmd);
	if (it == _commands.end())
		return nullptr;
	return it->second.get();
}

InputAdapter* CmdManager::getInputAdapter() const
{
	return g_inputAdapter;
}

bool CmdManager::setReturnValue(const string& key, const Any& retval)
{
	if (key != ZOO_ERROR_MESSAGE && s_retValue.getData(key).has_value())
	{
		lazyInitPromise(key);
		if (!s_promises[key].first)
		{
			s_promises[key].first = true;
			s_promises[key].second->set_value(retval);
			s_retValue.setData(key, retval);
			return true;
		}
	}

	return false;
}

Any CmdManager::getReturnValue(const string& key)
{
	if (key != ZOO_ERROR_MESSAGE && s_retValue.getData(key).has_value())
	{
		if (s_blockWhenWaitReturnValue)
		{
			lazyInitPromise(key);
			std::future<Any> fut = s_promises[key].second->get_future();

			// 如果渲染线程和执行当前成员函数的线程是同一个线程, 则无需阻塞命令线程.
			if (g_renderThreadID == std::this_thread::get_id())
				g_interlock.canExchange = true;

			Any temp = fut.get();
			if (temp.has_value())
				return temp;
		}

		return s_retValue.getData(key);
	}

	return Any();
}

bool CmdManager::setErrorMessage(const string& errMessage)
{
	lazyInitPromise(ZOO_ERROR_MESSAGE);
	if (!s_promises[ZOO_ERROR_MESSAGE].first)
	{
		s_promises[ZOO_ERROR_MESSAGE].first = true;
		s_promises[ZOO_ERROR_MESSAGE].second->set_value(errMessage);
		s_retValue.setData(errMessage);
		return true;
	}

	return false;
}

string CmdManager::getErrorMessage()
{
	if (s_blockWhenWaitReturnValue)
	{
		lazyInitPromise(ZOO_ERROR_MESSAGE);
		std::future<Any> fut = s_promises[ZOO_ERROR_MESSAGE].second->get_future();

		// 如果渲染线程和执行当前成员函数的线程是同一个线程, 则无需阻塞命令线程.
		if (g_renderThreadID == std::this_thread::get_id())
			g_interlock.canExchange = true;

		Any temp = fut.get();
		if (temp.has_value())
			return any_cast<string>(temp);
	}

	if (s_retValue.getData().has_value())
		return any_cast<string>(s_retValue.getData());

	return "";
}

void CmdManager::cancelRetValueBlock()
{
	s_blockWhenWaitReturnValue = false;

	string errTip = "";
	if (s_retValue.getData().has_value())
		errTip = any_cast<string>(s_retValue.getData());

	setErrorMessage(errTip);

	for (auto it = s_promises.begin(); it != s_promises.end(); ++it)
		setReturnValue(it->first, s_retValue.getData(it->first));
}

void CmdManager::lazyInitPromise(const string& key)
{
	std::lock_guard<std::mutex> lock(s_mutex);
	if (s_promises.find(key) == s_promises.end())
	{
		shared_ptr<std::promise<Any>> ptr(new std::promise<Any>());
		s_promises[key] = std::make_pair(false, ptr);
	}
}

void CmdManager::run()
{
	_running = true;
	do
	{
		if (!_busying && _curCmd)
		{
			_busying = true;
			g_interlock.beginExchanging();
			SignalTrigger::trigger(_curCmd->_subCommands);
			g_interlock.endExchanging();
			_curCmd->_subCommands.userData().clear();
			cancelRetValueBlock();
		}
		else
		{
			_running = !g_inputAdapter->isDone();
			if (_running)
			{
				_block[1].reset();
				_busying = false;
				_block[1].block();
			}
		}

	} while (_running);
	_busying = false;
}

}
