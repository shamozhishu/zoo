#pragma once

#include <zooCmd/Support.h>
#include <zooCmd/CmdUsage.h>

namespace zooCmd {

class _zooCmdExport CmdParser
{
	class Parameter
	{
	public:
		enum ParameterType
		{
			BOOL_PARAMETER,
			FLOAT_PARAMETER,
			DOUBLE_PARAMETER,
			INT_PARAMETER,
			UNSIGNED_INT_PARAMETER,
			STRING_PARAMETER
		};

		union ValueUnion
		{
			bool*           _bool;
			float*          _float;
			double*         _double;
			int*            _int;
			unsigned int*   _uint;
			std::string*    _string;
		};

		Parameter(bool& value) { _type = BOOL_PARAMETER; _value._bool = &value; }
		Parameter(float& value) { _type = FLOAT_PARAMETER; _value._float = &value; }
		Parameter(double& value) { _type = DOUBLE_PARAMETER; _value._double = &value; }
		Parameter(int& value) { _type = INT_PARAMETER; _value._int = &value; }
		Parameter(unsigned int& value) { _type = UNSIGNED_INT_PARAMETER; _value._uint = &value; }
		Parameter(std::string& value) { _type = STRING_PARAMETER; _value._string = &value; }
		Parameter(const Parameter& param) { _type = param._type; _value = param._value; }
		Parameter& operator = (const Parameter& param) { _type = param._type; _value = param._value; return *this; }

		bool valid(const char* str) const;
		bool assign(const char* str);

	protected:
		ParameterType   _type;
		ValueUnion      _value;
	};

	static bool isOption(const char* str);
	static bool isString(const char* str);
	static bool isNumber(const char* str);
	static bool isBool(const char* str);

public:
	CmdParser(int* argc, char **argv);
	void setCmdUsage(CmdUsage* usage) { _usage = usage; }
	CmdUsage* getCmdUsage() { return _usage; }
	const CmdUsage* getCmdUsage() const { return _usage; }
	int& argc() { return *_argc; }

	char** argv() { return _argv; }
	char* operator[](int pos) { return _argv[pos]; }
	const char* operator[](int pos) const { return _argv[pos]; }

	int  find(const std::string& str) const;
	std::string getCmdName() const;
	bool isOption(int pos) const;
	bool isString(int pos) const;
	bool isNumber(int pos) const;
	bool containsOptions() const;
	void remove(int pos, int num = 1);
	bool match(int pos, const std::string& str) const;

	bool read(const std::string& str);
	bool read(const std::string& str, Parameter value1);
	bool read(const std::string& str, Parameter value1, Parameter value2);
	bool read(const std::string& str, Parameter value1, Parameter value2, Parameter value3);
	bool read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4);
	bool read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5);
	bool read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6);
	bool read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7);
	bool read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7, Parameter value8);

	bool read(int pos, const std::string& str);
	bool read(int pos, const std::string& str, Parameter value1);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7);
	bool read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7, Parameter value8);

	enum ErrorSeverity
	{
		BENIGN = 0,
		CRITICAL = 1
	};

	typedef std::map<std::string, ErrorSeverity> ErrorMessageMap;
	bool errors(ErrorSeverity severity = BENIGN) const;
	void reportError(const std::string& message, ErrorSeverity severity = CRITICAL);
	void reportRemainingOptionsAsUnrecognized(ErrorSeverity severity = BENIGN);
	ErrorMessageMap& getErrorMessageMap() { return _errorMessageMap; }
	const ErrorMessageMap& getErrorMessageMap() const { return _errorMessageMap; }
	void writeErrorMessages(std::ostream& output, ErrorSeverity sevrity = BENIGN);
	CmdUsage::Type readHelpType();

protected:
	int*            _argc;
	char**          _argv;
	CmdUsage*       _usage;
	ErrorMessageMap _errorMessageMap;
};

}
