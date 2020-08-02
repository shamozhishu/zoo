#ifndef __LOG_PRINT_WGT_H__
#define __LOG_PRINT_WGT_H__

#include <QWidget>

namespace Ui
{
	class LogPrintWgt;
}

class LogPrintWgt : public QWidget
{
	Q_OBJECT
public:
	LogPrintWgt();
	~LogPrintWgt();

private Q_SLOTS:
	void onCleanLog();

private:
	Ui::LogPrintWgt* _ui;
	class DebugStream* _printOut;
};

#endif // __LOG_PRINT_WGT_H__
