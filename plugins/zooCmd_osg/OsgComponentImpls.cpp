#include "OsgComponentImpls.h"
#include "RenderDevice.h"
#include <zoo/Utils.h>
#include "StarrySky.h"
#include "SkyBox.h"
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
	, _currentOsgMat(nullptr)
	, _defaultStateSet(new osg::StateSet)
{
}

void MaterialImpl::update(Material* material)
{
	if (!material || !_currentOsgMat)
		return;

	_currentOsgMat->refresh(material->_uniforms);
}

void MaterialImpl::update(Material* material, osg::Node* node)
{
	if (!material || !node)
		return;

	MaterialManager::getSingleton().detach(_lastUseMatName, node);
	_lastUseMatName = material->_currentUseMatName;
	_currentOsgMat = MaterialManager::getSingleton().attach(material, node);
	if (!_currentOsgMat)
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
		osgEarth::MapNode* pMapNode = getEntity()->getSpawner()->getContext<OsgEarthContext>()->getEarthMapNode();
		if (pOsgEarthUtils && pMapNode)
		{
			double x, y, z;
			pOsgEarthUtils->convertLLHToXYZ(getEntity()->getSpawner()->getContext<OsgEarthContext>(), dof->_x, dof->_y, dof->_z, x, y, z);
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
		osg::Group* pScene = getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode();
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

	if (model->dirtyBit().checkState(Material::Uniform_))
		_material.update(&model->_material);
}

ZOO_REFLEX_IMPLEMENT(CameraImpl);
CameraImpl::CameraImpl()
	: _root(new osg::Group)
	, _viewID(-1)
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

	ServiceLocator<OsgDevice>::getService()->destroyView(_viewID);
}

void CameraImpl::awake()
{
	Camera* cam = getComponent<Camera>();
	_viewID = cam->_viewID;
	ServiceLocator<OsgDevice>::getService()->destroyView(cam->_viewID);
	_view = ServiceLocator<OsgDevice>::getService()->createView(cam->_viewID, ServiceLocator<OsgDevice>::getService()->getGraphicsWnd(cam->_windowID),
		cam->_lRatio, cam->_rRatio, cam->_bRatio, cam->_tRatio,
		osg::Vec4(cam->_red / 255.0f, cam->_green / 255.0f, cam->_blue / 255.0f, cam->_alpha / 255.0f));

	cam->dirtyBit().eraseState(Camera::Viewport_);
	cam->dirtyBit().eraseState(Camera::Bgcolour_);

	_manipulatorMgr = new CameraManipulatorManager(getEntity()->getSpawner()->getContext<OsgEarthContext>());
	_view->setCameraManipulator(_manipulatorMgr);
	_view->setSceneData(_root);

	osg::Group* pScene = getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode();
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
	bool haveTrackEnt = false;
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
			haveTrackEnt = true;
			_view->setSceneData(node);
		}
	}

	if (!haveTrackEnt)
		_view->setSceneData(_root);
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
		_passGroups[idx]->addChild(getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode());
		break;
	case Camera::TextureColor_:
		_passGroups[idx]->setAllChildrenOn();
		_passGroups[idx]->removeChildren(0, 1);
		_multiPasses[idx] = createColorPass(getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode());
		_passGroups[idx]->addChild(_multiPasses[idx].first);
		break;
	case Camera::TextureDepth_:
		_passGroups[idx]->setAllChildrenOn();
		_passGroups[idx]->removeChildren(0, 1);
		_multiPasses[idx] = createDepthPass(getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode());
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
	, _skyBox(new SkyBox)
{
	_skyBox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
}

EnvironmentImpl::~EnvironmentImpl()
{
	osg::Group* pScene = getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode();
	if (_switch)
		pScene->removeChild(_switch.get());
	if (_skyBox)
		pScene->removeChild(_skyBox.get());
}

void EnvironmentImpl::awake()
{
	OsgEarthContext* pOsgEarthContext = getEntity()->getSpawner()->getContext<OsgEarthContext>();
	_switch->removeChildren(0, _switch->getNumChildren());
	_switch->addChild(new WeatherEffect(pOsgEarthContext), false);
	_switch->setAllChildrenOff();
	if (_switch->getNumParents() == 0)
		pOsgEarthContext->getSceneNode()->addChild(_switch);
	if (_skyBox->getNumParents() == 0)
		pOsgEarthContext->getSceneNode()->addChild(_skyBox);
}

void EnvironmentImpl::update()
{
	Environment* pEnvir = getComponent<Environment>();
	if (pEnvir->dirtyBit().checkState(Environment::SkyBox_))
	{
		_skyBox->removeChildren(0, _skyBox->getNumChildren());
		if (pEnvir->_skyBox._currentUseMatName == ZOO_STRING(SkyBox))
		{
			osg::Group* pScene = getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode();
			osg::ref_ptr<osg::Geode> geode = new osg::Geode;
			geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, 0), pScene->getBound().radius())));
			geode->setCullingActive(false);
			osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;
			trans->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(-90.0f), osg::X_AXIS));
			trans->addChild(geode.get());
			_skyBox->addChild(trans.get());
		}
	}

	if (pEnvir->dirtyBit().checkState(Material::Changed_))
	{
		((SkyBox*)_skyBox.get())->setEnvironmentMap(0,
			osgDB::readImageFile(ZOO_DATA_ROOT_DIR + pEnvir->_skyBox._textureFiles[0].second),
			osgDB::readImageFile(ZOO_DATA_ROOT_DIR + pEnvir->_skyBox._textureFiles[1].second),
			osgDB::readImageFile(ZOO_DATA_ROOT_DIR + pEnvir->_skyBox._textureFiles[2].second),
			osgDB::readImageFile(ZOO_DATA_ROOT_DIR + pEnvir->_skyBox._textureFiles[3].second),
			osgDB::readImageFile(ZOO_DATA_ROOT_DIR + pEnvir->_skyBox._textureFiles[4].second),
			osgDB::readImageFile(ZOO_DATA_ROOT_DIR + pEnvir->_skyBox._textureFiles[5].second));
	}

	WeatherEffect* effect = dynamic_cast<WeatherEffect*>(_switch->getChild(0));
	if (effect && pEnvir->dirtyBit().checkState(Environment::Weather_))
	{
		_switch->setAllChildrenOff();
		switch (pEnvir->_type)
		{
		case Environment::Rain_: effect->rain(pEnvir->_intensity); _switch->setValue(0, true); break;
		case Environment::Snow_: effect->snow(pEnvir->_intensity); _switch->setValue(0, true); break;
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
	UserData evtProps;
	evtProps.setData(getEntity()->getSpawner()->id());
	CmdManager::getSingleton().sendEvent(EVENT_RESET_OSGEARTH_CONTEXT, evtProps);
	if (_mapNode)
		getEntity()->getSpawner()->getContext<OsgEarthContext>()->getSceneNode()->removeChild(_mapNode);
}

void EarthImpl::awake()
{
	Earth* earth = getComponent<Earth>();
	if (earth->_earthFile == "")
		return;

	UserData evtProps;
	evtProps.setData(getEntity()->getSpawner()->id());
	CmdManager::getSingleton().sendEvent(EVENT_RESET_OSGEARTH_CONTEXT, evtProps);
	OsgEarthContext* pOsgEarthContext = getEntity()->getSpawner()->getContext<OsgEarthContext>();
	if (_mapNode)
		pOsgEarthContext->getSceneNode()->removeChild(_mapNode);

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + earth->_earthFile);
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
	if (_mapNode)
	{
		CameraImpl* pCameraImpl = getEntity()->getSpawner()->getComponentImpl<CameraImpl>();
		_manipulator = dynamic_cast<osgEarth::Util::EarthManipulator*>(pCameraImpl->_manipulatorMgr->getMatrixManipulatorWithIndex(Camera::Earth_));
		_manipulator->getSettings()->setArcViewpointTransitions(true);
		_manipulator->setNode(_mapNode);
		pOsgEarthContext->getSceneNode()->addChild(_mapNode);
		pOsgEarthContext->setEarthMapNode(_mapNode);
		pOsgEarthContext->setEarthManipulator(_manipulator);
		pOsgEarthContext->setEarthView(getEntity()->getSpawner()->getComponentImpl<CameraImpl>()->_view);

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
