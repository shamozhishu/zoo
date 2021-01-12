#ifndef __LUA_SCRIPT_STACK_H__
#define __LUA_SCRIPT_STACK_H__

#include <string>

#define TOLUA_OPEN_FUNC_PTR(PKG_NAME) tolua_##PKG_NAME##_open
#define TOLUA_OPEN_FUNC_EXTERN(PKG_NAME) extern int tolua_##PKG_NAME##_open(lua_State* tolua_S)

using namespace std;
struct lua_State;
typedef int (*TOLUA_OPEN_FUNC)(lua_State* tolua_S);

class LuaScriptStack
{
public:
	LuaScriptStack(TOLUA_OPEN_FUNC funcs[]); // 参数数组的末尾元素必须为NULL
	~LuaScriptStack();
	bool executeString(const string& codes);
	bool executeScriptFile(const string& filename);
	int  executeGlobalFunction(const string& functionName);
	int  executeFunctionByHandler(int nHandler, int numArgs);
	void removeScriptHandler(int nHandler);
	int  reallocateScriptHandler(int nHandler);
	void call_va(const string& func, const string& sig, ...);

	lua_State* getLuaState() const;
	int  executeFunction(int numArgs);
	void clean();
	void pushInt(int intValue);
	void pushFloat(float floatValue);
	void pushBoolean(bool boolValue);
	void pushString(const char* stringValue);
	void pushString(const char* stringValue, int length);
	void pushNil(void);
	bool pushFunctionByHandler(int nHandler);
	void setVariable(const char* varName, const char* varType, void* pData);

	static void clearScriptFileName2ContentCache(const string& filename = ""); // 【filename == ""】则清理整个缓存
protected:
	lua_State* _state;
};

#endif // __LUA_SCRIPT_STACK_H__
