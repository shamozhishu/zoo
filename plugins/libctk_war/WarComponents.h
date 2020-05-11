#pragma once

#include <component/war/SharedComponents.h>

class LuaScript;
class Behavior : public zoo::Component, public Serializer
{
	bool _scriptValid;
	string _scriptFile;
	ZOO_REFLEX_DECLARE(Behavior)
	PROPERTY_R(LuaScript*, _script, Script)

public:
	Behavior();
	~Behavior();
	bool init();
	void update();
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);
};
