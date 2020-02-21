#include <osgCmd/Utils.h>
#include <Windows.h>
#include <direct.h>
#include <io.h>

namespace osgCmd {

string w2a_(const wstring& wideStr)
{
	if (wideStr == L"")
		return "";

	string ansiStr;
	int needBytes = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideStr.size(), NULL, 0, NULL, NULL);
	if (needBytes > 0)
	{
		ansiStr.resize(needBytes);
		WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideStr.size(), &ansiStr[0], needBytes, NULL, NULL);
	}

	return ansiStr;
}

wstring a2w_(const string& ansiStr)
{
	if (ansiStr == "")
		return L"";

	wstring wideStr;
	int needWChar = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), ansiStr.size(), NULL, 0);
	if (needWChar > 0)
	{
		wideStr.resize(needWChar);
		MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), ansiStr.size(), &wideStr[0], needWChar);
	}

	return wideStr;
}

string unicodeToUtf8(const wstring& wstr)
{
	int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *pAssii = (char*)malloc(sizeof(char)*ansiiLen);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, NULL, NULL);
	string ret_str = pAssii;
	free(pAssii);
	return ret_str;
}

wstring utf8ToUnicode(const string& str)
{
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pUnicode, unicodeLen);
	wstring ret_str = pUnicode;
	free(pUnicode);
	return ret_str;
}

string ansiToUtf8(const string& str)
{
	return unicodeToUtf8(a2w_(str));
}

string utf8ToAnsi(const string& str)
{
	return w2a_(utf8ToUnicode(str));
}

string strToUpper(string str)
{
	std::string strTmp = str;
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), towupper);
	return strTmp;
}

wstring strToUpper(wstring str)
{
	std::wstring strTmp = str;
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), towupper);
	return strTmp;
}

bool compareNoCase(string strA, const string strB)
{
	string str1 = strToUpper(strA);
	string str2 = strToUpper(strB);
	return (str1 == str2);
}

bool compareNoCase(wstring strA, const wstring strB)
{
	wstring str1 = strToUpper(strA);
	wstring str2 = strToUpper(strB);
	return (str1 == str2);
}

string encrypt(string str, unsigned short key)
{
	const int C1 = 52845;
	const int C2 = 22719;
	string strRet, strTmp;
	unsigned int i, j;
	strRet = str; // 初始化结果字符串

	for (i = 0; i < str.size(); ++i) // 依次对字符串中各字符进行操作
	{
		strRet[i] = str[i] ^ (key >> 8); // 将密钥移位后与字符异或
		key = ((BYTE)strRet[i] + key) * C1 + C2; // 产生下一个密钥
	}

	str = strRet; // 保存结果
	strRet.clear(); // 清除结果

	for (i = 0; i < str.size(); ++i) // 对加密结果进行转换
	{
		j = (BYTE)str[i]; // 提取字符
		// 将字符转换为两个字母保存
		strTmp = "12"; // 设置strTmp长度为2
		strTmp[0] = 65 + j / 26;
		strTmp[1] = 65 + j % 26;
		strRet += strTmp;
	}

	return strRet;
}

string decrypt(string str, unsigned short key)
{
	const int C1 = 52845;
	const int C2 = 22719;
	string strRet, strTmp;
	unsigned int i, j;

	for (i = 0; i < str.size() / 2; ++i) // 将字符串两个字母一组进行处理
	{
		j = ((BYTE)str[2 * i] - 65) * 26;
		j += (BYTE)str[2 * i + 1] - 65;
		strTmp = "1"; // 设置strTmp长度为1
		strTmp[0] = j;
		strRet += strTmp; // 追加字符，还原字符串
	}

	str = strRet; // 保存中间结果

	for (i = 0; i < str.size(); ++i) // 依次对字符串中各字符进行操作
	{
		strRet[i] = (BYTE)str[i] ^ (key >> 8); // 将密钥移位后与字符异或
		key = ((BYTE)str[i] + key) * C1 + C2; // 产生下一个密钥
	}

	return strRet;
}

string getWorkDir()
{
	string workDir = _getcwd(nullptr, 0);
	workDir += "\\";
	return workDir;
}

bool setWorkDir(const string& path)
{
	return chdir(path.c_str()) == 0;
}

string getExeDir()
{
	char exeFullPath[MAX_PATH];
	string strPath;
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (string)exeFullPath;
	size_t pos = strPath.find_last_of(L'\\', strPath.length());
	return strPath.substr(0, pos + 1);
}

bool findFileDir(vector<wstring>& arrFilePath, const wstring& strCurPath, const wstring& fileFormat)
{
	wstring searchPath = strCurPath + L"*";
	_wfinddata_t sFileInfo;
	intptr_t handle = _wfindfirst(searchPath.c_str(), &sFileInfo);
	if (-1l == handle)
		return false;
	do
	{
		// 判断是否包含子目录
		if (sFileInfo.attrib & _A_SUBDIR)
		{
			if (0 != wcscmp(sFileInfo.name, L".") && 0 != wcscmp(sFileInfo.name, L".."))
			{
				wstring newPath = strCurPath + sFileInfo.name + L"\\";
				findFileDir(arrFilePath, newPath, fileFormat);
			}
			continue;
		}

		wstring resultPath = strCurPath + sFileInfo.name;
		wstring strExName = resultPath;
		size_t idx = strExName.rfind('.');
		strExName = strExName.substr(++idx);
		if (0 == strExName.compare(fileFormat))
		{
			arrFilePath.push_back(resultPath);
		}

	} while (0 == _wfindnext(handle, &sFileInfo));

	_findclose(handle);
	return true;
}

string& strReplaceAll(string& str, const string& old_value, const string& new_value)
{
	while (true)
	{
		string::size_type pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

wstring& wstrReplaceAll(wstring& str, const wstring& old_value, const wstring& new_value)
{
	while (true)
	{
		wstring::size_type pos(0);
		if ((pos = str.find(old_value)) != wstring::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

string& strReplaceAllDistinct(string& str, const string& old_value, const string& new_value)
{
	for (string::size_type pos(0); pos != string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) == string::npos)
			break;
		str.replace(pos, old_value.length(), new_value);
	}
	return str;
}

wstring& wstrReplaceAllDistinct(wstring& str, const wstring& old_value, const wstring& new_value)
{
	for (wstring::size_type pos(0); pos != wstring::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != wstring::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

}
