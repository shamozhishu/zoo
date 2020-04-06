#pragma once

#include <zoo/Support.h>
#include <zoo/Singleton.h>

namespace zoo {

class _zooExport TableCSV
{
	friend class DatabaseCSV;
	TableCSV(DatabaseCSV* csvdb);
public:
	int                getItemCount() const;
	const string&      getTableName() const;
	const set<string>& getMajorKeys() const;

	const char*  item2str(const char* rowIdx, const char* colIdx);
	const char*  item2str(int rowIdx, const char* colIdx);

	int          item2int(const char* rowIdx, const char* colIdx);
	int          item2int(int rowIdx, const char* colIdx);

	unsigned int item2uint(const char* rowIdx, const char* colIdx);
	unsigned int item2uint(int rowIdx, const char* colIdx);

	float        item2float(const char* rowIdx, const char* colIdx);
	float        item2float(int rowIdx, const char* colIdx);

private:
	string                                   _tableName;
	set<string>                              _majorKeys;
	unordered_map<string, int>               _fieldName;
	unordered_map<string, vector<ptrdiff_t>> _tableData;
	DatabaseCSV*                             _associatedDB;
};

class _zooExport DatabaseCSV : public Singleton<DatabaseCSV>
{
	friend class TableCSV;
public:
	DatabaseCSV();
	~DatabaseCSV();
	bool init(string& csvTablePath);
	bool loadTable(const string& csvTablePath, string& tableDataBuff);
	void clear();
	int  getTableCount() const;
	TableCSV* getTable(const string& tableName);

private:
	void  lazyInit();
	void  parseTextLine(TableCSV* pTable, const char* pszLine, int lineNum);
	char* getCurCharBuff(int len);

private:
	char*                            _buffer;
	unordered_map<string, TableCSV*> _tables;
	bool                             _canInit;
	int                              _curCharOffset;
	int                              _maxBufferSize;
	const int                        _startValidLine;
};

}
