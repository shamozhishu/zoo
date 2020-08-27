#include "LuaExportClass.h"
#include <windows.h>
#include <dinput.h>
#include <ctk_service/zoocmd_ui/Win32Service.h>
#include "WarActivator.h"

//===============================================================
// Clean up
#define ReleaseCOM(x) { if(x) { x->Release();x = 0; } }

class DirectInput
{
public:
	DirectInput(DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
	{
		ZeroMemory(mKeyboardState, sizeof(mKeyboardState));
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDInput, 0);

		mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, 0);
		mKeyboard->SetDataFormat(&c_dfDIKeyboard);
		mKeyboard->SetCooperativeLevel(WarActivator::getService<Win32Service>()->getWnd(), keyboardCoopFlags);
		mKeyboard->Acquire();

		mDInput->CreateDevice(GUID_SysMouse, &mMouse, 0);
		mMouse->SetDataFormat(&c_dfDIMouse2);
		mMouse->SetCooperativeLevel(WarActivator::getService<Win32Service>()->getWnd(), mouseCoopFlags);
		mMouse->Acquire();
	}

	~DirectInput()
	{
		ReleaseCOM(mDInput);
		mKeyboard->Unacquire();
		mMouse->Unacquire();
		ReleaseCOM(mKeyboard);
		ReleaseCOM(mMouse);
	}

	void poll()
	{
		// Poll keyboard.
		HRESULT hr = mKeyboard->GetDeviceState(sizeof(mKeyboardState), (void**)&mKeyboardState);
		if (FAILED(hr))
		{
			// Keyboard lost, zero out keyboard data structure.
			ZeroMemory(mKeyboardState, sizeof(mKeyboardState));

			// Try to acquire for next time we poll.
			hr = mKeyboard->Acquire();
		}

		// Poll mouse.
		hr = mMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mMouseState);
		if (FAILED(hr))
		{
			// Mouse lost, zero out mouse data structure.
			ZeroMemory(&mMouseState, sizeof(mMouseState));

			// Try to acquire for next time we poll.
			hr = mMouse->Acquire();
		}
	}

	bool keyDown(char key)
	{
		return (mKeyboardState[key] & 0x80) != 0;
	}

	bool mouseButtonDown(int button)
	{
		return (mMouseState.rgbButtons[button] & 0x80) != 0;
	}

	float mouseDX()
	{
		return (float)mMouseState.lX;
	}

	float mouseDY()
	{
		return (float)mMouseState.lY;
	}

	float mouseDZ()
	{
		return (float)mMouseState.lZ;
	}

private:
	// Make private to prevent copying of members of this class.
	DirectInput(const DirectInput& rhs) = delete;
	DirectInput& operator=(const DirectInput& rhs) = delete;

private:
	IDirectInput8*       mDInput;
	IDirectInputDevice8* mKeyboard;
	char                 mKeyboardState[256];
	IDirectInputDevice8* mMouse;
	DIMOUSESTATE2        mMouseState;
};

static DirectInput* gDInput = nullptr;
Input::Input()
	: _service(WarActivator::getService<Win32Service>())
{
	if (!gDInput)
		gDInput = new DirectInput(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}

Input::~Input()
{
	delete gDInput;
	gDInput = nullptr;
}

void Input::poll()
{
	gDInput->poll();
}

bool Input::keyDown(EKey key)
{
	if (!_service || !_service->isFocus())
		return false;
	switch (key)
	{
	case Input::KEY_LBUTTON: return gDInput->mouseButtonDown(0);
	case Input::KEY_RBUTTON: return gDInput->mouseButtonDown(1);
	case Input::KEY_MBUTTON: return gDInput->mouseButtonDown(2);
	case Input::KEY_BACK: return gDInput->keyDown(DIK_BACK);
	case Input::KEY_TAB: return gDInput->keyDown(DIK_TAB);
	case Input::KEY_RETURN: return gDInput->keyDown(DIK_RETURN);
	case Input::KEY_SHIFT: return gDInput->keyDown(DIK_LSHIFT) || gDInput->keyDown(DIK_RSHIFT);
	case Input::KEY_CONTROL: return gDInput->keyDown(DIK_LCONTROL) || gDInput->keyDown(DIK_RCONTROL);
	case Input::KEY_MENU: return gDInput->keyDown(DIK_LMENU) || gDInput->keyDown(DIK_RMENU);
	case Input::KEY_PAUSE: return gDInput->keyDown(DIK_PAUSE);
	case Input::KEY_CAPITAL: return gDInput->keyDown(DIK_CAPITAL);
	case Input::KEY_ESCAPE: return gDInput->keyDown(DIK_ESCAPE);
	case Input::KEY_SPACE: return gDInput->keyDown(DIK_SPACE);
	case Input::KEY_PRIOR: return gDInput->keyDown(DIK_PRIOR);
	case Input::KEY_NEXT: return gDInput->keyDown(DIK_NEXT);
	case Input::KEY_END: return gDInput->keyDown(DIK_END);
	case Input::KEY_HOME: return gDInput->keyDown(DIK_HOME);
	case Input::KEY_LEFT: return gDInput->keyDown(DIK_LEFT);
	case Input::KEY_UP: return gDInput->keyDown(DIK_UP);
	case Input::KEY_RIGHT: return gDInput->keyDown(DIK_RIGHT);
	case Input::KEY_DOWN: return gDInput->keyDown(DIK_DOWN);
	case Input::KEY_INSERT: return gDInput->keyDown(DIK_INSERT);
	case Input::KEY_DELETE: return gDInput->keyDown(DIK_DELETE);
	case Input::KEY_KEY_0: return gDInput->keyDown(DIK_0);
	case Input::KEY_KEY_1: return gDInput->keyDown(DIK_1);
	case Input::KEY_KEY_2: return gDInput->keyDown(DIK_2);
	case Input::KEY_KEY_3: return gDInput->keyDown(DIK_3);
	case Input::KEY_KEY_4: return gDInput->keyDown(DIK_4);
	case Input::KEY_KEY_5: return gDInput->keyDown(DIK_5);
	case Input::KEY_KEY_6: return gDInput->keyDown(DIK_6);
	case Input::KEY_KEY_7: return gDInput->keyDown(DIK_7);
	case Input::KEY_KEY_8: return gDInput->keyDown(DIK_8);
	case Input::KEY_KEY_9: return gDInput->keyDown(DIK_9);
	case Input::KEY_KEY_A: return gDInput->keyDown(DIK_A);
	case Input::KEY_KEY_B: return gDInput->keyDown(DIK_B);
	case Input::KEY_KEY_C: return gDInput->keyDown(DIK_C);
	case Input::KEY_KEY_D: return gDInput->keyDown(DIK_D);
	case Input::KEY_KEY_E: return gDInput->keyDown(DIK_E);
	case Input::KEY_KEY_F: return gDInput->keyDown(DIK_F);
	case Input::KEY_KEY_G: return gDInput->keyDown(DIK_G);
	case Input::KEY_KEY_H: return gDInput->keyDown(DIK_H);
	case Input::KEY_KEY_I: return gDInput->keyDown(DIK_I);
	case Input::KEY_KEY_J: return gDInput->keyDown(DIK_J);
	case Input::KEY_KEY_K: return gDInput->keyDown(DIK_K);
	case Input::KEY_KEY_L: return gDInput->keyDown(DIK_L);
	case Input::KEY_KEY_M: return gDInput->keyDown(DIK_M);
	case Input::KEY_KEY_N: return gDInput->keyDown(DIK_N);
	case Input::KEY_KEY_O: return gDInput->keyDown(DIK_O);
	case Input::KEY_KEY_P: return gDInput->keyDown(DIK_P);
	case Input::KEY_KEY_Q: return gDInput->keyDown(DIK_Q);
	case Input::KEY_KEY_R: return gDInput->keyDown(DIK_R);
	case Input::KEY_KEY_S: return gDInput->keyDown(DIK_S);
	case Input::KEY_KEY_T: return gDInput->keyDown(DIK_T);
	case Input::KEY_KEY_U: return gDInput->keyDown(DIK_U);
	case Input::KEY_KEY_V: return gDInput->keyDown(DIK_V);
	case Input::KEY_KEY_W: return gDInput->keyDown(DIK_W);
	case Input::KEY_KEY_X: return gDInput->keyDown(DIK_X);
	case Input::KEY_KEY_Y: return gDInput->keyDown(DIK_Y);
	case Input::KEY_KEY_Z: return gDInput->keyDown(DIK_Z);
	case Input::KEY_NUMPAD0: return gDInput->keyDown(DIK_NUMPAD0);
	case Input::KEY_NUMPAD1: return gDInput->keyDown(DIK_NUMPAD1);
	case Input::KEY_NUMPAD2: return gDInput->keyDown(DIK_NUMPAD2);
	case Input::KEY_NUMPAD3: return gDInput->keyDown(DIK_NUMPAD3);
	case Input::KEY_NUMPAD4: return gDInput->keyDown(DIK_NUMPAD4);
	case Input::KEY_NUMPAD5: return gDInput->keyDown(DIK_NUMPAD5);
	case Input::KEY_NUMPAD6: return gDInput->keyDown(DIK_NUMPAD6);
	case Input::KEY_NUMPAD7: return gDInput->keyDown(DIK_NUMPAD7);
	case Input::KEY_NUMPAD8: return gDInput->keyDown(DIK_NUMPAD8);
	case Input::KEY_NUMPAD9: return gDInput->keyDown(DIK_NUMPAD9);
	case Input::KEY_F1: return gDInput->keyDown(DIK_F1);
	case Input::KEY_F2: return gDInput->keyDown(DIK_F2);
	case Input::KEY_F3: return gDInput->keyDown(DIK_F3);
	case Input::KEY_F4: return gDInput->keyDown(DIK_F4);
	case Input::KEY_F5: return gDInput->keyDown(DIK_F5);
	case Input::KEY_F6: return gDInput->keyDown(DIK_F6);
	case Input::KEY_F7: return gDInput->keyDown(DIK_F7);
	case Input::KEY_F8: return gDInput->keyDown(DIK_F8);
	case Input::KEY_F9: return gDInput->keyDown(DIK_F9);
	case Input::KEY_F10: return gDInput->keyDown(DIK_F10);
	case Input::KEY_F11: return gDInput->keyDown(DIK_F11);
	case Input::KEY_F12: return gDInput->keyDown(DIK_F12);
	}
	return false;
}
