#pragma once

#include <zoo/Support.h>

namespace zoo {

class _zooExport DoF
{
	PROPERTY_R(int, _id, ID)
	PROPERTY_R(int, _group, Group)
	PROPERTY_R(DoF*, _parent, Parent)
	PROPERTY_RW(double, _x, X)
	PROPERTY_RW(double, _y, Y)
	PROPERTY_RW(double, _z, Z)
	PROPERTY_RW(double, _h, H)
	PROPERTY_RW(double, _p, P)
	PROPERTY_RW(double, _r, R)
	PROPERTY_RW(double, _scale, Scale)
public:
	DoF(int id, int group);
	virtual ~DoF();
	void addChild(DoF* child);
	void removeChild(DoF* child);
	void removeAllChildren();
	int  getChildrenCount() const;

public:
	typedef std::map<int, DoF*> Map;
	static int const GroupCount = 10;
	static const Map& getMap(int group);
	static DoF* findDoF(int id, int group);

private:
	std::list<DoF*> _children;
};

}
