#ifndef __ZOO_LOG_H__
#define __ZOO_LOG_H__

#include <zoo/Support.h>

namespace zoo {

enum ELogLevel
{
	ELL_DEBUG = 0,
	ELL_INFO,
	ELL_WARNING,
	ELL_ERROR
};

class _zooExport Log
{
public:
	static void level(ELogLevel level);
	static void debug(const char* szFormat, ...);
	static void info(const char* szFormat, ...);
	static void warning(const char* szFormat, ...);
	static void error(const char* szFormat, ...);
	typedef void(*Listener)(ELogLevel level, const char* logContent);
	static void listen(Listener prePrint, Listener postPrint);
};

}

#endif // __ZOO_LOG_H__
