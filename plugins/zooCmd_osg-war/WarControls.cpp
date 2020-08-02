#include "WarControls.h"
#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth::Util::Controls;
static WarControls* s_worldControls = nullptr;

WarControls::WarControls()
{
	_canvas = ControlCanvas::getOrCreate(ServiceLocator<OsgEarthContext>::getService()->getOpView());
	_vboxControl = new VBox();
	_vboxControl->setBorderColor(0, 0, 0, 0);
	_vboxControl->setBackColor(0.6f, 0.5f, 0.4f, 0.5f);
	_vboxControl->setPadding(10);
	_vboxControl->setHorizAlign(Control::ALIGN_LEFT);
	_vboxControl->setVertAlign(Control::ALIGN_TOP);
	_vboxControl->setSize(500, 0);
	_vboxControl->setVisible(false);

	for (int i = lla_label_; i < count_label_; ++i)
		_labelControls[i] = nullptr;
}

WarControls::~WarControls()
{
	_canvas->removeControl(_vboxControl.get());
	if (_canvas->getNumParents() > 0)
	{
		auto node = _canvas->getParent(0);
		if (node)
			node->asGroup()->removeChild(_canvas.get());
	}
}

WarControls* WarControls::getIns()
{
	if (!s_worldControls)
		s_worldControls = new WarControls();
	return s_worldControls;
}

void WarControls::destroy()
{
	SAFE_DELETE(s_worldControls);
}

bool WarControls::isHasLabelControl(LabelIndex idx) const
{
	if (idx < count_label_)
		return _labelControls[idx] != nullptr;
	return false;
}

void WarControls::addLabelTextDisplay(const string& text, LabelIndex idx)
{
	if (idx < count_label_)
	{
		if (_labelControls[idx])
			_labelControls[idx]->setText(text);
		else
		{
			_labelControls[idx] = _vboxControl->addControl(new LabelControl(text, osg::Vec4f(1.0f, 0.1f, 0.1f, 1.0f), 14));
			if (!_vboxControl->visible())
				_canvas->addControl(_vboxControl.get());
			_vboxControl->setVisible(true);
			_vboxControl->dirty();
		}
	}
}

void WarControls::removeLabelTextDisplay(LabelIndex idx)
{
	if (idx < count_label_ && _labelControls[idx])
	{
		_vboxControl->removeChild(_labelControls[idx]);
		_labelControls[idx] = nullptr;
		_vboxControl->dirty();
	}

	if (_vboxControl->visible())
	{
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
		{
			_vboxControl->setVisible(false);
			_canvas->removeControl(_vboxControl.get());
		}
	}
}
