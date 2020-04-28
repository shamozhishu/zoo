#include <zooCmd/CmdParser.h>
#include <zoo/Utils.h>

namespace zooCmd {

bool CmdParser::isBool(const char* str)
{
	if (!str)
		return false;

	str = strtrim(str);
	return (_stricmp(str, "true") == 0 || _stricmp(str, "false") == 0 || strcmp(str, "0") == 0 || strcmp(str, "1") == 0);
}

bool CmdParser::isString(const char* str)
{
	if (!str)
		return false;

	str = strtrim(str);
	return true;
}

bool CmdParser::isNumber(const char* str)
{
	if (!str)
		return false;

	str = strtrim(str);
	bool hadPlusMinus = false;
	bool hadDecimalPlace = false;
	bool hadExponent = false;
	bool couldBeInt = true;
	bool couldBeFloat = true;
	int noZeroToNine = 0;

	const char* ptr = str;

	// check if could be a hex number.
	if (_strnicmp(ptr, "0x", 2) == 0)
	{
		// skip over leading 0x, and then go through rest of string
		// checking to make sure all values are 0...9 or a..f.
		ptr += 2;
		while (*ptr != 0 && ((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'a' && *ptr <= 'f') || (*ptr >= 'A' && *ptr <= 'F')))
			++ptr;

		// got to end of string without failure, therefore must be a hex integer.
		if (*ptr == 0)
			return true;
	}

	ptr = str;

	// check if a float or an int.
	while (*ptr != 0 && couldBeFloat)
	{
		if (*ptr == '+' || *ptr == '-')
		{
			if (hadPlusMinus)
			{
				couldBeInt = false;
				couldBeFloat = false;
			}
			else
				hadPlusMinus = true;
		}
		else if (*ptr >= '0' && *ptr <= '9')
		{
			noZeroToNine++;
		}
		else if (*ptr == '.')
		{
			if (hadDecimalPlace)
			{
				couldBeInt = false;
				couldBeFloat = false;
			}
			else
			{
				hadDecimalPlace = true;
				couldBeInt = false;
			}
		}
		else if (*ptr == 'e' || *ptr == 'E')
		{
			if (hadExponent || noZeroToNine == 0)
			{
				couldBeInt = false;
				couldBeFloat = false;
			}
			else
			{
				hadExponent = true;
				couldBeInt = false;
				hadDecimalPlace = false;
				hadPlusMinus = false;
				noZeroToNine = 0;
			}
		}
		else
		{
			couldBeInt = false;
			couldBeFloat = false;
		}
		++ptr;
	}

	if (couldBeInt && noZeroToNine > 0) return true;
	if (couldBeFloat && noZeroToNine > 0) return true;

	return false;
}

const char* CmdParser::strtrim(const char* str)
{
	char* p = const_cast<char*>(str);
	char* q = const_cast<char*>(str);
	while (*p == ' ' || *p == '\t' || *p == '\n') ++p;
	while (*q++ = *p++);
	q -= 2;
	while (*q == ' ' || *q == '\t' || *q == '\n') --q;
	*(q + 1) = '\0';
	return str;
}

bool CmdParser::Parameter::valid(const char* str) const
{
	switch (_type)
	{
	case Parameter::BOOL_PARAMETER:         return isBool(str);
	case Parameter::FLOAT_PARAMETER:        return isNumber(str);
	case Parameter::DOUBLE_PARAMETER:       return isNumber(str);
	case Parameter::INT_PARAMETER:          return isNumber(str);
	case Parameter::UNSIGNED_INT_PARAMETER: return isNumber(str);
	case Parameter::STRING_PARAMETER:       return isString(str);
	}
	return false;
}

bool CmdParser::Parameter::assign(const char* str)
{
	if (valid(str))
	{
		switch (_type)
		{
		case Parameter::BOOL_PARAMETER:
		{
			*_value._bool = (_stricmp(str, "true") == 0 || strcmp(str, "1") == 0);
			break;
		}
		case Parameter::FLOAT_PARAMETER:        *_value._float = atof(str); break;
		case Parameter::DOUBLE_PARAMETER:       *_value._double = atof(str); break;
		case Parameter::INT_PARAMETER:          *_value._int = atoi(str); break;
		case Parameter::UNSIGNED_INT_PARAMETER: *_value._uint = atoi(str); break;
		case Parameter::STRING_PARAMETER:       *_value._string = str; break;
		}
		return true;
	}

	return false;
}

CmdParser::CmdParser()
	: _usage(CmdUsage::instance())
{
}

bool CmdParser::parseToken(const string& cmdline)
{
	bool hasL = false;
	size_t i = 0, dotIdx = 0, lIdx = 0, rIdx = 0;
	const char* ch = cmdline.c_str();
	size_t len = cmdline.size();
	while (i < len)
	{
		switch (ch[i++])
		{
		case '.':
			if (!hasL)
				dotIdx = i - 1;
			break;
		case '(':
			if (!hasL)
			{
				hasL = true;
				lIdx = i - 1;
			}
			break;
		case ')':
			rIdx = i - 1;
			break;
		default:
			break;
		}
	}

	_arglist.clear();
	if (hasL)
	{
		if (lIdx >= rIdx)
			return false;

		_cmdproc = cmdline.substr(dotIdx == 0 ? 0 : dotIdx + 1, dotIdx == 0 ? lIdx - dotIdx : lIdx - dotIdx - 1);
		lIdx += 1;
		rIdx -= 1;
		if (lIdx <= rIdx)
		{
			string cmdpara = cmdline.substr(lIdx, rIdx - lIdx + 1);
			stringtok(_arglist, cmdpara, ",");
		}
	}
	else
	{
		_cmdproc = cmdline.substr(dotIdx == 0 ? 0 : dotIdx + 1);
	}

	if (dotIdx == 0)
		_cmdname = "__BUILTIN__";
	else
		_cmdname = strToUpper(cmdline.substr(0, dotIdx));

	return true;
}

std::string CmdParser::getCmdName() const
{
	return _cmdname;
}

bool CmdParser::read(const std::string& str)
{
	if (zoo::compareNoCase(str, _cmdproc))
		return true;
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 1)
		{
			if (value1.assign(_arglist[0].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 2)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 3)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()) &&
				value3.assign(_arglist[2].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 4)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()) &&
				value3.assign(_arglist[2].c_str()) &&
				value4.assign(_arglist[3].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 5)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()) &&
				value3.assign(_arglist[2].c_str()) &&
				value4.assign(_arglist[3].c_str()) &&
				value5.assign(_arglist[4].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 6)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()) &&
				value3.assign(_arglist[2].c_str()) &&
				value4.assign(_arglist[3].c_str()) &&
				value5.assign(_arglist[4].c_str()) &&
				value6.assign(_arglist[5].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 7)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()) &&
				value3.assign(_arglist[2].c_str()) &&
				value4.assign(_arglist[3].c_str()) &&
				value5.assign(_arglist[4].c_str()) &&
				value6.assign(_arglist[5].c_str()) &&
				value7.assign(_arglist[6].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7, Parameter value8)
{
	if (zoo::compareNoCase(str, _cmdproc))
	{
		if (_arglist.size() >= 8)
		{
			if (value1.assign(_arglist[0].c_str()) &&
				value2.assign(_arglist[1].c_str()) &&
				value3.assign(_arglist[2].c_str()) &&
				value4.assign(_arglist[3].c_str()) &&
				value5.assign(_arglist[4].c_str()) &&
				value6.assign(_arglist[5].c_str()) &&
				value7.assign(_arglist[6].c_str()) &&
				value8.assign(_arglist[7].c_str()))
				return true;
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::errors(ErrorSeverity severity) const
{
	for (ErrorMessageMap::const_iterator itr = _errorMessageMap.begin(); itr != _errorMessageMap.end(); ++itr)
	{
		if (itr->second >= severity)
			return true;
	}
	return false;
}

void CmdParser::reportError(const std::string& message, ErrorSeverity severity)
{
	_errorMessageMap[message] = severity;
}

void CmdParser::reportRemainingCallsAsUnrecognized(ErrorSeverity severity)
{
	std::set<std::string> procedures;
	if (_usage)
	{
		// parse the usage procedures to get all the procedure that the command parser can potential handle.
		CmdUsage::UsageMap::const_iterator it = _usage->getCommandProcedureCalls().begin();
		for (; it != _usage->getCommandProcedureCalls().end(); ++it)
		{
			const std::string& procedure = it->first;
			std::string::size_type prevpos = 0, pos = 0;
			if ((pos = procedure.find('(', prevpos)) != std::string::npos)
				procedures.insert(strToUpper(std::string(procedure, prevpos, pos - prevpos)));
		}
	}

	// if have an procedure and haven't been previous querried for report as unrecognized.
	if (procedures.find(strToUpper(_cmdproc)) == procedures.end())
		reportError(std::string("unrecognized command procedure ") + std::string(_cmdproc) + "()", severity);
}

void CmdParser::writeErrorMessages(std::ostream& output, ErrorSeverity severity)
{
	for (ErrorMessageMap::iterator itr = _errorMessageMap.begin(); itr != _errorMessageMap.end(); ++itr)
	{
		if (itr->second >= severity)
			output << getCmdName() << ": " << itr->first << std::endl;
	}
}

bool CmdParser::readHelpType()
{
	getCmdUsage()->addCommandProcedureCall("h()", "Display all command procedure calls");
	// if user request help write it out to cout.
	if (read("h"))
		return true;
	return false;
}

}
