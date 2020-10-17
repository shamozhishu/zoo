#ifndef __BUILTIN_EVENT_HANDLER_H__
#define __BUILTIN_EVENT_HANDLER_H__

#include <zooCmd_osg/OsgDevice.h>
#include "PublicFunctions.h"

class OsgEarthContext;
class ShowCursorWPosHandler : public PickHandler
{
public:
	ShowCursorWPosHandler(OsgEarthContext* context);
	~ShowCursorWPosHandler();
	void doUserOperations(osgUtil::LineSegmentIntersector::Intersection& result);

private:
	OsgEarthContext* _context;
	osg::ref_ptr<osgText::Text> _text;
	osg::ref_ptr<osg::Camera> _textHudCam;
};

#endif // __BUILTIN_EVENT_HANDLER_H__
