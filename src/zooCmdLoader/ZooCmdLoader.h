#ifndef __ZOOCMD_LOADER_H__
#define __ZOOCMD_LOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

extern bool zooCmdL_Open();
extern void zooCmdL_Close();
extern bool zooCmdL_Load(void);
extern bool zooCmdL_Send(const char* cmd, const char* format, ...);

#define zooCmd_InitA zooCmd_InitA_Loader
#define zooCmd_InitW zooCmd_InitW_Loader
#define zooCmd_IsInited zooCmd_IsInited_Loader
#define zooCmd_Send zooCmd_Send_Loader
#define zooCmd_Run zooCmd_Run_Loader
#define zooCmd_Tick zooCmd_Tick_Loader
#define zooCmd_Destroy zooCmd_Destroy_Loader
#define zooCmd_Render zooCmd_Render_Loader
#define zooCmd_Resize zooCmd_Resize_Loader
#define zooCmd_KeyPress zooCmd_KeyPress_Loader
#define zooCmd_KeyRelease zooCmd_KeyRelease_Loader
#define zooCmd_MouseMove zooCmd_MouseMove_Loader
#define zooCmd_Wheel zooCmd_Wheel_Loader
#define zooCmd_MousePress zooCmd_MousePress_Loader
#define zooCmd_MouseRelease zooCmd_MouseRelease_Loader
#define zooCmd_MouseDoubleClick zooCmd_MouseDoubleClick_Loader

// Return value, record the return value of the most recently executed command.
#define zooCmd_BoolValue zooCmd_BoolValue_Loader
#define zooCmd_IntValue zooCmd_IntValue_Loader
#define zooCmd_FloatValue zooCmd_FloatValue_Loader
#define zooCmd_DoubleValue zooCmd_DoubleValue_Loader
#define zooCmd_StringValue zooCmd_StringValue_Loader

// Error message, record the error tip message of the most recently executed command.
#define zooCmd_ErrorMessage zooCmd_ErrorMessage_Loader

// Remap Keyboard.
#define zooCmd_RemapKeyboard zooCmd_RemapKeyboard_Loader

// zooCmd_MouseButton
#define zooCmd_Button_No 0
#define zooCmd_Button_Left 1
#define zooCmd_Button_Mid 2
#define zooCmd_Button_Right 3

// zooCmd_Key
#define zooCmd_Key_Escape 0
#define zooCmd_Key_Delete 1
#define zooCmd_Key_Home 2
#define zooCmd_Key_Enter 3
#define zooCmd_Key_End 4
#define zooCmd_Key_Return 5
#define zooCmd_Key_PageUp 6
#define zooCmd_Key_PageDown 7
#define zooCmd_Key_Left 8
#define zooCmd_Key_Right 9
#define zooCmd_Key_Up 10
#define zooCmd_Key_Down 11
#define zooCmd_Key_Backspace 12
#define zooCmd_Key_Tab 13
#define zooCmd_Key_Space 14
#define zooCmd_Key_Alt 15
#define zooCmd_Key_Shift 16
#define zooCmd_Key_Control 17
#define zooCmd_Key_Meta 18
#define zooCmd_Key_F1 19
#define zooCmd_Key_F2 20
#define zooCmd_Key_F3 21
#define zooCmd_Key_F4 22
#define zooCmd_Key_F5 23
#define zooCmd_Key_F6 24
#define zooCmd_Key_F7 25
#define zooCmd_Key_F8 26
#define zooCmd_Key_F9 27
#define zooCmd_Key_F10 28
#define zooCmd_Key_F11 29
#define zooCmd_Key_F12 30
#define zooCmd_Key_F13 31
#define zooCmd_Key_F14 32
#define zooCmd_Key_F15 33
#define zooCmd_Key_F16 34
#define zooCmd_Key_F17 35
#define zooCmd_Key_F18 36
#define zooCmd_Key_F19 37
#define zooCmd_Key_F20 38
#define zooCmd_Key_Hyphen 39
#define zooCmd_Key_Equal 40
#define zooCmd_Key_Division 41
#define zooCmd_Key_Multiply 42
#define zooCmd_Key_Minus 43
#define zooCmd_Key_Plus 44
#define zooCmd_Key_Insert 45

// zooCmd_Modkey
#define zooCmd_Modkey_Shist 0x1
#define zooCmd_Modkey_Ctrl 0x2
#define zooCmd_Modkey_Alt 0x4

// zooCmd_Scroll
#define zooCmd_Scroll_None 0
#define zooCmd_Scroll_Left 1
#define zooCmd_Scroll_Right 2
#define zooCmd_Scroll_Up 3
#define zooCmd_Scroll_Down 4
#define zooCmd_Scroll_2D 5

typedef unsigned int zooCmd_Key, zooCmd_Scroll, zooCmd_MouseButton;

typedef bool(*ZOOCMDINITAPROC)(int cmdcount, const char* cmdset[], const char* input_adapter, const char* datadir /*= nullptr*/, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/);
extern ZOOCMDINITAPROC zooCmd_InitA_Loader;
typedef bool(*ZOOCMDINITWPROC)(int cmdcount, const char* cmdset[], const char* input_adapter, const wchar_t* datadir /*= nullptr*/, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/);
extern ZOOCMDINITWPROC zooCmd_InitW_Loader;
typedef bool(*ZOOCMDISINITEDPROC)();
extern ZOOCMDISINITEDPROC zooCmd_IsInited_Loader;
typedef bool(*ZOOCMDSENDPROC)(const char* cmdline);
extern ZOOCMDSENDPROC zooCmd_Send_Loader;
typedef int(*ZOOCMDRUNPROC)();
extern ZOOCMDRUNPROC zooCmd_Run_Loader;
typedef void(*ZOOCMDTICKPROC)();
extern ZOOCMDTICKPROC zooCmd_Tick_Loader;
typedef void(*ZOOCMDDESTROYPROC)();
extern ZOOCMDDESTROYPROC zooCmd_Destroy_Loader;
typedef void(*ZOOCMDRENDERPROC)();
extern ZOOCMDRENDERPROC zooCmd_Render_Loader;
typedef void(*ZOOCMDRESIZEPROC)(int windowWidth, int windowHeight, float windowScale);
extern ZOOCMDRESIZEPROC zooCmd_Resize_Loader;
typedef void(*ZOOCMDKEYPRESSPROC)(int key, unsigned int modkey);
extern ZOOCMDKEYPRESSPROC zooCmd_KeyPress_Loader;
typedef void(*ZOOCMDKEYRELEASEPROC)(int key, unsigned int modkey);
extern ZOOCMDKEYRELEASEPROC zooCmd_KeyRelease_Loader;
typedef void(*ZOOCMDMOUSEMOVEPROC)(int x, int y, unsigned int modkey);
extern ZOOCMDMOUSEMOVEPROC zooCmd_MouseMove_Loader;
typedef void(*ZOOCMDWHEELPROC)(int x, int y, unsigned int modkey, zooCmd_Scroll scroll);
extern ZOOCMDWHEELPROC zooCmd_Wheel_Loader;
typedef void(*ZOOCMDMOUSEPRESSPROC)(int x, int y, unsigned int modkey, zooCmd_MouseButton button);
extern ZOOCMDMOUSEPRESSPROC zooCmd_MousePress_Loader;
typedef void(*ZOOCMDMOUSERELEASEPROC)(int x, int y, unsigned int modkey, zooCmd_MouseButton button);
extern ZOOCMDMOUSERELEASEPROC zooCmd_MouseRelease_Loader;
typedef void(*ZOOCMDMOUSEDOUBLECLICKPROC)(int x, int y, unsigned int modkey, zooCmd_MouseButton button);
extern ZOOCMDMOUSEDOUBLECLICKPROC zooCmd_MouseDoubleClick_Loader;
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
typedef const char*(*ZOOCMDERRORMESSAGEPROC)();
extern ZOOCMDERRORMESSAGEPROC zooCmd_ErrorMessage_Loader;
typedef void(*ZOOCMDREMAPKEYBOARDPROC)(zooCmd_Key key, int remapkey);
extern ZOOCMDREMAPKEYBOARDPROC zooCmd_RemapKeyboard_Loader;

#ifdef __cplusplus
}
#endif

#endif // __ZOOCMD_LOADER_H__
