#ifndef __ZOOCMD_BUILTIN_CMD_H__
#define __ZOOCMD_BUILTIN_CMD_H__

#include <zooCmd/Cmd.h>

namespace zooCmd {

class _zooCmdExport BuiltinCmd : public Cmd
{
	ZOO_REFLEX_DECLARE(BuiltinCmd)
public:
	bool init();
	void help(CmdUsage* usage);
	void parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue);

private:
	void onEndMainLoop(const UserData& userdata);
	void onTestReturnValue(const UserData& userdata);
};

}

#endif // __ZOOCMD_BUILTIN_CMD_H__
