#include <osgCmd/ExitCmd.h>
#include <osgCmd/Renderer.h>
#include <osgCmd/CmdManager.h>

namespace osgCmd {

REFLEX_IMPLEMENT(ExitCmd);

bool ExitCmd::parseCmdArg(osg::ArgumentParser& cmdarg)
{
	if (cmdarg.argc() > 1)
		return false;

	SignalTrigger::connect<ExitCmd>(_subCmds, this, &ExitCmd::endMainLoop);
	return true;
}

void ExitCmd::endMainLoop(const UserData& userdata)
{
	CmdManager::getSingleton().getRenderer()->setDone(true);
}

}
