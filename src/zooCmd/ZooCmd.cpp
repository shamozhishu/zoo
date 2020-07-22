#include <zooCmd/ZooCmd.h>
#include <zooCmd/CmdManager.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd/Cmd.h>
#include <zoo/Reflection.h>
#include <zoo/Interlock.h>
#include <zoo/DynLib.h>
#include <zoo/Utils.h>
#include <zoo/Timer.h>

using namespace zooCmd;
namespace zooCmd
{
	extern InputAdapter* g_inputAdapter;
	extern std::thread::id g_renderThreadID;
}

typedef const char* (*DllGetTypeName)(void);

static bool s_isInited = false;
static string s_inputAdapter = "";

static bool s_AdaRegister(const char* input_adapter)
{
	if (g_inputAdapter)
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load(string(input_adapter) + "d");
#else
	lib = CmdManager::getSingleton().load(string(input_adapter));
#endif
	if (!lib)
		return false;

	DllGetTypeName getTypeName = (DllGetTypeName)lib->getSymbol("dllGetTypeName");
	if (!getTypeName)
	{
		zoo_error("%s: Input adapter is not registered!", input_adapter);
		return false;
	}

	g_inputAdapter = ReflexFactory<>::getInstance().create<InputAdapter>(getTypeName());
	if (!g_inputAdapter)
	{
		zoo_error("Input adapter %s does not exist!", getTypeName());
		return false;
	}

	return true;
}

bool zooCmd_InitA(const char* input_adapter, const char* datadir, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/)
{
	if (input_adapter == nullptr || 0 == strcmp(input_adapter, "") || datadir == nullptr || 0 == strcmp(datadir, ""))
		return false;

	DATA_ROOT_DIR_ANSI = datadir;
	unsigned int lastIdx = DATA_ROOT_DIR_ANSI.size() - 1;
	if (DATA_ROOT_DIR_ANSI[lastIdx] != '/' && DATA_ROOT_DIR_ANSI[lastIdx] != '\\')
		DATA_ROOT_DIR_ANSI += "/";

	DATA_ROOT_DIR_UTF8 = ansiToUtf8(DATA_ROOT_DIR_ANSI);

	CmdManager* pCmdMgr = new CmdManager();

	if (!s_AdaRegister(input_adapter))
	{
		zooCmd_Destroy();
		return false;
	}

	g_inputAdapter->setup(windowWidth, windowHeight, windowScale);
	pCmdMgr->initBuiltinCmd();
	s_inputAdapter = input_adapter;
	pCmdMgr->start();
	s_isInited = true;
	return true;
}

bool zooCmd_InitW(const char* input_adapter, const wchar_t* datadir, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/)
{
	if (datadir == nullptr || 0 == wcscmp(datadir, L""))
		return false;
	return zooCmd_InitA(input_adapter, w2a_(datadir).c_str(), windowWidth, windowHeight, windowScale);
}

bool zooCmd_IsInited()
{
	return s_isInited;
}

bool zooCmd_Register(const char* cmd)
{
	if (!s_isInited)
		return false;

	if (CmdManager::getSingleton().findCmd(cmd))
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load(string(s_inputAdapter) + "-" + string(cmd) + "d");
#else
	lib = CmdManager::getSingleton().load(string(s_inputAdapter) + "-" + string(cmd));
#endif
	if (!lib)
		return false;

	DllGetTypeName getTypeName = (DllGetTypeName)lib->getSymbol("dllGetTypeName");
	if (!getTypeName)
	{
		zoo_error("%s: Plugin is not registered!", cmd);
		return false;
	}

	return CmdManager::getSingleton().addCmd(cmd, ReflexFactory<>::getInstance().create<Cmd>(getTypeName()));
}

bool zooCmd_Unregister(const char* cmd)
{
	if (!s_isInited)
		return false;

	if (!CmdManager::getSingleton().findCmd(cmd))
		return false;

	CmdManager::getSingleton().removeCmd(cmd);

#ifdef _DEBUG
	CmdManager::getSingleton().unload(string(s_inputAdapter) + "-" + string(cmd) + "d");
#else
	CmdManager::getSingleton().unload(string(s_inputAdapter) + "-" + string(cmd));
#endif
	return true;
}

bool zooCmd_Send(const char* cmdline)
{
	if (!s_isInited)
		return false;
	return CmdManager::getSingleton().sendCmd(cmdline);
}

int zooCmd_Run()
{
	if (!s_isInited)
		return -1;
	g_renderThreadID = std::this_thread::get_id();
	return g_inputAdapter->run();
}

void zooCmd_Tick()
{
	if (s_isInited)
		CmdManager::getSingleton().refresh();
}

void zooCmd_Destroy()
{
	s_isInited = false;
	s_inputAdapter = "";
	g_renderThreadID = std::thread::id();
	delete CmdManager::getSingletonPtr();
	InputAdapter::clearKeyboardMap();
}

void zooCmd_Render()
{
	if (s_isInited)
	{
		if (g_renderThreadID == std::thread::id())
			g_renderThreadID = std::this_thread::get_id();

		static zoo::Timer timer;
		g_inputAdapter->frame(timer.elapsed());
		timer.reset();
	}
}

void zooCmd_Resize(int windowWidth, int windowHeight, float windowScale)
{
	if (s_isInited)
		g_inputAdapter->resize(windowWidth, windowHeight, windowScale);
}

void zooCmd_KeyPress(int key, unsigned int modkey)
{
	if (s_isInited)
		g_inputAdapter->keyPress(key, modkey);
}

void zooCmd_KeyRelease(int key, unsigned int modkey)
{
	if (s_isInited)
		g_inputAdapter->keyRelease(key, modkey);
}

void zooCmd_MousePress(int x, int y, unsigned int modkey, zooCmd_MouseButton button)
{
	if (s_isInited)
		g_inputAdapter->mousePress(x, y, modkey, button);
}

void zooCmd_MouseRelease(int x, int y, unsigned int modkey, zooCmd_MouseButton button)
{
	if (s_isInited)
		g_inputAdapter->mouseRelease(x, y, modkey, button);
}

void zooCmd_MouseDoubleClick(int x, int y, unsigned int modkey, zooCmd_MouseButton button)
{
	if (s_isInited)
		g_inputAdapter->mouseDoubleClick(x, y, modkey, button);
}

void zooCmd_MouseMove(int x, int y, unsigned int modkey)
{
	if (s_isInited)
		g_inputAdapter->mouseMove(x, y, modkey);
}

void zooCmd_Wheel(int x, int y, unsigned int modkey, zooCmd_Scroll scroll)
{
	if (s_isInited)
		g_inputAdapter->wheel(x, y, modkey, scroll);
}

bool zooCmd_BoolValue(const char* variable, bool* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<bool>(val);
		return true;
	}

	return false;
}

bool zooCmd_IntValue(const char* variable, int* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<int>(val);
		return true;
	}

	return false;
}

bool zooCmd_FloatValue(const char* variable, float* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<float>(val);
		return true;
	}

	return false;
}

bool zooCmd_DoubleValue(const char* variable, double* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<double>(val);
		return true;
	}

	return false;
}

const char* zooCmd_StringValue(const char* variable)
{
	static std::string s_returnValue;
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		s_returnValue = any_cast<string>(val);
		return s_returnValue.c_str();
	}

	return "";
}

const char* zooCmd_ErrorMessage()
{
	static std::string s_errorMessage;
	s_errorMessage = CmdManager::getErrorMessage().c_str();
	return s_errorMessage.c_str();
}

void zooCmd_RemapKeyboard(zooCmd_Key key, int remapkey)
{
	InputAdapter::remapKeyboard(key, remapkey);
}
