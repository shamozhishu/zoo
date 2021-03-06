#include "ZooCmdLoader.h"
#include <stdio.h>
#include <Windows.h>

ZOOCMDINITAPROC zooCmd_InitA_Loader;
ZOOCMDINITWPROC zooCmd_InitW_Loader;
ZOOCMDISINITEDPROC zooCmd_IsInited_Loader;
ZOOCMDREGISTERPROC zooCmd_Register_Loader;
ZOOCMDUNREGISTERPROC zooCmd_Unregister_Loader;
ZOOCMDUNREGISTERALLPROC zooCmd_UnregisterAll_Loader;
ZOOCMDSENDPROC zooCmd_Send_Loader;
ZOOCMDRUNPROC zooCmd_Run_Loader;
ZOOCMDREFRESHPROC zooCmd_Refresh_Loader;
ZOOCMDDESTROYPROC zooCmd_Destroy_Loader;
ZOOCMDRENDERPROC zooCmd_Render_Loader;
ZOOCMDGETWNDPROC zooCmd_GetWnd_Loader;
ZOOCMDSETUPPROC zooCmd_Setup_Loader;
ZOOCMDBOOLVALUEPROC zooCmd_BoolValue_Loader;
ZOOCMDINTVALUEPROC zooCmd_IntValue_Loader;
ZOOCMDFLOATVALUEPROC zooCmd_FloatValue_Loader;
ZOOCMDDOUBLEVALUEPROC zooCmd_DoubleValue_Loader;
ZOOCMDSTRINGVALUEPROC zooCmd_StringValue_Loader;
ZOOCMDTIPMESSAGEPROC zooCmd_TipMessage_Loader;

struct tagLoader
{
	tagLoader() : _libZooCmd(NULL)
	{
#ifdef _DEBUG
		_libZooCmd = LoadLibrary("zooCmdd.dll");
#else
		_libZooCmd = LoadLibrary("zooCmd.dll");
#endif
	}

	~tagLoader()
	{
		if (_libZooCmd != NULL)
			FreeLibrary(_libZooCmd);
	}
private:
	HMODULE _libZooCmd;
};

static void* get_proc(HMODULE libZooCmd, const char *namez)
{
	void* result = NULL;
	if (libZooCmd == NULL)
		return NULL;
#ifdef _WIN32
	result = (void*)GetProcAddress(libZooCmd, namez);
#else
	result = dlsym(libZooCmd, namez);
#endif
	return result;
}

static void load_zoocmd(HMODULE libZooCmd, void*(*load)(HMODULE, const char*))
{
	zooCmd_InitA_Loader = (ZOOCMDINITAPROC)load(libZooCmd, "zooCmd_InitA");
	zooCmd_InitW_Loader = (ZOOCMDINITWPROC)load(libZooCmd, "zooCmd_InitW");
	zooCmd_IsInited_Loader = (ZOOCMDISINITEDPROC)load(libZooCmd, "zooCmd_IsInited");
	zooCmd_Register_Loader = (ZOOCMDREGISTERPROC)load(libZooCmd, "zooCmd_Register");
	zooCmd_Unregister_Loader = (ZOOCMDUNREGISTERPROC)load(libZooCmd, "zooCmd_Unregister");
	zooCmd_UnregisterAll_Loader = (ZOOCMDUNREGISTERALLPROC)load(libZooCmd, "zooCmd_UnregisterAll");
	zooCmd_Send_Loader = (ZOOCMDSENDPROC)load(libZooCmd, "zooCmd_Send");
	zooCmd_Run_Loader = (ZOOCMDRUNPROC)load(libZooCmd, "zooCmd_Run");
	zooCmd_Refresh_Loader = (ZOOCMDREFRESHPROC)load(libZooCmd, "zooCmd_Refresh");
	zooCmd_Destroy_Loader = (ZOOCMDDESTROYPROC)load(libZooCmd, "zooCmd_Destroy");
	zooCmd_Render_Loader = (ZOOCMDRENDERPROC)load(libZooCmd, "zooCmd_Render");
	zooCmd_GetWnd_Loader = (ZOOCMDGETWNDPROC)load(libZooCmd, "zooCmd_GetWnd");
	zooCmd_Setup_Loader = (ZOOCMDSETUPPROC)load(libZooCmd, "zooCmd_Setup");
	zooCmd_BoolValue_Loader = (ZOOCMDBOOLVALUEPROC)load(libZooCmd, "zooCmd_BoolValue");
	zooCmd_IntValue_Loader = (ZOOCMDINTVALUEPROC)load(libZooCmd, "zooCmd_IntValue");
	zooCmd_FloatValue_Loader = (ZOOCMDFLOATVALUEPROC)load(libZooCmd, "zooCmd_FloatValue");
	zooCmd_DoubleValue_Loader = (ZOOCMDDOUBLEVALUEPROC)load(libZooCmd, "zooCmd_DoubleValue");
	zooCmd_StringValue_Loader = (ZOOCMDSTRINGVALUEPROC)load(libZooCmd, "zooCmd_StringValue");
	zooCmd_TipMessage_Loader = (ZOOCMDTIPMESSAGEPROC)load(libZooCmd, "zooCmd_TipMessage");
}

bool zooCmdL_Load(void)
{
	static tagLoader loader;
#ifdef _DEBUG
	HMODULE libZooCmd = LoadLibrary("zooCmdd.dll");
#else
	HMODULE libZooCmd = LoadLibrary("zooCmd.dll");
#endif
	if (!libZooCmd)
		return false;
	load_zoocmd(libZooCmd, &get_proc);
	FreeLibrary(libZooCmd);
	return true;
}

bool zooCmdL_Send(const char* cmd, const char* format, ...)
{
	static char szbuf[2048] = { 0 };
	size_t offset = 0;
	if (cmd != nullptr && strcmp(cmd, "") != 0)
	{
		offset = strlen(cmd);
		sprintf_s(szbuf, sizeof(szbuf), cmd);
		sprintf_s(szbuf + offset, sizeof(szbuf) - offset, ".");
		offset += 1;
	}

	va_list args;
	va_start(args, format);
	vsprintf_s(szbuf + offset, sizeof(szbuf) - offset, format, args);
	va_end(args);
	return zooCmd_Send_Loader(szbuf);
}
