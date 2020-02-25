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
	void initBuiltinCmd();
	bool addCmd(const string& cmd, Cmd* pCmd);
	void removeCmd(const string& cmd);
	void run();
	void block(bool isBlock);
	bool sendCmd(const string& cmdline);
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
	string                                 _cmdName;
	bool                                   _busying;
	Renderer*                              _renderer;
	OpenThreads::Block                     _block[2];
	unordered_map<string, shared_ptr<Cmd>> _commands;
};

}
