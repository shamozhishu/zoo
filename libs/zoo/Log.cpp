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
	Log::Listener _prePrint;
	Log::Listener _postPrint;

	Logger(bool hasConsole, const string& logFileName)
		: _hasConsole(hasConsole)
		, _level(ELL_DEBUG)
		, _prePrint(nullptr)
		, _postPrint(nullptr)
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

static inline void print(ELogLevel level, const char* szArgMessage)
{
	if (g_pLogger->_prePrint)
		g_pLogger->_prePrint(level, szArgMessage);

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

	if (g_pLogger->_postPrint)
		g_pLogger->_postPrint(level, szArgMessage);
}

bool Log::console()
{
	if (g_pLogger)
		return g_pLogger->_hasConsole;
	return false;
}

void Log::level(ELogLevel level)
{
	if (g_pLogger)
		g_pLogger->_level = level;
}

void Log::debug(const char* szFormat, ...)
{
	if (g_pLogger && ELL_DEBUG >= g_pLogger->_level)
	{
		static char szArgMessage[2048];
		va_list args;
		va_start(args, szFormat);
		vsprintf(szArgMessage, szFormat, args);
		va_end(args);
		print(ELL_DEBUG, szArgMessage);
	}
}

void Log::info(const char* szFormat, ...)
{
	if (g_pLogger && ELL_INFO >= g_pLogger->_level)
	{
		static char szArgMessage[2048];
		va_list args;
		va_start(args, szFormat);
		vsprintf(szArgMessage, szFormat, args);
		va_end(args);
		print(ELL_INFO, szArgMessage);
	}
}

void Log::warning(const char* szFormat, ...)
{
	if (g_pLogger && ELL_WARNING >= g_pLogger->_level)
	{
		static char szArgMessage[2048];
		va_list args;
		va_start(args, szFormat);
		vsprintf(szArgMessage, szFormat, args);
		va_end(args);
		print(ELL_WARNING, szArgMessage);
	}
}

void Log::error(const char* szFormat, ...)
{
	if (g_pLogger && ELL_ERROR >= g_pLogger->_level)
	{
		static char szArgMessage[2048];
		va_list args;
		va_start(args, szFormat);
		vsprintf(szArgMessage, szFormat, args);
		va_end(args);
		print(ELL_ERROR, szArgMessage);
	}
}

void Log::listen(Listener prePrint, Listener postPrint)
{
	if (g_pLogger)
	{
		g_pLogger->_prePrint = prePrint;
		g_pLogger->_postPrint = postPrint;
	}
}

}
