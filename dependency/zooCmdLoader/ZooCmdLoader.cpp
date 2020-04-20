#include "ZooCmdLoader.h"
#include <stdio.h>
#include <Windows.h>

ZOOCMDINITAPROC zooCmd_InitA_Loader;
ZOOCMDINITWPROC zooCmd_InitW_Loader;
ZOOCMDISINITEDPROC zooCmd_IsInited_Loader;
ZOOCMDSENDPROC zooCmd_Send_Loader;
ZOOCMDRUNPROC zooCmd_Run_Loader;
ZOOCMDTICKPROC zooCmd_Tick_Loader;
ZOOCMDDESTROYPROC zooCmd_Destroy_Loader;
ZOOCMDRENDERPROC zooCmd_Render_Loader;
ZOOCMDRESIZEPROC zooCmd_Resize_Loader;
ZOOCMDKEYPRESSPROC zooCmd_KeyPress_Loader;
ZOOCMDKEYRELEASEPROC zooCmd_KeyRelease_Loader;
ZOOCMDMOUSEMOVEPROC zooCmd_MouseMove_Loader;
ZOOCMDWHEELPROC zooCmd_Wheel_Loader;
ZOOCMDMOUSEPRESSPROC zooCmd_MousePress_Loader;
ZOOCMDMOUSERELEASEPROC zooCmd_MouseRelease_Loader;
ZOOCMDMOUSEDOUBLECLICKPROC zooCmd_MouseDoubleClick_Loader;
ZOOCMDBOOLVALUEPROC zooCmd_BoolValue_Loader;
ZOOCMDINTVALUEPROC zooCmd_IntValue_Loader;
ZOOCMDFLOATVALUEPROC zooCmd_FloatValue_Loader;
ZOOCMDDOUBLEVALUEPROC zooCmd_DoubleValue_Loader;
ZOOCMDSTRINGVALUEPROC zooCmd_StringValue_Loader;
ZOOCMDERRORMESSAGEPROC zooCmd_ErrorMessage_Loader;
ZOOCMDREMAPKEYBOARDPROC zooCmd_RemapKeyboard_Loader;

static HMODULE s_libZooCmd = NULL;
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
	zooCmd_Send_Loader = (ZOOCMDSENDPROC)load(libZooCmd, "zooCmd_Send");
	zooCmd_Run_Loader = (ZOOCMDRUNPROC)load(libZooCmd, "zooCmd_Run");
	zooCmd_Tick_Loader = (ZOOCMDTICKPROC)load(libZooCmd, "zooCmd_Tick");
	zooCmd_Destroy_Loader = (ZOOCMDDESTROYPROC)load(libZooCmd, "zooCmd_Destroy");
	zooCmd_Render_Loader = (ZOOCMDRENDERPROC)load(libZooCmd, "zooCmd_Render");
	zooCmd_Resize_Loader = (ZOOCMDRESIZEPROC)load(libZooCmd, "zooCmd_Resize");
	zooCmd_KeyPress_Loader = (ZOOCMDKEYPRESSPROC)load(libZooCmd, "zooCmd_KeyPress");
	zooCmd_KeyRelease_Loader = (ZOOCMDKEYRELEASEPROC)load(libZooCmd, "zooCmd_KeyRelease");
	zooCmd_MouseMove_Loader = (ZOOCMDMOUSEMOVEPROC)load(libZooCmd, "zooCmd_MouseMove");
	zooCmd_Wheel_Loader = (ZOOCMDWHEELPROC)load(libZooCmd, "zooCmd_Wheel");
	zooCmd_MousePress_Loader = (ZOOCMDMOUSEPRESSPROC)load(libZooCmd, "zooCmd_MousePress");
	zooCmd_MouseRelease_Loader = (ZOOCMDMOUSERELEASEPROC)load(libZooCmd, "zooCmd_MouseRelease");
	zooCmd_MouseDoubleClick_Loader = (ZOOCMDMOUSEDOUBLECLICKPROC)load(libZooCmd, "zooCmd_MouseDoubleClick");
	zooCmd_BoolValue_Loader = (ZOOCMDBOOLVALUEPROC)load(libZooCmd, "zooCmd_BoolValue");
	zooCmd_IntValue_Loader = (ZOOCMDINTVALUEPROC)load(libZooCmd, "zooCmd_IntValue");
	zooCmd_FloatValue_Loader = (ZOOCMDFLOATVALUEPROC)load(libZooCmd, "zooCmd_FloatValue");
	zooCmd_DoubleValue_Loader = (ZOOCMDDOUBLEVALUEPROC)load(libZooCmd, "zooCmd_DoubleValue");
	zooCmd_StringValue_Loader = (ZOOCMDSTRINGVALUEPROC)load(libZooCmd, "zooCmd_StringValue");
	zooCmd_ErrorMessage_Loader = (ZOOCMDERRORMESSAGEPROC)load(libZooCmd, "zooCmd_ErrorMessage");
	zooCmd_RemapKeyboard_Loader = (ZOOCMDREMAPKEYBOARDPROC)load(libZooCmd, "zooCmd_RemapKeyboard");
}

bool zooCmdL_Open()
{
	if (s_libZooCmd)
		return true;
#ifdef _DEBUG
	s_libZooCmd = LoadLibrary("zooCmdd.dll");
#else
	s_libZooCmd = LoadLibrary("zooCmd.dll");
#endif
	return s_libZooCmd != NULL;
}

void zooCmdL_Close()
{
	if (s_libZooCmd != NULL)
	{
		FreeLibrary(s_libZooCmd);
		s_libZooCmd = NULL;
	}
}

bool zooCmdL_Load(void)
{
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

bool zooCmdL_Send(const char* cmdlineFormat, ...)
{
	static char szbuf[2048] = { 0 };
	va_list args;
	va_start(args, cmdlineFormat);
	vsprintf_s(szbuf, sizeof(szbuf), cmdlineFormat, args);
	va_end(args);
	return zooCmd_Send_Loader(szbuf);
}
