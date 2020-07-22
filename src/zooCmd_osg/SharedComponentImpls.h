#ifndef __SHARED_COMPONENTIMPLS_H__
#define __SHARED_COMPONENTIMPLS_H__

#include <zooCmd_osg/Support.h>
#include <component/war/SharedComponents.h>

class _zooCmd_OSG_Export DoFImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(DoFImpl)
public:
	DoFImpl();
	~DoFImpl();
	void awake();
	void update();

public:
	void onAddChild(const UserData& userdata);

private:
	void remove();
	void locate(DoF* dof);
	void changeParent(DoF* dof);

public:
	osg::ref_ptr<osg::MatrixTransform> _transform;
};

class _zooCmd_OSG_Export ModelImpl : public ComponentImpl
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

class _zooCmd_OSG_Export CameraImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(CameraImpl)
public:
	CameraImpl();
	~CameraImpl();
	void awake();
	void update();

protected:
	virtual void createTracker();
	virtual void setTrackNode(osg::Node* node);

private:
	void onSetTrackEnt(const UserData& userdata);

public:
	osgViewer::View* _view;
	osg::ref_ptr<osgGA::CameraManipulator> _tracker;
};

class _zooCmd_OSG_Export EnvironmentImpl : public ComponentImpl
{
	ZOO_REFLEX_DECLARE(EnvironmentImpl)
public:
	EnvironmentImpl();
	~EnvironmentImpl();
	void awake();
	void update();
};

#endif // __SHARED_COMPONENTIMPLS_H__
