#include "WorldControls.h"
#include <osgCmd/CmdManager.h>
#include <osgCmd/Renderer.h>

using namespace osgEarth::Util::Controls;
static WorldControls* s_worldControls = nullptr;

WorldControls::WorldControls()
{
	ControlCanvas* canvas = ControlCanvas::getOrCreate(osgCmd::CmdManager::getSingleton().getRenderer());
	_vboxControl = new VBox();
	_vboxControl->setBorderColor(0, 0, 0, 0);
	_vboxControl->setBackColor(0.6f, 0.5f, 0.4f, 0.5f);
	_vboxControl->setPadding(10);
	_vboxControl->setHorizAlign(Control::ALIGN_LEFT);
	_vboxControl->setVertAlign(Control::ALIGN_TOP);
	_vboxControl->setSize(500, 0);
	canvas->addControl(_vboxControl.get());

	for (int i = lla_label_; i < count_label_; ++i)
		_labelControls[i] = nullptr;
}

WorldControls::~WorldControls()
{
	ControlCanvas* canvas = ControlCanvas::getOrCreate(osgCmd::CmdManager::getSingleton().getRenderer());
	canvas->removeControl(_vboxControl.get());
}

WorldControls* WorldControls::getIns()
{
	if (!s_worldControls)
		s_worldControls = new WorldControls();
	return s_worldControls;
}

void WorldControls::destroy()
{
	SAFE_DELETE(s_worldControls);
}

void WorldControls::addLabelTextDisplay(const string& text, LabelIndex idx)
{
	if (idx < count_label_)
	{
		if (_labelControls[idx])
			_labelControls[idx]->setText(text);
		else
		{
			_labelControls[idx] = _vboxControl->addControl(new LabelControl(text, osg::Vec4f(1.0f, 0.1f, 0.1f, 1.0f), 14));
			_vboxControl->setVisible(true);
			_vboxControl->dirty();
		}
	}
}

void WorldControls::removeLabelTextDisplay(LabelIndex idx)
{
	if (idx < count_label_ && _labelControls[idx])
	{
		_vboxControl->removeChild(_labelControls[idx]);
		_labelControls[idx] = nullptr;
		_vboxControl->dirty();
	}

	bool isshow = false;
	for (int i = lla_label_; i < count_label_; ++i)
	{
		if (_labelControls[i])
		{
			isshow = true;
			break;
		}
	}

	if (!isshow)
		_vboxControl->setVisible(false);
}
