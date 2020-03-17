#pragma once

#include <osgCmd/Sigslot.h>
#include <osgCmd/UserData.h>
#include <osgCmd/Reflection.h>

namespace osgCmd {

class _osgCmdExport Cmd : public Type
{
public:
	virtual bool init() { return true; }
	virtual void parseCmdArg(osg::ArgumentParser& cmdarg, UserData& retValue) = 0;
	virtual void helpInformation(osg::ApplicationUsage* usage) = 0;

osgcmd_signals:
	Signal _subCommands;
};

}
