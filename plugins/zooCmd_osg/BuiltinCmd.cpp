#include "BuiltinCmd.h"
#include "InputDevice.h"
#include <zooCmd/CmdManager.h>

using namespace zooCmd;

ZOO_REFLEX_IMPLEMENT(BuiltinCmd);

void BuiltinCmd::help(CmdUsage* usage)
{
	usage->setDescription("built-in commands: encapsulation of many basic functions.");
	usage->addCommandProcedureCall("home(string view_name)", "摄像机[view_name]复位");
	usage->addCommandProcedureCall("exit()", "End main loop.");
	usage->addCommandProcedureCall("bool(bool b) // bool& b", "Test bool return value.");
	usage->addCommandProcedureCall("int(int i) // int& i", "Test int return value.");
	usage->addCommandProcedureCall("float(float f) // float& f", "Test float return value.");
	usage->addCommandProcedureCall("double(double d) // double& d", "Test double return value.");
	usage->addCommandProcedureCall("string(string str) // string& str", "Test string return value.");
	usage->addCommandProcedureCall("tips(string tip)", "Test tip message.");
}

void BuiltinCmd::parse(Signal& subcmd, CmdParser& cmdarg, UserData& returnValue)
{
	do
	{
		string viewname;
		if (cmdarg.read("home", viewname))
		{
			SignalTrigger::connect(subcmd, [viewname]
			{
				osgViewer::View* pView = ServiceLocator<OsgDevice>::getService()->getView(viewname);
				if (!pView)
				{
					CmdManager::setTipMessage("视图[" + viewname + "]不存在！");
					return;
				}

				pView->getCameraManipulator()->home(2);
			});
			break;
		}

		if (cmdarg.read("exit"))
		{
			SignalTrigger::connect(subcmd, []
			{
				ServiceLocator<OsgDevice>::getService()->as<InputAdapter>()->setDone(true);
			});
			break;
		}

		bool b;
		if (cmdarg.read("bool", b))
		{
			returnValue.setData("b", false);
			SignalTrigger::connect(subcmd, [b]
			{
				CmdManager::getSingleton().setReturnValue("b", b);
				zoo_info("return value is %s", b ? "true" : "false");
			});
			break;
		}

		int i;
		if (cmdarg.read("int", i))
		{
			returnValue.setData("i", 0);
			SignalTrigger::connect(subcmd, [i]
			{
				CmdManager::getSingleton().setReturnValue("i", i);
				zoo_info("return value is %d", i);
			});
			break;
		}

		float f;
		if (cmdarg.read("float", f))
		{
			returnValue.setData("f", 0.0f);
			SignalTrigger::connect(subcmd, [f]
			{
				CmdManager::getSingleton().setReturnValue("f", f);
				zoo_info("return value is %.3f", f);
			});
			break;
		}

		double d;
		if (cmdarg.read("double", d))
		{
			returnValue.setData("d", 0.0);
			SignalTrigger::connect(subcmd, [d]
			{
				CmdManager::getSingleton().setReturnValue("d", d);
				zoo_info("return value is %.6f", d);
			});
			break;
		}

		string str;
		if (cmdarg.read("string", str))
		{
			returnValue.setData("str", string(""));
			SignalTrigger::connect(subcmd, [str]
			{
				CmdManager::getSingleton().setReturnValue("str", string("\"") + str + "\"");
				zoo_info("return value is %s", (string("\"") + str + "\"").c_str());
			});
			break;
		}

		if (cmdarg.read("tips", str))
		{
			SignalTrigger::connect(subcmd, [str]
			{
				CmdManager::getSingleton().setTipMessage(str);
				zoo_info("tip message is %s", (string("\"") + str + "\"").c_str());
			});
			break;
		}
	} while (0);
}
