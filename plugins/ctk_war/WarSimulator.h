#ifndef __WAR_SIMULATOR_H__
#define __WAR_SIMULATOR_H__

#include <component/WarComponents.h>

enum ESimState { null_ = 0, ready_, running_, paused_ };

class LuaScriptStack;
class WarSimulator : zoo::Type
{	
public:
	WarSimulator(string scriptFile);
	~WarSimulator();
	void resetSimuScene();
	virtual void stepping();
	ESimState getCurSimState() const;
	void transition(ESimState simState);
	void addBehavior(Behavior* behavior);
	void removeBehavior(Behavior* behavior);

protected:
	void execBehaviors(const char* func);
	void execSimuScene(const char* func);

protected:
	bool _scriptValid;
	bool _scriptInited;
	string _scriptFile;
	LuaScriptStack* _script;
	ESimState _currentState;
	list<Behavior*> _behaviors;
};

#endif // __WAR_SIMULATOR_H__
