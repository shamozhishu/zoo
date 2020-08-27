#ifndef __LUA_EXPORT_CLASS_H__
#define __LUA_EXPORT_CLASS_H__

#include <zoo/Singleton.h>

class Input : public zoo::Singleton<Input> { // tolua_export
	class Win32Service* _service;
public:
	// tolua_begin
	enum EKey
	{
		KEY_LBUTTON = 0,  // Left mouse button
		KEY_RBUTTON,      // Right mouse button
		KEY_MBUTTON,      // Middle mouse button
		KEY_BACK,         // BACKSPACE key
		KEY_TAB,          // TAB key
		KEY_RETURN,       // ENTER key
		KEY_SHIFT,        // SHIFT key
		KEY_CONTROL,      // CTRL key
		KEY_MENU,         // ALT key
		KEY_PAUSE,        // PAUSE key
		KEY_CAPITAL,      // CAPS LOCK key
		KEY_ESCAPE,       // ESC key
		KEY_SPACE,        // SPACEBAR
		KEY_PRIOR,        // PAGE UP key
		KEY_NEXT,         // PAGE DOWN key
		KEY_END,          // END key
		KEY_HOME,         // HOME key
		KEY_LEFT,         // LEFT ARROW key
		KEY_UP,           // UP ARROW key
		KEY_RIGHT,        // RIGHT ARROW key
		KEY_DOWN,         // DOWN ARROW key
		KEY_INSERT,       // INS key
		KEY_DELETE,       // DEL key
		KEY_KEY_0,        // 0 key
		KEY_KEY_1,        // 1 key
		KEY_KEY_2,        // 2 key
		KEY_KEY_3,        // 3 key
		KEY_KEY_4,        // 4 key
		KEY_KEY_5,        // 5 key
		KEY_KEY_6,        // 6 key
		KEY_KEY_7,        // 7 key
		KEY_KEY_8,        // 8 key
		KEY_KEY_9,        // 9 key
		KEY_KEY_A,        // A key
		KEY_KEY_B,        // B key
		KEY_KEY_C,        // C key
		KEY_KEY_D,        // D key
		KEY_KEY_E,        // E key
		KEY_KEY_F,        // F key
		KEY_KEY_G,        // G key
		KEY_KEY_H,        // H key
		KEY_KEY_I,        // I key
		KEY_KEY_J,        // J key
		KEY_KEY_K,        // K key
		KEY_KEY_L,        // L key
		KEY_KEY_M,        // M key
		KEY_KEY_N,        // N key
		KEY_KEY_O,        // O key
		KEY_KEY_P,        // P key
		KEY_KEY_Q,        // Q key
		KEY_KEY_R,        // R key
		KEY_KEY_S,        // S key
		KEY_KEY_T,        // T key
		KEY_KEY_U,        // U key
		KEY_KEY_V,        // V key
		KEY_KEY_W,        // W key
		KEY_KEY_X,        // X key
		KEY_KEY_Y,        // Y key
		KEY_KEY_Z,        // Z key
		KEY_NUMPAD0,      // Numeric keypad 0 key
		KEY_NUMPAD1,      // Numeric keypad 1 key
		KEY_NUMPAD2,      // Numeric keypad 2 key
		KEY_NUMPAD3,      // Numeric keypad 3 key
		KEY_NUMPAD4,      // Numeric keypad 4 key
		KEY_NUMPAD5,      // Numeric keypad 5 key
		KEY_NUMPAD6,      // Numeric keypad 6 key
		KEY_NUMPAD7,      // Numeric keypad 7 key
		KEY_NUMPAD8,      // Numeric keypad 8 key
		KEY_NUMPAD9,      // Numeric keypad 9 key
		KEY_F1,           // F1 key
		KEY_F2,           // F2 key
		KEY_F3,           // F3 key
		KEY_F4,           // F4 key
		KEY_F5,           // F5 key
		KEY_F6,           // F6 key
		KEY_F7,           // F7 key
		KEY_F8,           // F8 key
		KEY_F9,           // F9 key
		KEY_F10,          // F10 key
		KEY_F11,          // F11 key
		KEY_F12,          // F12 key
		KEY_COUNT
	};
	// tolua_end
	Input();
	~Input();
	void poll();

public:
	// tolua_begin
	bool keyDown(EKey key);
};
// tolua_end

#endif // __LUA_EXPORT_CLASS_H__
