#include "OsgEarthUtilsImpl.h"
#include <zooCmd_osg/OsgEarthContext.h>

bool OsgEarthUtilsImpl::convertLatLongHeightToXYZ(double latitude, double longitude, double height, double& X, double& Y, double& Z)
{
	OsgEarthContext* pOsgEarthContext = ServiceLocator<OsgEarthContext>::getService();
	if (!pOsgEarthContext)
		return false;
	osgEarth::MapNode* pMapNode = pOsgEarthContext->getOpMapNode();
	if (!pMapNode)
		return false;
	pMapNode->getMapSRS()->getEllipsoid()->convertLatLongHeightToXYZ(osg::DegreesToRadians(latitude), osg::DegreesToRadians(longitude), height, X, Y, Z);
	return true;
}

bool OsgEarthUtilsImpl::convertXYZToLatLongHeight(double X, double Y, double Z, double& latitude, double& longitude, double& height)
{
	OsgEarthContext* pOsgEarthContext = ServiceLocator<OsgEarthContext>::getService();
	if (!pOsgEarthContext)
		return false;
	osgEarth::MapNode* pMapNode = pOsgEarthContext->getOpMapNode();
	if (!pMapNode)
		return false;
	pMapNode->getMapSRS()->getEllipsoid()->convertXYZToLatLongHeight(X, Y, Z, latitude, longitude, height);
	latitude = osg::RadiansToDegrees(latitude);
	longitude = osg::RadiansToDegrees(longitude);
	return true;
}
