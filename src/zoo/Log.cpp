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
public:
	Logger(bool hasConsole, const string& logFileName)
		: _hasConsole(hasConsole)
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

#ifdef _DEBUG
static Logger s_logger(true, "log.txt");
#endif

inline void Log::print(ELogLevel level, const char* szFormat, ...)
{
#if defined(_DEBUG)
	char szArgMessage[2048] = { 0 };
	va_list args;
	va_start(args, szFormat);
	vsprintf(szArgMessage, szFormat, args);
	va_end(args);

	if (s_pLogger)
	{
		if (s_pLogger->_hasConsole)
		{
			WORD wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			switch (level)
			{
			case ELL_INFORMATION: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
			case ELL_WARNING: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE; break;
			case ELL_ERROR: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
			printf("%s\n", szArgMessage);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
		}

		if (s_pLogger->_fout)
			s_pLogger->_fout << szArgMessage << std::endl;
	}
#endif
}

inline void Log::wprint(ELogLevel level, const wchar_t* szFormat, ...)
{
#if defined(_DEBUG)
	wchar_t szArgMessage[2048] = {0};
	va_list args;
	va_start(args, szFormat);
	vswprintf(szArgMessage, 2048, szFormat, args);
	va_end(args);

	if (s_pLogger)
	{
		if (s_pLogger->_hasConsole)
		{
			WORD wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			switch (level)
			{
			case ELL_INFORMATION: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
			case ELL_WARNING: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE; break;
			case ELL_ERROR: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
			wprintf(L"%s\n", szArgMessage);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
		}

		if (s_pLogger->_fout)
			s_pLogger->_fout << szArgMessage << std::endl;
	}
#endif
}

}
