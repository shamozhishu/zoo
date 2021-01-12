#ifndef __ZOO_UTILS_H__
#define __ZOO_UTILS_H__

#include <zoo/Support.h>

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
// 相等性测试
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

/* 多字节字符串分割函数 */
template<typename Container>
inline void stringtok(Container& container, const std::string& in,
	const char* const delimiters = " \t\n", const int deep = 0)
{
	const std::string::size_type len = in.length();
	std::string::size_type i = 0;
	int count = 0;
	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::string::npos)
			return; // nothing left
		// find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);
		count++;
		// push token
		if (j == std::string::npos || (deep > 0 && count > deep))
		{
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));
		// set up for next loop
		i = j + 1;
	}
}

/* 宽字符字符串分割函数 */
template<typename Container>
inline void wstringtok(Container& container, const std::wstring& in,
	const wchar_t* const delimiters = L" \t\n", const int deep = 0)
{
	const std::wstring::size_type len = in.length();
	std::wstring::size_type i = 0;
	int count = 0;
	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::wstring::npos)
			return; // nothing left
		// find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);
		count++;
		// push token
		if (j == std::wstring::npos || (deep > 0 && count > deep))
		{
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));
		// set up for next loop
		i = j + 1;
	}
}

/* 清理vector容器里的重复元素 */
template<typename T>
void clearDupElements(vector<T>& elems)
{
	typename vector<T>::iterator it, dupit;
	for (it = ++elems.begin(); it != elems.end();)
	{
		dupit = find(elems.begin(), it, *it);
		if (dupit != it)
			it = elems.erase(it);
		else
			it++;
	}
}

/* 清理list容器里的重复元素 */
template<typename T>
void clearDupElements(list<T>& elems)
{
	typename list<T>::iterator it, dupit;
	for (it = ++elems.begin(); it != elems.end();)
	{
		dupit = find(elems.begin(), it, *it);
		if (dupit != it)
			it = elems.erase(it);
		else
			it++;
	}
}

/* 修剪掉多字节字符串里前后的空白符 */
static inline string& trim(string& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" \n\r\t"));
		str.erase(str.find_last_not_of(" \n\r\t") + 1);
	}
	return str;
}

/* 修剪掉宽字符字符串里前后的空白符 */
static inline wstring& trim(wstring& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(L" \n\r\t"));
		str.erase(str.find_last_not_of(L" \n\r\t") + 1);
	}
	return str;
}

/* 宽字符与多字节字符串之间的转换 */
_zooExport string  w2a_(const wstring& wideStr);
_zooExport wstring a2w_(const string& ansiStr);
_zooExport string  unicodeToUtf8(const wstring& wstr);
_zooExport wstring utf8ToUnicode(const string& str);
_zooExport string  ansiToUtf8(const string& str);
_zooExport string  utf8ToAnsi(const string& str);

/* 字符串转换为小写 */
_zooExport string  strToLower(string str);
_zooExport wstring strToLower(wstring str);

/* 字符串转换为大写 */
_zooExport string  strToUpper(string str);
_zooExport wstring strToUpper(wstring str);

/* 字符串比较 */
_zooExport bool    compareNoCase(string strA, const string strB);
_zooExport bool    compareNoCase(wstring strA, const wstring strB);

/* 加密解密 */
_zooExport string  encrypt(string str, unsigned short key);
_zooExport string  decrypt(string str, unsigned short key);

/* 工作目录 */
_zooExport string getWorkDir();
_zooExport bool   setWorkDir(const string& path);

/* 得到可执行文件目录 */
_zooExport string getExeDir();

/* 在指定文件夹下寻找所有指定格式的文件路径名 */
_zooExport bool findFileDir(vector<string>& arrFilePath, const string& strCurPath, const string& fileFormat);
_zooExport bool findFileDir(vector<wstring>& arrFilePath, const wstring& strCurPath, const wstring& fileFormat);

/* 字符串替换函数 */
_zooExport string&  strReplaceAll(string& str, const string& old_value, const string& new_value);
_zooExport wstring& wstrReplaceAll(wstring& str, const wstring& old_value, const wstring& new_value);
_zooExport string&  strReplaceAllDistinct(string& str, const string& old_value, const string& new_value);
_zooExport wstring& wstrReplaceAllDistinct(wstring& str, const wstring& old_value, const wstring& new_value);

}

#endif // __ZOO_UTILS_H__
