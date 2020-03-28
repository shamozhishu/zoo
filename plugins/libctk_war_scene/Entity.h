#pragma once

#include <map>

enum ENTITY_TYPE
{
	ENTITY_SCENE = 0,
	ENTITY_EFFECT,
	ENTITY_WEAPON,
	ENTITY_REDARMY,
	ENTITY_BLUEARMY,
	ENTITY_ALLYARMY,
	ENTITY_STATICOBJ
};

class Entity
{
protected:
	Entity(int id);
	virtual ~Entity();
public:
	virtual void reset();
	virtual int getID() const;
	virtual void update(float dt);
	virtual Entity* clone() = 0;
	virtual ENTITY_TYPE getType() const = 0;
	virtual void setPos(double x, double y, double z);
	virtual void setRot(double h, double p, double r);
	virtual void setScale(double scale);


public:
	int getChildrenCount() const;
	void addChild(Entity* ent);
	void removeChild(int id, int type);
	Entity* findChild(int id, int type);

protected:
	int _id;
	double _x;
	double _y;
	double _z;
	double _h;
	double _p;
	double _r;
	double _scale;
	Entity* _parent;
	std::map<long long, Entity*> _children;
};
