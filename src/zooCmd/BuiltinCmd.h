#pragma once

#include <zooCmd/Cmd.h>

namespace zooCmd {

class _zooCmdExport BuiltinCmd : public Cmd
{
	REFLEX_DECLARE(BuiltinCmd)
public:
	bool init();
	void parseCmdArg(Signal& subCmd, CmdParser& cmdarg, UserData& retValue);
	void helpInformation(CmdUsage* usage);

private:
	void onEndMainLoop(const UserData& userdata);
	void onTestReturnValue(const UserData& userdata);
};

}
