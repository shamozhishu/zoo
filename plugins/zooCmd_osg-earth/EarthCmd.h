#ifndef __EARTH_CMD_H__
#define __EARTH_CMD_H__

#include <zooCmd/Cmd.h>
#include "LonLatDistHandler.h"

class OsgEarthContext;
class EarthCmd : public zooCmd::Cmd
{
	ZOO_REFLEX_DECLARE(EarthCmd)
public:
	EarthCmd();
	~EarthCmd();
	bool init();
	void handle(const zooCmd::Event& evt);
	void help(zooCmd::CmdUsage* usage);
	void parse(Signal& subcmd, zooCmd::CmdParser& cmdarg, zoo::UserData& returnValue);

private:
	OsgEarthContext* _osgEarthContext;
	osg::ref_ptr<LonLatDistHandler> _lonLatHandler;
};

#endif // __EARTH_CMD_H__
