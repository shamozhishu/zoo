#include "OsgComponentImpls.h"
#include "InputDevice.h"
#include <zoo/Utils.h>
#include "StarrySky.h"
#include <zooCmd_osg/OsgEarthContext.h>
#include <zooCmd/CmdManager.h>

using namespace zooCmd_osg;

ZOO_REFLEX_IMPLEMENT(DoFImpl);
DoFImpl::DoFImpl()
	: _transform(new osg::MatrixTransform)
{
}

DoFImpl::~DoFImpl()
{
	remove();
}

void DoFImpl::awake()
{
	_transform->setMatrix(osg::Matrix::identity());
}

void DoFImpl::update()
{
	DoF* dof = getComponent<DoF>();
	if (dof->dirtyBit().checkState(DoF::parent_))
		changeParent(dof);

	if (dof->dirtyBit().checkState(DoF::dof_))
		locate(dof);
}

void DoFImpl::remove()
{
	if (_transform->getNumParents() > 0 && !getEntity()->isSpawner())
	{
		auto node = _transform->getParent(0);
		if (node)
			node->asGroup()->removeChild(_transform.get());
	}
}

void DoFImpl::locate(DoF* dof)
{
	_transform->setMatrix(osg::Matrix::scale(dof->_sx, dof->_sy, dof->_sz)
		* osg::Matrix::rotate(osg::DegreesToRadians(dof->_roll), osg::Y_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(dof->_pitch), osg::X_AXIS)
		* osg::Matrix::rotate(osg::DegreesToRadians(dof->_heading), osg::Z_AXIS)
		* osg::Matrix::translate(dof->_x, dof->_y, dof->_z));
}

void DoFImpl::changeParent(DoF* dof)
{
	remove();
	if (dof->_parent)
	{
		DoFImpl* parent = (DoFImpl*)(dof->_parent->getImp());
		parent->_transform->addChild(_transform.get());
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
			getEntity()->getComponentImpl<DoFImpl>()->_transform->addChild(pModelImpl->_switch);
	});
}

void ModelImpl::update()
{
	Model* model = getComponent<Model>();
	if (model->dirtyBit().checkState(Model::visible_))
	{
		if (model->_visible)
			_switch->setAllChildrenOn();
		else
			_switch->setAllChildrenOff();
	}
}

ZOO_REFLEX_IMPLEMENT(CameraImpl);
CameraImpl::CameraImpl()
	: _view(nullptr)
{
}

CameraImpl::~CameraImpl()
{
	if (_view.get())
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

	setTracker(new osgGA::NodeTrackerManipulator);
	_view->setSceneData(getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transform);

	SignalTrigger::connect(*getEntity()->getSpawner(), this, &CameraImpl::onSetTrackEnt);
}

void CameraImpl::update()
{
	Camera* cam = getComponent<Camera>();
	if (cam->dirtyBit().checkState(Camera::trackEnt_))
		onSetTrackEnt(nullptr);

	if (cam->dirtyBit().checkState(Camera::bgcolour_))
		_view->getCamera()->setClearColor(osg::Vec4(cam->_red / 255.0f, cam->_green / 255.0f, cam->_blue / 255.0f, cam->_alpha / 255.0f));

	if (cam->dirtyBit().checkState(Camera::viewport_))
		ServiceLocator<OsgDevice>::getService()->resizeView(_view.get(), cam->_lRatio, cam->_rRatio, cam->_bRatio, cam->_tRatio);
}

void CameraImpl::setTracker(osgGA::CameraManipulator* pTracker)
{
	if (_view.get())
		_view->setCameraManipulator(pTracker);
}

void CameraImpl::setTrackNode(osg::Node* node)
{
	osgGA::NodeTrackerManipulator* pTracker = dynamic_cast<osgGA::NodeTrackerManipulator*>(_view->getCameraManipulator());
	if (pTracker)
		pTracker->setTrackNode(node);
}

void CameraImpl::onSetTrackEnt(const UserData& userdata)
{
	Camera* pCam = getComponent<Camera>();
	int id = pCam->_trackEntID;
	int breed = pCam->_trackEntBreed;
	Entity* pTrackEnt = getEntity()->getSpawner()->gain(id, breed);
	if (pTrackEnt)
	{
		ModelImpl* pModelImpl = pTrackEnt->getComponentImpl<ModelImpl>();
		if (pModelImpl)
			setTrackNode(pModelImpl->_model);
		else
			setTrackNode(pTrackEnt->getComponentImpl<DoFImpl>()->_transform);
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
	if (_mapNode && getEntity()->getSpawner()->getComponentImpl<DoFImpl>())
		getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transform->removeChild(_mapNode);
}

void EarthImpl::awake()
{
	Earth* earth = getComponent<Earth>();
	if (earth->_earthFile == "")
		return;

	CmdManager::getSingleton().sendEvent(EVENT_RESET_OSGEARTH_CONTEXT);

	if (_mapNode)
		getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transform->removeChild(_mapNode);

	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + earth->_earthFile);
	_mapNode = dynamic_cast<osgEarth::MapNode*>(node.get());
	if (_mapNode)
	{
		_manipulator = new osgEarth::Util::EarthManipulator();
		_manipulator->getSettings()->setArcViewpointTransitions(true);
		_manipulator->setNode(_mapNode);
		getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transform->addChild(_mapNode);
		getEntity()->getSpawner()->getComponentImpl<CameraImpl>()->setTracker(_manipulator);

		ServiceLocator<OsgEarthContext>::getService()->setOpMapNode(_mapNode);
		ServiceLocator<OsgEarthContext>::getService()->setOpManipulator(_manipulator);
		ServiceLocator<OsgEarthContext>::getService()->setOpView(getEntity()->getSpawner()->getComponentImpl<CameraImpl>()->_view.get());

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
		if (earth->dirtyBit().checkState(Earth::sunVisible_))
			_starrySky->setSunVisible(earth->_skyVisibility[Earth::sun_]);
		if (earth->dirtyBit().checkState(Earth::moonVisible_))
			_starrySky->setMoonVisible(earth->_skyVisibility[Earth::moon_]);
		if (earth->dirtyBit().checkState(Earth::starVisible_))
			_starrySky->setStarsVisible(earth->_skyVisibility[Earth::star_]);
		if (earth->dirtyBit().checkState(Earth::nebulaVisible_))
			_starrySky->setNebulaVisible(earth->_skyVisibility[Earth::nebula_]);
		if (earth->dirtyBit().checkState(Earth::atmosphereVisible_))
			_starrySky->setAtmosphereVisible(earth->_skyVisibility[Earth::atmosphere_]);
		if (earth->dirtyBit().checkState(Earth::sunlightIntensity_))
			_starrySky->getLight()->setAmbient(osg::Vec4d(earth->_sunlightIntensity, earth->_sunlightIntensity, earth->_sunlightIntensity, 1));
	}
}
#endif
