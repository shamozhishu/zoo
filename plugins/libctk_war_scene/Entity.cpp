#include "Entity.h"

Entity::Entity(int id)
	: _id(id)
	, _x(0)
	, _y(0)
	, _z(0)
	, _h(0)
	, _p(0)
	, _r(0)
	, _scale(1)
{
}

Entity::~Entity()
{
}

void Entity::reset()
{
	_x = 0;
	_y = 0;
	_z = 0;
	_h = 0;
	_p = 0;
	_r = 0;
	_scale = 0;

	std::map<long long, Entity*>::iterator it = _children.begin();
	for (; it != _children.end(); it++)
		it->second->reset();
}

int Entity::getID() const
{
	return _id;
}

void Entity::update(float dt)
{
	if (_children.size() > 0)
	{
		for (std::map<long long, Entity*>::iterator it = _children.begin(); it != _children.end(); ++it)
		{
			Entity* pEnt = it->second;
			if (pEnt)
				pEnt->update(dt);
		}
	}
}

void Entity::setPos(double x, double y, double z)
{
	_x = x;
	_y = y;
	_z = z;
}

void Entity::setRot(double h, double p, double r)
{
	_h = h;
	_p = p;
	_r = r;
}

void Entity::setScale(double scale)
{
	_scale = scale;
}

int Entity::getChildrenCount() const
{
	return (int)_children.size();
}

void Entity::addChild(Entity* ent)
{
	int id = ent->getID();
	int type = ent->getType();
	long long i64ID = type;
	i64ID = i64ID << 32 | id;

	std::map<long long, Entity*>::iterator it = _children.find(i64ID);
	if (it == _children.end())
		_children[i64ID] = ent;
}

void Entity::removeChild(int id, int type)
{
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _children.find(i64ID);
	if (it != _children.end())
	{
		delete it->second;
		_children.erase(it);
	}
}

Entity* Entity::findChild(int id, int type)
{
	long long i64ID = type;
	i64ID = i64ID << 32 | id;
	std::map<long long, Entity*>::iterator it = _children.find(i64ID);
	if (it != _children.end())
		return it->second;
	return nullptr;
}
