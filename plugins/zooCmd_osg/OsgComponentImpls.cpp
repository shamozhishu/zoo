#include "OsgComponentImpls.h"
#include "InputDevice.h"
#include <zoo/Utils.h>
#include "StarrySky.h"
#include "WeatherEffect.h"
#include <zooCmd/ZooCmd.h>
#include <zooCmd/CmdManager.h>
#include <UniversalGlobalServices.h>
#include <zooCmd_osg/OsgEarthContext.h>
#include "PublicFunctions.h"
#include "MeshManager.h"
#include "MaterialManager.h"

using namespace osgGA;
using namespace zooCmd_osg;
#ifdef NEED_OSGEARTH_LIBRARY
using namespace osgEarth::Util;
#endif

MeshImpl::MeshImpl(osg::Group* parent)
	: _meshNode(new osg::Group)
{
	parent->addChild(_meshNode);
}

MeshImpl::~MeshImpl()
{
	if (_meshNode->getNumParents() > 0)
		_meshNode->getParent(0)->removeChild(_meshNode);
}

void MeshImpl::update(Mesh* mesh)
{
	if (mesh)
	{
		MeshManager::getSingleton().detach(_meshNode);
		MeshManager::getSingleton().attach(mesh, _meshNode);
	}
}

osg::Group* MeshImpl::getMeshNode() const
{
	return _meshNode.get();
}

MaterialImpl::MaterialImpl()
	: _lastUseMatName("Default")
{
	_defaultStateSet = new osg::StateSet;
}

void MaterialImpl::update(Material* material, osg::Node* node)
{
	if (!material || !node)
		return;

	MaterialManager::getSingleton().detach(_lastUseMatName, node);
	_lastUseMatName = material->_currentUseMatName;
	if (!MaterialManager::getSingleton().attach(material, node))
		node->setStateSet(_defaultStateSet);
}

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
		CoordTransformUtil* pOsgEarthUtils = ServiceLocator<CoordTransformUtil>::getService();
		osgEarth::MapNode* pMapNode = ServiceLocator<OsgEarthContext>::getService()->getOpMapNode();
		if (pOsgEarthUtils && pMapNode)
		{
			double x, y, z;
			pOsgEarthUtils->convertLLHToXYZ(dof->_x, dof->_y, dof->_z, x, y, z);
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
		osg::Group* pScene = ServiceLocator<OsgEarthContext>::getService()->getSceneNode();
		pScene->addChild(_transWorld.get());
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
	, _mesh(_switch)
{
	_switch->setAllChildrenOn();
	_switch->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
}

ModelImpl::~ModelImpl()
{
	if (_switch->getNumParents() > 0)
	{
		osg::Group* parent = _switch->getParent(0);
		if (parent)
			parent->removeChild(_switch.get());
	}
}

void ModelImpl::awake()
{
	SignalTrigger::connect(*getEntity(), [this]
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

	if (model->dirtyBit().checkState(Mesh::Changed_))
		_mesh.update(&model->_mesh);

	if (model->dirtyBit().checkState(Material::Changed_))
		_material.update(&model->_material, _mesh.getMeshNode());
}

ZOO_REFLEX_IMPLEMENT(CameraImpl);
CameraImpl::CameraImpl()
	: _root(new osg::Group)
{
	for (unsigned int i = 0; i < Camera::MaxPassCount; ++i)
	{
		_passGroups[i] = new osg::Switch;
		_passGroups[i]->setAllChildrenOff();
		_root->addChild(_passGroups[i]);
	}
}

CameraImpl::~CameraImpl()
{
	if (_view)
	{
		_view->setCameraManipulator(nullptr);
		_view->setSceneData(nullptr);
	}

	string viewName = getEntity()->breed() == -1 ? "" : string("_") + std::to_string(getEntity()->breed());
	viewName = string("view") + std::to_string(getEntity()->id()) + viewName;
	ServiceLocator<OsgDevice>::getService()->destroyView(viewName);
}

void CameraImpl::awake()
{
	string viewName = getEntity()->breed() == -1 ? "" : string("_") + std::to_string(getEntity()->breed());
	viewName = string("view") + std::to_string(getEntity()->id()) + viewName;

	Camera* cam = getComponent<Camera>();
	ServiceLocator<OsgDevice>::getService()->destroyView(viewName);
	_view = ServiceLocator<OsgDevice>::getService()->createView(viewName, cam->_lRatio, cam->_rRatio, cam->_bRatio, cam->_tRatio,
		osg::Vec4(cam->_red / 255.0f, cam->_green / 255.0f, cam->_blue / 255.0f, cam->_alpha / 255.0f));
	cam->dirtyBit().eraseState(Camera::Viewport_);
	cam->dirtyBit().eraseState(Camera::Bgcolour_);

	_manipulatorMgr = new CameraManipulatorManager;
	_view->setCameraManipulator(_manipulatorMgr);
	_view->setSceneData(_root);

	osg::Group* pScene = ServiceLocator<OsgEarthContext>::getService()->getSceneNode();
	osg::Node* pWorld = getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transWorld;
	if (pWorld->getNumParents() == 0)
		pScene->addChild(pWorld);

	update();
	cam->dirtyBit().clearState();
}

void CameraImpl::update()
{
	Camera* cam = getComponent<Camera>();
	if (cam->dirtyBit().checkState(Camera::Manipulator_))
		_manipulatorMgr->selectMatrixManipulator(cam->_manipulatorKey);

	if (cam->dirtyBit().checkState(Camera::TrackEnt_))
		trackingEntity();

	if (cam->dirtyBit().checkState(Camera::Viewport_))
		ServiceLocator<OsgDevice>::getService()->resizeView(_view.get(), cam->_lRatio, cam->_rRatio, cam->_bRatio, cam->_tRatio);

	if (cam->dirtyBit().checkState(Camera::Bgcolour_))
		_view->getCamera()->setClearColor(osg::Vec4(cam->_red / 255.0f, cam->_green / 255.0f, cam->_blue / 255.0f, cam->_alpha / 255.0f));

	if (cam->dirtyBit().checkState(Camera::Pass1_))
		changePassRT(0);

	if (cam->dirtyBit().checkState(Camera::Pass2_))
		changePassRT(1);

	if (cam->dirtyBit().checkState(Camera::Pass3_))
		changePassRT(2);

	if (cam->dirtyBit().checkState(Camera::Pass4_))
		changePassRT(3);

	if (cam->dirtyBit().checkState(Camera::Pass5_))
		changePassRT(4);

	if (cam->dirtyBit().checkState(Camera::Pass6_))
		changePassRT(5);

	if (cam->dirtyBit().checkState(Camera::Pass7_))
		changePassRT(6);

	if (cam->dirtyBit().checkState(Camera::Pass8_))
		changePassRT(7);
}

void CameraImpl::trackingEntity()
{
	Camera* cam = getComponent<Camera>();
	int id = cam->_trackEntID;
	int breed = cam->_trackEntBreed;
	Entity* pTrackEnt = getEntity()->getSpawner()->gain(id, breed);
	if (pTrackEnt)
	{
		osg::Node* node = nullptr;
		ModelImpl* pModelImpl = pTrackEnt->getComponentImpl<ModelImpl>();
		if (pModelImpl && pModelImpl->_switch)
			node = pModelImpl->_switch;
		else
			node = pTrackEnt->getComponentImpl<DoFImpl>()->_transWorld;

		if (node)
		{
			switch (cam->_manipulatorKey)
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

void CameraImpl::changePassRT(Camera::PassIndex idx)
{
	osg::Camera* hudCam = nullptr;
	Camera* cam = getComponent<Camera>();
	switch (cam->_passes[idx]._rt)
	{
	case Camera::Nothing_:
		_passGroups[idx]->setAllChildrenOff();
		_passGroups[idx]->removeChildren(0, 1);
		_multiPasses[idx] = make_pair(_view->getCamera(), nullptr);
		break;
	case Camera::Window_:
		_passGroups[idx]->setAllChildrenOn();
		_passGroups[idx]->removeChildren(0, 1);
		_multiPasses[idx] = make_pair(_view->getCamera(), nullptr);
		_passGroups[idx]->addChild(ServiceLocator<OsgEarthContext>::getService()->getSceneNode());
		break;
	case Camera::TextureColor_:
		_passGroups[idx]->setAllChildrenOn();
		_passGroups[idx]->removeChildren(0, 1);
		_multiPasses[idx] = createColorPass(ServiceLocator<OsgEarthContext>::getService()->getSceneNode());
		_passGroups[idx]->addChild(_multiPasses[idx].first);
		break;
	case Camera::TextureDepth_:
		_passGroups[idx]->setAllChildrenOn();
		_passGroups[idx]->removeChildren(0, 1);
		_multiPasses[idx] = createDepthPass(ServiceLocator<OsgEarthContext>::getService()->getSceneNode());
		_passGroups[idx]->addChild(_multiPasses[idx].first);
		break;
	case Camera::HeadUpDisplay_:
		_passGroups[idx]->setAllChildrenOn();
		_passGroups[idx]->removeChildren(0, 1);
		hudCam = createHUDCamera(0.0, 1.0, 0.0, 1.0);
		hudCam->addChild(createScreenQuad(1.0f, 1.0f));
		_multiPasses[idx] = make_pair(hudCam, nullptr);
		_passGroups[idx]->addChild(hudCam);
		break;
	}
}

CameraImpl::Pass CameraImpl::createColorPass(osg::Node* scene)
{
	osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D;
	osg::Viewport* vp = _view->getCamera()->getViewport();
	tex2D->setTextureSize(vp->width(), vp->height());
	tex2D->setInternalFormat(GL_RGBA);
	osg::ref_ptr<osg::Camera> camera = createRTTCamera(osg::Camera::COLOR_BUFFER, tex2D.get());
	camera->addChild(scene);
	return Pass(camera.release(), tex2D.get());
}

CameraImpl::Pass CameraImpl::createDepthPass(osg::Node* scene)
{
	osg::ref_ptr<osg::Texture2D> tex2D = new osg::Texture2D;
	osg::Viewport* vp = _view->getCamera()->getViewport();
	tex2D->setTextureSize(vp->width(), vp->height());
	tex2D->setInternalFormat(GL_DEPTH_COMPONENT24);
	tex2D->setSourceFormat(GL_DEPTH_COMPONENT);
	tex2D->setSourceType(GL_FLOAT);
	osg::ref_ptr<osg::Camera> camera = createRTTCamera(osg::Camera::DEPTH_BUFFER, tex2D.get());
	camera->addChild(scene);
	return Pass(camera.release(), tex2D.get());
}

ZOO_REFLEX_IMPLEMENT(EnvironmentImpl);
EnvironmentImpl::EnvironmentImpl()
	: _switch(new osg::Switch)
{
	_switch->addChild(new WeatherEffect, false);
	osg::Group* pScene = ServiceLocator<OsgEarthContext>::getService()->getSceneNode();
	if (pScene)
		pScene->addChild(_switch);
}

EnvironmentImpl::~EnvironmentImpl()
{
	osg::Group* pScene = ServiceLocator<OsgEarthContext>::getService()->getSceneNode();
	if (_switch)
		pScene->removeChild(_switch.get());
}

void EnvironmentImpl::awake()
{
	_switch->setAllChildrenOff();
}

void EnvironmentImpl::update()
{
	Environment* pEnvironment = getComponent<Environment>();
	WeatherEffect* effect = dynamic_cast<WeatherEffect*>(_switch->getChild(0));
	if (effect && pEnvironment->dirtyBit().checkState(Environment::Weather_))
	{
		_switch->setAllChildrenOff();
		switch (pEnvironment->_type)
		{
		case Environment::Rain_: effect->rain(pEnvironment->_intensity); _switch->setValue(0, true); break;
		case Environment::Snow_: effect->snow(pEnvironment->_intensity); _switch->setValue(0, true); break;
		}
	}
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
		ServiceLocator<OsgEarthContext>::getService()->getSceneNode()->removeChild(_mapNode);
}

void EarthImpl::awake()
{
	Earth* earth = getComponent<Earth>();
	if (earth->_earthFile == "")
		return;

	CmdManager::getSingleton().sendEvent(EVENT_RESET_OSGEARTH_CONTEXT);

	if (_mapNode)
		ServiceLocator<OsgEarthContext>::getService()->getSceneNode()->removeChild(_mapNode);

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + earth->_earthFile);
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
	if (_mapNode)
	{
		CameraImpl* pCameraImpl = getEntity()->getSpawner()->getComponentImpl<CameraImpl>();
		_manipulator = dynamic_cast<osgEarth::Util::EarthManipulator*>(pCameraImpl->_manipulatorMgr->getMatrixManipulatorWithIndex(Camera::Earth_));
		_manipulator->getSettings()->setArcViewpointTransitions(true);
		_manipulator->setNode(_mapNode);
		ServiceLocator<OsgEarthContext>::getService()->getSceneNode()->addChild(_mapNode);
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
