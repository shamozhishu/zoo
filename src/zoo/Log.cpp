#include <zoo/Log.h>
#include <tchar.h>
#include <Windows.h>
#include <zoo/Utils.h>
#pragma warning(disable:4996)

namespace zoo {

class Logger* g_pLogger = nullptr;
class Logger
{
public:
	ofstream _fout;
	bool _hasConsole;
	ELogLevel _level;
	Log::Listener _listener;

	Logger(bool hasConsole, const string& logFileName)
		: _hasConsole(hasConsole)
		, _level(ELL_DEBUG)
		, _listener(nullptr)
	{
		if (NULL == g_pLogger)
		{
			g_pLogger = this;

			if (_hasConsole)
			{
				TCHAR szProgramName[MAX_PATH];
				HMODULE hModule(NULL);
				GetModuleFileName(hModule, szProgramName, MAX_PATH);
				SetConsoleTitle(szProgramName);
				setlocale(LC_ALL, "chs");
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
		if (NULL != g_pLogger)
		{
			if (g_pLogger->_fout)
				_fout.close();
			g_pLogger = NULL;
		}
	}
};

class LogConfig
{
public:
	LogConfig()
	{
		string retStr, configfile;
		configfile = getExeDir() + "config.ini";
		retStr.resize(128);
		GetPrivateProfileString("ZOO_ENABLE_LOG_PRINT", "logfile", "", &retStr[0], 128, configfile.c_str());
		string logFileName = retStr.c_str();
#ifdef _DEBUG
		static Logger s_logger(true, logFileName);
#else
		GetPrivateProfileString("ZOO_ENABLE_LOG_PRINT", "console", "0", &retStr[0], 128, configfile.c_str());
		bool hasConsole = atoi(retStr.c_str()) == 1;
		static Logger s_logger(hasConsole, logFileName);
#endif
	}
};

LogConfig g_logConfig;

inline void Log::print(ELogLevel level, const char* szFormat, ...)
{
	if (g_pLogger && level >= g_pLogger->_level)
	{
		static char szArgMessage[2048];
		va_list args;
		va_start(args, szFormat);
		vsprintf(szArgMessage, szFormat, args);
		va_end(args);

		if (g_pLogger->_listener)
			g_pLogger->_listener(level, szArgMessage);

		if (g_pLogger->_hasConsole)
		{
			WORD wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			switch (level)
			{
			case ELL_INFO: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN; break;
			case ELL_WARNING: wAttributes = FOREGROUND_RED | FOREGROUND_GREEN; break;
			case ELL_ERROR: wAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED; break;
			}
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
			std::cout << szArgMessage << std::endl;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		if (g_pLogger->_fout)
			g_pLogger->_fout << szArgMessage << std::endl;
	}
}

void Log::level(ELogLevel level)
{
	if (g_pLogger)
		g_pLogger->_level = level;
}

void Log::listen(Listener listenFunc)
{
	if (g_pLogger)
		g_pLogger->_listener = listenFunc;
}

}
