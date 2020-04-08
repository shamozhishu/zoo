#pragma once

#include <zoo/Sigslot.h>
#include <zoo/UserData.h>
#include <zoo/Reflection.h>
#include <zooCmd/CmdUsage.h>
#include <zooCmd/CmdParser.h>

namespace zooCmd {

class _zooCmdExport Cmd : public Type
{
public:
	virtual bool init() { return true; }
	virtual void parseCmdArg(CmdParser& cmdarg, UserData& retValue) = 0;
	virtual void helpInformation(CmdUsage* usage) = 0;

zoo_signals:
	Signal _subCommands;
};

}
