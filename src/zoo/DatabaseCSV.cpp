#include <zoo/DatabaseCSV.h>
#include <zoo/Utils.h>
#include <zoo/Log.h>

#define BUFFER_SIZE        (64)
#define BUFFER_MAX_SIZE    (1024)

namespace zoo {

TableCSV::TableCSV(DatabaseCSV* csvdb)
	: _associatedDB(csvdb)
{
}

int TableCSV::getItemCount() const
{
	return (int)_tableData.size();
}

const string& TableCSV::getTableName() const
{
	return _tableName;
}

const std::set<std::string>& TableCSV::getMajorKeys() const
{
	return _majorKeys;
}

const char* TableCSV::item2str(const char* rowIdx, const char* colIdx)
{
	int indexField = 0;
	unordered_map<string, int>::iterator findVal = _fieldName.find(colIdx);
	if (findVal == _fieldName.end())
		return "";
	indexField = findVal->second;
	unordered_map<string, vector<ptrdiff_t>>::iterator findData = _tableData.find(rowIdx);
	if (findData != _tableData.end())
	{
		const vector<ptrdiff_t>& arrRowText = findData->second;
		if (indexField >= 0 && indexField < (int)arrRowText.size() && arrRowText[indexField] != -1)
			return _associatedDB->_buffer + arrRowText[indexField];
	}
	return "";
}

const char* TableCSV::item2str(int rowIdx, const char* colIdx)
{
	char szRowIdx[BUFFER_SIZE] = {};
	snprintf(szRowIdx, BUFFER_SIZE, "%d", rowIdx);
	return item2str(szRowIdx, colIdx);
}

int TableCSV::item2int(const char* rowIdx, const char* colIdx)
{
	const char* val = item2str(rowIdx, colIdx);
	if (NULL == val)
		return 0;
	return atoi(val);
}

int TableCSV::item2int(int rowIdx, const char* colIdx)
{
	char szRowIdx[BUFFER_SIZE] = {};
	snprintf(szRowIdx, BUFFER_SIZE, "%d", rowIdx);
	return item2int(szRowIdx, colIdx);
}

unsigned int TableCSV::item2uint(const char* rowIdx, const char* colIdx)
{
	const char* val = item2str(rowIdx, colIdx);
	if (NULL == val)
		return 0u;
	return (unsigned int)atoi(val);
}

unsigned int TableCSV::item2uint(int rowIdx, const char* colIdx)
{
	char szRowIdx[BUFFER_SIZE] = {};
	snprintf(szRowIdx, BUFFER_SIZE, "%d", rowIdx);
	return item2uint(szRowIdx, colIdx);
}

float TableCSV::item2float(const char* rowIdx, const char* colIdx)
{
	const char* val = item2str(rowIdx, colIdx);
	if (NULL == val || 0 == strcmp(val, ""))
		return 0.0f;
	return std::stof(val);
}

float TableCSV::item2float(int rowIdx, const char* colIdx)
{
	char szRowIdx[BUFFER_SIZE] = {};
	snprintf(szRowIdx, BUFFER_SIZE, "%d", rowIdx);
	return item2float(szRowIdx, colIdx);
}

DatabaseCSV::DatabaseCSV()
	: _buffer(NULL)
	, _canInit(true)
	, _mainTable(nullptr)
	, _curCharOffset(0)
	, _maxBufferSize(BUFFER_MAX_SIZE * BUFFER_MAX_SIZE * 3)
	, _startValidLine(2)
{
}

DatabaseCSV::~DatabaseCSV()
{
	clear();
}

bool DatabaseCSV::init(string& csvTablePath, const string& mainTable /*= ""*/)
{
	_mainTable = nullptr;
	if (_canInit)
	{
		clear();
		size_t len = csvTablePath.size();
		if (len > 0 && csvTablePath[len - 1] != '/' && csvTablePath[len - 1] != '\\')
			csvTablePath += "\\";

		vector<string> allFilePath;
		csvTablePath = strReplaceAll(csvTablePath, "/", "\\");
		findFileDir(allFilePath, csvTablePath, "csv");

		string tableDataBuff;
		len = allFilePath.size();
		if (len > 0)
			tableDataBuff.resize(_maxBufferSize);

		for (unsigned i = 0; i < len; i++)
			loadTable(allFilePath[i], tableDataBuff);

		_canInit = false;
	}

	string tablename;
	auto it = _tables.begin();
	for (; it != _tables.end(); ++it)
	{
		tablename = it->first;
		tablename = tablename.substr(tablename.find_last_of('\\') + 1);
		if (zoo::compareNoCase(mainTable, tablename))
		{
			_mainTable = it->second;
			break;
		}
	}

	return true;
}

bool DatabaseCSV::loadTable(const string& csvTablePath, string& tableDataBuff)
{
	string fileName = csvTablePath;
	fileName = strReplaceAll(fileName, "/", "\\");
	if (csvTablePath.rfind(".csv") == string::npos)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]文件格式错误", a2w_(fileName).c_str());
		return false;
	}

	lazyInit();

	ifstream fin(fileName);
	if (!fin)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]打开失败", a2w_(fileName).c_str());
		return false;
	}

	fin.imbue(locale("chs"));
	fin.seekg(0, std::ios_base::end);
	int sizeToRead = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	if (sizeToRead == 0)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]为空", a2w_(fileName).c_str());
		return false;
	}

	if (tableDataBuff.size() < sizeToRead)
	{
		tableDataBuff.clear();
		tableDataBuff.resize(sizeToRead);
	}

	fin.read(&tableDataBuff[0], sizeToRead);
	sizeToRead = (int)fin.gcount();
	if (sizeToRead == 0)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]读取失败", a2w_(fileName).c_str());
		return false;
	}

	char buff[BUFFER_MAX_SIZE * 4];
	int lineNum = 0;

	char* p = &tableDataBuff[0];
	int readCharOffset = 0;
	TableCSV* pTable = NULL;

	unordered_map<string, TableCSV*>::iterator it = _tables.find(fileName);
	if (it != _tables.end())
	{
		pTable = it->second;
		pTable->_tableName.clear();
		pTable->_majorKeys.clear();
		pTable->_fieldName.clear();
		pTable->_tableData.clear();
	}
	else
	{
		pTable = new TableCSV(this);
		pTable->_tableName = fileName;
		_tables[pTable->_tableName] = pTable;
	}

	for (; sizeToRead > 0; p++, sizeToRead--)
	{
		char curCharacter = *p;

		if ('\n' == curCharacter || '\r' == curCharacter)
		{
			if (readCharOffset > 0)
			{
				if (readCharOffset < BUFFER_MAX_SIZE * 4)
					buff[readCharOffset] = '\0';
				else
					buff[BUFFER_MAX_SIZE * 4 - 1] = '\0';
				lineNum++;
				parseTextLine(pTable, buff, lineNum);
				readCharOffset = 0;
			}

			continue;
		}

		if (readCharOffset >= 0 && readCharOffset < BUFFER_MAX_SIZE * 4)
		{
			buff[readCharOffset++] = curCharacter;
		}
	}

	if (readCharOffset > 0)
	{
		lineNum++;
		buff[readCharOffset] = '\0';
		parseTextLine(pTable, buff, lineNum);
	}

	return true;
}

void DatabaseCSV::clear()
{
	unordered_map<string, TableCSV*>::iterator itor = _tables.begin();
	for (; itor != _tables.end(); ++itor)
	{
		delete itor->second;
		itor->second = nullptr;
	}

	_tables.clear();
	_curCharOffset = 0;
	_canInit = true;
	if (_buffer)
	{
		free(_buffer);
		_buffer = NULL;
	}
}

int DatabaseCSV::getTableCount() const
{
	return (int)_tables.size();
}

TableCSV* DatabaseCSV::getTable(const string& tableName)
{
	string strTableName = tableName;
	strTableName = strReplaceAll(strTableName, "/", "\\");
	unordered_map<string, TableCSV*>::iterator findVal = _tables.find(strTableName);
	if (findVal != _tables.end())
		return findVal->second;
	return NULL;
}

TableCSV* DatabaseCSV::getMainTable() const
{
	return _mainTable;
}

void DatabaseCSV::lazyInit()
{
	if (NULL != _buffer)
		return;

	_buffer = (char*)malloc(_maxBufferSize * sizeof(char));
	_curCharOffset = 0;
}

void DatabaseCSV::parseTextLine(TableCSV* pTable, const char* pszLine, int lineNum)
{
	static char buff[BUFFER_MAX_SIZE * 4];
	if (lineNum < _startValidLine)
		return;

	int len = (int)strlen(pszLine);
	int curByteOffset = 0;
	buff[curByteOffset] = 0;
	char firstChar = 0;
	int semicolonNum = 0;

	// 解析字段名文本行
	if (_startValidLine == lineNum)
	{
		int fieldIndex = 0;

		for (int i = 0; i < len; i++)
		{
			if (0 == curByteOffset)
				firstChar = pszLine[i];

			if ('"' != firstChar)
			{
				if (',' == pszLine[i])
				{
					buff[curByteOffset] = 0;
					pTable->_fieldName[buff] = fieldIndex++;
					curByteOffset = 0;
					continue;
				}

				buff[curByteOffset++] = pszLine[i];
			}
			else
			{
				if ('"' == pszLine[i])
				{
					++semicolonNum;
				}

				if (0 == semicolonNum % 2 && '"' == pszLine[i - 1] && ',' == pszLine[i])
				{
					buff[curByteOffset] = 0;
					pTable->_fieldName[buff] = fieldIndex++;
					curByteOffset = 0;
					semicolonNum = 0;
					continue;
				}

				if ('"' == pszLine[i] && semicolonNum > 1 && 0 == semicolonNum % 2)
				{
					continue;
				}

				buff[curByteOffset++] = pszLine[i];
			}
		}

		if (curByteOffset > 0)
		{
			buff[curByteOffset] = 0;
			pTable->_fieldName[buff] = fieldIndex;
		}

		return; // 返回
	}

	vector<ptrdiff_t> tableDataOffset;

	for (int i = 0; i < len; i++)
	{
		if (0 == curByteOffset)
			firstChar = pszLine[i];

		if ('"' != firstChar)
		{
			if (',' == pszLine[i])
			{
				buff[curByteOffset] = 0;
				char* pDst = getCurCharBuff(curByteOffset + 1);

				if (NULL != pDst)
				{
					if (curByteOffset > 0)
						memmove(pDst, buff, sizeof(char)*curByteOffset);

					pDst[curByteOffset] = 0;
					tableDataOffset.push_back(pDst - _buffer);
				}
				else
				{
					tableDataOffset.push_back(-1);
				}

				curByteOffset = 0;
				continue;
			}

			buff[curByteOffset++] = pszLine[i];
		}
		else
		{
			if ('"' == pszLine[i])
			{
				++semicolonNum;
			}

			if (0 == semicolonNum % 2 && '"' == pszLine[i - 1] && ',' == pszLine[i])
			{
				buff[curByteOffset] = 0;
				char* pDst = getCurCharBuff(curByteOffset + 1);

				if (NULL != pDst)
				{
					if (curByteOffset > 0)
						memmove(pDst, buff, sizeof(char)*curByteOffset);

					pDst[curByteOffset] = 0;
					tableDataOffset.push_back(pDst - _buffer);
				}
				else
				{
					tableDataOffset.push_back(-1);
				}

				curByteOffset = 0;
				semicolonNum = 0;
				continue;
			}

			if ('"' == pszLine[i] && semicolonNum > 1 && 0 == semicolonNum % 2)
			{
				continue;
			}

			buff[curByteOffset++] = pszLine[i];
		}
	}

	if (curByteOffset > 0)
	{
		buff[curByteOffset] = 0;
		char* pDst = getCurCharBuff(curByteOffset + 1);

		if (NULL != pDst)
		{
			memmove(pDst, buff, sizeof(char)*curByteOffset);
			pDst[curByteOffset] = 0;
			tableDataOffset.push_back(pDst - _buffer);
		}
		else
		{
			tableDataOffset.push_back(-1);
		}
	}

	if (tableDataOffset.size() <= 0)
	{
		return;
	}

	string idx = _buffer + tableDataOffset[0];
	pTable->_majorKeys.insert(idx);
	pTable->_tableData[idx] = tableDataOffset;

	int restNum = (int)(pTable->_fieldName.size()) - (int)(tableDataOffset.size());

	for (int i = 0; i < restNum + 1; i++)
	{
		pTable->_tableData[idx].push_back(-1);
	}
}

char* DatabaseCSV::getCurCharBuff(int len)
{
	if (NULL == _buffer)
		return NULL;

	if ((_curCharOffset + len) >= _maxBufferSize - BUFFER_MAX_SIZE * BUFFER_MAX_SIZE)
		_canInit = true;

	if ((_curCharOffset + len) >= _maxBufferSize)
	{
		int bufferSize = (BUFFER_MAX_SIZE * BUFFER_MAX_SIZE + _maxBufferSize) * sizeof(char);
		void* pNewBuffer = realloc(_buffer, bufferSize);
		if (pNewBuffer)
		{
			_buffer = (char*)pNewBuffer;
			_maxBufferSize = bufferSize;
		}
		else
			return NULL;
	}

	char* pRet = _buffer + _curCharOffset;
	_curCharOffset += len;
	return pRet;
}

}
