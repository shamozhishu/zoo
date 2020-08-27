#ifndef __OPEN_BATTLEFIELD_DLG_H__
#define __OPEN_BATTLEFIELD_DLG_H__

#include <qdialog.h>

namespace Ui
{
	class OpenBattlefieldDlg;
}

enum EBattlefieldFileType { SceneFile_, ReadyScript_, RunningScript_, PausedScript_, FileCount_ };

class OpenBattlefieldDlg : public QDialog
{
	Q_OBJECT
public:
	OpenBattlefieldDlg();
	~OpenBattlefieldDlg();
	int getCurBattlefieldID() const;
	static QString getCurBattlefieldFile(EBattlefieldFileType fileType);

private:
	int _curBattlefieldID;
	Ui::OpenBattlefieldDlg* _ui;
	static QString _curBattlefieldFile[FileCount_];
};

#endif // __OPEN_BATTLEFIELD_DLG_H__
