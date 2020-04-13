#include <zooCmd/BuiltinCmd.h>
#include <zooCmd/InputAdapter.h>
#include <zooCmd/CmdManager.h>

namespace zooCmd {

REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	return CmdManager::getSingleton().getInputAdapter()->init();
}

void BuiltinCmd::parseCmdArg(Signal& subCmd, CmdParser& cmdarg, UserData& retValue)
{
	do
	{
		if (cmdarg.argc() == 2 && cmdarg.read("-exit"))
		{
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onEndMainLoop);
			break;
		}

		bool b;
		if (cmdarg.read("--test-bool", b))
		{
			retValue.setData("test_bool", false);
			subCmd.userData().setData("test_bool", b);
			subCmd.userData().setData("test_mark", 0);
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		int i;
		if (cmdarg.read("--test-int", i))
		{
			retValue.setData("test_int", 0);
			subCmd.userData().setData("test_int", i);
			subCmd.userData().setData("test_mark", 1);
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		float f;
		if (cmdarg.read("--test-float", f))
		{
			retValue.setData("test_float", 0.0f);
			subCmd.userData().setData("test_float", f);
			subCmd.userData().setData("test_mark", 2);
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		double d;
		if (cmdarg.read("--test-double", d))
		{
			retValue.setData("test_double", 0.0);
			subCmd.userData().setData("test_double", d);
			subCmd.userData().setData("test_mark", 3);
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		if (cmdarg.read("--test-str"))
		{
			retValue.setData("test_str", string(""));
			subCmd.userData().setData("test_str", string("zooCmd is generated for module decoupling!"));
			subCmd.userData().setData("test_mark", 4);
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

		if (cmdarg.read("--test-err"))
		{
			subCmd.userData().setData(string("Tip: you are using zooCmd for modularity!"));
			subCmd.userData().setData("test_mark", 5);
			SignalTrigger::connect<BuiltinCmd>(subCmd, this, &BuiltinCmd::onTestReturnValue);
			break;
		}

	} while (0);
}

void BuiltinCmd::helpInformation(CmdUsage* usage)
{
	usage->setDescription("Built-in commands: encapsulation of many basic functions.");
	usage->addCommandLineOption("-exit", "End main loop.");
	usage->addCommandLineOption("--test-bool <value:bool> (test_bool:bool)", "Test bool return value.");
	usage->addCommandLineOption("--test-int <value:int> (test_int:int)", "Test int return value.");
	usage->addCommandLineOption("--test-float <value:float> (test_float:float)", "Test float return value.");
	usage->addCommandLineOption("--test-double <value:double> (test_double:double)", "Test double return value.");
	usage->addCommandLineOption("--test-str (test_str:string)", "Test string return value.");
	usage->addCommandLineOption("--test-err (error_message:string)", "Test error tip message.");
}

void BuiltinCmd::onEndMainLoop(const UserData& userdata)
{
	CmdManager::getSingleton().getInputAdapter()->setDone(true);
}

void BuiltinCmd::onTestReturnValue(const UserData& userdata)
{
	int mark = any_cast<int>(userdata.getData("test_mark"));
	switch (mark)
	{
	case 0:
		CmdManager::getSingleton().setReturnValue("test_bool", any_cast<bool>(userdata.getData("test_bool")));
		break;
	case 1:
		CmdManager::getSingleton().setReturnValue("test_int", any_cast<int>(userdata.getData("test_int")));
		break;
	case 2:
		CmdManager::getSingleton().setReturnValue("test_float", any_cast<float>(userdata.getData("test_float")));
		break;
	case 3:
		CmdManager::getSingleton().setReturnValue("test_double", any_cast<double>(userdata.getData("test_double")));
		break;
	case 4:
		CmdManager::getSingleton().setReturnValue("test_str", any_cast<string>(userdata.getData("test_str")));
		break;
	case 5:
		CmdManager::getSingleton().setErrorMessage(any_cast<string>(userdata.getData()));
		break;
	default:
		break;
	}
}

}
