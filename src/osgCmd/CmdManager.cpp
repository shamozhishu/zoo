#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>
#include <osgCmd/Utils.h>
#include <osgCmd/BuiltinCmd.h>

namespace osgCmd {

static BuiltinCmd* s_builtinCmd = nullptr;
CmdManager::CmdManager()
	: _renderer(new Renderer())
	, _curCmd(nullptr)
	, _busying(false)
{
}

CmdManager::~CmdManager()
{
	_renderer->setDone(true);
	_cmdBlock.release();
	_commands.clear();
	delete _renderer;
	while (isRunning())
		OpenThreads::Thread::YieldCurrentThread();
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

void CmdManager::run()
{
	bool running = true;
	do
	{
		if (_curCmd)
		{
			_busying = true;
			SignalTrigger::emit(_curCmd->_subCmds);
			_curCmd->_subCmds.userData().clear();
			_curCmd = nullptr;
		}
		else
		{
			running = !testCancel() && !_renderer->done();
			if (running)
			{
				_cmdBlock.reset();
				_busying = false;
				_cmdBlock.block();
			}
		}

	} while (running);
	_busying = false;
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
	do
	{
		auto it = _commands.find(strToUpper(arglist[0]));
		if (it != _commands.end())
		{
			pCmd = it->second.get();
			break;
		}

		argc += 1;
		arglist.insert(arglist.begin(), "__BUILTIN__");
		pCmd = s_builtinCmd;

	} while (0);

	if (argc <= 1 || arglist[1][0] != '-')
	{
		OSG_FATAL << "Invalid command entered!" << std::endl;
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
		cmdArg.getApplicationUsage()->setCommandLineUsage(cmdArg.getApplicationName() + " --options [args...]");
		pCmd->helpInformation(cmdArg.getApplicationUsage());
		cmdArg.getApplicationUsage()->write(std::cout, helpType);
		return true;
	}

	SignalTrigger::disconnect(pCmd->_subCmds);
	pCmd->parseCmdArg(cmdArg);
	cmdArg.reportRemainingOptionsAsUnrecognized();
	if (cmdArg.errors())
	{
		SignalTrigger::disconnect(pCmd->_subCmds);
		cmdArg.writeErrorMessages(std::cout);
		SAFE_DELETE_ARRAY(argv);
		return false;
	}

	SAFE_DELETE_ARRAY(argv);
	_curCmd = pCmd;
	_cmdBlock.release();
	return true;
}

Renderer* CmdManager::getRenderer() const
{
	return _renderer;
}

Cmd* CmdManager::findCmd(const char* cmd)
{
	string strcmd = strToUpper(cmd);
	auto it = _commands.find(strcmd);
	if (it == _commands.end())
		return nullptr;
	return it->second.get();
}

}
