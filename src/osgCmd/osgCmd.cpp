#include <osgCmd.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Reflection.h>
#include <osgCmd/Renderer.h>
#include <osgCmd/DynLib.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Cmd.h>

using namespace osgCmd;

typedef const char* (*DllGetCmdTypeName)(void);

void osgCmd_Init(const char* workdir /*= nullptr*/)
{
	if (workdir == nullptr || 0 == strcmp(workdir, ""))
		workdir = ".\\";

	setWorkDir(workdir);
	new CmdManager();
	CmdManager::getSingleton().initBuiltInCmd();
}

bool osgCmd_Send(const char* cmdline, bool lazyLoad /*= false*/)
{
	vector<string> arglist;
	stringtok(arglist, cmdline);
	if (arglist.size() == 0)
	{
		OSG_FATAL << "Null command!" << std::endl;
		return false;
	}

	if (lazyLoad)
	{
		if (!osgCmd_Register(arglist[0].c_str()))
			return false;
	}

	return CmdManager::getSingleton().sendCmd(arglist);
}

bool osgCmd_Register(const char* cmd)
{
	if (CmdManager::getSingleton().findCmd<Cmd>(cmd))
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load("osgCmd-" + string(cmd) + "d");
#else
	lib = CmdManager::getSingleton().load("osgCmd-" + string(cmd));
#endif
	if (!lib)
		return false;

	DllGetCmdTypeName getCmdTypeName = (DllGetCmdTypeName)lib->getSymbol("dllGetCmdTypeName");
	if (!getCmdTypeName)
	{
		OSG_FATAL << cmd << ": Plugin not registered!" << std::endl;
		return false;
	}

	return CmdManager::getSingleton().addCmd(cmd, ReflexFactory<>::getInstance().create<Cmd>(getCmdTypeName()));
}

void osgCmd_Unregister(const char* cmd, bool unloadPlugin /*= true*/)
{
	CmdManager::getSingleton().removeCmd(cmd);
	if (unloadPlugin)
	{
#ifdef _DEBUG
		CmdManager::getSingleton().unload("osgCmd-" + string(cmd) + "d");
#else
		CmdManager::getSingleton().unload("osgCmd-" + string(cmd));
#endif
	}
}

void osgCmd_Run()
{
	CmdManager::getSingleton().getRenderer()->setKeyEventSetsDone(0);
	CmdManager::getSingleton().getRenderer()->run();
}

void osgCmd_Destroy()
{
	delete CmdManager::getSingletonPtr();
}

void osgCmd_Render()
{
	CmdManager::getSingleton().getRenderer()->frame();
}

void osgCmd_Resize(int windowWidth, int windowHeight, float windowScale)
{
	CmdManager::getSingleton().getRenderer()->resize(windowWidth, windowHeight, windowScale);
}

void osgCmd_KeyPressEvent(osgCmd_Key key, unsigned int modkey)
{
	CmdManager::getSingleton().getRenderer()->keyPressEvent((Key)key, modkey);
}

void osgCmd_KeyReleaseEvent(osgCmd_Key key, unsigned int modkey)
{
	CmdManager::getSingleton().getRenderer()->keyReleaseEvent((Key)key, modkey);
}

void osgCmd_MousePressEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button)
{
	CmdManager::getSingleton().getRenderer()->mousePressEvent(x, y, modkey, (MouseButton)button);
}

void osgCmd_MouseReleaseEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button)
{
	CmdManager::getSingleton().getRenderer()->mouseReleaseEvent(x, y, modkey, (MouseButton)button);
}

void osgCmd_MouseDoubleClickEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button)
{
	CmdManager::getSingleton().getRenderer()->mouseDoubleClickEvent(x, y, modkey, (MouseButton)button);
}

void osgCmd_MouseMoveEvent(int x, int y, unsigned int modkey)
{
	CmdManager::getSingleton().getRenderer()->mouseMoveEvent(x, y, modkey);
}

void osgCmd_WheelEvent(int x, int y, unsigned int modkey, osgCmd_Scroll scroll)
{
	CmdManager::getSingleton().getRenderer()->wheelEvent(x, y, modkey, (Scroll)scroll);
}
