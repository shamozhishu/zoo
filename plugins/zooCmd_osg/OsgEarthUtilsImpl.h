#ifndef __OSG_EARTH_UTILS_IMPL_H__
#define __OSG_EARTH_UTILS_IMPL_H__

#include <zooCmd_osg/OsgEarthUtils.h>

class OsgEarthUtilsImpl : public OsgEarthUtils
{
public:
	bool convertLatLongHeightToXYZ(double latitude, double longitude, double height, double& X, double& Y, double& Z);
	bool convertXYZToLatLongHeight(double X, double Y, double Z, double& latitude, double& longitude, double& height);
};

#endif // __OSG_EARTH_UTILS_IMPL_H__
