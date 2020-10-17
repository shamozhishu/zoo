#include <zooCmd/ZooCmd.h>
#include <zooCmd/CmdManager.h>
#include <zooCmd/RenderAdapter.h>
#include <zooCmd/Cmd.h>
#include <zoo/Reflection.h>
#include <zoo/Interlock.h>
#include <zoo/DynLib.h>
#include <zoo/Utils.h>
#include <zoo/Timer.h>

using namespace zooCmd;
namespace zooCmd
{
	extern RenderAdapter* g_renderAdapter;
	extern std::thread::id g_renderThreadID;
}

typedef const char* (*DllGetTypeName)(void);

static bool s_isInited = false;
static string s_inputAdapter = "";

static bool s_RegisterRenderAdapter(const char* render_adapter)
{
	if (g_renderAdapter)
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load(string(render_adapter) + "d");
#else
	lib = CmdManager::getSingleton().load(string(render_adapter));
#endif
	if (!lib)
		return false;

	DllGetTypeName getTypeName = (DllGetTypeName)lib->getSymbol("dllGetTypeName");
	if (!getTypeName)
	{
		zoo_error("%s: Render adapter is not registered!", render_adapter);
		return false;
	}

	g_renderAdapter = ReflexFactory<>::getInstance().create<RenderAdapter>(getTypeName());
	if (!g_renderAdapter)
	{
		zoo_error("Render adapter %s does not exist!", getTypeName());
		return false;
	}

	return true;
}

bool zooCmd_InitA(const char* render_adapter, const char* datadir)
{
	if (render_adapter == nullptr || 0 == strcmp(render_adapter, "") || datadir == nullptr || 0 == strcmp(datadir, ""))
		return false;

	DATA_ROOT_DIR_ANSI = datadir;
	unsigned int lastIdx = DATA_ROOT_DIR_ANSI.size() - 1;
	if (DATA_ROOT_DIR_ANSI[lastIdx] != '/' && DATA_ROOT_DIR_ANSI[lastIdx] != '\\')
		DATA_ROOT_DIR_ANSI += "/";

	DATA_ROOT_DIR_UTF8 = ansiToUtf8(DATA_ROOT_DIR_ANSI);

	CmdManager* pCmdMgr = new CmdManager();

	if (!s_RegisterRenderAdapter(render_adapter))
	{
		zooCmd_Destroy();
		return false;
	}

	pCmdMgr->initBuiltinCmd();
	s_inputAdapter = render_adapter;
	pCmdMgr->start();
	s_isInited = true;
	return true;
}

bool zooCmd_InitW(const char* input_adapter, const wchar_t* datadir)
{
	if (datadir == nullptr || 0 == wcscmp(datadir, L""))
		return false;
	return zooCmd_InitA(input_adapter, w2a_(datadir).c_str());
}

bool zooCmd_IsInited()
{
	return s_isInited;
}

bool zooCmd_Register(const char* cmd)
{
	if (!s_isInited || strcmp(cmd, "") == 0)
		return false;

	if (CmdManager::getSingleton().findCmd(cmd))
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load(string(s_inputAdapter) + "-" + strToLower(cmd) + "d");
#else
	lib = CmdManager::getSingleton().load(string(s_inputAdapter) + "-" + strToLower(cmd));
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
	CmdManager::getSingleton().unload(string(s_inputAdapter) + "-" + strToLower(cmd) + "d");
#else
	CmdManager::getSingleton().unload(string(s_inputAdapter) + "-" + strToLower(cmd));
#endif
	return true;
}

void zooCmd_UnregisterAll()
{
	if (s_isInited)
	{
		vector<string> cmdset = CmdManager::getSingleton().removeAllCmds();
		auto len = cmdset.size();
		for (auto i = 0; i < len; ++i)
		{
#ifdef _DEBUG
			CmdManager::getSingleton().unload(string(s_inputAdapter) + "-" + cmdset[i] + "d");
#else
			CmdManager::getSingleton().unload(string(s_inputAdapter) + "-" + cmdset[i]);
#endif
		}
	}
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
	return g_renderAdapter->run();
}

void zooCmd_Refresh()
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
}

void zooCmd_Render()
{
	if (s_isInited)
	{
		if (g_renderThreadID == std::thread::id())
			g_renderThreadID = std::this_thread::get_id();

		static zoo::Timer timer;
		g_renderAdapter->frame(timer.elapsed());
		timer.reset();
	}
}

void* zooCmd_Setup(int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/, int windowID /*= 0*/)
{
	if (s_isInited)
		return g_renderAdapter->setup(windowWidth, windowHeight, windowScale, windowID);
	return nullptr;
}

bool zooCmd_BoolValue(const char* variable, bool* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = val.to<bool>();
		return true;
	}

	return false;
}

bool zooCmd_IntValue(const char* variable, int* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = val.to<int>();
		return true;
	}

	return false;
}

bool zooCmd_FloatValue(const char* variable, float* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = val.to<float>();
		return true;
	}

	return false;
}

bool zooCmd_DoubleValue(const char* variable, double* value)
{
	Any val = CmdManager::getReturnValue(variable);
	if (val.has_value())
	{
		*value = val.to<double>();
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
		s_returnValue = val.to<string>();
		return s_returnValue.c_str();
	}

	return "";
}

const char* zooCmd_TipMessage()
{
	static std::string s_tipMessage;
	s_tipMessage = CmdManager::getTipMessage().c_str();
	return s_tipMessage.c_str();
}
