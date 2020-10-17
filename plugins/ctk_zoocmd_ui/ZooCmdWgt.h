#ifndef __ZOO_CMD_WGT_H__
#define __ZOO_CMD_WGT_H__

#include <QtWidgets/QWidget>
#include <ctk_service/zoocmd_ui/Win32Service.h>

class RenderThread;
class CmdRegThread;
class ZooCmdWgt : public QWidget, public Win32Service
{
	Q_OBJECT
	Q_INTERFACES(Win32Service)
public:
	ZooCmdWgt(QString renderAdaName, QStringList cmdset, QString datadir = "", QWidget* parent = Q_NULLPTR);
	~ZooCmdWgt();
	HWND getWnd();
	bool isFocus();
	void resgisterCmdset(QStringList cmdset);
	bool isCmdRegCompleted() const;

signals:
	void cmdRegistered();

private:
	RenderThread* _renderThread;
	CmdRegThread* _cmdRegThread;
};

#endif // __ZOO_CMD_WGT_H__
