#pragma once

#include <osgCmd/Cmd.h>

namespace osgCmd {

class _osgCmdExport BuiltinCmd : public Cmd
{
	REFLEX_DECLARE(BuiltinCmd)
public:
	bool init();
	void parseCmdArg(osg::ArgumentParser& cmdarg);
	void helpInformation(osg::ApplicationUsage* usage);
private slots:
	void endMainLoop(const UserData& userdata);
	void changeBgColor(const UserData& userdata);
private:
	osg::ref_ptr<osg::ClearNode> _clearNode;
};

}
