#include <SharedObject/DoF.h>

DoF::DoF(int id, int group)
	: SharedObject(id, group)
	, _x(0)
	, _y(0)
	, _z(0)
	, _heading(0)
	, _pitch(0)
	, _roll(0)
	, _sx(1)
	, _sy(1)
	, _sz(1)
	, _parent(nullptr)
{
}

DoF::~DoF()
{
	removeAllChildren();
	DoF* parent = _parent;
	if (parent)
		parent->removeChild(this);
}

int DoF::getChildrenCount() const
{
	return (int)_children.size();
}

bool DoF::addChild(DoF* child)
{
	if (!child)
		return false;

	auto it = std::find(_children.begin(), _children.end(), child);
	if (it == _children.end())
	{
		DoF* parent = child->_parent;
		if (parent)
			parent->removeChild(child);

		child->_parent = this;
		_children.push_back(child);
		return true;
	}

	return false;
}

bool DoF::removeChild(DoF* child)
{
	if (!child)
		return false;

	auto it = std::find(_children.begin(), _children.end(), child);
	if (it != _children.end())
	{
		_children.erase(it);
		child->_parent = nullptr;
		return true;
	}

	return false;
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
