#pragma once

#include <osgCmd/Support.h>
#include <osgCmd/UserData.h>
#include <osgCmd/Singleton.h>
#include <osgCmd/DynLibManager.h>

namespace osgCmd {

class _osgCmdExport CmdManager : public Singleton<CmdManager>, public DynLibManager, public OpenThreads::Thread
{
public:
	CmdManager();
	~CmdManager();
	void run();
	void block(bool isBlock);
	void initBuiltinCmd();
	bool addCmd(const string& cmd, Cmd* pCmd);
	void removeCmd(const string& cmd);
	bool sendCmd(const string& cmdline);
	Cmd* findCmd(const char* cmd);
	Viewers* getViewers() const;
	bool setReturnValue(const string& key, const Any& retval);
	Any  getReturnValue(const string& key);
	bool setErrorMessage(const string& errMessage);
	string getErrorMessage();

private:
	void lazyInitPromise(const string& key);
	void cancelRetValueBlock();

private:
	Cmd*                                   _curCmd;
	string                                 _cmdName;
	bool                                   _busying;
	Viewers*                               _viewers;
	OpenThreads::ReadWriteMutex            _rwMutex;
	OpenThreads::Block                     _block[2];
	unordered_map<string, shared_ptr<Cmd>> _commands;
};

}
