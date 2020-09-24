#include "CameraManipulatorManager.h"
#include <UniversalGlobalServices.h>
#include <osgEarth/Version>

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

	osg::Group* pParent = pNode->asGroup();
	if (pParent)
	{
		pParent = (pParent->getNumChildren() > 0 && pParent->getChild(0)) ? pParent->getChild(0)->asGroup() : nullptr;
		if (pParent)
		{
			pParent = (pParent->getNumChildren() > 0 && pParent->getChild(0)) ? pParent->getChild(0)->asGroup() : nullptr;
			if (pParent)
			{
				if (pParent->getNumChildren() > 0 && pParent->getChild(0))
					pNode = pParent->getChild(0);
			}
		}
	}

	switch (num)
	{
	case 0:
	{
		CoordTransformUtil* pOsgEarthUtils = ServiceLocator<CoordTransformUtil>::getService();
		EarthManipulator* pEarthManipu = dynamic_cast<EarthManipulator*>(getMatrixManipulatorWithIndex(num));
		if (pOsgEarthUtils && pEarthManipu)
		{
			pNode->dirtyBound();
			osg::BoundingSphere bound = pNode->getBound();
			osg::Vec3d llh;
			osg::Matrix localToWorld = osg::computeLocalToWorld(pNode->getParent(0)->getParentalNodePaths()[0]);
			osg::Vec3d XYZ = bound.center() * localToWorld;
			pOsgEarthUtils->convertXYZToLLH(XYZ.x(), XYZ.y(), XYZ.z(), llh._v[0], llh._v[1], llh._v[2]);

			Viewpoint vp = pEarthManipu->getViewpoint();
			vp.setNode(pNode);
			vp.focalPoint().mutable_value().x() = llh.x();
			vp.focalPoint().mutable_value().y() = llh.y();
			osg::Vec3d scale = localToWorld.getScale();
			vp.range() = bound.radius() * max(max(scale.x(), scale.y()), scale.z()) * 6;
			pEarthManipu->setViewpoint(vp);
		}
	}
	break;
	case 1:
	{
		NodeTrackerManipulator* pNodeTrackerManipu = dynamic_cast<NodeTrackerManipulator*>(getMatrixManipulatorWithIndex(num));
		if (pNodeTrackerManipu)
			pNodeTrackerManipu->setTrackNode(pNode);
	}
	break;
	case 2:
		break;
	case 3:
		break;
	case 4:
	{
		DriveManipulator* pDriveManipu = dynamic_cast<DriveManipulator*>(getMatrixManipulatorWithIndex(num));
		if (pDriveManipu)
			pDriveManipu->setNode(pNode);
	}
	break;
	case 5:
	{
		TerrainManipulator* pTerrainManipu = dynamic_cast<TerrainManipulator*>(getMatrixManipulatorWithIndex(num));
		if (pTerrainManipu)
		{
			pNode->dirtyBound();
			osg::BoundingSphere bound = pNode->getBound();
			osg::Matrix localToWorld = osg::computeLocalToWorld(pNode->getParent(0)->getParentalNodePaths()[0]);
			osg::Vec3d XYZ = bound.center() * localToWorld;
			osg::Vec3d eye, center, up, direc;
			pTerrainManipu->getTransformation(eye, center, up);
			direc = eye - center;
			direc.normalize();
			osg::Vec3d scale = localToWorld.getScale();
			direc *= bound.radius() * max(max(scale.x(), scale.y()), scale.z()) * 4;
			pTerrainManipu->setTransformation(XYZ + direc, XYZ, up);
		}
	}
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
	EarthManipulator* pEarthManipu = dynamic_cast<EarthManipulator*>(getCurrentMatrixManipulator());
	if (pEarthManipu && ea.getEventType() == GUIEventAdapter::KEYUP)
	{
		int key = ea.getKey();
		if (key == GUIEventAdapter::KEY_Escape || key == (GUIEventAdapter::KEY_Escape & 0xFF))
#if OSGEARTH_MIN_VERSION_REQUIRED(2, 8, 0)
			pEarthManipu->setTetherCallback(nullptr);
#else
			pEarthManipu->setTetherNode(nullptr);
#endif
	}

	if (_switchEnabled)
		return KeySwitchMatrixManipulator::handle(ea, aa);

	return getCurrentMatrixManipulator() && getCurrentMatrixManipulator()->handle(ea, aa);
}
