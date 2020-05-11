#pragma once

#include <zoo/Support.h>

#if defined(ZOOCMD_NONCLIENT_BUILD)
#	define _zooCmdExport __declspec(dllexport)
#else
#	if defined(__MINGW32__)
#		define _zooCmdExport
#	else
#		define _zooCmdExport __declspec(dllimport)
#	endif
#endif

using namespace zoo;

namespace zooCmd {

class Cmd;
class CmdUsage;
class CmdParser;
class CmdManager;
class BuiltinCmd;
class InputAdapter;
template class _zooCmdExport ReflexFactory<>;

}
