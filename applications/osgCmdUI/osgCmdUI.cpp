#include "osgCmdUI.h"
#include <osgCmd.h>

static const char* szCmdset[] = { "World" };

osgCmdUI::osgCmdUI(QWidget *parent)
	: QMainWindow(parent)
	, _widget(1, szCmdset, "../../resource", this)
{
	ui.setupUi(this);
	setCentralWidget(&_widget);
}
