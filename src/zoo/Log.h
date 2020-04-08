#pragma once

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
	static void wprint(ELogLevel level, const wchar_t* szFormat, ...);
};

}
