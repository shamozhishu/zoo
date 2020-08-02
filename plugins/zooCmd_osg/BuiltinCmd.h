#ifndef __BUILTIN_CMD_H__
#define __BUILTIN_CMD_H__

#include <zooCmd/Cmd.h>
#include <zooCmd_osg/OsgEarthSupport.h>

class BuiltinCmd : public zooCmd::Cmd
{
	ZOO_REFLEX_DECLARE(BuiltinCmd)
public:
	void help(zooCmd::CmdUsage* usage);
	void parse(Signal& subcmd, zooCmd::CmdParser& cmdarg, UserData& returnValue);
};

#endif // __BUILTIN_CMD_H__
