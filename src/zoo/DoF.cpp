#include <zoo/DoF.h>

namespace zoo {

static DoF::Map s_dofPool[DoF::GroupCount];
DoF::DoF(int id, int group)
	: _id(id)
	, _x(0)
	, _y(0)
	, _z(0)
	, _h(0)
	, _p(0)
	, _r(0)
	, _scale(1)
	, _parent(nullptr)
{
	if (group < 0 || group >= GroupCount)
		group = 0;

	_group = group;
	s_dofPool[group][id] = this;
}

DoF::~DoF()
{
	auto it = s_dofPool[_group].find(_id);
	if (it != s_dofPool[_group].end())
		s_dofPool[_group].erase(it);

	removeAllChildren();
	DoF* parent = _parent;
	if (parent)
		parent->removeChild(this);
}

void DoF::addChild(DoF* child)
{
	if (!child)
		return;

	auto it = std::find(_children.begin(), _children.end(), child);
	if (it == _children.end())
	{
		DoF* parent = child->_parent;
		if (parent)
			parent->removeChild(child);

		child->_parent = this;
		_children.push_back(child);
	}
}

void DoF::removeChild(DoF* child)
{
	if (!child)
		return;

	auto it = std::find(_children.begin(), _children.end(), child);
	if (it != _children.end())
	{
		_children.erase(it);
		child->_parent = nullptr;
	}
}

void DoF::removeAllChildren()
{
	auto it = _children.begin();
	for (; it != _children.end(); ++it)
	{
		(*it)->_parent = nullptr;
	}

	_children.clear();
}

int DoF::getChildrenCount() const
{
	return (int)_children.size();
}

const DoF::Map& DoF::getMap(int group)
{
	if (group < 0 || group >= GroupCount)
		group = 0;
	return s_dofPool[group];
}

DoF* DoF::findDoF(int id, int group)
{
	if (group < 0 || group >= GroupCount)
		return nullptr;
	auto it = s_dofPool[group].find(id);
	if (it == s_dofPool[group].end())
		return nullptr;
	return it->second;
}

}
