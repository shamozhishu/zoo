#pragma once

#include <zooCmd/Support.h>
#include <zoo/Block.h>
#include <zoo/UserData.h>
#include <zoo/Singleton.h>
#include <zoo/DynLibManager.h>

namespace zooCmd {

class _zooCmdExport CmdManager : public Singleton<CmdManager>, public DynLibManager
{
public:
	CmdManager();
	~CmdManager();
	void start();
	void initBuiltinCmd();
	void block(bool isBlock);
	bool addCmd(const string& cmd, Cmd* pCmd);
	void removeCmd(const string& cmd);
	bool sendCmd(const string& cmdline);
	Cmd* findCmd(const char* cmd);
	InputAdapter* getInputAdapter() const;
	static bool setReturnValue(const string& key, const Any& retval);
	static Any  getReturnValue(const string& key);
	static bool setErrorMessage(const string& errMessage);
	static string getErrorMessage();
	static void cancelRetValueBlock();

private:
	static void lazyInitPromise(const string& key);
	void run();

private:
	Cmd*                                   _curCmd;
	thread*                                _thread;
	string                                 _cmdName;
	bool                                   _busying;
	bool                                   _running;
	Block                                  _block[2];
	unordered_map<string, shared_ptr<Cmd>> _commands;
};

}
