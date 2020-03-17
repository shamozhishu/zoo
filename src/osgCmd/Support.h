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
#include <future>
#include <chrono>
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
#include <osg/LineWidth>
#include <osg/MatrixTransform>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
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

#ifdef OSG_USE_UTF8_FILENAME
#define OSGCMD_DATA_DIR osgCmd::utf8_data_dir
#else
#define OSGCMD_DATA_DIR osgCmd::ansi_data_dir
#endif

using namespace std;

namespace osgCmd {

_osgCmdExport extern std::string ansi_data_dir;
_osgCmdExport extern std::string utf8_data_dir;
template<typename ...Args> class ReflexFactory;
template class _osgCmdExport ReflexFactory<>;
class Cmd;
class BuiltinCmd;
class CmdManager;
class Viewers;
class Any;
class AnyNumeric;
class Signal;
class SignalTrigger;
class UserData;
class DynLib;
class DynLibManager;

}
