#pragma once

#include <SharedObject/SharedObject.h>

class SharedObject_Export DoF : public SharedObject
{
	PROPERTY_RW(double, _x, X)
	PROPERTY_RW(double, _y, Y)
	PROPERTY_RW(double, _z, Z)
	PROPERTY_RW(float, _h, H)
	PROPERTY_RW(float, _p, P)
	PROPERTY_RW(float, _r, R)
	PROPERTY_RW(float, _scale, Scale)
	PROPERTY_R(DoF*, _parent, Parent)
public:
	DoF(int id, int group);
	~DoF();
	int getChildrenCount() const;
	virtual bool addChild(DoF* child);
	virtual bool removeChild(DoF* child);
	virtual void removeAllChildren();

private:
	std::list<DoF*> _children;
};
