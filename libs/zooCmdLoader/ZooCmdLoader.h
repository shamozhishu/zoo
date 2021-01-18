#ifndef __ZOOCMD_LOADER_H__
#define __ZOOCMD_LOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

extern bool zooCmdL_Load(void);
extern bool zooCmdL_Send(const char* cmd, const char* format, ...);

#define zooCmd_InitA zooCmd_InitA_Loader
#define zooCmd_InitW zooCmd_InitW_Loader
#define zooCmd_IsInited zooCmd_IsInited_Loader
#define zooCmd_Register zooCmd_Register_Loader
#define zooCmd_Unregister zooCmd_Unregister_Loader
#define zooCmd_UnregisterAll zooCmd_UnregisterAll_Loader
#define zooCmd_Send zooCmd_Send_Loader
#define zooCmd_Run zooCmd_Run_Loader
#define zooCmd_Refresh zooCmd_Refresh_Loader
#define zooCmd_Destroy zooCmd_Destroy_Loader
#define zooCmd_Render zooCmd_Render_Loader
#define zooCmd_GetWnd zooCmd_GetWnd_Loader
#define zooCmd_Setup zooCmd_Setup_Loader

// Return value, record the return value of the most recently executed command.
#define zooCmd_BoolValue zooCmd_BoolValue_Loader
#define zooCmd_IntValue zooCmd_IntValue_Loader
#define zooCmd_FloatValue zooCmd_FloatValue_Loader
#define zooCmd_DoubleValue zooCmd_DoubleValue_Loader
#define zooCmd_StringValue zooCmd_StringValue_Loader

// Tip message, record the tip message of the most recently executed command.
#define zooCmd_TipMessage zooCmd_TipMessage_Loader

typedef bool(*ZOOCMDINITAPROC)(const char* input_adapter, const char* datadir);
extern ZOOCMDINITAPROC zooCmd_InitA_Loader;
typedef bool(*ZOOCMDINITWPROC)(const char* input_adapter, const wchar_t* datadir);
extern ZOOCMDINITWPROC zooCmd_InitW_Loader;
typedef bool(*ZOOCMDISINITEDPROC)();
extern ZOOCMDISINITEDPROC zooCmd_IsInited_Loader;
typedef bool(*ZOOCMDREGISTERPROC)(const char* cmd);
extern ZOOCMDREGISTERPROC zooCmd_Register_Loader;
typedef bool(*ZOOCMDUNREGISTERPROC)(const char* cmd);
extern ZOOCMDUNREGISTERPROC zooCmd_Unregister_Loader;
typedef void(*ZOOCMDUNREGISTERALLPROC)();
extern ZOOCMDUNREGISTERALLPROC zooCmd_UnregisterAll_Loader;
typedef bool(*ZOOCMDSENDPROC)(const char* cmdline);
extern ZOOCMDSENDPROC zooCmd_Send_Loader;
typedef int(*ZOOCMDRUNPROC)();
extern ZOOCMDRUNPROC zooCmd_Run_Loader;
typedef void(*ZOOCMDREFRESHPROC)();
extern ZOOCMDREFRESHPROC zooCmd_Refresh_Loader;
typedef void(*ZOOCMDDESTROYPROC)();
extern ZOOCMDDESTROYPROC zooCmd_Destroy_Loader;
typedef void(*ZOOCMDRENDERPROC)();
extern ZOOCMDRENDERPROC zooCmd_Render_Loader;
typedef void*(*ZOOCMDGETWNDPROC)(int windowID, bool wndHanle);
extern ZOOCMDGETWNDPROC zooCmd_GetWnd_Loader;
typedef void*(*ZOOCMDSETUPPROC)(int windowWidth, int windowHeight, float windowScale, int windowID);
extern ZOOCMDSETUPPROC zooCmd_Setup_Loader;
typedef bool(*ZOOCMDBOOLVALUEPROC)(const char* variable, bool* value);
extern ZOOCMDBOOLVALUEPROC zooCmd_BoolValue_Loader;
typedef bool(*ZOOCMDINTVALUEPROC)(const char* variable, int* value);
extern ZOOCMDINTVALUEPROC zooCmd_IntValue_Loader;
typedef bool(*ZOOCMDFLOATVALUEPROC)(const char* variable, float* value);
extern ZOOCMDFLOATVALUEPROC zooCmd_FloatValue_Loader;
typedef bool(*ZOOCMDDOUBLEVALUEPROC)(const char* variable, double* value);
extern ZOOCMDDOUBLEVALUEPROC zooCmd_DoubleValue_Loader;
typedef const char*(*ZOOCMDSTRINGVALUEPROC)(const char* variable);
extern ZOOCMDSTRINGVALUEPROC zooCmd_StringValue_Loader;
typedef const char*(*ZOOCMDTIPMESSAGEPROC)();
extern ZOOCMDTIPMESSAGEPROC zooCmd_TipMessage_Loader;

#ifdef __cplusplus
}
#endif

#endif // __ZOOCMD_LOADER_H__
