#include <zoo/Reflection.h>

namespace zoo {

std::string Type::typeName() const
{
	return ReflexFactory<>::getTypeName(typeid(*this).name());
}

}
