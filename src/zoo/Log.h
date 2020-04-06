#pragma once

#include <zoo/Support.h>

namespace zoo {

enum ELogLevel
{
	ELL_DEBUG = 0,
	ELL_INFORMATION = 2,
	ELL_WARNING = 6,
	ELL_ERROR = 4
};

class _zooExport Log
{
public:
	static void print(ELogLevel level, const char* szFormat, ...);
	static void wprint(ELogLevel level, const wchar_t* szFormat, ...);
};

}
