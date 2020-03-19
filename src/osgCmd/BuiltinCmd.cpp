#include <osgCmd/BuiltinCmd.h>
#include <osgCmd/Viewers.h>
#include <osgCmd/CmdManager.h>

namespace osgCmd {

REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	Viewers* pViewers = CmdManager::getSingleton().getViewers();
	if (pViewers->getNumViews() == 0)
	{
		osgViewer::View* view = pViewers->createView(0, 1, 0, 1);
		osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
		keyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator);
		keyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator);
		keyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator);
		keyswitchManipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator);
		keyswitchManipulator->addMatrixManipulator('5', "Orbit", new osgGA::OrbitManipulator);
		keyswitchManipulator->addMatrixManipulator('6', "FirstPerson", new osgGA::FirstPersonManipulator);
		keyswitchManipulator->addMatrixManipulator('7', "Spherical", new osgGA::SphericalManipulator);
		view->setCameraManipulator(keyswitchManipulator.get());
	}

	pViewers->realize();
	return true;
}

void BuiltinCmd::parseCmdArg(osg::ArgumentParser& cmdarg, UserData& retValue)
{
	do
	{
		if (cmdarg.argc() == 2 && cmdarg.read("-exit"))
		{
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::endMainLoop);
			break;
		}

		string filename;
		unsigned int idx = 0;
		if (cmdarg.read("--load", filename, idx))
		{
			_subCommands.userData().setData("idx", idx);
			_subCommands.userData().setData("filename", filename);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::loadSceneData);
			break;
		}

		if (cmdarg.read("-load", filename))
		{
			_subCommands.userData().setData("idx", 0U);
			_subCommands.userData().setData("filename", filename);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::loadSceneData);
			break;
		}

		if (cmdarg.read("--save", filename, idx))
		{
			_subCommands.userData().setData("idx", idx);
			_subCommands.userData().setData("filename", filename);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::saveSceneData);
			break;
		}

		if (cmdarg.read("-save", filename))
		{
			_subCommands.userData().setData("idx", 0U);
			_subCommands.userData().setData("filename", filename);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::saveSceneData);
			break;
		}

		if (cmdarg.read("--home", idx))
		{
			_subCommands.userData().setData("idx", idx);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::viewHomePosition);
			break;
		}

		if (cmdarg.read("-home"))
		{
			_subCommands.userData().setData("idx", 0U);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::viewHomePosition);
			break;
		}

		bool b;
		if (cmdarg.read("--test-bool", b))
		{
			retValue.setData("test_bool", false);
			_subCommands.userData().setData("test_bool", b);
			_subCommands.userData().setData("test_mark", 0);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		int i;
		if (cmdarg.read("--test-int", i))
		{
			retValue.setData("test_int", 0);
			_subCommands.userData().setData("test_int", i);
			_subCommands.userData().setData("test_mark", 1);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		float f;
		if (cmdarg.read("--test-float", f))
		{
			retValue.setData("test_float", 0.0f);
			_subCommands.userData().setData("test_float", f);
			_subCommands.userData().setData("test_mark", 2);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		double d;
		if (cmdarg.read("--test-double", d))
		{
			retValue.setData("test_double", 0.0);
			_subCommands.userData().setData("test_double", d);
			_subCommands.userData().setData("test_mark", 3);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::testReturnValue);
			break;
		}

		if (cmdarg.read("--test-str"))
		{
			retValue.setData("test_str", string(""));
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
	usage->addCommandLineOption("-exit", "End OSG main loop.");
	usage->addCommandLineOption("-load <filename:string>", "Load the file to display in the view of the 0 index.");
	usage->addCommandLineOption("--load <filename:string> <viewers_view_index:unsigned int>", "Load the file to display in the view of the specified index.");
	usage->addCommandLineOption("-save <filename:string>", "Save the root node of the 0 index to the specified file.");
	usage->addCommandLineOption("--save <filename:string> <root_node_index:unsigned int>", "Save the root node of the specified index to the specified file.");
	usage->addCommandLineOption("-home", "Set the view of 0 index to the CameraManipulator's home position.");
	usage->addCommandLineOption("--home <viewers_view_index:unsigned int>", "Set the view of specified index to the CameraManipulator's home position.");
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

void BuiltinCmd::loadSceneData(const UserData& userdata)
{
	unsigned int idx = any_cast<unsigned int>(userdata.getData("idx"));
	string filename = any_cast<string>(userdata.getData("filename"));

	osg::Node* node = osgDB::readNodeFile(OSGCMD_DATA_DIR + filename);
	if (!node)
	{
		CmdManager::getSingleton().setErrorMessage("加载指定文件失败！");
		return;
	}

	Viewers* pViewers = CmdManager::getSingleton().getViewers();
	if (idx >= pViewers->getNumViews())
	{
		CmdManager::getSingleton().setErrorMessage("输入的视图索引越界！");
		return;
	}

	pViewers->getView(idx)->setSceneData(node);
}

void BuiltinCmd::saveSceneData(const UserData& userdata)
{
	unsigned int idx = any_cast<unsigned int>(userdata.getData("idx"));
	string filename = any_cast<string>(userdata.getData("filename"));
	osg::Node* node = CmdManager::getSingleton().getViewers()->getRootNode(idx, false);
	if (node)
		osgDB::Registry::instance()->writeNode(*node, OSGCMD_DATA_DIR + filename, osgDB::Registry::instance()->getOptions());
	else
		CmdManager::getSingleton().setErrorMessage("指定索引下的根节点不存在，无法保存当前场景！");
}

void BuiltinCmd::viewHomePosition(const UserData& userdata)
{
	unsigned int idx = any_cast<unsigned int>(userdata.getData("idx"));
	Viewers* pViewers = CmdManager::getSingleton().getViewers();
	if (idx >= pViewers->getNumViews())
	{
		CmdManager::getSingleton().setErrorMessage("输入的视图索引越界！");
		return;
	}

	pViewers->getView(idx)->home();
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
