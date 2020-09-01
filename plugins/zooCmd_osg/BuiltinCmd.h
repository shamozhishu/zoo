#ifndef __BUILTIN_CMD_H__
#define __BUILTIN_CMD_H__

#include <zooCmd/Cmd.h>
#include <zooCmd_osg/OsgEarthSupport.h>
#include <osgViewer/ViewerEventHandlers>

class BuiltinCmd : public zooCmd::Cmd
{
	ZOO_REFLEX_DECLARE(BuiltinCmd)
public:
	bool init();
	void help(zooCmd::CmdUsage* usage);
	void parse(Signal& subcmd, zooCmd::CmdParser& cmdarg, UserData& returnValue);

private:
	osg::ref_ptr<osgViewer::ScreenCaptureHandler> _screenshotHandler;
};

#endif // __BUILTIN_CMD_H__
