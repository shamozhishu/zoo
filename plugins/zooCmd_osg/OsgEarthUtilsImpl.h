#ifndef __OSG_EARTH_UTILS_IMPL_H__
#define __OSG_EARTH_UTILS_IMPL_H__

#include <UniversalGlobalServices.h>

class OsgEarthUtilsImpl : public CoordTransformUtil
{
public:
	bool convertLLHToXYZ(Converter* converter, double longitude, double latitude, double height, double& X, double& Y, double& Z);
	bool convertXYZToLLH(Converter* converter, double X, double Y, double Z, double& longitude, double& latitude, double& height);
};

#endif // __OSG_EARTH_UTILS_IMPL_H__
