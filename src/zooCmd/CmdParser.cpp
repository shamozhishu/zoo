#include <zooCmd/CmdParser.h>

namespace zooCmd {

bool CmdParser::isOption(const char* str)
{
	return str && str[0] == '-';
}

bool CmdParser::isString(const char* str)
{
	if (!str || isOption(str))
		return false;
	return true;
}

bool CmdParser::isBool(const char* str)
{
	if (!str)
		return false;
	return (strcmp(str, "True") == 0 || strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
		strcmp(str, "False") == 0 || strcmp(str, "false") == 0 || strcmp(str, "FALSE") == 0 ||
		strcmp(str, "0") == 0 || strcmp(str, "1") == 0);
}

bool CmdParser::isNumber(const char* str)
{
	if (!str) return false;

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

bool CmdParser::Parameter::valid(const char* str) const
{
	switch (_type)
	{
	case Parameter::BOOL_PARAMETER:         return isBool(str); break;
	case Parameter::FLOAT_PARAMETER:        return isNumber(str); break;
	case Parameter::DOUBLE_PARAMETER:       return isNumber(str); break;
	case Parameter::INT_PARAMETER:          return isNumber(str); break;
	case Parameter::UNSIGNED_INT_PARAMETER: return isNumber(str); break;
	case Parameter::STRING_PARAMETER:       return isString(str); break;
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
			*_value._bool = (strcmp(str, "True") == 0 || strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0);
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

CmdParser::CmdParser(int* argc, char **argv)
	: _argc(argc)
	, _argv(argv)
	, _usage(CmdUsage::instance())
{
#ifdef __APPLE__
	// On OSX, any -psn arguments need to be removed because they will
	// confuse the application. -psn plus a concatenated argument are
	// passed by the finder to application bundles
	for (int pos = 1; pos < this->argc(); ++pos)
	{
		if (std::string(_argv[pos]).compare(0, 4, std::string("-psn")) == 0)
		{
			remove(pos, 1);
		}
	}
#endif
#ifdef _WIN32
	// Remove linefeed from last argument if it exist
	char* lastline = argc == 0 ? 0 : _argv[*argc - 1];
	if (lastline)
	{
		int len = strlen(lastline);
		if (len > 0 && lastline[len - 1] == '\n')
			lastline[len - 1] = '\0';
	}
#endif
}

int CmdParser::find(const std::string& str) const
{
	for (int pos = 1; pos < *_argc; ++pos)
	{
		if (str == _argv[pos])
			return pos;
	}
	return -1;
}

std::string CmdParser::getCmdName() const
{
	if (_argc && *_argc > 0)
		return std::string(_argv[0]);
	return "";
}

bool CmdParser::isOption(int pos) const
{
	return pos < *_argc && isOption(_argv[pos]);
}

bool CmdParser::isString(int pos) const
{
	return pos < *_argc && isString(_argv[pos]);
}

bool CmdParser::isNumber(int pos) const
{
	return pos < *_argc && isNumber(_argv[pos]);
}

bool CmdParser::containsOptions() const
{
	for (int pos = 1; pos < *_argc; ++pos)
	{
		if (isOption(pos))
			return true;
	}
	return false;
}

void CmdParser::remove(int pos, int num)
{
	if (num == 0)
		return;
	for (; pos + num < *_argc; ++pos)
	{
		_argv[pos] = _argv[pos + num];
	}
	for (; pos < *_argc; ++pos)
	{
		_argv[pos] = 0;
	}
	*_argc -= num;
}

bool CmdParser::match(int pos, const std::string& str) const
{
	return pos < *_argc && str == _argv[pos];
}

bool CmdParser::read(const std::string& str)
{
	int pos = find(str);
	if (pos <= 0) return false;
	remove(pos);
	return true;
}

bool CmdParser::read(const std::string& str, Parameter value1)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2, value3);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2, value3, value4);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2, value3, value4, value5);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2, value3, value4, value5, value6);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2, value3, value4, value5, value6, value7);
}

bool CmdParser::read(const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7, Parameter value8)
{
	int pos = find(str);
	if (pos <= 0) return false;
	return read(pos, str, value1, value2, value3, value4, value5, value6, value7, value8);
}

// if the argument value at the position pos matches specified string,
// and subsequent parameters are also matched then set the parameter values and remove it from the list of arguments.
bool CmdParser::read(int pos, const std::string& str)
{
	if (match(pos, str))
	{
		remove(pos, 1);
		return true;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1)
{
	if (match(pos, str))
	{
		if ((pos + 1) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]))
			{
				value1.assign(_argv[pos + 1]);
				remove(pos, 2);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2)
{
	if (match(pos, str))
	{
		if ((pos + 2) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				remove(pos, 3);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3)
{
	if (match(pos, str))
	{
		if ((pos + 3) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]) &&
				value3.valid(_argv[pos + 3]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				value3.assign(_argv[pos + 3]);
				remove(pos, 4);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4)
{
	if (match(pos, str))
	{
		if ((pos + 4) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]) &&
				value3.valid(_argv[pos + 3]) &&
				value4.valid(_argv[pos + 4]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				value3.assign(_argv[pos + 3]);
				value4.assign(_argv[pos + 4]);
				remove(pos, 5);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5)
{
	if (match(pos, str))
	{
		if ((pos + 5) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]) &&
				value3.valid(_argv[pos + 3]) &&
				value4.valid(_argv[pos + 4]) &&
				value5.valid(_argv[pos + 5]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				value3.assign(_argv[pos + 3]);
				value4.assign(_argv[pos + 4]);
				value5.assign(_argv[pos + 5]);
				remove(pos, 6);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6)
{
	if (match(pos, str))
	{
		if ((pos + 6) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]) &&
				value3.valid(_argv[pos + 3]) &&
				value4.valid(_argv[pos + 4]) &&
				value5.valid(_argv[pos + 5]) &&
				value6.valid(_argv[pos + 6]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				value3.assign(_argv[pos + 3]);
				value4.assign(_argv[pos + 4]);
				value5.assign(_argv[pos + 5]);
				value6.assign(_argv[pos + 6]);
				remove(pos, 7);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7)
{
	if (match(pos, str))
	{
		if ((pos + 7) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]) &&
				value3.valid(_argv[pos + 3]) &&
				value4.valid(_argv[pos + 4]) &&
				value5.valid(_argv[pos + 5]) &&
				value6.valid(_argv[pos + 6]) &&
				value7.valid(_argv[pos + 7]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				value3.assign(_argv[pos + 3]);
				value4.assign(_argv[pos + 4]);
				value5.assign(_argv[pos + 5]);
				value6.assign(_argv[pos + 6]);
				value7.assign(_argv[pos + 7]);
				remove(pos, 8);
				return true;
			}
			reportError("argument to `" + str + "` is not valid");
			return false;
		}
		reportError("argument to `" + str + "` is missing");
		return false;
	}
	return false;
}

bool CmdParser::read(int pos, const std::string& str, Parameter value1, Parameter value2, Parameter value3, Parameter value4, Parameter value5, Parameter value6, Parameter value7, Parameter value8)
{
	if (match(pos, str))
	{
		if ((pos + 8) < *_argc)
		{
			if (value1.valid(_argv[pos + 1]) &&
				value2.valid(_argv[pos + 2]) &&
				value3.valid(_argv[pos + 3]) &&
				value4.valid(_argv[pos + 4]) &&
				value5.valid(_argv[pos + 5]) &&
				value6.valid(_argv[pos + 6]) &&
				value7.valid(_argv[pos + 7]) &&
				value8.valid(_argv[pos + 8]))
			{
				value1.assign(_argv[pos + 1]);
				value2.assign(_argv[pos + 2]);
				value3.assign(_argv[pos + 3]);
				value4.assign(_argv[pos + 4]);
				value5.assign(_argv[pos + 5]);
				value6.assign(_argv[pos + 6]);
				value7.assign(_argv[pos + 7]);
				value8.assign(_argv[pos + 8]);
				remove(pos, 9);
				return true;
			}
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

void CmdParser::reportRemainingOptionsAsUnrecognized(ErrorSeverity severity)
{
	std::set<std::string> options;
	if (_usage)
	{
		// parse the usage options to get all the option that the application can potential handle.
		for (CmdUsage::UsageMap::const_iterator itr = _usage->getCommandLineOptions().begin();
			itr != _usage->getCommandLineOptions().end();
			++itr)
		{
			const std::string& option = itr->first;
			std::string::size_type prevpos = 0, pos = 0;
			while ((pos = option.find(' ', prevpos)) != std::string::npos)
			{
				if (option[prevpos] == '-')
				{
					options.insert(std::string(option, prevpos, pos - prevpos));
				}
				prevpos = pos + 1;
			}
			if (option[prevpos] == '-')
			{

				options.insert(std::string(option, prevpos, std::string::npos));
			}
		}

	}

	for (int pos = 1; pos < argc(); ++pos)
	{
		// if an option and haven't been previous querried for report as unrecognized.
		if (isOption(pos) && options.find(_argv[pos]) == options.end())
		{
			reportError(std::string("unrecognized option ") + std::string(_argv[pos]), severity);
		}
	}
}

void CmdParser::writeErrorMessages(std::ostream& output, ErrorSeverity severity)
{
	for (ErrorMessageMap::iterator itr = _errorMessageMap.begin(); itr != _errorMessageMap.end(); ++itr)
	{
		if (itr->second >= severity)
			output << getCmdName() << ": " << itr->first << std::endl;
	}
}

CmdUsage::Type CmdParser::readHelpType()
{
	getCmdUsage()->addCommandLineOption("-h or --help", "Display command line parameters");
	getCmdUsage()->addCommandLineOption("--help-env", "Display environmental variables available");
	getCmdUsage()->addCommandLineOption("--help-keys", "Display keyboard & mouse bindings available");
	getCmdUsage()->addCommandLineOption("--help-all", "Display all command line, env vars and keyboard & mouse bindings.");

	// if user request help write it out to cout.
	if (read("--help-all"))             return CmdUsage::HELP_ALL;
	if (read("-h") || read("--help"))   return CmdUsage::COMMAND_LINE_OPTION;
	if (read("--help-env"))             return CmdUsage::ENVIRONMENTAL_VARIABLE;
	if (read("--help-keys"))            return CmdUsage::KEYBOARD_MOUSE_BINDING;

	return CmdUsage::NO_HELP;
}

}