#include <osgCmd/UserData.h>

namespace osgCmd {

static Any s_emptyAny;

static bool putInUserDataType(stringstream& ss, const type_info& typeinfo)
{
	bool isPutin = true;
	if (typeinfo == typeid(bool))
		ss << "bool( ";
	else if (typeinfo == typeid(int))
		ss << "int( ";
	else if (typeinfo == typeid(unsigned int))
		ss << "uint( ";
	else if (typeinfo == typeid(short))
		ss << "short( ";
	else if (typeinfo == typeid(unsigned short))
		ss << "ushort( ";
	else if (typeinfo == typeid(long))
		ss << "long( ";
	else if (typeinfo == typeid(unsigned long))
		ss << "ulong( ";
	else if (typeinfo == typeid(long long))
		ss << "longlong( ";
	else if (typeinfo == typeid(unsigned long long))
		ss << "ulonglong( ";
	else if (typeinfo == typeid(float))
		ss << "float( ";
	else if (typeinfo == typeid(double))
		ss << "double( ";
	else if (typeinfo == typeid(string))
		ss << "string( ";
	else
		isPutin = false;
	return isPutin;
}

static bool putOutUserDataType(stringstream& ss, string& tag, Any& data)
{
	bool isPutout = true;
	string ignore;
	ss >> tag;
	if (tag == "bool(")
	{
		bool var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "int(")
	{
		int var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "uint(")
	{
		unsigned int var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "short(")
	{
		short var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "ushort(")
	{
		unsigned short var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "long(")
	{
		long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "ulong(")
	{
		unsigned long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "longlong(")
	{
		long long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "ulonglong(")
	{
		unsigned long long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "float(")
	{
		float var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "double(")
	{
		double var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == "string(")
	{
		string var;
		ss >> var >> ignore;
		data = var;
	}
	else
		isPutout = false;
	return isPutout;
}

UserData::UserData(const UserData& other)
{
	if (other._attributes)
		_attributes.reset(new Attributes(*other._attributes));
}

stringstream& UserData::operator<<(stringstream& ss) const
{
	if (_attributes)
	{
		ss << "UserData{ ";
		if (putInUserDataType(ss, _attributes->_keylessAny.type()))
			ss << _attributes->_keylessAny << ") ";
		
		if (_attributes->_userObjectsMap)
		{
			ss << "[ " << (int)(_attributes->_userObjectsMap->size()) << " ";
			map<string, Any>::iterator it = _attributes->_userObjectsMap->begin();
			for (; it != _attributes->_userObjectsMap->end(); ++it)
			{
				if (putInUserDataType(ss, it->second.type()))
				{
					ss << it->second << ") ";
					ss << ": " << it->first << " ";
				}
			}
			ss << "] ";
		}
		ss << "}";
	}
	return ss;
}

stringstream& UserData::operator>>(stringstream& ss)
{
	string tag;
	ss >> tag;
	if (tag != "UserData{")
		return ss;

	if (!_attributes)
		_attributes.reset(new UserData::Attributes());

	if (putOutUserDataType(ss, tag, _attributes->_keylessAny))
		ss >> tag;

	if (tag == "}")
		return ss;

	if (tag == "[")
	{
		int size;
		ss >> size;
		if (size > 0)
		{
			if (!_attributes->_userObjectsMap)
				_attributes->_userObjectsMap.reset(new map<string, Any>());

			Any keyValue;
			for (int i = 0; i < size; ++i)
			{
				if (putOutUserDataType(ss, tag, keyValue))
				{
					ss >> tag >> tag;
					(*_attributes->_userObjectsMap)[tag] = keyValue;
				}
			}
		}
		ss >> tag >> tag;
	}

	return ss;
}

void UserData::setData(const Any& anything)
{
	if (!_attributes)
		_attributes.reset(new Attributes());

	_attributes->_keylessAny = anything;
}

void UserData::setData(const string& key, const Any& anything)
{
	if (!_attributes)
		_attributes.reset(new Attributes());

	if (!_attributes->_userObjectsMap)
		_attributes->_userObjectsMap.reset(new map<string, Any>());

	(*_attributes->_userObjectsMap)[key] = anything;
}

const Any& UserData::getData(void) const
{
	if (!_attributes)
		return s_emptyAny;

	return _attributes->_keylessAny;
}

const Any& UserData::getData(const string& key) const
{
	if (!_attributes)
		return s_emptyAny;

	if (!_attributes->_userObjectsMap)
		return s_emptyAny;

	map<string, Any>::iterator it = _attributes->_userObjectsMap->find(key);
	if (it != _attributes->_userObjectsMap->end())
		return it->second;

	return s_emptyAny;
}

void UserData::eraseData(const string& key)
{
	if (_attributes && _attributes->_userObjectsMap)
	{
		map<string, Any>::iterator it = _attributes->_userObjectsMap->find(key);
		if (it != _attributes->_userObjectsMap->end())
			_attributes->_userObjectsMap->erase(it);
	}
}

void UserData::clear()
{
	_attributes.reset();
}

}
