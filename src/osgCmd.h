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
		NoButton = 0,
		LeftButton,
		MidButton,
		RightButton
	};

	enum osgCmd_Key
	{
		Key_Escape = 0,
		Key_Delete,
		Key_Home,
		Key_Enter,
		Key_End,
		Key_Return,
		Key_PageUp,
		Key_PageDown,
		Key_Left,
		Key_Right,
		Key_Up,
		Key_Down,
		Key_Backspace,
		Key_Tab,
		Key_Space,
		Key_Alt,
		Key_Shift,
		Key_Control,
		Key_Meta,
		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,
		Key_F13,
		Key_F14,
		Key_F15,
		Key_F16,
		Key_F17,
		Key_F18,
		Key_F19,
		Key_F20,
		Key_hyphen,
		Key_Equal,
		Key_division,
		Key_multiply,
		Key_Minus,
		Key_Plus,
		Key_Insert
	};

	enum osgCmd_Scroll
	{
		SCROLL_NONE = 0,
		SCROLL_LEFT,
		SCROLL_RIGHT,
		SCROLL_UP,
		SCROLL_DOWN,
		SCROLL_2D
	};

	_osgCmdExport void osgCmd_Init(int cmdcount, char* cmdset[], const char* workdir = nullptr, int wndWidth = 0, int wndHeight = 0, float wndScale = 1.0f);
	_osgCmdExport bool osgCmd_Send(const char* cmdline);
	_osgCmdExport void osgCmd_Run();
	_osgCmdExport void osgCmd_Destroy();
	_osgCmdExport void osgCmd_Render();
	_osgCmdExport void osgCmd_Resize(int windowWidth, int windowHeight, float windowScale);
	// Event.
	_osgCmdExport void osgCmd_KeyPressEvent(osgCmd_Key key, unsigned int modkey);
	_osgCmdExport void osgCmd_KeyReleaseEvent(osgCmd_Key key, unsigned int modkey);
	_osgCmdExport void osgCmd_MousePressEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button);
	_osgCmdExport void osgCmd_MouseReleaseEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button);
	_osgCmdExport void osgCmd_MouseDoubleClickEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button);
	_osgCmdExport void osgCmd_MouseMoveEvent(int x, int y, unsigned int modkey);
	_osgCmdExport void osgCmd_WheelEvent(int x, int y, unsigned int modkey, osgCmd_Scroll scroll);
}
