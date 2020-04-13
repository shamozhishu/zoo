#include <zooCmd/zooCmd.h>
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
	std::string ansi_data_dir;
	std::string utf8_data_dir;
	extern InputAdapter* g_inputAdapter;
	extern std::thread::id g_renderThreadID;
}

typedef const char* (*DllGetTypeName)(void);

static bool s_isInited = false;

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

static bool s_CmdRegister(const char* cmd, const char* input_adapter)
{
	if (CmdManager::getSingleton().findCmd(cmd))
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load(string(input_adapter) + "\\" + string(input_adapter) + "-" + string(cmd) + "d");
#else
	lib = CmdManager::getSingleton().load(string(input_adapter) + "\\" + string(input_adapter) + "-" + string(cmd));
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

bool zooCmd_InitA(int cmdcount, const char* cmdset[], const char* input_adapter, const char* datadir /*= nullptr*/, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/)
{
	if (input_adapter == nullptr || strcmp(input_adapter, "") == 0)
		return false;

	if (datadir != nullptr && 0 != strcmp(datadir, ""))
	{
		ansi_data_dir = datadir;
		unsigned int lastIdx = ansi_data_dir.size() - 1;
		if (ansi_data_dir[lastIdx] != '/' && ansi_data_dir[lastIdx] != '\\')
			ansi_data_dir += "/";
	}

	if (ansi_data_dir != "")
		utf8_data_dir = ansiToUtf8(ansi_data_dir);

	CmdManager* pCmdMgr = new CmdManager();

	if (!s_AdaRegister(input_adapter))
	{
		zooCmd_Destroy();
		return false;
	}

	g_inputAdapter->setup(windowWidth, windowHeight, windowScale);

	if (cmdset)
	{
		string strCmdset;
		for (int i = 0; i < cmdcount; ++i)
		{
			if (!s_CmdRegister(cmdset[i], input_adapter))
			{
				if (strCmdset == "")
					strCmdset = cmdset[i];
				else
					strCmdset += (string(", ") + cmdset[i]);
			}
		}

		if (strCmdset != "")
			pCmdMgr->setErrorMessage(string("¼ÓÔØ[") + strCmdset + "]ÃüÁî²å¼þÊ§°Ü£¡");
	}

	pCmdMgr->initBuiltinCmd();
	CmdManager::cancelRetValueBlock();
	pCmdMgr->start();
	s_isInited = true;
	return true;
}

bool zooCmd_InitW(int cmdcount, const char* cmdset[], const char* input_adapter, const wchar_t* datadir /*= nullptr*/, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/)
{
	return zooCmd_InitA(cmdcount, cmdset, input_adapter, w2a_(datadir).c_str(), windowWidth, windowHeight, windowScale);
}

bool zooCmd_IsInited()
{
	return s_isInited;
}

bool zooCmd_Send(const char* cmdlineFormat, ...)
{
	if (!s_isInited)
		return false;

	static char szbuf[2048] = { 0 };
	va_list args;
	va_start(args, cmdlineFormat);
	vsprintf_s(szbuf, sizeof(szbuf), cmdlineFormat, args);
	va_end(args);

	vector<string> arglist;
	stringtok(arglist, szbuf);
	return CmdManager::getSingleton().sendCmd(arglist);
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
		CmdManager::getSingleton().update();
}

void zooCmd_Destroy()
{
	s_isInited = false;
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
