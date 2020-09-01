#ifndef __EARTH_CONTROLS_H__
#define __EARTH_CONTROLS_H__

#include <zooCmd_osg/OsgEarthSupport.h>

enum LabelIndex
{
	lla_label_,
	ipt_label_,
	dist_label_,
	count_label_
};

class EarthControls
{
public:
	EarthControls();
	~EarthControls();
	static EarthControls* getIns();
	static void destroy();
	bool isHasLabelControl(LabelIndex idx) const;
	void addLabelTextDisplay(const string& text, LabelIndex idx);
	void removeLabelTextDisplay(LabelIndex idx);

private:
	osg::ref_ptr<osgEarth::Util::Controls::VBox> _vboxControl;
	osg::ref_ptr<osgEarth::Util::Controls::ControlCanvas> _canvas;
	osgEarth::Util::Controls::LabelControl* _labelControls[count_label_];
};

#endif // __EARTH_CONTROLS_H__
