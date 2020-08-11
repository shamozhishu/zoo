#include "CameraManipulatorManager.h"
#include <zooCmd_osg/OsgEarthUtils.h>

using namespace osgGA;
using namespace osgEarth::Util;

CameraManipulatorManager::CameraManipulatorManager()
	: _switchEnabled(false)
{
	addMatrixManipulator('0', "Earth", new EarthManipulator());
	addMatrixManipulator('1', "NodeTracker", new NodeTrackerManipulator());
	addMatrixManipulator('2', "Trackball", new TrackballManipulator());
	addMatrixManipulator('3', "Flight", new FlightManipulator());
	addMatrixManipulator('4', "Drive", new DriveManipulator());
	addMatrixManipulator('5', "Terrain", new TerrainManipulator());
	addMatrixManipulator('6', "Orbit", new OrbitManipulator());
	addMatrixManipulator('7', "FirstPerson", new FirstPersonManipulator());
	addMatrixManipulator('8', "Spherical", new SphericalManipulator());
}

CameraManipulatorManager::~CameraManipulatorManager()
{
}

bool CameraManipulatorManager::isSwitchEnabled() const
{
	return _switchEnabled;
}

void CameraManipulatorManager::setSwitchEnabled(bool enabled)
{
	_switchEnabled = enabled;
}

void CameraManipulatorManager::home(double currentTime)
{
	CameraManipulator* pCurManipulator = getCurrentMatrixManipulator();
	if (pCurManipulator)
		pCurManipulator->home(currentTime);
}

void CameraManipulatorManager::focus(int num, osg::Node* pNode)
{
	if (!pNode)
		return;

	switch (num)
	{
	case 0:
	{
		OsgEarthUtils* pOsgEarthUtils = ServiceLocator<OsgEarthUtils>::getService();
		EarthManipulator* pEarthManipu = dynamic_cast<EarthManipulator*>(getMatrixManipulatorWithIndex(num));
		if (pOsgEarthUtils && pEarthManipu)
		{
			pNode->dirtyBound();
			osg::BoundingSphere bound = pNode->getBound();
			osg::Vec3d llh;
			osg::Matrix localToWorld = osg::computeLocalToWorld(pNode->getParent(0)->getParentalNodePaths()[0]);
			osg::Vec3d XYZ = bound.center() * localToWorld;
			pOsgEarthUtils->convertXYZToLatLongHeight(XYZ.x(), XYZ.y(), XYZ.z(), llh._v[0], llh._v[1], llh._v[2]);

			Viewpoint vp = pEarthManipu->getViewpoint();
			vp.focalPoint().mutable_value().x() = llh.y();
			vp.focalPoint().mutable_value().y() = llh.x();
			vp.range() = llh._v[2];
			vp.setNode(pNode);
			pEarthManipu->setViewpoint(vp);
			pEarthManipu->clearViewpoint();
		}
	}
	break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		break;
	}
}

bool CameraManipulatorManager::handle(const GUIEventAdapter& ea, GUIActionAdapter& aa)
{
	if (_switchEnabled)
		return KeySwitchMatrixManipulator::handle(ea, aa);

	return getCurrentMatrixManipulator() && getCurrentMatrixManipulator()->handle(ea, aa);
}
