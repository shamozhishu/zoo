#include "Log.h"
#include <tchar.h>
#include <Windows.h>
#pragma warning(disable:4996)

namespace zoo {

static class Logger* s_pLogger = nullptr;
class Logger
{
public:
	ofstream _fout;
	bool _hasConsole;
	ELogLevel _level;
public:
	Logger(bool hasConsole, const string& logFileName)
		: _hasConsole(hasConsole)
		, _level(ELL_DEBUG)
	{
		if (NULL == s_pLogger)
		{
			s_pLogger = this;

			if (_hasConsole)
			{
				_hasConsole = (AllocConsole() == TRUE);
				if (_hasConsole)
				{
					_tfreopen(_TEXT("CONOUT$"), _TEXT("w+t"), stdout);
					TCHAR szProgramName[MAX_PATH];
					HMODULE hModule(NULL);
					GetModuleFileName(hModule, szProgramName, MAX_PATH);
					SetConsoleTitle(szProgramName);
					setlocale(LC_ALL, "chs");
				}
			}

			if (!logFileName.empty())
			{
				_fout.open(logFileName);
				_fout.imbue(std::locale("chs"));
			}
		}
	}

	~Logger()
	{
		if (NULL != s_pLogger)
		{
			if (s_pLogger->_fout)
				_fout.close();
			if (s_pLogger->_hasConsole)
				FreeConsole();
			s_pLogger = NULL;
		}
	}
};

#if defined(_DEBUG) || defined(ZOO_ENABLE_LOG_PRINT)
static Logger s_logger(true, "");
#endif

inline void Log::print(ELogLevel level, const char* szFormat, ...)
{
	if (s_pLogger && level >= s_pLogger->_level)
	{
		static char szArgMessage[2048];
		va_list args;
		va_start(args, szFormat);
		vsprintf(szArgMessage, szFormat, args);
		va_end(args);

		if (s_pLogger->_hasConsole)
		{
			WORD wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			switch (level)
			{
			case ELL_INFORMATION: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
			case ELL_WARNING: wAttributes = FOREGROUND_RED | FOREGROUND_GREEN; break;
			case ELL_ERROR: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
			printf("%s\n", szArgMessage);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
		}

		if (s_pLogger->_fout)
			s_pLogger->_fout << szArgMessage << std::endl;
	}
}

void Log::setLevel(ELogLevel level)
{
	if (s_pLogger)
		s_pLogger->_level = level;
}

}
