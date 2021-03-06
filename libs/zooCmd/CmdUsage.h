#ifndef __ZOOCMD_CMD_USAGE_H__
#define __ZOOCMD_CMD_USAGE_H__

#include <zooCmd/Support.h>

namespace zooCmd {

class _zooCmdExport CmdUsage
{
public:
	static CmdUsage* instance();
	void setCommandName(const std::string& name) { _commandName = name; }
	const std::string& getCommandName() const { return _commandName; }
	void setDescription(const std::string& desc) { _description = desc; }
	const std::string& getDescription() const { return _description; }

	typedef std::map<std::string, std::string> UsageMap;
	typedef std::vector<std::pair<std::string, std::string>> UsageVec;

	void setCmdProcedureUsage(const std::string& explanation) { _commandProcedureUsage = explanation; }
	const std::string& getCommandProcedureUsage() const { return _commandProcedureUsage; }

	void addCommandProcedureCall(const std::string& procedure, const std::string& explanation, const std::string &defaultValue = "");
	void setCommandProcedureCalls(const UsageVec& usageMap) { _commandProcedureCalls = usageMap; }
	const UsageVec& getCommandProcedureCalls() const { return _commandProcedureCalls; }

	void setCommandProcedureCallsDefaults(const UsageMap& usageMap) { _commandProcedureCallsDefaults = usageMap; }
	const UsageMap& getCommandProcedureCallsDefaults() const { return _commandProcedureCallsDefaults; }

	void getFormattedString(std::string& str, const UsageVec& um, unsigned int widthOfOutput = 80, bool showDefaults = false, const UsageMap& ud = UsageMap());
	void write(std::ostream& output, const UsageVec& um, unsigned int widthOfOutput = 80, bool showDefaults = false, const UsageMap& ud = UsageMap());
	void write(std::ostream& output, unsigned int widthOfOutput = 80, bool showDefaults = false);

protected:
	std::string _commandName;
	std::string _description;
	std::string _commandProcedureUsage;
	UsageVec    _commandProcedureCalls;
	UsageMap    _commandProcedureCallsDefaults;
};

}

#endif // __ZOOCMD_CMD_USAGE_H__
