#ifndef __OPEN_BATTLEFIELD_DLG_H__
#define __OPEN_BATTLEFIELD_DLG_H__

#include <qdialog.h>

namespace Ui
{
	class OpenBattlefieldDlg;
}

enum EBattlefieldFileType { SceneFile_ = 0, ScriptFile_, FileCount_ };

class OpenBattlefieldDlg : public QDialog
{
	Q_OBJECT
public:
	OpenBattlefieldDlg();
	~OpenBattlefieldDlg();
	int getCurBattlefieldID() const;
	int getCurBattlefieldBreed() const;
	static QString getCurBattlefieldFile(EBattlefieldFileType fileType);

private:
	int _curBattlefieldID;
	int _curBattlefieldBreed;
	Ui::OpenBattlefieldDlg* _ui;
	static QString _curBattlefieldFile[FileCount_];
};

#endif // __OPEN_BATTLEFIELD_DLG_H__
