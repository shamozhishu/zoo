#ifndef __ZOOCMD_CMD_MANAGER_H__
#define __ZOOCMD_CMD_MANAGER_H__

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
	void refresh();
	void block(bool isBlock);
	void initBuiltinCmd();
	bool addCmd(const string& cmd, Cmd* pCmd);
	void removeCmd(const string& cmd);
	vector<string> removeAllCmds();
	bool sendCmd(const string& cmdline);
	Cmd* findCmd(const string& cmdname);
	void sendEvent(const string& topic, const UserData& props = nullptr);

public:
	static void waitExchanged();
	static void releaseWait();
public:
	static bool setReturnValue(const string& key, const Any& retval);
	static Any  getReturnValue(const string& key);
	static void setTipMessage(const string& tips);
	static string getTipMessage();

private:
	void runCmd();
	static void releaseBlockAndRetValue();
	static void lazyInitPromise(const string& key);

private:
	bool _running;
	bool _isBlock;
	Cmd* _lastCmd;
	string _lastCmdName;
	thread* _cmdThread;
	zoo::Block _block[2];
	queue<shared_ptr<Signal>> _cmdQueue;
	unordered_map<string, shared_ptr<Cmd>> _commands;
};

}

#endif // __ZOOCMD_CMD_MANAGER_H__
