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

#include <zoo/Common.h>

using namespace std;

namespace zoo {

class _zooExport Type
{
public:
	virtual ~Type() {}
	virtual string typeName() const;
protected:
	Type() {}
};

template<typename ...Args> class ReflexFactory;
class Log;
class Any;
class AnyNumeric;
class Block;
class Timer;
class Entity;
class EntityImpl;
class Component;
class ComponentImpl;
class Signal;
class SignalTrigger;
class Interlock;
class UserData;
class DynLib;
class DynLibManager;
class TableCSV;
class DatabaseCSV;

}
