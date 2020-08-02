#ifndef __ZOOCMD_CMD_H__
#define __ZOOCMD_CMD_H__

#include <zoo/Sigslot.h>
#include <zoo/UserData.h>
#include <zoo/Reflection.h>
#include <zooCmd/CmdUsage.h>
#include <zooCmd/CmdParser.h>

namespace zooCmd {

class Event
{
	PROPERTY_R_REF(string, _topic, Topic)
	PROPERTY_R_REF(UserData, _props, Props)
public:
	Event(string topic, const UserData& props) : _topic(topic), _props(props) {}
};

class _zooCmdExport Cmd : public Type
{
public:
	virtual bool init() { return true; }
	virtual void refresh() {}
	virtual void handle(const Event& evt) { (void)evt; }
	virtual void help(CmdUsage* usage) = 0;
	virtual void parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue) = 0;
};

}

#endif // __ZOOCMD_CMD_H__
