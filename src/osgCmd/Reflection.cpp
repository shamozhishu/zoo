#include <osgCmd/Reflection.h>

namespace osgCmd {

std::string Type::getType() const
{
	return ReflexFactory<>::getTypeName(typeid(*this).name());
}

}
