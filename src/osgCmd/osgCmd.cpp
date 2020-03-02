#include <osgCmd.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Reflection.h>
#include <osgCmd/Renderer.h>
#include <osgCmd/DynLib.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Cmd.h>

using namespace osgCmd;

typedef const char* (*DllGetCmdTypeName)(void);

static bool s_CmdRegister(const char* cmd)
{
	if (CmdManager::getSingleton().findCmd(cmd))
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

void osgCmd_Init(int cmdcount, char* cmdset[], const char* workdir /*= nullptr*/, int wndWidth /*= 0*/, int wndHeight /*= 0*/, float wndScale /*= 1.0f*/)
{
	if (workdir != nullptr && 0 != strcmp(workdir, ""))
		setWorkDir(workdir);

	CmdManager* pCmdMgr = new CmdManager();
	pCmdMgr->getRenderer()->setupOSG(wndWidth, wndHeight, wndScale);
	
	if (cmdset)
	{
		for (int i = 0; i < cmdcount; ++i)
			s_CmdRegister(cmdset[i]);
	}

	pCmdMgr->initBuiltinCmd();
	pCmdMgr->start();
}

bool osgCmd_Send(const char* cmdline)
{
	return CmdManager::getSingleton().sendCmd(cmdline);
}

void osgCmd_Run()
{
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

bool osgCmd_BoolValue(const char* variable, bool* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<bool>(val);
		return true;
	}

	return false;
}

bool osgCmd_IntValue(const char* variable, int* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<int>(val);
		return true;
	}

	return false;
}

bool osgCmd_FloatValue(const char* variable, float* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<float>(val);
		return true;
	}

	return false;
}

bool osgCmd_DoubleValue(const char* variable, double* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<double>(val);
		return true;
	}

	return false;
}

const char* osgCmd_StringValue(const char* variable)
{
	static std::string s_returnValue;
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		s_returnValue = any_cast<string>(val);
		return s_returnValue.c_str();
	}

	return "";
}

const char* osgCmd_ErrorMessage()
{
	static std::string s_errorMessage;
	s_errorMessage = CmdManager::getSingleton().getErrorMessage().c_str();
	return s_errorMessage.c_str();
}
