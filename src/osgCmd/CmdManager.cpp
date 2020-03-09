#include <osgCmd/CmdManager.h>
#include <osgCmd/Viewers.h>
#include <osgCmd/Utils.h>
#include <osgCmd/BuiltinCmd.h>
#include <osgCmd/Interlock.h>

namespace osgCmd {

Interlock g_interlock;

static BuiltinCmd* s_builtinCmd = nullptr;
CmdManager::CmdManager()
	: _curCmd(nullptr)
	, _busying(false)
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	ds->setNvOptimusEnablement(1);
	ds->setStereo(false);
	_viewers = new Viewers();
}

CmdManager::~CmdManager()
{
	_viewers->setDone(true);
	_block[0].release();
	_block[1].release();
	_commands.clear();
	delete _viewers;
	while (isRunning())
		OpenThreads::Thread::YieldCurrentThread();
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
			SignalTrigger::emit(_curCmd->_subCommands);
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
	cancelRetValueBlock();
	_busying = false;
}

void CmdManager::block(bool isBlock)
{
	if (isBlock)
	{
		g_interlock.endExchanging();
		_block[0].reset();
		_block[0].block();
		g_interlock.beginExchanging();
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
		cmdArg.getApplicationUsage()->setCommandLineUsage(cmdArg.getApplicationName() + " --options <input-args...> || (retrun-value...)");
		pCmd->helpInformation(cmdArg.getApplicationUsage());
		cmdArg.getApplicationUsage()->write(std::cout, helpType);
		SAFE_DELETE_ARRAY(argv);
		return true;
	}

	SignalTrigger::disconnect(pCmd->_subCommands);
	pCmd->parseCmdArg(cmdArg);
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
	_retValue.clear();
	_block[1].release();
	SAFE_DELETE_ARRAY(argv);
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
	bool hasVal;
	if (key == "____OSGCMD____")
		hasVal = _retValue.getData().has_value();
	else
		hasVal = _retValue.getData(key).has_value();

	if (!hasVal)
	{
		_retValue.setData(key, retval);
		lazyInitPromise(key);
		_promises[key].get()->set_value(retval);
		return true;
	}

	return false;
}

Any CmdManager::getReturnValue(const string& key)
{
	lazyInitPromise(key);
	std::future<Any> fut = _promises[key].get()->get_future();
	return fut.get();
}

bool CmdManager::setErrorMessage(const string& errMessage)
{
	if (!_retValue.getData().has_value())
	{
		_retValue.setData(errMessage);
		lazyInitPromise("____OSGCMD____");
		_promises["____OSGCMD____"].get()->set_value(errMessage);
		return true;
	}

	return false;
}

string CmdManager::getErrorMessage()
{
	lazyInitPromise("____OSGCMD____");
	std::future<Any> fut = _promises["____OSGCMD____"].get()->get_future();
	Any temp = fut.get();
	if (temp.has_value())
		return any_cast<string>(temp);
	return "";
}

void CmdManager::lazyInitPromise(const string& key)
{
	OpenThreads::ScopedWriteLock lock(_rwMutex);
	if (_promises.find(key) == _promises.end())
	{
		shared_ptr<std::promise<Any>> ptr(new std::promise<Any>());
		_promises[key] = ptr;
	}
}

void CmdManager::cancelRetValueBlock()
{
	for (auto it = _promises.begin(); it != _promises.end(); ++it)
		setReturnValue(it->first, Any());
	_promises.clear();
	_retValue.clear();
}

}
