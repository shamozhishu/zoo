#include <osgCmd/BuiltinCmd.h>
#include <osgCmd/Viewers.h>
#include <osgCmd/CmdManager.h>

namespace osgCmd {

REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	CmdManager::getSingleton().getViewers()->realize();
	return true;
}

void BuiltinCmd::parseCmdArg(osg::ArgumentParser& cmdarg)
{
	do
	{
		if (cmdarg.argc() == 2 && cmdarg.read("--exit"))
		{
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::endMainLoop);
			break;
		}

		bool b;
		if (cmdarg.read("--test-bool", b))
		{
			_subCommands.userData().setData("test_bool", b);
			_subCommands.userData().setData("test_mark", 0);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		int i;
		if (cmdarg.read("--test-int", i))
		{
			_subCommands.userData().setData("test_int", i);
			_subCommands.userData().setData("test_mark", 1);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		float f;
		if (cmdarg.read("--test-float", f))
		{
			_subCommands.userData().setData("test_float", f);
			_subCommands.userData().setData("test_mark", 2);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		double d;
		if (cmdarg.read("--test-double", d))
		{
			_subCommands.userData().setData("test_double", d);
			_subCommands.userData().setData("test_mark", 3);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		if (cmdarg.read("--test-str"))
		{
			_subCommands.userData().setData("test_str", string("osgCmd is generated for module decoupling!"));
			_subCommands.userData().setData("test_mark", 4);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		if (cmdarg.read("--test-err"))
		{
			_subCommands.userData().setData(string("Tip: you are using osgCmd for modularity!"));
			_subCommands.userData().setData("test_mark", 5);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

	} while (0);
}

void BuiltinCmd::helpInformation(osg::ApplicationUsage* usage)
{
	usage->setDescription("Built-in commands: encapsulation of many basic functions of OSG.");
	usage->addCommandLineOption("--exit", "End OSG main loop.");
	usage->addCommandLineOption("--test-bool <value:bool> (test_bool:bool)", "Test bool return value.");
	usage->addCommandLineOption("--test-int <value:int> (test_int:int)", "Test int return value.");
	usage->addCommandLineOption("--test-float <value:float> (test_float:float)", "Test float return value.");
	usage->addCommandLineOption("--test-double <value:double> (test_double:double)", "Test double return value.");
	usage->addCommandLineOption("--test-str (test_str:string)", "Test string return value.");
	usage->addCommandLineOption("--test-err (error_message:string)", "Test error tip message.");
}

void BuiltinCmd::endMainLoop(const UserData& userdata)
{
	CmdManager::getSingleton().getViewers()->setDone(true);
}

void BuiltinCmd::testReturnValue(const UserData& userdata)
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
