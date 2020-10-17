#include "BuiltinEventHandler.h"
#include <zooCmd_osg/OsgEarthContext.h>

ShowCursorWPosHandler::ShowCursorWPosHandler(OsgEarthContext* context)
	: _context(context)
{
	_textHudCam = createHUDCamera(0, 800, 0, 600);
	_text = createText(osg::Vec3d(5, 5, 0), "", 10);
	_textHudCam->addChild(_text);
	_context->getSceneNode()->addChild(_textHudCam);
}

ShowCursorWPosHandler::~ShowCursorWPosHandler()
{
	_context->getSceneNode()->removeChild(_textHudCam);
}

void ShowCursorWPosHandler::doUserOperations(osgUtil::LineSegmentIntersector::Intersection& result)
{
	osg::Vec3d wpos = result.getWorldIntersectPoint();
	string strWPos = string("XPos: ") + std::to_string(wpos.x()) + "; YPos: " + std::to_string(wpos.y()) + "; ZPos: " + std::to_string(wpos.z());
	_text->setText(strWPos);
}
