#ifndef __OSG_COMPONENTIMPLS_H__
#define __OSG_COMPONENTIMPLS_H__

#include <zooCmd_osg/OsgEarthSupport.h>
#include <component/war/SharedComponents.h>

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
	osg::ref_ptr<osg::MatrixTransform> _transform;
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
	ZOO_REFLEX_DECLARE(CameraImpl)
public:
	CameraImpl();
	~CameraImpl();
	void awake();
	void update();
	void setTracker(osgGA::CameraManipulator* pTracker);

private:
	void setTrackNode(osg::Node* node);
	void onSetTrackEnt(const UserData& userdata);

public:
	osg::ref_ptr<osgViewer::View> _view;
};

class EnvironmentImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(EnvironmentImpl)
public:
	EnvironmentImpl();
	~EnvironmentImpl();
	void awake();
	void update();
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
