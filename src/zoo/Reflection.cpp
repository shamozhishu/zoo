#include <zoo/Reflection.h>

namespace zoo {

std::string Type::getType() const
{
	return ReflexFactory<>::getTypeName(typeid(*this).name());
}

}
