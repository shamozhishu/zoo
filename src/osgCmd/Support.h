#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>
#include <typeinfo>
#include <memory>
// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
}

#include <osgCmd/Common.h>

#include <osg/Group>
#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgGA/StateSetManipulator>
#include <OpenThreads/ReadWriteMutex>
#ifdef NEED_OSGEARTH_LIBRARY
#include <osgEarth/MapNode>
#include <osgEarth/SpatialReference>
#include <osgEarth/TerrainEngineNode>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Controls>
#endif
#ifdef _DEBUG
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "osgViewerd.lib")
#pragma comment(lib, "osgUtild.lib")
#pragma comment(lib, "osgSimd.lib")
#pragma comment(lib, "osgTerraind.lib")
#pragma comment(lib, "osgFXd.lib")
#pragma comment(lib, "osgTextd.lib")
#pragma comment(lib, "osgGAd.lib")
#pragma comment(lib, "OpenThreadsd.lib")
#ifdef NEED_OSGEARTH_LIBRARY
#pragma comment(lib, "osgEarthd.lib")
#pragma comment(lib, "osgEarthUtild.lib")
#endif
#else
#pragma comment(lib, "osg.lib")
#pragma comment(lib, "osgDB.lib")
#pragma comment(lib, "osgViewer.lib")
#pragma comment(lib, "osgUtil.lib")
#pragma comment(lib, "osgSim.lib")
#pragma comment(lib, "osgTerrain.lib")
#pragma comment(lib, "osgFX.lib")
#pragma comment(lib, "osgText.lib")
#pragma comment(lib, "osgGA.lib")
#pragma comment(lib, "OpenThreads.lib")
#ifdef NEED_OSGEARTH_LIBRARY
#pragma comment(lib, "osgEarth.lib")
#pragma comment(lib, "osgEarthUtil.lib")
#endif
#endif

using namespace std;

namespace osgCmd {

enum MouseButton
{
	NoButton = 0,
	LeftButton,
	MidButton,
	RightButton
};

enum Key
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

enum Scroll
{
	SCROLL_NONE = 0,
	SCROLL_LEFT,
	SCROLL_RIGHT,
	SCROLL_UP,
	SCROLL_DOWN,
	SCROLL_2D
};

template<typename ...Args> class ReflexFactory;
template class _osgCmdExport ReflexFactory<>;
class Cmd;
class BuiltinCmd;
class CmdManager;
class Renderer;
class Any;
class AnyNumeric;
class Signal;
class SignalTrigger;
class UserData;
class DynLib;
class DynLibManager;

}
