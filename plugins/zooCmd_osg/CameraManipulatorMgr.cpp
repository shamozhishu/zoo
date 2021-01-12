#include "CameraManipulatorMgr.h"
#include <osgEarth/Version>
#include <UniversalGlobalServices.h>
#include <zooCmd_osg/OsgEarthContext.h>

using namespace osgGA;
using namespace osgEarth::Util;

CameraManipulatorMgr::CameraManipulatorMgr(OsgContext* context, bool isEarth)
	: OsgManipulatorMgr(context)
	, _isEarth(isEarth)
{
	if (_isEarth)
		addMatrixManipulator('0', "Earth", new EarthManipulator());
	else
		addMatrixManipulator('0', "Terrain", new TerrainManipulator());
	addMatrixManipulator('1', "NodeTracker", new NodeTrackerManipulator());
	addMatrixManipulator('2', "Trackball", new TrackballManipulator());
	addMatrixManipulator('3', "Flight", new FlightManipulator());
	addMatrixManipulator('4', "Drive", new DriveManipulator());
	addMatrixManipulator('5', "Orbit", new OrbitManipulator());
	addMatrixManipulator('6', "FirstPerson", new FirstPersonManipulator());
	addMatrixManipulator('7', "Spherical", new SphericalManipulator());
}

CameraManipulatorMgr::~CameraManipulatorMgr()
{
}

void CameraManipulatorMgr::home(double currentTime)
{
	CameraManipulator* pCurManipulator = getCurrentMatrixManipulator();
	if (pCurManipulator)
		pCurManipulator->home(currentTime);
}

void CameraManipulatorMgr::focus(int num, osg::Node* pNode)
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
		if (_isEarth)
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
				pOsgEarthUtils->convertXYZToLLH(dynamic_cast<CoordTransformUtil::Converter*>(_context), XYZ.x(), XYZ.y(), XYZ.z(), llh._v[0], llh._v[1], llh._v[2]);

				Viewpoint vp = pEarthManipu->getViewpoint();
				vp.setNode(pNode);
				vp.focalPoint().mutable_value().x() = llh.x();
				vp.focalPoint().mutable_value().y() = llh.y();
				osg::Vec3d scale = localToWorld.getScale();
				vp.range() = bound.radius() * max(max(scale.x(), scale.y()), scale.z()) * 6;
				pEarthManipu->setViewpoint(vp);
			}
		}
		else
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
		break;
	case 6:
		break;
	case 7:
		break;
	default:
		break;
	}
}

bool CameraManipulatorMgr::handle(const GUIEventAdapter& ea, GUIActionAdapter& aa)
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
