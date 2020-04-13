#pragma once

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

	enum zooCmd_MouseButton
	{
		zooCmd_Button_No = 0,
		zooCmd_Button_Left,
		zooCmd_Button_Mid,
		zooCmd_Button_Right
	};

	enum zooCmd_Key
	{
		zooCmd_Key_Escape = 0,
		zooCmd_Key_Delete,
		zooCmd_Key_Home,
		zooCmd_Key_Enter,
		zooCmd_Key_End,
		zooCmd_Key_Return,
		zooCmd_Key_PageUp,
		zooCmd_Key_PageDown,
		zooCmd_Key_Left,
		zooCmd_Key_Right,
		zooCmd_Key_Up,
		zooCmd_Key_Down,
		zooCmd_Key_Backspace,
		zooCmd_Key_Tab,
		zooCmd_Key_Space,
		zooCmd_Key_Alt,
		zooCmd_Key_Shift,
		zooCmd_Key_Control,
		zooCmd_Key_Meta,
		zooCmd_Key_F1,
		zooCmd_Key_F2,
		zooCmd_Key_F3,
		zooCmd_Key_F4,
		zooCmd_Key_F5,
		zooCmd_Key_F6,
		zooCmd_Key_F7,
		zooCmd_Key_F8,
		zooCmd_Key_F9,
		zooCmd_Key_F10,
		zooCmd_Key_F11,
		zooCmd_Key_F12,
		zooCmd_Key_F13,
		zooCmd_Key_F14,
		zooCmd_Key_F15,
		zooCmd_Key_F16,
		zooCmd_Key_F17,
		zooCmd_Key_F18,
		zooCmd_Key_F19,
		zooCmd_Key_F20,
		zooCmd_Key_Hyphen,
		zooCmd_Key_Equal,
		zooCmd_Key_Division,
		zooCmd_Key_Multiply,
		zooCmd_Key_Minus,
		zooCmd_Key_Plus,
		zooCmd_Key_Insert
	};

	enum zooCmd_Modkey
	{
		zooCmd_Modkey_Shist = 1 << 0,
		zooCmd_Modkey_Ctrl = 1 << 1,
		zooCmd_Modkey_Alt = 1 << 2
	};

	enum zooCmd_Scroll
	{
		zooCmd_Scroll_None = 0,
		zooCmd_Scroll_Left,
		zooCmd_Scroll_Right,
		zooCmd_Scroll_Up,
		zooCmd_Scroll_Down,
		zooCmd_Scroll_2D
	};

	_zooCmdExport bool zooCmd_InitA(int cmdcount, const char* cmdset[], const char* input_adapter, const char* datadir = nullptr, int windowWidth = 0, int windowHeight = 0, float windowScale = 1.0f);
	_zooCmdExport bool zooCmd_InitW(int cmdcount, const char* cmdset[], const char* input_adapter, const wchar_t* datadir = nullptr, int windowWidth = 0, int windowHeight = 0, float windowScale = 1.0f);
	_zooCmdExport bool zooCmd_IsInited();
	_zooCmdExport bool zooCmd_Send(const char* cmdlineFormat, ...);
	_zooCmdExport int  zooCmd_Run();
	_zooCmdExport void zooCmd_Tick();
	_zooCmdExport void zooCmd_Destroy();
	_zooCmdExport void zooCmd_Render();
	_zooCmdExport void zooCmd_Resize(int windowWidth, int windowHeight, float windowScale);
	_zooCmdExport void zooCmd_KeyPress(int key, unsigned int modkey);
	_zooCmdExport void zooCmd_KeyRelease(int key, unsigned int modkey);
	_zooCmdExport void zooCmd_MouseMove(int x, int y, unsigned int modkey);
	_zooCmdExport void zooCmd_Wheel(int x, int y, unsigned int modkey, zooCmd_Scroll scroll);
	_zooCmdExport void zooCmd_MousePress(int x, int y, unsigned int modkey, zooCmd_MouseButton button);
	_zooCmdExport void zooCmd_MouseRelease(int x, int y, unsigned int modkey, zooCmd_MouseButton button);
	_zooCmdExport void zooCmd_MouseDoubleClick(int x, int y, unsigned int modkey, zooCmd_MouseButton button);

	// Return value, record the return value of the most recently executed command.
	_zooCmdExport bool zooCmd_BoolValue(const char* variable, bool* value);
	_zooCmdExport bool zooCmd_IntValue(const char* variable, int* value);
	_zooCmdExport bool zooCmd_FloatValue(const char* variable, float* value);
	_zooCmdExport bool zooCmd_DoubleValue(const char* variable, double* value);
	_zooCmdExport const char* zooCmd_StringValue(const char* variable);

	// Error message, record the error tip message of the most recently executed command.
	_zooCmdExport const char* zooCmd_ErrorMessage();

	// Remap Keyboard.
	_zooCmdExport void zooCmd_RemapKeyboard(zooCmd_Key key, int remapkey);

#ifdef __cplusplus
}
#endif
