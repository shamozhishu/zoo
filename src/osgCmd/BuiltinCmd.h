#pragma once

#include <osgCmd/Cmd.h>

namespace osgCmd {

class _osgCmdExport BuiltinCmd : public Cmd
{
	REFLEX_DECLARE(BuiltinCmd)
public:
	bool init();
	void parseCmdArg(osg::ArgumentParser& cmdarg, UserData& retValue);
	void helpInformation(osg::ApplicationUsage* usage);

private osgcmd_slots:
	void endMainLoop(const UserData& userdata);
	void loadSceneData(const UserData& userdata);
	void saveSceneData(const UserData& userdata);
	void viewHomePosition(const UserData& userdata);
	void testReturnValue(const UserData& userdata);
};

}
