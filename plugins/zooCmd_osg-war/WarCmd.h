#ifndef __WAR_CMD_H__
#define __WAR_CMD_H__

#include <zooCmd/Cmd.h>
#include "LonLatDistHandler.h"

class OsgEarthContext;
class WarCmd : public zooCmd::Cmd
{
	ZOO_REFLEX_DECLARE(WarCmd)
public:
	WarCmd();
	~WarCmd();
	bool init();
	void handle(const zooCmd::Event& evt);
	void help(zooCmd::CmdUsage* usage);
	void parse(Signal& subcmd, zooCmd::CmdParser& cmdarg, zoo::UserData& returnValue);

private:
	OsgEarthContext* _osgEarthContext;
	osg::ref_ptr<LonLatDistHandler> _lonLatHandler;
};

#endif // __WAR_CMD_H__
