#include <zooCmd/CmdUsage.h>

namespace zooCmd {

CmdUsage* CmdUsage::instance()
{
	static std::unique_ptr<CmdUsage> s_appUsage(new CmdUsage());
	return s_appUsage.get();
}

void CmdUsage::addCommandProcedureCall(const std::string& procedure, const std::string& explanation, const std::string& defaultValue)
{
	_commandProcedureCalls[procedure] = explanation;
	_commandProcedureCallsDefaults[procedure] = defaultValue;
}

void CmdUsage::getFormattedString(std::string& str, const UsageMap& um, unsigned int widthOfOutput, bool showDefaults, const UsageMap& ud)
{
	unsigned int maxNumCharsInCalls = 0;
	CmdUsage::UsageMap::const_iterator citr;
	for (citr = um.begin();
		citr != um.end();
		++citr)
	{
		maxNumCharsInCalls = max(maxNumCharsInCalls, (unsigned int)citr->first.length());
	}

	unsigned int fullWidth = widthOfOutput;
	unsigned int callPos = 2;
	unsigned int explanationPos = callPos + maxNumCharsInCalls + 2;

	double ratioOfExplanationToOutputWidth = float(explanationPos) / float(widthOfOutput);
	double maxRatioOfExplanationToOutputWidth = 0.25f;

	if (ratioOfExplanationToOutputWidth > maxRatioOfExplanationToOutputWidth)
	{
		explanationPos = static_cast<unsigned int>(maxRatioOfExplanationToOutputWidth*float(widthOfOutput));
	}

	unsigned int defaultPos = 0;
	if (showDefaults)
	{
		defaultPos = explanationPos;
		explanationPos = callPos + 8;
	}
	unsigned int explanationWidth = fullWidth - explanationPos;

	std::string line;

	for (citr = um.begin();
		citr != um.end();
		++citr)
	{
		line.assign(fullWidth, ' ');
		line.replace(callPos, citr->first.length(), citr->first);
		unsigned int currentEndPos = callPos + citr->first.length();

		if (showDefaults)
		{

			UsageMap::const_iterator ditr = ud.find(citr->first);
			if (ditr != ud.end())
			{
				if (currentEndPos + 1 >= defaultPos)
				{
					str += line; str += "\n";
					line.assign(fullWidth, ' ');
				}

				line.replace(defaultPos, std::string::npos, "");
				if (ditr->second != "")
				{
					line += "[";
					line += ditr->second;
					line += "]";
				}
				str += line;
				str += "\n";
				line.assign(fullWidth, ' ');

				currentEndPos = 0;
			}
		}

		const std::string& explanation = citr->second;
		std::string::size_type pos = 0;
		std::string::size_type offset = 0;
		bool firstInLine = true;
		if (!explanation.empty())
		{

			if (currentEndPos + 1 > explanationPos)
			{
				str += line; str += "\n";
				line.assign(fullWidth, ' ');
			}

			while (pos < explanation.length())
			{
				if (firstInLine) offset = 0;

				// skip any leading white space.
				while (pos < explanation.length() && explanation[pos] == ' ')
				{
					if (firstInLine) ++offset;
					++pos;
				}

				firstInLine = false;

				std::string::size_type width = min((std::string::size_type)(explanation.length() - pos), (std::string::size_type)(explanationWidth - offset));
				std::string::size_type slashn_pos = explanation.find('\n', pos);

				unsigned int extraSkip = 0;
				bool concatinated = false;
				if (slashn_pos != std::string::npos)
				{
					if (slashn_pos < pos + width)
					{
						width = slashn_pos - pos;
						++extraSkip;
						firstInLine = true;
					}
					else if (slashn_pos == pos + width)
					{
						++extraSkip;
						firstInLine = true;
					}
				}

				if (pos + width < explanation.length())
				{
					// now reduce width until we get a space or a return
					// so that we ensure that whole words are printed.
					while (width > 0 &&
						explanation[pos + width] != ' ' &&
						explanation[pos + width] != '\n') --width;

					if (width == 0)
					{
						// word must be longer than a whole line so will need
						// to concatenate it.
						width = explanationWidth - 1;
						concatinated = true;
					}
				}

				line.replace(explanationPos + offset, explanationWidth, explanation, pos, width);

				if (concatinated) { str += line; str += "-\n"; }
				else { str += line; str += "\n"; }

				// move to the next line of output.
				line.assign(fullWidth, ' ');

				pos += width + extraSkip;

			}
		}
		else
		{
			str += line; str += "\n";
		}
	}
}

void CmdUsage::write(std::ostream& output, const CmdUsage::UsageMap& um, unsigned int widthOfOutput, bool showDefaults, const CmdUsage::UsageMap& ud)
{
	std::string str;
	getFormattedString(str, um, widthOfOutput, showDefaults, ud);
	output << str << std::endl;
}

void CmdUsage::write(std::ostream& output, unsigned int widthOfOutput, bool showDefaults)
{
	output << "Usage: " << getCommandProcedureUsage() << std::endl;
	if (!getCommandProcedureCalls().empty())
	{
		output << "Procedures";
		if (showDefaults) output << " [and default value]";
		output << ":" << std::endl;
		write(output, getCommandProcedureCalls(), widthOfOutput, showDefaults, getCommandProcedureCallsDefaults());
	}
}

}
