#pragma once

#include <osgCmd/Support.h>
#include <osgCmd/Sigslot.h>
#include <osgCmd/Reflection.h>

namespace osgCmd {

class _osgCmdExport Cmd : public Type
{
	friend class CmdManager;
public:
	Cmd();
	virtual ~Cmd();
	virtual bool init() { return true; }
	osg::Group* getParent() const;

protected:
	virtual bool parseCmdArg(osg::ArgumentParser& cmdarg) = 0;

protected:
	osg::ref_ptr<osg::Group> _parent;

signals:
	Signal _subCmds;
};

}
