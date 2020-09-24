#include "OsgEarthUtilsImpl.h"
#include <zooCmd_osg/OsgEarthContext.h>

bool OsgEarthUtilsImpl::convertLLHToXYZ(double longitude, double latitude, double height, double& X, double& Y, double& Z)
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

bool OsgEarthUtilsImpl::convertXYZToLLH(double X, double Y, double Z, double& longitude, double& latitude, double& height)
{
	OsgEarthContext* pOsgEarthContext = ServiceLocator<OsgEarthContext>::getService();
	if (!pOsgEarthContext)
		return false;
	osgEarth::MapNode* pMapNode = pOsgEarthContext->getOpMapNode();
	if (!pMapNode)
		return false;
	pMapNode->getMapSRS()->getEllipsoid()->convertXYZToLatLongHeight(X, Y, Z, latitude, longitude, height);
	longitude = osg::RadiansToDegrees(longitude);
	latitude = osg::RadiansToDegrees(latitude);
	return true;
}
