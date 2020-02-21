#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Cmd.h>

namespace osgCmd {

CmdManager::CmdManager()
	: _renderer(new Renderer())
	, _curCmd(nullptr)
	, _busying(false)
{
	_renderer->addEventHandler(new osgGA::StateSetManipulator(_renderer->getCamera()->getOrCreateStateSet()));
	_rootNode = new osg::Group();
	_renderer->setSceneData(_rootNode);
	_renderer->realize();
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

bool CmdManager::sendCmd(const vector<string>& arglist)
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

	int argc = arglist.size();
	if (argc <= 0)
	{
		OSG_FATAL << "Null command!" << std::endl;
		return false;
	}

	auto it = _commands.find(strToUpper(arglist[0]));
	if (it == _commands.end())
	{
		OSG_FATAL << "Illegal command!" << std::endl;
		return false;
	}

	char** argv = new char*[argc];
	for (int i = 0; i < argc; ++i)
		argv[i] = const_cast<char*>(arglist[i].c_str());

	osg::ArgumentParser cmdArg(&argc, argv);
	Cmd* pCmd = it->second.get();
	SignalTrigger::disconnect(pCmd->_subCmds);
	if (!pCmd->parseCmdArg(cmdArg))
	{
		SAFE_DELETE_ARRAY(argv);
		SignalTrigger::disconnect(pCmd->_subCmds);
		OSG_FATAL << "Unresolved command!" << std::endl;
		return false;
	}

	SAFE_DELETE_ARRAY(argv);
	_curCmd = pCmd;
	_cmdBlock.release();
	return true;
}

osg::Group* CmdManager::getRootNode() const
{
	return _rootNode.get();
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

void CmdManager::initBuiltInCmd()
{
	const char* pszBuiltInCmd[] = {
		"Exit",
		"null"
	};

	string strCmd;
	unsigned int idx = 0;
	while (0 != strcmp(pszBuiltInCmd[idx], "null"))
	{
		strCmd = pszBuiltInCmd[idx++];
		addCmd(strCmd, ReflexFactory<>::getInstance().create<Cmd>("osgCmd::" + strCmd + "Cmd"));
	}

	start();
}

}
