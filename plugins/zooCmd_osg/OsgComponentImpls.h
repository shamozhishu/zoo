#ifndef __OSG_COMPONENTIMPLS_H__
#define __OSG_COMPONENTIMPLS_H__

#include <zooCmd_osg/OsgEarthSupport.h>
#include <component/war/WarComponents.h>
#include "CameraManipulatorManager.h"

class DoFImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(DoFImpl)
public:
	DoFImpl();
	~DoFImpl();
	void awake();
	void update();

private:
	void remove();
	void locate(DoF* dof);
	void changeParent(DoF* dof);

public:
	osg::ref_ptr<osg::MatrixTransform> _transLocal;
	osg::ref_ptr<osg::MatrixTransform> _transWorld;
};

class ModelImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(ModelImpl)
public:
	ModelImpl();
	~ModelImpl();
	void awake();
	void update();

public:
	osg::ref_ptr<osg::Node> _model;
	osg::ref_ptr<osg::Switch> _switch;
};

class CameraImpl : public ComponentImpl
{
	typedef std::pair<osg::Camera*, osg::Texture2D*> Pass;
	ZOO_REFLEX_DECLARE(CameraImpl)
public:
	CameraImpl();
	~CameraImpl();
	void awake();
	void update();

private:
	void trackingEntity();
	void changePassRT(Camera::PassIndex idx);
	Pass createColorPass(osg::Node* scene);
	Pass createDepthPass(osg::Node* scene);

public:
	osg::ref_ptr<osg::Group> _root;
	osg::ref_ptr<osgViewer::View> _view;
	osg::ref_ptr<CameraManipulatorManager> _manipulatorMgr;
	Pass _multiPasses[Camera::MaxPassCount]; // 最多支持八通道
	osg::ref_ptr<osg::Switch> _passGroups[Camera::MaxPassCount];
};

class EnvironmentImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(EnvironmentImpl)
public:
	EnvironmentImpl();
	~EnvironmentImpl();
	void awake();
	void update();

public:
	osg::ref_ptr<osg::Switch> _switch;
};

#ifdef NEED_OSGEARTH_LIBRARY
class EarthImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(EarthImpl)
public:
	EarthImpl();
	~EarthImpl();
	void awake();
	void update();

public:
	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<class StarrySky> _starrySky;
	osg::ref_ptr<osgEarth::Util::EarthManipulator> _manipulator;
};
#endif

#endif // __OSG_COMPONENTIMPLS_H__
