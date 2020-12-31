#include "LogPrintWgt.h"
#include "ui_LogPrintWgt.h"
#include <zoo/Log.h>
#include <iostream>

class DebugStream : public std::basic_streambuf<char>
{
public:
	DebugStream(std::ostream& stream, QTextEdit* textEdit) : _stream(stream)
	{
		_logWindow = textEdit;
		_oldBuf = stream.rdbuf();
		stream.rdbuf(this);
	}

	~DebugStream()
	{
		// output anything that is left.
		if (!_logInfo.empty())
			_logWindow->append(_logColor + QString::fromLocal8Bit(_logInfo.c_str()) + "</font>");
		_stream.rdbuf(_oldBuf);
	}

	static void extractLogColor(zoo::ELogLevel level, const char* logContent)
	{
		(void)logContent;
		switch (level)
		{
		case zoo::ELL_DEBUG: _logColor = "<font>"; break;
		case zoo::ELL_INFO: _logColor = "<font color=\"#00AA00\">"; break;
		case zoo::ELL_WARNING: _logColor = "<font color=\"#AAAA00\">"; break;
		case zoo::ELL_ERROR: _logColor = "<font color=\"#AA0000\">"; break;
		default: _logColor = "<font>"; break;
		}
	}

	static void recoverLogColor(zoo::ELogLevel level, const char* logContent)
	{
		_logColor = "<font>";
	}

protected:
	virtual int_type overflow(int_type v)
	{
		if (v == '\n')
		{
			_logWindow->append(_logColor + QString::fromLocal8Bit(_logInfo.c_str()) + "</font>");
			_logInfo.erase(_logInfo.begin(), _logInfo.end());
		}
		else
			_logInfo += v;
		return v;
	}

	virtual std::streamsize xsputn(const char* p, std::streamsize n)
	{
		_logInfo.append(p, p + n);
		size_t pos = 0;
		while (pos != std::string::npos)
		{
			pos = _logInfo.find('\n');
			if (pos != std::string::npos)
			{
				std::string tmp(_logInfo.begin(), _logInfo.begin() + pos);
				_logWindow->append(_logColor + QString::fromLocal8Bit(tmp.c_str()) + "</font>");
				_logInfo.erase(_logInfo.begin(), _logInfo.begin() + pos + 1);
			}
		}
		return n;
	}

private:
	std::ostream&   _stream;
	std::streambuf* _oldBuf;
	std::string     _logInfo;
	static QString  _logColor;
	QTextEdit*      _logWindow;
};

QString DebugStream::_logColor = "<font color=\"#000000\">";

LogPrintWgt::LogPrintWgt(QWidget* parent)
	: QWidget(parent)
	, _ui(new Ui::LogPrintWgt)
	, _printOut(nullptr)
{
	_ui->setupUi(this);
	zoo::Log::listen(&DebugStream::extractLogColor, &DebugStream::recoverLogColor);
	_printOut = new DebugStream(std::cout, _ui->textBrowser);
	connect(_ui->toolButton, SIGNAL(clicked()), this, SLOT(onCleanLog()));
}

LogPrintWgt::~LogPrintWgt()
{
	delete _printOut;
	delete _ui;
}

void LogPrintWgt::onCleanLog()
{
	_ui->textBrowser->clear();
}
