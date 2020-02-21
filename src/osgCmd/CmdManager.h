#pragma once

#include <osgCmd/Support.h>
#include <osgCmd/Singleton.h>
#include <osgCmd/DynLibManager.h>

namespace osgCmd {

class _osgCmdExport CmdManager : public Singleton<CmdManager>, public DynLibManager, public OpenThreads::Thread
{
public:
	CmdManager();
	~CmdManager();
	void initBuiltInCmd();
	bool addCmd(const string& cmd, Cmd* pCmd);
	void removeCmd(const string& cmd);
	void run();
	bool sendCmd(const vector<string>& arglist);
	osg::Group* getRootNode() const;
	Renderer* getRenderer() const;

	template<typename T>
	T* findCmd(const string& cmd)
	{
		return dynamic_cast<T*>(findCmd(cmd.c_str()));
	}

protected:
	Cmd* findCmd(const char* cmd);

private:
	Cmd*                                   _curCmd;
	bool                                   _busying;
	Renderer*                              _renderer;
	OpenThreads::Block                     _cmdBlock;
	osg::ref_ptr<osg::Group>               _rootNode;
	unordered_map<string, shared_ptr<Cmd>> _commands;
};

}
