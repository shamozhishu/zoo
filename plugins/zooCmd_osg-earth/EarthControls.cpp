#include "EarthControls.h"
#include <zoo/ServiceLocator.h>
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgEarth::Util::Controls;
static EarthControls* s_earthControls = nullptr;

EarthControls::EarthControls()
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

EarthControls::~EarthControls()
{
	_canvas->removeControl(_vboxControl.get());
	if (_canvas->getNumParents() > 0)
	{
		auto node = _canvas->getParent(0);
		if (node)
			node->asGroup()->removeChild(_canvas.get());
	}
}

EarthControls* EarthControls::getIns()
{
	if (!s_earthControls)
		s_earthControls = new EarthControls();
	return s_earthControls;
}

void EarthControls::destroy()
{
	SAFE_DELETE(s_earthControls);
}

bool EarthControls::isHasLabelControl(LabelIndex idx) const
{
	if (idx < count_label_)
		return _labelControls[idx] != nullptr;
	return false;
}

void EarthControls::addLabelTextDisplay(const string& text, LabelIndex idx)
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

void EarthControls::removeLabelTextDisplay(LabelIndex idx)
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
