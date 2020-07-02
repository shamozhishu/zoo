#ifndef __ZOO_LOG_H__
#define __ZOO_LOG_H__

#include <zoo/Support.h>

namespace zoo {

enum ELogLevel
{
	ELL_DEBUG = 0,
	ELL_INFORMATION,
	ELL_WARNING,
	ELL_ERROR
};

class _zooExport Log
{
public:
	static void print(ELogLevel level, const char* szFormat, ...);
	static void setLevel(ELogLevel level);
};

}

#endif // __ZOO_LOG_H__
