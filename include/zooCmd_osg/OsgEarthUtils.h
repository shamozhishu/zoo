#ifndef __OSG_EARTH_UTILS_H__
#define __OSG_EARTH_UTILS_H__

#include <zoo/ServiceLocator.h>

class OsgEarthUtils : public zoo::Service
{
	ZOO_SERVICE(OsgEarthUtils)
public:
	virtual bool convertLatLongHeightToXYZ(double latitude, double longitude, double height, double& X, double& Y, double& Z) = 0;
	virtual bool convertXYZToLatLongHeight(double X, double Y, double Z, double& latitude, double& longitude, double& height) = 0;
};

#endif // __OSG_EARTH_UTILS_H__
