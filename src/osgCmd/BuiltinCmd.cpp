#include <osgCmd/BuiltinCmd.h>
#include <osgCmd/Renderer.h>
#include <osgCmd/CmdManager.h>

namespace osgCmd {

REFLEX_IMPLEMENT(BuiltinCmd);

bool BuiltinCmd::init()
{
	Renderer* pRenderer = CmdManager::getSingleton().getRenderer();
	osg::Group* pRootNode = pRenderer->getRootNode();

	pRenderer->addEventHandler(new osgGA::StateSetManipulator(pRenderer->getCamera()->getOrCreateStateSet()));

	_clearNode = new osg::ClearNode;
	_clearNode->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	pRootNode->addChild(_clearNode);

	osgUtil::Optimizer optimzer;
	optimzer.optimize(pRootNode);

	pRenderer->setSceneData(pRootNode);
	pRenderer->realize();

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

		float r, g, b, a;
		if (cmdarg.read("--bgcolor", r, g, b, a))
		{
			_subCommands.userData().setData("red", r);
			_subCommands.userData().setData("green", g);
			_subCommands.userData().setData("blue", b);
			_subCommands.userData().setData("alpha", a);
			SignalTrigger::connect<BuiltinCmd>(_subCommands, this, &BuiltinCmd::changeBgColor);
			break;
		}

	} while (0);
}

void BuiltinCmd::helpInformation(osg::ApplicationUsage* usage)
{
	usage->setDescription("Built-in commands: encapsulation of many basic functions of OSG.");
	usage->addCommandLineOption("--exit", "End OSG main loop.");
	usage->addCommandLineOption("--bgcolor <red:float> <green:float> <blue:float> <alpha:float>", "Change background color.");
}

void BuiltinCmd::endMainLoop(const UserData& userdata)
{
	CmdManager::getSingleton().getRenderer()->setDone(true);
}

void BuiltinCmd::changeBgColor(const UserData& userdata)
{
	float r = any_cast<float>(userdata.getData("red"));
	float g = any_cast<float>(userdata.getData("green"));
	float b = any_cast<float>(userdata.getData("blue"));
	float a = any_cast<float>(userdata.getData("alpha"));
	_clearNode->setClearColor(osg::Vec4(r, g, b, a));
}

}
