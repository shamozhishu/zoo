#pragma once

#include <osgCmd/Cmd.h>

namespace osgCmd {

class _osgCmdExport ExitCmd : public Cmd
{
	REFLEX_DECLARE(ExitCmd)
protected:
	bool parseCmdArg(osg::ArgumentParser& cmdarg);

private slots:
	void endMainLoop(const UserData& userdata);
};

}
