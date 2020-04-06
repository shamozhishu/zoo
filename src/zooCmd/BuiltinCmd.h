#pragma once

#include <zooCmd/Cmd.h>

namespace zooCmd {

class _zooCmdExport BuiltinCmd : public Cmd
{
	REFLEX_DECLARE(BuiltinCmd)
public:
	bool init();
	void parseCmdArg(CmdParser& cmdarg, UserData& retValue);
	void helpInformation(AppUsage* usage);

private zoo_slots:
	void endMainLoop(const UserData& userdata);
	void testReturnValue(const UserData& userdata);
};

}
