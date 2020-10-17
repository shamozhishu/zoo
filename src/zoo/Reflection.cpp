#include <zoo/Reflection.h>

namespace zoo {

std::string Type::typeName() const
{
	return ReflexFactory<>::getTypeName(typeid(*this).name());
}

std::unordered_map<std::string, std::function<void()>>* getCreateFuncMap()
{
	static unordered_map<string, std::function<void()>> s_createFuncMap;
	return &s_createFuncMap;
}

}
