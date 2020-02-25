#pragma once

#include <osgCmd/Support.h>

enum LabelIndex
{
	lla_label_,
	ipt_label_,
	dist_label_,
	count_label_
};

class WorldControls
{
public:
	WorldControls();
	~WorldControls();
	static WorldControls* getIns();
	static void destroy();
	void addLabelTextDisplay(const string& text, LabelIndex idx);
	void removeLabelTextDisplay(LabelIndex idx);

private:
	osg::ref_ptr<osgEarth::Util::Controls::VBox> _vboxControl;
	osgEarth::Util::Controls::LabelControl* _labelControls[count_label_];
};
