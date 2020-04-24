#pragma once

#include <SharedObject/SharedObject.h>

class SharedObject_Export DoF : public SharedObject
{
	PROPERTY_RW(double, _x, PosX)
	PROPERTY_RW(double, _y, PosY)
	PROPERTY_RW(double, _z, PosZ)
	PROPERTY_RW(float, _heading, Heading)
	PROPERTY_RW(float, _pitch, Pitch)
	PROPERTY_RW(float, _roll, Roll)
	PROPERTY_RW(float, _sx, ScaleX)
	PROPERTY_RW(float, _sy, ScaleY)
	PROPERTY_RW(float, _sz, ScaleZ)
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
