#pragma once

#if defined(OSGCMD_NONCLIENT_BUILD)
#	define _osgCmdExport __declspec(dllexport)
#else
#	if defined(__MINGW32__)
#		define _osgCmdExport
#	else
#		define _osgCmdExport __declspec(dllimport)
#	endif
#endif

extern "C"
{
	enum osgCmd_MouseButton
	{
		osgCmd_Button_No = 0,
		osgCmd_Button_Left,
		osgCmd_Button_Mid,
		osgCmd_Button_Right
	};

	enum osgCmd_Key
	{
		osgCmd_Key_Escape = 0,
		osgCmd_Key_Delete,
		osgCmd_Key_Home,
		osgCmd_Key_Enter,
		osgCmd_Key_End,
		osgCmd_Key_Return,
		osgCmd_Key_PageUp,
		osgCmd_Key_PageDown,
		osgCmd_Key_Left,
		osgCmd_Key_Right,
		osgCmd_Key_Up,
		osgCmd_Key_Down,
		osgCmd_Key_Backspace,
		osgCmd_Key_Tab,
		osgCmd_Key_Space,
		osgCmd_Key_Alt,
		osgCmd_Key_Shift,
		osgCmd_Key_Control,
		osgCmd_Key_Meta,
		osgCmd_Key_F1,
		osgCmd_Key_F2,
		osgCmd_Key_F3,
		osgCmd_Key_F4,
		osgCmd_Key_F5,
		osgCmd_Key_F6,
		osgCmd_Key_F7,
		osgCmd_Key_F8,
		osgCmd_Key_F9,
		osgCmd_Key_F10,
		osgCmd_Key_F11,
		osgCmd_Key_F12,
		osgCmd_Key_F13,
		osgCmd_Key_F14,
		osgCmd_Key_F15,
		osgCmd_Key_F16,
		osgCmd_Key_F17,
		osgCmd_Key_F18,
		osgCmd_Key_F19,
		osgCmd_Key_F20,
		osgCmd_Key_Hyphen,
		osgCmd_Key_Equal,
		osgCmd_Key_Division,
		osgCmd_Key_Multiply,
		osgCmd_Key_Minus,
		osgCmd_Key_Plus,
		osgCmd_Key_Insert
	};

	enum osgCmd_Modkey
	{
		osgCmd_Modkey_Shist = 1 << 0,
		osgCmd_Modkey_Ctrl = 1 << 1,
		osgCmd_Modkey_Alt = 1 << 2
	};

	enum osgCmd_Scroll
	{
		osgCmd_Scroll_None = 0,
		osgCmd_Scroll_Left,
		osgCmd_Scroll_Right,
		osgCmd_Scroll_Up,
		osgCmd_Scroll_Down,
		osgCmd_Scroll_2D
	};

	_osgCmdExport void osgCmd_InitA(int cmdcount, const char* cmdset[], const char* datadir = nullptr, int windowWidth = 0, int windowHeight = 0, float windowScale = 1.0f);
	_osgCmdExport void osgCmd_InitW(int cmdcount, const char* cmdset[], const wchar_t* datadir = nullptr, int windowWidth = 0, int windowHeight = 0, float windowScale = 1.0f);
	_osgCmdExport bool osgCmd_Send(const char* cmdline);
	_osgCmdExport void osgCmd_Run();
	_osgCmdExport void osgCmd_Destroy();
	_osgCmdExport void osgCmd_Render();
	_osgCmdExport void osgCmd_Resize(int windowWidth, int windowHeight, float windowScale);

	// Input event.
	_osgCmdExport void osgCmd_KeyPressEvent(int key, unsigned int modkey);
	_osgCmdExport void osgCmd_KeyReleaseEvent(int key, unsigned int modkey);
	_osgCmdExport void osgCmd_MousePressEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button);
	_osgCmdExport void osgCmd_MouseReleaseEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button);
	_osgCmdExport void osgCmd_MouseDoubleClickEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button);
	_osgCmdExport void osgCmd_MouseMoveEvent(int x, int y, unsigned int modkey);
	_osgCmdExport void osgCmd_WheelEvent(int x, int y, unsigned int modkey, osgCmd_Scroll scroll);

	// Return value, record the return value of the most recently executed command.
	_osgCmdExport bool osgCmd_BoolValue(const char* variable, bool* value);
	_osgCmdExport bool osgCmd_IntValue(const char* variable, int* value);
	_osgCmdExport bool osgCmd_FloatValue(const char* variable, float* value);
	_osgCmdExport bool osgCmd_DoubleValue(const char* variable, double* value);
	_osgCmdExport const char* osgCmd_StringValue(const char* variable);

	// Error message, record the error tip message of the most recently executed command.
	_osgCmdExport const char* osgCmd_ErrorMessage();

	// Remap Keyboard.
	_osgCmdExport void osgCmd_RemapKeyboard(osgCmd_Key key, int remapkey);
}
