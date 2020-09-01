#include <zooCmd/CmdManager.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd/CmdParser.h>
#include <zoo/Reflection.h>
#include <zoo/Interlock.h>
#include <zoo/Utils.h>

#define ZOO_TIP_MESSAGE "__ZOO_TIP_MESSAGE__"

namespace zooCmd {

InputAdapter* g_inputAdapter = nullptr;
std::thread::id g_renderThreadID = std::thread::id();

static std::mutex s_mutex;
static UserData s_retValue;
static Interlock s_interlock;
static Cmd* s_builtinCmd = nullptr;
static map<string, shared_ptr<promise<Any>>> s_promises;

struct BlockMutex
{
	void lockRetValue() {
		std::lock_guard<std::mutex> lock(_mut);
		_blockWhenWaitReturnValue = true;
	}

	void releaseRetValue() {
		std::lock_guard<std::mutex> lock(_mut);
		_blockWhenWaitReturnValue = false;
	}

	bool isBlockWhenWaitReturnValue() const {
		return _blockWhenWaitReturnValue;
	}

private:
	std::mutex _mut;
	bool _blockWhenWaitReturnValue;
};

static BlockMutex s_blockMutex;

CmdManager::CmdManager()
	: _running(false)
	, _isBlock(false)
	, _lastCmd(nullptr)
	, _cmdThread(nullptr)
{
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
}

void CmdManager::start()
{
	if (!_cmdThread)
		_cmdThread = new thread(&CmdManager::runCmd, this);
}

void CmdManager::refresh()
{
	auto it = _commands.begin();
	auto itEnd = _commands.end();
	for (; it != itEnd; ++it)
		it->second->refresh();
}

void CmdManager::block(bool isBlock)
{
	if (isBlock)
	{
		if (_cmdThread->get_id() == std::this_thread::get_id()) // 如果当前执行block()的线程是命令线程,则阻塞命令执行.
		{
			s_interlock.endExchanging();
			releaseBlockAndRetValue();
			_isBlock = true;
			_block[0].reset();
			_block[0].block();
			s_blockMutex.lockRetValue();
			s_interlock.beginExchanging();
		}
	}
	else
	{
		_block[0].release();
		_isBlock = false;
	}
}

void CmdManager::initBuiltinCmd()
{
	s_builtinCmd = ReflexFactory<>::getInstance().create<Cmd>(__BUILTIN__);
	addCmd(__BUILTIN__, s_builtinCmd);
}

bool CmdManager::addCmd(const string& cmd, Cmd* pCmd)
{
	if (!pCmd)
		return false;

	std::shared_ptr<Cmd> cmdPtr(pCmd);
	if (!cmdPtr->init())
		return false;

	_commands[strToLower(cmd)] = cmdPtr;
	return true;
}

void CmdManager::removeCmd(const string& cmd)
{
	auto it = _commands.find(strToLower(cmd));
	if (it != _commands.end())
		_commands.erase(it);
}

vector<string> CmdManager::removeAllCmds()
{
	vector<string> cmdset;
	auto it = _commands.begin();
	while (it != _commands.end())
	{
		if (it->first == __BUILTIN__)
		{
			it++;
			continue;
		}

		cmdset.push_back(it->first);
		it = _commands.erase(it);
	}

	return cmdset;
}

bool CmdManager::sendCmd(const string& cmdline)
{
	static string tips;

	if (!_running)
	{
		tips = "Command manager not started!";
		zoo_warning(tips.c_str());
		setTipMessage(tips);
		return false;
	}

	if (_isBlock)
	{
		tips = "Command thread has been block!";
		zoo_warning(tips.c_str());
		setTipMessage(tips);
		return false;
	}

	CmdParser cmdArg;
	if (!cmdArg.parseToken(cmdline))
	{
		tips = "Please enter valid command parameters!";
		zoo_warning(tips.c_str());
		setTipMessage(tips);
		return false;
	}

	Cmd* pCmd = nullptr;
	string cmdname = trim(cmdArg.getCmdName());
	do 
	{
		if (cmdname == __BUILTIN__)
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
		tips = "Non-existent command!";
		zoo_warning(tips.c_str());
		setTipMessage(tips);
		return false;
	}

	if (_lastCmd != pCmd)
	{
		_lastCmd = pCmd;
		_lastCmdName = cmdname;
		cmdArg.getCmdUsage()->setCommandProcedureCalls(CmdUsage::UsageVec());
		cmdArg.getCmdUsage()->setCommandProcedureCallsDefaults(CmdUsage::UsageMap());
		cmdArg.getCmdUsage()->setCmdProcedureUsage(cmdArg.getCmdName() + ".procedure(type1 arg1, type2 arg2, ...); // return_value_type& return_value_variable_name, ...");
		pCmd->help(cmdArg.getCmdUsage());
		cmdArg.getCmdUsage()->addCommandProcedureCall("h()", "Display all command procedure calls");
	}

	if ((cmdArg.readHelpType()))
	{
		std::stringstream ss;
		cmdArg.getCmdUsage()->write(ss);
		zoo_info(ss.str().c_str());
		setTipMessage(ss.str());
		return true;
	}

	s_retValue.clear();
	std::shared_ptr<Signal> subCmd((new Signal));
	pCmd->parse(*subCmd, cmdArg, s_retValue); // s_retValue: 在此处调用parse()用于给返回值设置默认值.
	cmdArg.reportRemainingCallsAsUnrecognized();
	if (cmdArg.errors())
	{
		std::stringstream ss;
		cmdArg.writeErrorMessages(ss);
		zoo_warning(ss.str().c_str());
		setTipMessage(ss.str());
		return false;
	}

	s_blockMutex.lockRetValue();
	_cmdQueue.push(subCmd);
	_block[1].release();
	return true;
}

Cmd* CmdManager::findCmd(const string& cmdname)
{
	auto it = _commands.find(strToLower(cmdname));
	if (it == _commands.end())
		return nullptr;
	return it->second.get();
}

void CmdManager::sendEvent(const string& topic, const UserData& props /*= nullptr*/)
{
	if (_commands.size() > 0)
	{
		Event evt(topic, props);
		auto it = _commands.begin();
		auto itEnd = _commands.end();
		for (; it != itEnd; ++it)
			it->second->handle(evt);
	}
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
	if (key != ZOO_TIP_MESSAGE && s_retValue.getData(key).has_value())
	{
		s_retValue.setData(key, retval);
		return true;
	}

	return false;
}

Any CmdManager::getReturnValue(const string& key)
{
	if (key != ZOO_TIP_MESSAGE && s_retValue.getData(key).has_value())
	{
		if (s_blockMutex.isBlockWhenWaitReturnValue())
		{
			lazyInitPromise(key);
			std::future<Any> fut = s_promises[key]->get_future();

			// 如果渲染线程和执行当前成员函数的线程是同一个线程, 则无需阻塞命令线程.
			if (g_renderThreadID == std::this_thread::get_id()
				|| g_renderThreadID == std::thread::id()) // 如果渲染线程未启动, 则无需阻塞命令线程.
				s_interlock.canExchange = true;

			Any temp = fut.get();
			if (temp.has_value())
				return temp;
		}

		return s_retValue.getData(key);
	}

	return Any();
}

void CmdManager::setTipMessage(const string& tips)
{
	s_retValue.setData(ZOO_TIP_MESSAGE, tips);
}

string CmdManager::getTipMessage()
{
	if (s_blockMutex.isBlockWhenWaitReturnValue())
	{
		lazyInitPromise(ZOO_TIP_MESSAGE);
		std::future<Any> fut = s_promises[ZOO_TIP_MESSAGE]->get_future();

		// 如果渲染线程和执行当前成员函数的线程是同一个线程, 则无需阻塞命令线程.
		if (g_renderThreadID == std::this_thread::get_id()
			|| g_renderThreadID == std::thread::id()) // 如果渲染线程未启动, 则无需阻塞命令线程.
			s_interlock.canExchange = true;

		Any temp = fut.get();
		if (temp.has_value())
			return temp.to<string>();
	}

	if (s_retValue.getData(ZOO_TIP_MESSAGE).has_value())
		return s_retValue.getData(ZOO_TIP_MESSAGE).to<string>();

	return "";
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
			releaseBlockAndRetValue();
		}

		_running = !g_inputAdapter->isDone();
		if (_running)
		{
			_block[1].reset();
			_block[1].block();
		}

	} while (_running);
}

void CmdManager::releaseBlockAndRetValue()
{
	s_blockMutex.releaseRetValue();
	for (auto it = s_promises.begin(); it != s_promises.end(); ++it)
		it->second->set_value(s_retValue.getData(it->first));
	s_promises.clear();
}

void CmdManager::lazyInitPromise(const string& key)
{
	std::lock_guard<std::mutex> lock(s_mutex);
	if (s_promises.find(key) == s_promises.end())
	{
		shared_ptr<std::promise<Any>> ptr(new std::promise<Any>());
		s_promises[key] = ptr;
	}
}

}
