#pragma once

#include <zoo/Singleton.h>
#include <zooCmd_osg/Support.h>

enum ENTITY_TYPE
{
	ENTITY_EFFECT,
	ENTITY_WEAPON,
	ENTITY_REDARMY,
	ENTITY_BLUEARMY,
	ENTITY_ALLYARMY,
	ENTITY_STATICOBJ
};

struct ENTITY
{
	DoF* _dof;
	osg::ref_ptr<osg::Node> _entNode;
	osg::ref_ptr<osg::Group> _entGroup;
	osg::ref_ptr<osg::Switch> _switchNode;
	osg::ref_ptr<osg::MatrixTransform> _entTransform;
	osg::ref_ptr<osgUtil::LineSegmentIntersector> m_lineSeg;
};

class EntityManager : public Singleton<EntityManager>
{
public:
	EntityManager();
	~EntityManager();

public zoo_slots :
	void onCreateEffect(const UserData& userdata);
	void onCreateWeapon(const UserData& userdata);
	void onCreateRedArmy(const UserData& userdata);
	void onCreateBlueArmy(const UserData& userdata);
	void onCreateAllyArmy(const UserData& userdata);
	void onCreateStaticObj(const UserData& userdata);

private:
	ENTITY addEntity(const UserData& userdata, ENTITY_TYPE entType);

private:
	std::map<long long, ENTITY> _entities;
};
