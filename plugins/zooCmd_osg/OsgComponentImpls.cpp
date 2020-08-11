#include "OsgComponentImpls.h"
#include "InputDevice.h"
#include <zoo/Utils.h>
#include "StarrySky.h"
#include <zooCmd/CmdManager.h>
#include <zooCmd_osg/OsgEarthUtils.h>
#include <zooCmd_osg/OsgEarthContext.h>

using namespace zooCmd_osg;
#ifdef NEED_OSGEARTH_LIBRARY
using namespace osgEarth::Util;
#endif

ZOO_REFLEX_IMPLEMENT(DoFImpl);
DoFImpl::DoFImpl()
	: _transLocal(new osg::MatrixTransform)
	, _transWorld(new osg::MatrixTransform)
{
	_transWorld->addChild(_transLocal);
}

DoFImpl::~DoFImpl()
{
	remove();
}

void DoFImpl::awake()
{
	_transLocal->setMatrix(osg::Matrix::identity());
	_transWorld->setMatrix(osg::Matrix::identity());
}

void DoFImpl::update()
{
	DoF* dof = getComponent<DoF>();
	if (dof->dirtyBit().checkState(DoF::Parent_))
		changeParent(dof);

	if (dof->dirtyBit().checkState(DoF::Dof_))
		locate(dof);
}

void DoFImpl::remove()
{
	if (_transWorld->getNumParents() > 0 && !getEntity()->isSpawner())
	{
		auto node = _transWorld->getParent(0);
		if (node)
			node->asGroup()->removeChild(_transWorld.get());
	}
}

void DoFImpl::locate(DoF* dof)
{
	if (dof->_lonLatHeight)
	{
#ifdef NEED_OSGEARTH_LIBRARY
		OsgEarthUtils* pOsgEarthUtils = ServiceLocator<OsgEarthUtils>::getService();
		osgEarth::MapNode* pMapNode = ServiceLocator<OsgEarthContext>::getService()->getOpMapNode();
		if (pOsgEarthUtils && pMapNode)
		{
			double x, y, z;
			pOsgEarthUtils->convertLatLongHeightToXYZ(dof->_y, dof->_x, dof->_z, x, y, z);
			osg::Matrix mat;
			osgEarth::GeoPoint mapPoint;
			mapPoint.fromWorld(pMapNode->getMapSRS(), osg::Vec3d(x, y, z));
			mapPoint.createLocalToWorld(mat);
			_transWorld->setMatrix(mat);
		}
		else
		{
			zoo_warning("DoFImpl: Map node does not exist!");
		}
#else
		zoo_error("need osgearth library!");
#endif
	}
	else
	{
		_transWorld->setMatrix(osg::Matrix::translate(dof->_x, dof->_y, dof->_z));
	}

	_transLocal->setMatrix(osg::Matrix::scale(dof->_sx, dof->_sy, dof->_sz)
		* osg::Matrix::rotate(osg::DegreesToRadians(dof->_roll), osg::Y_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(dof->_pitch), osg::X_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(dof->_heading), osg::Z_AXIS));
}

void DoFImpl::changeParent(DoF* dof)
{
	remove();
	if (dof->_lonLatHeight)
	{
		osg::Group* pRoot = ServiceLocator<OsgEarthContext>::getService()->getRootNode();
		pRoot->addChild(_transWorld.get());
	}
	else
	{
		if (dof->_parent)
		{
			DoFImpl* parent = (DoFImpl*)(dof->_parent->getImp());
			parent->_transWorld->addChild(_transWorld.get());
		}
	}
}

ZOO_REFLEX_IMPLEMENT(ModelImpl);
ModelImpl::ModelImpl()
	: _switch(new osg::Switch)
{
	_switch->setAllChildrenOn();
}

ModelImpl::~ModelImpl()
{
	if (_switch->getNumParents() > 0)
	{
		auto node = _switch->getParent(0);
		if (node)
			node->asGroup()->removeChild(_switch.get());
	}
}

void ModelImpl::awake()
{
	Model* model = getComponent<Model>();
	if (_model)
		_switch->removeChild(_model);

	_model = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + model->_modelFile);
	if (!_model.get())
	{
		zoo_warning("Read node file [%s] failed!", model->_modelFile.c_str());
		return;
	}

	_model->getOrCreateStateSet()->setMode(GL_LIGHTING, 0);
	_switch->addChild(_model);
	Entity* pEnt = model->getEntity();
	SignalTrigger::connect(*pEnt, [this]
	{
		ModelImpl* pModelImpl = getEntity()->getComponentImpl<ModelImpl>();
		if (pModelImpl)
			getEntity()->getComponentImpl<DoFImpl>()->_transLocal->addChild(pModelImpl->_switch);
	});
}

void ModelImpl::update()
{
	Model* model = getComponent<Model>();
	if (model->dirtyBit().checkState(Model::Visible_))
	{
		if (model->_visible)
			_switch->setAllChildrenOn();
		else
			_switch->setAllChildrenOff();
	}
}

ZOO_REFLEX_IMPLEMENT(CameraImpl);
CameraImpl::CameraImpl()
{
}

CameraImpl::~CameraImpl()
{
	if (_view)
		_view->setCameraManipulator(nullptr);

	string viewName = getEntity()->breed() == -1 ? "" : string("_") + std::to_string(getEntity()->breed());
	viewName = string("view") + std::to_string(getEntity()->id()) + viewName;
	ServiceLocator<OsgDevice>::getService()->destroyView(viewName);
}

void CameraImpl::awake()
{
	string viewName = getEntity()->breed() == -1 ? "" : string("_") + std::to_string(getEntity()->breed());
	viewName = string("view") + std::to_string(getEntity()->id()) + viewName;

	Camera* pCam = getComponent<Camera>();
	ServiceLocator<OsgDevice>::getService()->destroyView(viewName);
	_view = ServiceLocator<OsgDevice>::getService()->createView(viewName, pCam->_lRatio, pCam->_rRatio, pCam->_bRatio, pCam->_tRatio,
		osg::Vec4(pCam->_red / 255.0f, pCam->_green / 255.0f, pCam->_blue / 255.0f, pCam->_alpha / 255.0f));

	_manipulatorMgr = new CameraManipulatorManager;
	_view->setCameraManipulator(_manipulatorMgr);

	osg::Group* pRoot = ServiceLocator<OsgEarthContext>::getService()->getRootNode();
	pRoot->addChild(getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transWorld);
	_view->setSceneData(pRoot);
}

void CameraImpl::update()
{
	Camera* cam = getComponent<Camera>();
	if (cam->dirtyBit().checkState(Camera::Manipulator_))
		_manipulatorMgr->selectMatrixManipulator(cam->_manipulatorKey);

	if (cam->dirtyBit().checkState(Camera::TrackEnt_))
		trackingEnt();

	if (cam->dirtyBit().checkState(Camera::Viewport_))
		ServiceLocator<OsgDevice>::getService()->resizeView(_view.get(), cam->_lRatio, cam->_rRatio, cam->_bRatio, cam->_tRatio);

	if (cam->dirtyBit().checkState(Camera::Bgcolour_))
		_view->getCamera()->setClearColor(osg::Vec4(cam->_red / 255.0f, cam->_green / 255.0f, cam->_blue / 255.0f, cam->_alpha / 255.0f));
}

void CameraImpl::trackingEnt()
{
	Camera* pCam = getComponent<Camera>();
	int id = pCam->_trackEntID;
	int breed = pCam->_trackEntBreed;
	Entity* pTrackEnt = getEntity()->getSpawner()->gain(id, breed);
	if (pTrackEnt)
	{
		osg::Node* node = nullptr;
		ModelImpl* pModelImpl = pTrackEnt->getComponentImpl<ModelImpl>();
		if (pModelImpl && pModelImpl->_model)
			node = pModelImpl->_model;
		else
			node = pTrackEnt->getComponentImpl<DoFImpl>()->_transWorld;

		if (node)
		{
			switch (pCam->_manipulatorKey)
			{
			case Camera::Earth_:
				break;
			case Camera::NodeTracker_:
				break;
			case Camera::Trackball_:
				break;
			case Camera::Flight_:
				break;
			case Camera::Drive_:
				break;
			case Camera::Terrain_:
				break;
			case Camera::Orbit_:
				break;
			case Camera::FirstPerson_:
				break;
			case Camera::Spherical_:
				break;
			default:
				break;
			}
		}
	}
}

ZOO_REFLEX_IMPLEMENT(EnvironmentImpl);
EnvironmentImpl::EnvironmentImpl()
{

}

EnvironmentImpl::~EnvironmentImpl()
{

}

void EnvironmentImpl::awake()
{

}

void EnvironmentImpl::update()
{

}

#ifdef NEED_OSGEARTH_LIBRARY
ZOO_REFLEX_IMPLEMENT(EarthImpl);
EarthImpl::EarthImpl()
{
}

EarthImpl::~EarthImpl()
{
	CmdManager::getSingleton().sendEvent(EVENT_RESET_OSGEARTH_CONTEXT);
	if (_mapNode)
		ServiceLocator<OsgEarthContext>::getService()->getRootNode()->removeChild(_mapNode);
}

void EarthImpl::awake()
{
	Earth* earth = getComponent<Earth>();
	if (earth->_earthFile == "")
		return;

	CmdManager::getSingleton().sendEvent(EVENT_RESET_OSGEARTH_CONTEXT);

	if (_mapNode)
		ServiceLocator<OsgEarthContext>::getService()->getRootNode()->removeChild(_mapNode);

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + earth->_earthFile);
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
	if (_mapNode)
	{
		CameraImpl* pCameraImpl = getEntity()->getSpawner()->getComponentImpl<CameraImpl>();
		_manipulator = dynamic_cast<osgEarth::Util::EarthManipulator*>(pCameraImpl->_manipulatorMgr->getMatrixManipulatorWithIndex(Camera::Earth_));
		_manipulator->getSettings()->setArcViewpointTransitions(true);
		_manipulator->setNode(_mapNode);
		ServiceLocator<OsgEarthContext>::getService()->getRootNode()->addChild(_mapNode);
		ServiceLocator<OsgEarthContext>::getService()->setOpMapNode(_mapNode);
		ServiceLocator<OsgEarthContext>::getService()->setOpManipulator(_manipulator);
		ServiceLocator<OsgEarthContext>::getService()->setOpView(getEntity()->getSpawner()->getComponentImpl<CameraImpl>()->_view.get());

		if (_mapNode->getMap()->getSRS()->isGeographic())
		{
			osgEarth::Viewpoint vp;
			vp.focalPoint() = GeoPoint(_mapNode->getMap()->getSRS(), 110.0, 30, 0, ALTMODE_ABSOLUTE);
			vp.heading()->set(0.0, Units::DEGREES);
			vp.pitch()->set(-89.0, Units::DEGREES);
			vp.range()->set(_mapNode->getMap()->getSRS()->getEllipsoid()->getRadiusEquator() * 3.0, Units::METERS);
			vp.positionOffset()->set(0, 0, 0);
			_manipulator->setHomeViewpoint(vp);
		}

		_starrySky = new StarrySky(_mapNode, getEntity()->getSpawner()->getComponentImpl<CameraImpl>()->_view.get());
		_starrySky->initialize();
		_starrySky->setSunVisible(earth->_skyVisibility[Earth::sun_]);
		_starrySky->setMoonVisible(earth->_skyVisibility[Earth::moon_]);
		_starrySky->setStarsVisible(earth->_skyVisibility[Earth::star_]);
		_starrySky->setNebulaVisible(earth->_skyVisibility[Earth::nebula_]);
		_starrySky->setAtmosphereVisible(earth->_skyVisibility[Earth::atmosphere_]);
		earth->_sunlightIntensity = _starrySky->getLight()->getAmbient().x();
	}
}

void EarthImpl::update()
{
	Earth* earth = getComponent<Earth>();
	if (_starrySky)
	{
		if (earth->dirtyBit().checkState(Earth::SunVisible_))
			_starrySky->setSunVisible(earth->_skyVisibility[Earth::sun_]);
		if (earth->dirtyBit().checkState(Earth::MoonVisible_))
			_starrySky->setMoonVisible(earth->_skyVisibility[Earth::moon_]);
		if (earth->dirtyBit().checkState(Earth::StarVisible_))
			_starrySky->setStarsVisible(earth->_skyVisibility[Earth::star_]);
		if (earth->dirtyBit().checkState(Earth::NebulaVisible_))
			_starrySky->setNebulaVisible(earth->_skyVisibility[Earth::nebula_]);
		if (earth->dirtyBit().checkState(Earth::AtmosphereVisible_))
			_starrySky->setAtmosphereVisible(earth->_skyVisibility[Earth::atmosphere_]);
		if (earth->dirtyBit().checkState(Earth::SunlightIntensity_))
			_starrySky->getLight()->setAmbient(osg::Vec4d(earth->_sunlightIntensity, earth->_sunlightIntensity, earth->_sunlightIntensity, 1));
	}
}
#endif
