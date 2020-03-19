#include <osgCmd/CmdManager.h>
#include <osgCmd/Viewers.h>
#include <osgCmd/Utils.h>
#include <osgCmd/BuiltinCmd.h>
#include <osgCmd/Interlock.h>
#ifdef _WIN32
#include "Windows.h"
#define GET_CURRENT_THREAD_ID() GetCurrentThreadId()
#else
#include "unistd.h"
#define GET_CURRENT_THREAD_ID() gettid()
#endif
#define OSGCMD_ERROR_MESSAGE "OSGCMD_ERROR_MESSAGE"

namespace osgCmd {

Interlock g_interlock;
unsigned long g_renderThreadID = -1;
static UserData s_retValue;
static OpenThreads::ReadWriteMutex s_rwMutex;
static bool s_blockWhenWaitReturnValue = true;
static map<string, std::pair<bool, shared_ptr<promise<Any>>>> s_promises;

static BuiltinCmd* s_builtinCmd = nullptr;
CmdManager::CmdManager()
	: _curCmd(nullptr)
	, _busying(false)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	ds->setNvOptimusEnablement(1);
	ds->setStereo(false);
	_viewers = new Viewers();
	s_blockWhenWaitReturnValue = true;
}

CmdManager::~CmdManager()
{
	_viewers->setDone(true);
	g_interlock.canExchange = true;
	_block[0].release();
	if (!_busying)
		_curCmd = nullptr;
	_block[1].release();
	while (isRunning())
		OpenThreads::Thread::YieldCurrentThread();
	_commands.clear();
	delete _viewers;
	s_promises.clear();
	s_retValue.clear();
	s_blockWhenWaitReturnValue = true;
}

void CmdManager::run()
{
	bool running = true;
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
			running = !testCancel() && !_viewers->done();
			if (running)
			{
				_block[1].reset();
				_busying = false;
				_block[1].block();
			}
		}

	} while (running);
	_busying = false;
}

void CmdManager::block(bool isBlock)
{
	if (isBlock)
	{
		if (getThreadId() == (int)GET_CURRENT_THREAD_ID())
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

void CmdManager::initBuiltinCmd()
{
	s_builtinCmd = ReflexFactory<>::getInstance().create<BuiltinCmd>("osgCmd::BuiltinCmd");
	addCmd("__BUILTIN__", s_builtinCmd);
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
	if (!isRunning())
	{
		OSG_FATAL << "Command manager not started!" << std::endl;
		return false;
	}

	if (_busying)
	{
		OSG_FATAL << "Last command not completed!" << std::endl;
		return false;
	}

	vector<string> arglist;
	stringtok(arglist, cmdline);
	int argc = arglist.size();
	if (argc <= 0)
	{
		OSG_FATAL << "Please enter a command!" << std::endl;
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

		OSG_FATAL << "Non-existent command!" << std::endl;
		return false;

	} while (0);

	if (argc <= 1 || arglist[1][0] != '-')
	{
		OSG_FATAL << "Please enter valid command parameters!" << std::endl;
		return false;
	}

	char** argv = new char*[argc];
	for (int i = 0; i < argc; ++i)
		argv[i] = const_cast<char*>(arglist[i].c_str());

	osg::ArgumentParser cmdArg(&argc, argv);
	if (_curCmd != pCmd)
	{
		cmdArg.getApplicationUsage()->setCommandLineOptions(osg::ApplicationUsage::UsageMap());
		cmdArg.getApplicationUsage()->setCommandLineOptionsDefaults(osg::ApplicationUsage::UsageMap());
		cmdArg.getApplicationUsage()->setEnvironmentalVariables(osg::ApplicationUsage::UsageMap());
		cmdArg.getApplicationUsage()->setEnvironmentalVariablesDefaults(osg::ApplicationUsage::UsageMap());
		cmdArg.getApplicationUsage()->setKeyboardMouseBindings(osg::ApplicationUsage::UsageMap());
	}

	unsigned int helpType = 0;
	if ((helpType = cmdArg.readHelpType()))
	{
		_curCmd = pCmd;
		_cmdName = cmdname;
		cmdArg.getApplicationUsage()->setCommandLineUsage(cmdArg.getApplicationName() + " --options [<input-args...>] [(retrun-value...)], [] means default.");
		pCmd->helpInformation(cmdArg.getApplicationUsage());
		cmdArg.getApplicationUsage()->write(std::cout, helpType);
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

Viewers* CmdManager::getViewers() const
{
	return _viewers;
}

bool CmdManager::setReturnValue(const string& key, const Any& retval)
{
	if (key != OSGCMD_ERROR_MESSAGE && s_retValue.getData(key).has_value())
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
	if (key != OSGCMD_ERROR_MESSAGE && s_retValue.getData(key).has_value())
	{
		if (s_blockWhenWaitReturnValue)
		{
			lazyInitPromise(key);
			std::future<Any> fut = s_promises[key].second->get_future();

			// 如果渲染线程和执行当前成员函数的线程是同一个线程, 则无需阻塞命令线程.
			if (g_renderThreadID == GET_CURRENT_THREAD_ID())
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
	lazyInitPromise(OSGCMD_ERROR_MESSAGE);
	if (!s_promises[OSGCMD_ERROR_MESSAGE].first)
	{
		s_promises[OSGCMD_ERROR_MESSAGE].first = true;
		s_promises[OSGCMD_ERROR_MESSAGE].second->set_value(errMessage);
		s_retValue.setData(errMessage);
		return true;
	}

	return false;
}

string CmdManager::getErrorMessage()
{
	if (s_blockWhenWaitReturnValue)
	{
		lazyInitPromise(OSGCMD_ERROR_MESSAGE);
		std::future<Any> fut = s_promises[OSGCMD_ERROR_MESSAGE].second->get_future();

		// 如果渲染线程和执行当前成员函数的线程是同一个线程, 则无需阻塞命令线程.
		if (g_renderThreadID == GET_CURRENT_THREAD_ID())
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
	OpenThreads::ScopedWriteLock lock(s_rwMutex);
	if (s_promises.find(key) == s_promises.end())
	{
		shared_ptr<std::promise<Any>> ptr(new std::promise<Any>());
		s_promises[key] = std::make_pair(false, ptr);
	}
}

}
