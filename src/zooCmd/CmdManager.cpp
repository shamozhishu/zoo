#include <zooCmd/CmdManager.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd/BuiltinCmd.h>
#include <zooCmd/CmdParser.h>
#include <zoo/Reflection.h>
#include <zoo/Interlock.h>
#include <zoo/Utils.h>

#define ZOO_ERROR_MESSAGE "ZOO_ERROR_MESSAGE"

namespace zooCmd {

std::thread::id g_renderThreadID;
InputAdapter* g_inputAdapter = nullptr;

static UserData s_retValue;
static std::mutex s_mutex;
static bool s_blockWhenWaitReturnValue = true;
static map<string, std::pair<bool, shared_ptr<promise<Any>>>> s_promises;
static BuiltinCmd* s_builtinCmd = nullptr;
static Interlock s_interlock;

CmdManager::CmdManager()
	: _running(false)
	, _lastCmd(nullptr)
	, _cmdThread(nullptr)
{
	s_blockWhenWaitReturnValue = true;
}

CmdManager::~CmdManager()
{
	if (g_inputAdapter)
		g_inputAdapter->setDone(true);

	s_interlock.canExchange = true;
	_block[0].release();
	_block[1].release();

	if (_cmdThread)
		_cmdThread->join();

	delete _cmdThread;
	_cmdThread = nullptr;
	_commands.clear();
	delete g_inputAdapter;
	g_inputAdapter = nullptr;
	s_promises.clear();
	s_retValue.clear();
	s_blockWhenWaitReturnValue = true;
}

void CmdManager::start()
{
	if (!_cmdThread)
		_cmdThread = new thread(&CmdManager::runCmd, this);
}

void CmdManager::update()
{
	auto it = _commands.begin();
	auto itEnd = _commands.end();
	for (; it != itEnd; ++it)
	{
		it->second->update();
	}
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
		if (_cmdThread->get_id() == std::this_thread::get_id())
		{
			s_interlock.endExchanging();
			cancelRetValueBlock();
			_block[0].reset();
			_block[0].block();
			s_interlock.beginExchanging();
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

bool CmdManager::sendCmd(const string& cmdline)
{
	if (!_running)
	{
		zoo_warning("Command manager not started!");
		return false;
	}

	CmdParser cmdArg;
	if (!cmdArg.parseToken(cmdline))
	{
		zoo_warning("Please enter valid command parameters!");
		return false;
	}

	Cmd* pCmd = nullptr;
	string cmdname = cmdArg.getCmdName();
	do 
	{
		if (cmdname == "__BUILTIN__")
		{
			pCmd = s_builtinCmd;
			break;
		}

		if (cmdname == _lastCmdName)
		{
			pCmd = _lastCmd;
			break;
		}

		auto it = _commands.find(cmdname);
		if (it != _commands.end())
		{
			pCmd = it->second.get();
			break;
		}

	} while (0);

	if (!pCmd)
	{
		zoo_warning("Non-existent command!");
		return false;
	}

	if (_lastCmd != pCmd)
	{
		cmdArg.getCmdUsage()->setCommandProcedureCalls(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setCommandProcedureCallsDefaults(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setCmdProcedureUsage(cmdArg.getCmdName() + ".Procedure(type1 arg1, type2 arg2, ...); // return_value_type& return_value_variable_name, ...");
		pCmd->help(cmdArg.getCmdUsage());
	}

	if ((cmdArg.readHelpType()))
	{
		_lastCmd = pCmd;
		_lastCmdName = cmdname;
		cmdArg.getCmdUsage()->write(std::cout);
		return true;
	}

	s_promises.clear();
	s_retValue.clear();
	std::shared_ptr<Signal> subCmd((new Signal));
	pCmd->parse(*subCmd, cmdArg, s_retValue);
	cmdArg.reportRemainingCallsAsUnrecognized();
	if (cmdArg.errors())
	{
		cmdArg.writeErrorMessages(std::cout);
		return false;
	}

	_lastCmd = pCmd;
	_lastCmdName = cmdname;
	_cmdQueue.push(subCmd);
	s_blockWhenWaitReturnValue = true;
	_block[1].release();
	return true;
}

Cmd* CmdManager::findCmd(const string& cmdname)
{
	auto it = _commands.find(strToUpper(cmdname));
	if (it == _commands.end())
		return nullptr;
	return it->second.get();
}

InputAdapter* CmdManager::getInputAdapter() const
{
	return g_inputAdapter;
}

void CmdManager::waitExchanged()
{
	s_interlock.waitExchanged();
}

void CmdManager::releaseWait()
{
	s_interlock.releaseWait();
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
				s_interlock.canExchange = true;

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
			s_interlock.canExchange = true;

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

void CmdManager::runCmd()
{
	_running = true;
	do
	{
		while (!_cmdQueue.empty())
		{
			shared_ptr<Signal> subcmd = _cmdQueue.front();
			_cmdQueue.pop();
			s_interlock.beginExchanging();
			SignalTrigger::trigger(*subcmd);
			s_interlock.endExchanging();
			cancelRetValueBlock();
		}

		_running = !g_inputAdapter->isDone();
		if (_running)
		{
			_block[1].reset();
			_block[1].block();
		}

	} while (_running);
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

}
