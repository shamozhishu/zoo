#pragma once

#include <zoo/Any.h>

namespace zoo {

class _zooExport UserData
{
public:
	UserData(const UserData* pdata = nullptr);
	UserData(const UserData& other);
	UserData& swap(UserData& rhs)
	{
		std::swap(_attributes, rhs._attributes);
		return *this;
	}
	UserData& operator=(const UserData& rhs)
	{
		UserData(rhs).swap(*this);
		return *this;
	}
	inline friend stringstream& operator<<(stringstream& ss, const UserData& in)
	{
		return in.operator<<(ss);
	}
	inline friend stringstream& operator>>(stringstream& ss, UserData& out)
	{
		return out.operator>>(ss);
	}
	stringstream& operator<<(stringstream& ss) const;
	stringstream& operator>>(stringstream& ss);

public:
	void setData(const Any& anything);
	void setData(const string& key, const Any& anything);
	const Any& getData(void) const;
	const Any& getData(const string& key) const;
	void eraseData(const string& key);
	void clear();

protected:
	struct Attributes
	{
		Attributes() {}
		Attributes(const Attributes& other) : _keylessAny(other._keylessAny)
		{
			if (other._userObjectsMap)
				_userObjectsMap.reset(new map<string, Any>(*other._userObjectsMap));
		}

		Any                               _keylessAny;
		std::unique_ptr<map<string, Any>> _userObjectsMap;
	};

	const Attributes* getAttributes() const { return _attributes.get(); }
	Attributes* getAttributes() { return _attributes.get(); }

private:
	std::unique_ptr<Attributes> _attributes;
};

}
