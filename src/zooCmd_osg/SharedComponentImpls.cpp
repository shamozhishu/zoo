#include "SharedComponentImpls.h"
#include <zooCmd_osg/InputDevice.h>
#include <zoo/Utils.h>

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

void DoFImpl::onAddChild(const UserData& userdata)
{
	ModelImpl* pModelImpl = getEntity()->getComponentImpl<ModelImpl>();
	if (pModelImpl)
		_transform->addChild(pModelImpl->_switch);
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
	SignalTrigger::connect(*pEnt, pEnt->getComponentImpl<DoFImpl>(), &DoFImpl::onAddChild);
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
}

void CameraImpl::awake()
{
	createTracker();
	Camera* pCam = getComponent<Camera>();
	if (_view)
		InputDevice::getIns()->getViewer()->removeView(_view);
	_view = InputDevice::getIns()->createView(pCam->_lRatio, pCam->_rRatio, pCam->_bRatio, pCam->_tRatio,
		osg::Vec4(pCam->_red / 255.0f, pCam->_green / 255.0f, pCam->_blue / 255.0f, pCam->_alpha / 255.0f));
	_view->setSceneData(getEntity()->getSpawner()->getComponentImpl<DoFImpl>()->_transform);
	_view->setCameraManipulator(_tracker);
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
		InputDevice::getIns()->resizeView(_view, cam->_lRatio, cam->_rRatio, cam->_bRatio, cam->_tRatio);
}

void CameraImpl::createTracker()
{
	_tracker = new osgGA::NodeTrackerManipulator;
}

void CameraImpl::setTrackNode(osg::Node* node)
{
	osgGA::NodeTrackerManipulator* pTracker = dynamic_cast<osgGA::NodeTrackerManipulator*>(_tracker.get());
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
