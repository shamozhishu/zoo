#include <zooCmd/BuiltinCmd.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd/CmdManager.h>

namespace zooCmd {

ZOO_REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	return CmdManager::getSingleton().getInputAdapter()->init();
}

void BuiltinCmd::help(CmdUsage* usage)
{
	usage->setDescription("Built-in commands: encapsulation of many basic functions.");
	usage->addCommandProcedureCall("Exit()", "End main loop.");
	usage->addCommandProcedureCall("Bool(bool) // bool& b", "Test bool return value.");
	usage->addCommandProcedureCall("Int(int) // int& i", "Test int return value.");
	usage->addCommandProcedureCall("Float(float) // float& f", "Test float return value.");
	usage->addCommandProcedureCall("Double(double) // double& d", "Test double return value.");
	usage->addCommandProcedureCall("Str(string) // string& str", "Test string return value.");
	usage->addCommandProcedureCall("Err(string)", "Test error tip message.");
}

void BuiltinCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	do
	{
		if (cmdarg.read("Exit"))
		{
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onEndMainLoop);
			break;
		}

		bool b;
		if (cmdarg.read("Bool", b))
		{
			returnValue.setData("b", false);
			subcmd.userData().setData("b", b);
			subcmd.userData().setData("mark", 0);
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		int i;
		if (cmdarg.read("Int", i))
		{
			returnValue.setData("i", 0);
			subcmd.userData().setData("i", i);
			subcmd.userData().setData("mark", 1);
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		float f;
		if (cmdarg.read("Float", f))
		{
			returnValue.setData("f", 0.0f);
			subcmd.userData().setData("f", f);
			subcmd.userData().setData("mark", 2);
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		double d;
		if (cmdarg.read("Double", d))
		{
			returnValue.setData("d", 0.0);
			subcmd.userData().setData("d", d);
			subcmd.userData().setData("mark", 3);
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		string str;
		if (cmdarg.read("Str", str))
		{
			returnValue.setData("str", string(""));
			subcmd.userData().setData("str", str);
			subcmd.userData().setData("mark", 4);
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		if (cmdarg.read("Err", str))
		{
			subcmd.userData().setData(str);
			subcmd.userData().setData("mark", 5);
			SignalTrigger::connect<BuiltinCmd>(subcmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

	} while (0);
}

void BuiltinCmd::onEndMainLoop(const UserData& userdata)
{
	CmdManager::getSingleton().getInputAdapter()->setDone(true);
}

void BuiltinCmd::onTestReturnValue(const UserData& userdata)
{
	int mark = any_cast<int>(userdata.getData("mark"));
	switch (mark)
	{
	case 0:
		CmdManager::getSingleton().setReturnValue("b", any_cast<bool>(userdata.getData("b")));
		break;
	case 1:
		CmdManager::getSingleton().setReturnValue("i", any_cast<int>(userdata.getData("i")));
		break;
	case 2:
		CmdManager::getSingleton().setReturnValue("f", any_cast<float>(userdata.getData("f")));
		break;
	case 3:
		CmdManager::getSingleton().setReturnValue("d", any_cast<double>(userdata.getData("d")));
		break;
	case 4:
		CmdManager::getSingleton().setReturnValue("str", string("\"") + any_cast<string>(userdata.getData("str")) + "\"");
		break;
	case 5:
		CmdManager::getSingleton().setErrorMessage(any_cast<string>(userdata.getData()));
		break;
	default:
		break;
	}
}

}
