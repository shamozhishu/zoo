#ifndef __ZOOCMD_SUPPORT_H__
#define __ZOOCMD_SUPPORT_H__

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

}

#endif // __ZOOCMD_SUPPORT_H__
