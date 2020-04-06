#pragma once

#define ZOO_DISABLE_ASSERT 0
#if ZOO_DISABLE_ASSERT > 0
#define ZOO_ASSERT(cond)
#else
#include <cassert>
#define ZOO_ASSERT(cond)    assert(cond)
#endif

#if defined(_DEBUG) && defined(ZOO_ENABLE_MEM_LEAK_CHECK)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
// Enable run-time memory check for debug builds.
#define EnableMemLeakCheck() _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF)
#else
#define EnableMemLeakCheck()
#endif

#if defined(ZOO_NONCLIENT_BUILD)
#	define _zooExport __declspec(dllexport)
#else
#	if defined(__MINGW32__)
#		define _zooExport
#	else
#		define _zooExport __declspec(dllimport)
#	endif
#endif

#define zoo_signals public
#define zoo_slots

#define REFLEX_DECLARE(CLASS_TYPE) private: static zoo::Reflex<CLASS_TYPE> _dynReflex;
#define REFLEX_IMPLEMENT(CLASS_TYPE) zoo::Reflex<CLASS_TYPE> CLASS_TYPE::_dynReflex

#define SAFE_DELETE(p)       do { if (p) { delete (p); (p) = 0; } } while (0)
#define SAFE_DELETE_ARRAY(p) do { if (p) { delete [] (p); (p) = 0; } } while (0)
#define IF_FALSE_BREAK(p)    if (!(p)) break
#define MAX(x, y)            (((x)>(y)) ? (x) : (y))
#define MIN(x, y)            (((x)<(y)) ? (x) : (y))

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                 \
private: TypeName(const TypeName&);                                        \
private: TypeName& operator=(const TypeName&);

#define DISALLOW_COPY(TypeName)                                            \
private: TypeName(const TypeName&);

#define DISALLOW_ASSIGN(TypeName)                                          \
private: TypeName& operator=(const TypeName&);

#define PROPERTY_R(varType, varName, funName)                              \
protected: varType varName;                                                \
public: varType get##funName(void) const { return varName; }

#define PROPERTY_VR(varType, varName, funName)                             \
protected: varType varName;                                                \
public: virtual varType get##funName(void) const { return varName; }

#define PROPERTY_R_REF(varType, varName, funName)                          \
protected: varType varName;                                                \
public: const varType& get##funName(void) const { return varName; }

#define PROPERTY_VR_REF(varType, varName, funName)                         \
protected: varType varName;                                                \
public: virtual const varType& get##funName(void) const { return varName; }

#define PROPERTY_RW(varType, varName, funName)                             \
protected: varType varName;                                                \
public: varType get##funName(void) const { return varName; }               \
public: void set##funName(varType var) { varName = var; }

#define PROPERTY_VRW(varType, varName, funName)                            \
protected: varType varName;                                                \
public: virtual varType get##funName(void) const { return varName; }       \
public: virtual void set##funName(varType var) { varName = var; }

#define PROPERTY_RW_REF(varType, varName, funName)                         \
protected: varType varName;                                                \
public: const varType& get##funName(void) const { return varName; }        \
public: void set##funName(const varType& var) { varName = var; }

#define PROPERTY_VRW_REF(varType, varName, funName)                        \
protected: varType varName;                                                \
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var) { varName = var; }

#define ZOO_REGISTER(type)                                                 \
extern "C" __declspec(dllexport) const char* dllGetTypeName(void)          \
{                                                                          \
	return #type;                                                          \
}
