#include <SharedObject/SharedObject.h>

std::vector<SharedObject*> SharedObjectPool::_objectList;
std::map<int, SharedObject*> SharedObjectPool::_objectMap[SharedObjectPool::GroupCount];

SharedObject::SharedObject(int id, int group)
	: _id(id)
{
	if (group < 0 || group >= SharedObjectPool::GroupCount)
		group = 0;
	_group = group;
	SharedObjectPool::_objectMap[group][id] = this;
	SharedObjectPool::_objectList.push_back(this);
}

SharedObject::~SharedObject()
{
	auto it = std::find(SharedObjectPool::_objectList.begin(), SharedObjectPool::_objectList.end(), this);
	if (it != SharedObjectPool::_objectList.end())
		SharedObjectPool::_objectList.erase(it);

	auto itMap = SharedObjectPool::_objectMap[_group].find(_id);
	if (itMap != SharedObjectPool::_objectMap[_group].end())
		SharedObjectPool::_objectMap[_group].erase(itMap);
}

std::vector<SharedObject*> SharedObjectPool::getObjectList()
{
	return _objectList;
}

std::map<int, SharedObject*> SharedObjectPool::getObjectMap(int group)
{
	if (group < 0 || group >= GroupCount)
		group = 0;
	return _objectMap[group];
}

SharedObject* SharedObjectPool::findObject(int id, int group)
{
	if (group < 0 || group >= GroupCount)
		return nullptr;
	auto it = _objectMap[group].find(id);
	if (it == _objectMap[group].end())
		return nullptr;
	return it->second;
}
