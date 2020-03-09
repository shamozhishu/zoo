#include <osgCmd.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Reflection.h>
#include <osgCmd/Viewers.h>
#include <osgCmd/DynLib.h>
#include <osgCmd/Utils.h>
#include <osgCmd/Cmd.h>

using namespace osgCmd;
namespace osgCmd
{
	extern std::map<unsigned int, int> g_keyboardMap;
}

typedef const char* (*DllGetCmdTypeName)(void);

static bool s_CmdRegister(const char* cmd)
{
	if (CmdManager::getSingleton().findCmd(cmd))
		return true;

	DynLib* lib = nullptr;
#ifdef _DEBUG
	lib = CmdManager::getSingleton().load("osgCmd-" + string(cmd) + "d");
#else
	lib = CmdManager::getSingleton().load("osgCmd-" + string(cmd));
#endif
	if (!lib)
		return false;

	DllGetCmdTypeName getCmdTypeName = (DllGetCmdTypeName)lib->getSymbol("dllGetCmdTypeName");
	if (!getCmdTypeName)
	{
		OSG_FATAL << cmd << ": Plugin not registered!" << std::endl;
		return false;
	}

	return CmdManager::getSingleton().addCmd(cmd, ReflexFactory<>::getInstance().create<Cmd>(getCmdTypeName()));
}

void osgCmd_Init(int cmdcount, const char* cmdset[], const char* workdir /*= nullptr*/, int windowWidth /*= 0*/, int windowHeight /*= 0*/, float windowScale /*= 1.0f*/)
{
	if (workdir != nullptr && 0 != strcmp(workdir, ""))
		setWorkDir(workdir);

	CmdManager* pCmdMgr = new CmdManager();
	pCmdMgr->getViewers()->init(windowWidth, windowHeight, windowScale);
	
	if (cmdset)
	{
		for (int i = 0; i < cmdcount; ++i)
			s_CmdRegister(cmdset[i]);
	}

	pCmdMgr->initBuiltinCmd();
	pCmdMgr->start();
}

bool osgCmd_Send(const char* cmdline)
{
	return CmdManager::getSingleton().sendCmd(cmdline);
}

void osgCmd_Run()
{
	CmdManager::getSingleton().getViewers()->run();
}

void osgCmd_Destroy()
{
	delete CmdManager::getSingletonPtr();
}

void osgCmd_Render()
{
	CmdManager::getSingleton().getViewers()->frame();
}

void osgCmd_Resize(int windowWidth, int windowHeight, float windowScale)
{
	CmdManager::getSingleton().getViewers()->resize(windowWidth, windowHeight, windowScale);
}

void osgCmd_KeyPressEvent(int key, unsigned int modkey)
{
	CmdManager::getSingleton().getViewers()->keyPressEvent(key, modkey);
}

void osgCmd_KeyReleaseEvent(int key, unsigned int modkey)
{
	CmdManager::getSingleton().getViewers()->keyReleaseEvent(key, modkey);
}

void osgCmd_MousePressEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button)
{
	CmdManager::getSingleton().getViewers()->mousePressEvent(x, y, modkey, button);
}

void osgCmd_MouseReleaseEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button)
{
	CmdManager::getSingleton().getViewers()->mouseReleaseEvent(x, y, modkey, button);
}

void osgCmd_MouseDoubleClickEvent(int x, int y, unsigned int modkey, osgCmd_MouseButton button)
{
	CmdManager::getSingleton().getViewers()->mouseDoubleClickEvent(x, y, modkey, button);
}

void osgCmd_MouseMoveEvent(int x, int y, unsigned int modkey)
{
	CmdManager::getSingleton().getViewers()->mouseMoveEvent(x, y, modkey);
}

void osgCmd_WheelEvent(int x, int y, unsigned int modkey, osgCmd_Scroll scroll)
{
	CmdManager::getSingleton().getViewers()->wheelEvent(x, y, modkey, scroll);
}

bool osgCmd_BoolValue(const char* variable, bool* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<bool>(val);
		return true;
	}

	return false;
}

bool osgCmd_IntValue(const char* variable, int* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<int>(val);
		return true;
	}

	return false;
}

bool osgCmd_FloatValue(const char* variable, float* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<float>(val);
		return true;
	}

	return false;
}

bool osgCmd_DoubleValue(const char* variable, double* value)
{
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		*value = any_cast<double>(val);
		return true;
	}

	return false;
}

const char* osgCmd_StringValue(const char* variable)
{
	static std::string s_returnValue;
	Any val = CmdManager::getSingleton().getReturnValue(variable);
	if (val.has_value())
	{
		s_returnValue = any_cast<string>(val);
		return s_returnValue.c_str();
	}

	return "";
}

const char* osgCmd_ErrorMessage()
{
	static std::string s_errorMessage;
	s_errorMessage = CmdManager::getSingleton().getErrorMessage().c_str();
	return s_errorMessage.c_str();
}

void osgCmd_RemapKeyboard(osgCmd_Key key, int remapkey)
{
	switch (key)
	{
	case osgCmd_Key_Escape: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Escape; break;
	case osgCmd_Key_Delete: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Delete; break;
	case osgCmd_Key_Home: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Home; break;
	case osgCmd_Key_Enter: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_KP_Enter; break;
	case osgCmd_Key_End: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_End; break;
	case osgCmd_Key_Return: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Return; break;
	case osgCmd_Key_PageUp: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Page_Up; break;
	case osgCmd_Key_PageDown: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Page_Down; break;
	case osgCmd_Key_Left: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Left; break;
	case osgCmd_Key_Right: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Right; break;
	case osgCmd_Key_Up: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Up; break;
	case osgCmd_Key_Down: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Down; break;
	case osgCmd_Key_Backspace: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_BackSpace; break;
	case osgCmd_Key_Tab: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Tab; break;
	case osgCmd_Key_Space: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Space; break;
	case osgCmd_Key_Alt: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Alt_L; break;
	case osgCmd_Key_Shift: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Shift_L; break;
	case osgCmd_Key_Control: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Control_L; break;
	case osgCmd_Key_Meta: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_Meta_L; break;
	case osgCmd_Key_F1: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F1; break;
	case osgCmd_Key_F2: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F2; break;
	case osgCmd_Key_F3: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F3; break;
	case osgCmd_Key_F4: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F4; break;
	case osgCmd_Key_F5: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F5; break;
	case osgCmd_Key_F6: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F6; break;
	case osgCmd_Key_F7: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F7; break;
	case osgCmd_Key_F8: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F8; break;
	case osgCmd_Key_F9: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F9; break;
	case osgCmd_Key_F10: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F10; break;
	case osgCmd_Key_F11: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F11; break;
	case osgCmd_Key_F12: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F12; break;
	case osgCmd_Key_F13: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F13; break;
	case osgCmd_Key_F14: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F14; break;
	case osgCmd_Key_F15: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F15; break;
	case osgCmd_Key_F16: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F16; break;
	case osgCmd_Key_F17: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F17; break;
	case osgCmd_Key_F18: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F18; break;
	case osgCmd_Key_F19: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F19; break;
	case osgCmd_Key_F20: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_F20; break;
	case osgCmd_Key_Hyphen: g_keyboardMap[remapkey] = '-'; break;
	case osgCmd_Key_Equal: g_keyboardMap[remapkey] = '='; break;
	case osgCmd_Key_Division: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_KP_Divide; break;
	case osgCmd_Key_Multiply: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_KP_Multiply; break;
	case osgCmd_Key_Minus: g_keyboardMap[remapkey] = '-'; break;
	case osgCmd_Key_Plus: g_keyboardMap[remapkey] = '+'; break;
	case osgCmd_Key_Insert: g_keyboardMap[remapkey] = osgGA::GUIEventAdapter::KEY_KP_Insert; break;
	default: break;
	}
}
