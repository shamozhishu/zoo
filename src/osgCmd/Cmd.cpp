#include <osgCmd/Cmd.h>
#include <osgCmd/CmdManager.h>
#include <osgCmd/Utils.h>

namespace osgCmd {

Cmd::Cmd()
{
	_parent = new osg::Group();
	CmdManager::getSingleton().getRootNode()->addChild(_parent);
}

Cmd::~Cmd()
{
	CmdManager::getSingleton().getRootNode()->removeChild(_parent);
}

osg::Group* Cmd::getParent() const
{
	return _parent.get();
}

}
