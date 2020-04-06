#pragma once

#include <string>
using namespace std;

struct lua_State;
class LuaScript
{
public:
	LuaScript();
	~LuaScript();
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

	static void clearScriptFileName2ContentCache();

protected:
	lua_State* _state;
};
