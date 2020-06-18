#pragma once

#include <component/war/SharedComponents.h>

using namespace zoo;

class LuaScript;
class Behavior : public Component
{
	bool _scriptValid;
	bool _scriptInited;
	string _scriptFile;
	ZOO_REFLEX_DECLARE(Behavior)
	PROPERTY_R(LuaScript*, _script, Script)

public:
	Behavior();
	~Behavior();
	void exec();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);
	void serializeField(Spawner* spawner);
	void serializeHeader(Spawner* spawner);
};

class AI : public Component
{
public:
	AI();
	~AI();
};
