#ifndef __ZOO_ANY_H__
#define __ZOO_ANY_H__

#include <zoo/Support.h>

namespace zoo {

class Any
{
public:
	Any() : _content(NULL) {}

	Any(const Any& other)
		: _content(other._content ? other._content->clone() : NULL) {}

	template<typename ValueType>
	Any(const ValueType& value)
		: _content(new holder<ValueType>(value)) {}

	virtual ~Any() { reset(); }

	Any& swap(Any& rhs)
	{
		std::swap(_content, rhs._content);
		return *this;
	}

	Any& operator=(const Any& rhs)
	{
		Any(rhs).swap(*this);
		return *this;
	}

	template<typename ValueType>
	Any& operator=(const ValueType& rhs)
	{
		Any(rhs).swap(*this);
		return *this;
	}

	bool has_value() const { return _content != NULL; }

	const std::type_info& type() const { return _content ? _content->getType() : typeid(void); }

	inline friend stringstream& operator<<(stringstream& ss, const Any& in)
	{
		if (in._content)
			in._content->writeToStream(ss);
		return ss;
	}

	inline friend stringstream& operator>>(stringstream& ss, Any& out)
	{
		if (out._content)
			out._content->readFromStream(ss);
		return ss;
	}

	void reset()
	{
		delete _content;
		_content = NULL;
	}

	template<typename ValueType>
	ValueType to() const
	{
		const ValueType* result = (type() == typeid(ValueType)) ? &static_cast<holder<ValueType>*>(_content)->held : NULL;
		if (!result)
		{
			ZOO_ASSERT(false && "Bad cast for Any!");
		}
		return *result;
	}

protected:
	class placeholder
	{
	public:
		virtual ~placeholder() {}
		virtual const std::type_info& getType() const = 0;
		virtual placeholder* clone() const = 0;
		virtual void writeToStream(stringstream& ss) = 0;
		virtual void readFromStream(stringstream& ss) = 0;
	};

	template<typename ValueType>
	class holder : public placeholder
	{
	public:
		holder(const ValueType& value) : held(value) {}

		virtual const std::type_info& getType() const
		{
			return typeid(ValueType);
		}

		virtual placeholder* clone() const
		{
			return new holder(held);
		}

		virtual void writeToStream(stringstream& ss)
		{
			ss << held << " ";
		}

		virtual void readFromStream(stringstream& ss)
		{
			ss >> held;
		}

		ValueType held;
	};

	placeholder* _content;
	template<typename ValueType> friend ValueType* any_cast(Any* operand);
};

class AnyNumeric : public Any
{
public:
	AnyNumeric() : Any() {}

	AnyNumeric(const AnyNumeric& other) : Any()
	{
		_content = other._content ? other._content->clone() : NULL;
	}

	template<typename ValueType>
	AnyNumeric(const ValueType& value)
	{
		_content = new numholder<ValueType>(value);
	}

protected:
	class numplaceholder : public Any::placeholder
	{
	public:
		~numplaceholder() {}
		virtual placeholder* add(placeholder* rhs) = 0;
		virtual placeholder* subtract(placeholder* rhs) = 0;
		virtual placeholder* multiply(placeholder* rhs) = 0;
		virtual placeholder* multiply(float factor) = 0;
		virtual placeholder* divide(placeholder* rhs) = 0;
	};

	template<typename ValueType>
	class numholder : public numplaceholder
	{
	public:
		numholder(const ValueType& value) : held(value) {}
		virtual const std::type_info& getType() const
		{
			return typeid(ValueType);
		}
		virtual placeholder* clone() const
		{
			return new numholder(held);
		}
		virtual placeholder* add(placeholder* rhs)
		{
			return new numholder(held + static_cast<numholder*>(rhs)->held);
		}
		virtual placeholder* subtract(placeholder* rhs)
		{
			return new numholder(held - static_cast<numholder*>(rhs)->held);
		}
		virtual placeholder* multiply(placeholder* rhs)
		{
			return new numholder(held * static_cast<numholder*>(rhs)->held);
		}
		virtual placeholder* multiply(float factor)
		{
			return new numholder(held * factor);
		}
		virtual placeholder* divide(placeholder* rhs)
		{
			return new numholder(held / static_cast<numholder*>(rhs)->held);
		}
		virtual void writeToStream(stringstream& ss)
		{
			ss << held << " ";
		}
		virtual void readFromStream(stringstream& ss)
		{
			ss >> held;
		}
	public:
		ValueType held;
	};

	AnyNumeric(placeholder* pholder)
	{
		_content = pholder;
	}

public:
	AnyNumeric& operator=(const AnyNumeric& rhs)
	{
		AnyNumeric(rhs).swap(*this);
		return *this;
	}
	AnyNumeric operator+(const AnyNumeric& rhs) const
	{
		return AnyNumeric(static_cast<numplaceholder*>(_content)->add(rhs._content));
	}
	AnyNumeric operator-(const AnyNumeric& rhs) const
	{
		return AnyNumeric(static_cast<numplaceholder*>(_content)->subtract(rhs._content));
	}
	AnyNumeric operator*(const AnyNumeric& rhs) const
	{
		return AnyNumeric(static_cast<numplaceholder*>(_content)->multiply(rhs._content));
	}
	AnyNumeric operator*(float factor) const
	{
		return AnyNumeric(static_cast<numplaceholder*>(_content)->multiply(factor));
	}
	AnyNumeric operator/(const AnyNumeric& rhs) const
	{
		return AnyNumeric(static_cast<numplaceholder*>(_content)->divide(rhs._content));
	}
	AnyNumeric& operator+=(const AnyNumeric& rhs)
	{
		*this = AnyNumeric(static_cast<numplaceholder*>(_content)->add(rhs._content));
		return *this;
	}
	AnyNumeric& operator-=(const AnyNumeric& rhs)
	{
		*this = AnyNumeric(static_cast<numplaceholder*>(_content)->subtract(rhs._content));
		return *this;
	}
	AnyNumeric& operator*=(const AnyNumeric& rhs)
	{
		*this = AnyNumeric(static_cast<numplaceholder*>(_content)->multiply(rhs._content));
		return *this;
	}
	AnyNumeric& operator/=(const AnyNumeric& rhs)
	{
		*this = AnyNumeric(static_cast<numplaceholder*>(_content)->divide(rhs._content));
		return *this;
	}
};

}

#endif // __ZOO_ANY_H__
