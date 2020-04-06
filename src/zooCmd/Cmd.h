#pragma once

#include <zoo/Sigslot.h>
#include <zoo/UserData.h>
#include <zoo/Reflection.h>
#include <zooCmd/CmdParser.h>
#include <zooCmd/AppUsage.h>

namespace zooCmd {

class _zooCmdExport Cmd : public Type
{
public:
	virtual bool init() { return true; }
	virtual void parseCmdArg(CmdParser& cmdarg, UserData& retValue) = 0;
	virtual void helpInformation(AppUsage* usage) = 0;

zoo_signals:
	Signal _subCommands;
};

}
