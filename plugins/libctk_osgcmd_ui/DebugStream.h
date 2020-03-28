#pragma once

#include <string>
#include <iostream>
#include <streambuf>
#include <QTextEdit>

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
			_logWindow->append(_logInfo.c_str());
		_stream.rdbuf(_oldBuf);
	}

protected:
	virtual int_type overflow(int_type v)
	{
		if (v == '\n')
		{
			_logWindow->append(_logInfo.c_str());
			_logInfo.erase(_logInfo.begin(), _logInfo.end());
		}
		else
			_logInfo += v;
		return v;
	}

	virtual std::streamsize xsputn(const char* p, std::streamsize n)
	{
		_logInfo.append(p, p + n);
		int pos = 0;
		while (pos != std::string::npos)
		{
			pos = _logInfo.find('\n');
			if (pos != std::string::npos)
			{
				std::string tmp(_logInfo.begin(), _logInfo.begin() + pos);
				_logWindow->append(tmp.c_str());
				_logInfo.erase(_logInfo.begin(), _logInfo.begin() + pos + 1);
			}
		}
		return n;
	}

private:
	std::ostream&   _stream;
	std::streambuf* _oldBuf;
	std::string     _logInfo;
	QTextEdit*      _logWindow;
};
