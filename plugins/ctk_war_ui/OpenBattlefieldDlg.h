#ifndef __OPEN_BATTLEFIELD_DLG_H__
#define __OPEN_BATTLEFIELD_DLG_H__

#include <qdialog.h>

namespace Ui
{
	class OpenBattlefieldDlg;
}

class OpenBattlefieldDlg : public QDialog
{
	Q_OBJECT
public:
	OpenBattlefieldDlg();
	~OpenBattlefieldDlg();
	int getCurBattlefieldID() const;

private:
	int _curBattlefieldID;
	Ui::OpenBattlefieldDlg* _ui;
};

#endif // __OPEN_BATTLEFIELD_DLG_H__
