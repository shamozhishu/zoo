#ifndef __ZOO_MATH_H__
#define __ZOO_MATH_H__

#include <cmath>
#include <typeinfo>
using namespace std;

#define VAYO_MATH_PI                 (3.14159265358979323846)
#define VAYO_MATH_2PI                (2.0 * VAYO_MATH_PI)
#define VAYO_MATH_RECIPROCAL_PI      (1.0 / VAYO_MATH_PI)
#define VAYO_MATH_PI_DIV_180         (0.017453292519943296)
#define VAYO_MATH_INV_PI_DIV_180     (57.2957795130823229)

#define VayoDegToRad(x)	             ((float)((x)*VAYO_MATH_PI_DIV_180))
#define VayoRadToDeg(x)	             ((float)((x)*VAYO_MATH_INV_PI_DIV_180))

#define ROUNDING_ERROR_SINT          0
#define ROUNDING_ERROR_UINT          0u
#define ROUNDING_ERROR_FLOAT         0.000001f
#define ROUNDING_ERROR_DOUBLE        0.00000001

namespace zoo {

template<typename T>
inline T upperLimit()
{
	const char* typeName = typeid(T).name();
	if (0 == strcmp(typeName, "int"))
		return INT_MAX;
	else if (0 == strcmp(typeName, "float"))
		return FLT_MAX;
	else if (0 == strcmp(typeName, "double"))
		return DBL_MAX;
	else if (0 == strcmp(typeName, "short"))
		return SHRT_MAX;
	else if (0 == strcmp(typeName, "long"))
		return LONG_MAX;
	else if (0 == strcmp(typeName, "unsigned int"))
		return UINT_MAX;
	else if (0 == strcmp(typeName, "unsigned short"))
		return USHRT_MAX;
	else if (0 == strcmp(typeName, "unsigned long"))
		return ULONG_MAX;
	return 0;
}

template<typename T>
inline T lowerLimit()
{
	const char* typeName = typeid(T).name();
	if (0 == strcmp(typeName, "int"))
		return INT_MIN;
	else if (0 == strcmp(typeName, "float"))
		return FLT_MIN;
	else if (0 == strcmp(typeName, "double"))
		return DBL_MIN;
	else if (0 == strcmp(typeName, "short"))
		return SHRT_MIN;
	else if (0 == strcmp(typeName, "long"))
		return LONG_MIN;
	return 0;
}

inline unsigned int ispow2_(unsigned int iValue)
{
	unsigned int nPow2 = 1;

	while (iValue > nPow2)
		nPow2 = (nPow2 << 1);

	return nPow2;
}

template<typename T>
inline T abs_(const T& a)
{
	return a < (T)0 ? -a : a;
}

inline float round_(float x)
{
	return floor(x + 0.5f);
}

template<typename T>
inline const T& min_(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
inline const T& max_(const T& a, const T& b)
{
	return a < b ? b : a;
}

template<typename T>
inline const T& min_(const T& a, const T& b, const T& c)
{
	return a < b ? min_(a, c) : min_(b, c);
}

template<typename T>
inline const T& max_(const T& a, const T& b, const T& c)
{
	return a < b ? max_(b, c) : max_(a, c);
}

template<typename T>
inline T lerp_(const T& a, const T& b, const float t)
{
	return (T)(a*(1.0f - t) + (b*t));
}

template<typename T>
inline const T clamp_(const T& value, const T& low, const T& high)
{
	return min_(max_(value, low), high);
}

//------------------------------------------------------------------------
// ÏàµÈÐÔ²âÊÔ
//------------------------------------------------------------------------
inline bool equals(const int a, const int b, const int tolerance = ROUNDING_ERROR_SINT)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

inline bool equals(const unsigned a, const unsigned b, const unsigned tolerance = ROUNDING_ERROR_UINT)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

inline bool equals(const float a, const float b, const float tolerance = ROUNDING_ERROR_FLOAT)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

inline bool equals(const double a, const double b, const double tolerance = ROUNDING_ERROR_DOUBLE)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

inline bool isZero(const int a, const int tolerance = ROUNDING_ERROR_SINT)
{
	return (a & 0x7ffffff) <= tolerance;
}

inline bool isZero(const unsigned a, const unsigned tolerance = ROUNDING_ERROR_UINT)
{
	return a <= tolerance;
}

inline bool isZero(const float a, const float tolerance = ROUNDING_ERROR_FLOAT)
{
	return fabs(a) <= tolerance;
}

inline bool isZero(const double a, const double tolerance = ROUNDING_ERROR_DOUBLE)
{
	return fabs(a) <= tolerance;
}

}

#endif // __ZOO_MATH_H__
