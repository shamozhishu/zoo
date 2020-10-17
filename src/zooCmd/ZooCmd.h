#ifndef __ZOOCMD_ZOO_CMD_H__
#define __ZOOCMD_ZOO_CMD_H__

#if defined(ZOOCMD_NONCLIENT_BUILD)
#	define _zooCmdExport __declspec(dllexport)
#else
#	if defined(__MINGW32__)
#		define _zooCmdExport
#	else
#		define _zooCmdExport __declspec(dllimport)
#	endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

	_zooCmdExport bool  zooCmd_InitA(const char* render_adapter, const char* datadir);
	_zooCmdExport bool  zooCmd_InitW(const char* render_adapter, const wchar_t* datadir);
	_zooCmdExport bool  zooCmd_IsInited();
	_zooCmdExport bool  zooCmd_Register(const char* cmd);
	_zooCmdExport bool  zooCmd_Unregister(const char* cmd);
	_zooCmdExport void  zooCmd_UnregisterAll();
	_zooCmdExport bool  zooCmd_Send(const char* cmdline);
	_zooCmdExport int   zooCmd_Run();
	_zooCmdExport void  zooCmd_Refresh();
	_zooCmdExport void  zooCmd_Destroy();
	_zooCmdExport void  zooCmd_Render();
	_zooCmdExport void* zooCmd_Setup(int windowWidth = 0, int windowHeight = 0, float windowScale = 1.0f, int windowID = 0);

	// Return value, record the return value of the most recently executed command.
	_zooCmdExport bool zooCmd_BoolValue(const char* variable, bool* value);
	_zooCmdExport bool zooCmd_IntValue(const char* variable, int* value);
	_zooCmdExport bool zooCmd_FloatValue(const char* variable, float* value);
	_zooCmdExport bool zooCmd_DoubleValue(const char* variable, double* value);
	_zooCmdExport const char* zooCmd_StringValue(const char* variable);

	// Tip message, record the tip message of the most recently executed command.
	_zooCmdExport const char* zooCmd_TipMessage();

#ifdef __cplusplus
}
#endif

#endif // __ZOOCMD_ZOO_CMD_H__
