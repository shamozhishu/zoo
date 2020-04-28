#pragma once

#include <zooCmd/Support.h>
#include <zooCmd/Cmd.h>
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
	void update();
	void initBuiltinCmd();
	void block(bool isBlock);
	bool addCmd(const string& cmd, Cmd* pCmd);
	void removeCmd(const string& cmd);
	bool sendCmd(const string& cmdline);
	Cmd* findCmd(const string& cmdname);
	InputAdapter* getInputAdapter() const;

public:
	static void waitExchanged();
	static void releaseWait();
public:
	static bool setReturnValue(const string& key, const Any& retval);
	static Any  getReturnValue(const string& key);
	static bool setErrorMessage(const string& errMessage);
	static string getErrorMessage();
	static void cancelRetValueBlock();

private:
	void runCmd();
	static void lazyInitPromise(const string& key);

private:
	bool _running;
	Cmd* _lastCmd;
	string _lastCmdName;
	thread* _cmdThread;
	zoo::Block _block[2];
	queue<shared_ptr<Signal>> _cmdQueue;
	unordered_map<string, shared_ptr<Cmd>> _commands;
};

}
