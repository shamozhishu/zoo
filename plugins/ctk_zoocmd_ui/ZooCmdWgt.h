#ifndef __ZOO_CMD_WGT_H__
#define __ZOO_CMD_WGT_H__

#include <QtWidgets/QWidget>

class RenderThread;
class CmdRegThread;
class ZooCmdWgt : public QWidget
{
	Q_OBJECT
public:
	ZooCmdWgt(QString renderAdaName, QStringList cmdset, QString datadir = "", QWidget* parent = Q_NULLPTR);
	~ZooCmdWgt();
	void resgisterCmdset(QStringList cmdset);
	bool isCmdRegCompleted() const;

signals:
	void cmdRegistered();

private:
	RenderThread* _renderThread;
	CmdRegThread* _cmdRegThread;
};

#endif // __ZOO_CMD_WGT_H__
