#pragma once

#include <zoo/Singleton.h>
#include <zooCmd_osg/Support.h>

enum ENTITY_TYPE
{
	ENTITY_WEAPON = 0,
	ENTITY_EFFECT,
	ENTITY_REDARMY,
	ENTITY_BLUEARMY,
	ENTITY_ALLYARMY,
	ENTITY_STATIONARY,
	ENTITY_WARREPORTER,
	ENTITY_COUNT
};

struct ENTITY
{
	class DoF* _dof;
	osg::ref_ptr<osg::Node> _entNode;
	osg::ref_ptr<osg::Group> _entGroup;
	osg::ref_ptr<osg::Switch> _switchNode;
	osg::ref_ptr<osg::MatrixTransform> _entTransform;
	osg::ref_ptr<osgUtil::LineSegmentIntersector> m_lineSeg;
	ENTITY() : _dof(nullptr) {}
};

class EntityManager : public Singleton<EntityManager>
{
public:
	EntityManager();
	~EntityManager();
	void updateEntitiesDoF();
	ENTITY findEnt(int id, ENTITY_TYPE type);
	bool isEmpty() const;

public:
	void onCreateWeapon(const UserData& userdata);
	void onCreateEffect(const UserData& userdata);
	void onCreateRedArmy(const UserData& userdata);
	void onCreateBlueArmy(const UserData& userdata);
	void onCreateAllyArmy(const UserData& userdata);
	void onCreateStationary(const UserData& userdata);
	void onChangeParentDoF(const UserData& userdata);

private:
	ENTITY addEntity(const UserData& userdata, ENTITY_TYPE entType);

private:
	const char* _entNames[ENTITY_COUNT];
	std::map<long long, ENTITY> _entities;
};
