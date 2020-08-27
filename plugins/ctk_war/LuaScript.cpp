#include "LuaScript.h"
#include <zoo/Log.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tolua.h"

extern int tolua_ToLuaWar_open(lua_State* tolua_S);

using namespace zoo;

#define TOLUA_REFID_PTR_MAPPING "tolua_refid_ptr_mapping"
#define TOLUA_REFID_TYPE_MAPPING "tolua_refid_type_mapping"
#define TOLUA_REFID_FUNCTION_MAPPING "tolua_refid_function_mapping"

static std::map<string, string> s_scriptFileName2ContentCache;
lua_State* LuaScript::getLuaState() const
{
	return _state;
}

LuaScript::LuaScript()
{
	_state = luaL_newstate();
	luaL_openlibs(_state);

	lua_pushstring(_state, TOLUA_REFID_PTR_MAPPING);
	lua_newtable(_state);
	lua_rawset(_state, LUA_REGISTRYINDEX);

	lua_pushstring(_state, TOLUA_REFID_TYPE_MAPPING);
	lua_newtable(_state);
	lua_rawset(_state, LUA_REGISTRYINDEX);

	lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
	lua_newtable(_state);
	lua_rawset(_state, LUA_REGISTRYINDEX);

	tolua_ToLuaWar_open(_state);
}

LuaScript::~LuaScript()
{
	lua_close(_state);
}

bool LuaScript::executeString(const string& codes)
{
	int status = lua_status(_state);
	if (status != LUA_OK && status != LUA_ERRRUN)
		return false;

	status = luaL_dostring(_state, codes.c_str());
	lua_gc(_state, LUA_GCCOLLECT, 0);

	if (status != LUA_OK)
	{
		zoo_error("%s", lua_tostring(_state, -1));
		lua_pop(_state, 1);
		return false;
	}

	return true;
}

bool LuaScript::executeScriptFile(const string& filename)
{
	int status = lua_status(_state);
	if (status != LUA_OK && status != LUA_ERRRUN)
		return false;

	if (filename == "")
	{
		zoo_warning("Script file name is null");
		return false;
	}

	bool hasCache = false;
	string contentCache = "";
	stringstream filestream;

	std::map<string, string>::iterator it = s_scriptFileName2ContentCache.find(filename);
	if (it != s_scriptFileName2ContentCache.end())
	{
		contentCache = it->second.c_str();
		hasCache = true;
	}
	else
	{
		std::ifstream fin(filename);
		if (!fin)
		{
			zoo_error("打开[%s]文件失败", filename.c_str());
			return false;
		}

		fin.imbue(locale("chs"));
		filestream << fin.rdbuf();
		contentCache = filestream.str().c_str();
	}

	if (contentCache == "")
	{
		zoo_error("Not found script file: %s", filename.c_str());
		return false;
	}

	status = luaL_loadstring(_state, contentCache.c_str());
	lua_gc(_state, LUA_GCCOLLECT, 0);

	if (status != LUA_OK)
	{
		const char* errMsg = lua_tostring(_state, -1);
		zoo_error("%s,%s", filename.c_str(), errMsg);
		lua_pop(_state, 1);
		return false;
	}

	status = lua_resume(_state, NULL, 0);
	if (status != LUA_OK)
	{
		const char* errMsg = lua_tostring(_state, -1);
		zoo_error("%s,%s", filename.c_str(), errMsg);
		lua_pop(_state, 1);
		return false;
	}

	if (!hasCache)
		s_scriptFileName2ContentCache[filename] = filestream.str();

	return true;
}

int LuaScript::executeGlobalFunction(const string& functionName)
{
	lua_getglobal(_state, functionName.c_str());

	if (!lua_isfunction(_state, -1))
	{
		zoo_warning("\"%s\" does not represent a lua function", functionName.c_str());
		lua_pop(_state, 1);
		return 0;
	}

	return executeFunction(0);
}

int LuaScript::executeFunctionByHandler(int nHandler, int numArgs)
{
	int ret = 0;
	if (pushFunctionByHandler(nHandler))                             /* stack: ... arg1 arg2 ... func */
	{
		if (numArgs > 0)
			lua_insert(_state, -(numArgs + 1));                      /* stack: ... func arg1 arg2 ... */
		ret = executeFunction(numArgs);
	}
	lua_settop(_state, 0);
	return ret;
}

void LuaScript::removeScriptHandler(int nHandler)
{
	lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(_state, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(_state, nHandler);                               /* stack: ... refid_fun refid */
	lua_pushnil(_state);                                             /* stack: ... refid_fun refid nil */
	lua_rawset(_state, -3);                  /* refid_fun[refid] = nil, stack: ... refid_fun */
	lua_pop(_state, 1);                                              /* stack: ... */
}

int LuaScript::reallocateScriptHandler(int nHandler)
{
	static int s_function_ref_id = 0;
	int nNewHandle = -1;

	if (pushFunctionByHandler(nHandler))
	{
		// function at lo
		int lo = lua_gettop(_state);
		if (!lua_isfunction(_state, lo))
			return nNewHandle;

		++s_function_ref_id;

		lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
		lua_rawget(_state, LUA_REGISTRYINDEX);                       /* stack: fun ... refid_fun */
		lua_pushinteger(_state, s_function_ref_id);                  /* stack: fun ... refid_fun refid */
		lua_pushvalue(_state, lo);                                   /* stack: fun ... refid_fun refid fun */
		lua_rawset(_state, -3);              /* refid_fun[refid] = fun, stack: fun ... refid_fun */
		lua_pop(_state, 1);                                          /* stack: fun ... */

		nNewHandle = s_function_ref_id;
	}

	return nNewHandle;
}

void LuaScript::call_va(const string& func, const string& sig, ...)
{
	const char* pszfunc = func.c_str();
	const char* pszsig = sig.c_str();
	va_list vl;
	int narg, nres; // 参数和结果的数量
	va_start(vl, pszsig);
	lua_getglobal(_state, pszfunc); // 压入函数

	// 压入参数
	bool endargs = false;
	for (narg = 0; *pszsig; ++narg) // 遍历所有参数
	{
		// 检查栈中空间
		luaL_checkstack(_state, 1, "too many arguments");

		switch (*pszsig++)
		{
		case 'd': // double参数
			lua_pushnumber(_state, va_arg(vl, double));
			break;
		case 'i': // int参数
			lua_pushinteger(_state, va_arg(vl, int));
			break;
		case 's': // string参数
			lua_pushstring(_state, va_arg(vl, char*));
			break;
		case '>': // 结束参数
			endargs = true;
			break;
		default:
			zoo_error("invalid option (%c)", *(pszsig - 1));
			lua_error(_state);
			break;
		}

		if (endargs)
			break;
	}

	nres = strlen(pszsig); // 期望的结果数量
	if (lua_pcall(_state, narg, nres, 0) != 0) // 完成调用
	{
		zoo_error("error calling '%s':%s", pszfunc, lua_tostring(_state, -1));
		lua_error(_state);
	}

	// 检查结果
	nres = -nres;
	while (*pszsig)
	{
		switch (*pszsig++)
		{
		case 'd': // double结果
			if (!lua_isnumber(_state, nres))
			{
				zoo_error("error result type");
				lua_error(_state);
			}
			*va_arg(vl, double*) = lua_tonumber(_state, nres);
			break;
		case 'i': // int结果
			if (!lua_isnumber(_state, nres))
			{
				zoo_error("error result type");
				lua_error(_state);
			}
			*va_arg(vl, int*) = lua_tointeger(_state, nres);
			break;
		case 's': // string结果
			if (!lua_isstring(_state, nres))
			{
				zoo_error("error result type");
				lua_error(_state);
			}
			*va_arg(vl, const char**) = lua_tostring(_state, nres);
			break;
		default:
			zoo_error("invalid option (%c)", *(pszsig - 1));
			lua_error(_state);
			break;
		}

		++nres;
	}

	va_end(vl);
}

int LuaScript::executeFunction(int numArgs)
{
	int functionIndex = -(numArgs + 1);
	if (!lua_isfunction(_state, functionIndex))
	{
		zoo_warning("value at stack [%d] is not function", functionIndex);
		lua_pop(_state, numArgs + 1); // remove function and arguments
		return 0;
	}

	int traceback = 0;
	lua_getglobal(_state, "__G__TRACKBACK__");                                 /* stack: ... func arg1 arg2 ... G */

	if (!lua_isfunction(_state, -1))
	{
		lua_pop(_state, 1);                                                    /* stack: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(_state, functionIndex - 1);                                 /* stack: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}

	int error = lua_pcall(_state, numArgs, 1, traceback);                      /* stack: ... [G] ret */
	if (error)
	{
		if (traceback == 0)
		{
			zoo_error("%s", lua_tostring(_state, -1));                         /* stack: ... error */
			lua_pop(_state, 1); // remove error message from stack
		}
		else                                                                   /* stack: ... G error */
		{
			lua_pop(_state, 2); // remove __G__TRACKBACK__ and error message from stack
		}

		return 0;
	}

	// get return value
	int ret = 0;
	if (lua_isnumber(_state, -1))
	{
		ret = lua_tointeger(_state, -1);
	}
	else if (lua_isboolean(_state, -1))
	{
		ret = lua_toboolean(_state, -1);
	}

	// remove return value from stack
	lua_pop(_state, 1);                                                        /* stack: ... [G] */

	if (traceback)
		lua_pop(_state, 1); // remove __G__TRACKBACK__ from stack              /* stack: ... */

	return ret;
}

void LuaScript::clean()
{
	lua_settop(_state, 0);
}

void LuaScript::pushInt(int intValue)
{
	lua_pushinteger(_state, intValue);
}

void LuaScript::pushFloat(float floatValue)
{
	lua_pushnumber(_state, floatValue);
}

void LuaScript::pushBoolean(bool boolValue)
{
	lua_pushboolean(_state, boolValue);
}

void LuaScript::pushString(const char* stringValue)
{
	lua_pushstring(_state, stringValue);
}

void LuaScript::pushString(const char* stringValue, int length)
{
	lua_pushlstring(_state, stringValue, length);
}

void LuaScript::pushNil(void)
{
	lua_pushnil(_state);
}

bool LuaScript::pushFunctionByHandler(int nHandler)
{
	lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(_state, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(_state, nHandler);                               /* stack: ... refid_fun refid */
	lua_rawget(_state, -2);                                          /* stack: ... refid_fun fun */
	lua_remove(_state, -2);                                          /* stack: ... fun */

	if (!lua_isfunction(_state, -1))
	{
		zoo_error("Function refid '%d' does not reference a Lua function", nHandler);
		lua_pop(_state, 1);
		return false;
	}

	return true;
}

void LuaScript::setVariable(const char* varName, const char* varType, void* pData)
{
	if (varName == nullptr || strcmp(varName, "") == 0)
		return;
	if (varType == nullptr || strcmp(varType, "") == 0)
		return;
	tolua_pushusertype(_state, pData, varType);
	lua_setglobal(_state, varName);
}

void LuaScript::clearScriptFileName2ContentCache(const string& filename /*= ""*/)
{
	if (filename == "")
	{
		s_scriptFileName2ContentCache.clear();
		return;
	}

	auto it = s_scriptFileName2ContentCache.find(filename);
	if (it != s_scriptFileName2ContentCache.end())
		s_scriptFileName2ContentCache.erase(it);
}
