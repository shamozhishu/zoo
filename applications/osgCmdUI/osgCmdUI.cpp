#include "osgCmdUI.h"
#include <osgCmd.h>
#include <QLineEdit>
#include <QPushButton>

static const char* szCmdset[] = { "World" };

osgCmdUI::osgCmdUI(QWidget *parent)
	: QMainWindow(parent)
	, _widget(1, szCmdset, "../../resource", this)
{
	ui.setupUi(this);
	setCentralWidget(&_widget);

	_cmdlineEdit = new QLineEdit();
	ui.statusBar->addPermanentWidget(_cmdlineEdit);

	QPushButton* sendBtn = new QPushButton(tr("Send"));
	ui.statusBar->addPermanentWidget(sendBtn);
	connect(sendBtn, SIGNAL(clicked()), this, SLOT(testCommand()));
}

void osgCmdUI::testCommand()
{
	std::string cmdline = _cmdlineEdit->text().toStdString();
	if (cmdline.find("--exit") == -1)
		osgCmd_Send(cmdline.c_str());
}
