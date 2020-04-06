#include <zooCmd/AppUsage.h>

namespace zooCmd {

AppUsage::AppUsage(const std::string& commandLineUsage):
    _commandLineUsage(commandLineUsage)
{
}

AppUsage* AppUsage::instance()
{
    static std::unique_ptr<AppUsage> s_appUsage(new AppUsage());
    return s_appUsage.get();
}

void AppUsage::addUsageExplanation(Type type,const std::string& option,const std::string& explanation)
{
    switch(type)
    {
        case(COMMAND_LINE_OPTION):
            addCommandLineOption(option,explanation);
            break;
        case(ENVIRONMENTAL_VARIABLE):
            addEnvironmentalVariable(option,explanation);
            break;
        case(KEYBOARD_MOUSE_BINDING):
            //addKeyboardMouseBinding(option,explanation);
            break;
        default:
            break;
    }
}

void AppUsage::addCommandLineOption(const std::string& option,const std::string& explanation,const std::string& defaultValue)
{
    _commandLineOptions[option]=explanation;
    _commandLineOptionsDefaults[option]=defaultValue;
}

void AppUsage::addEnvironmentalVariable(const std::string& option,const std::string& explanation, const std::string& defaultValue)
{
    _environmentalVariables[option]=explanation;
    _environmentalVariablesDefaults[option]=defaultValue;
}

void AppUsage::addKeyboardMouseBinding(const std::string& prefix, int key, const std::string& explanation)
{
    if (key!=0)
    {
        std::ostringstream ostr;
        ostr<<prefix;

        if (key==' ')
        {
            ostr<<"Space";
        }
        else if (key!=0)
        {
            ostr<<char(key);
        }

        _keyboardMouse[ostr.str()]=explanation;
    }
}

void AppUsage::addKeyboardMouseBinding(int key,const std::string& explanation)
{
    addKeyboardMouseBinding("",key,explanation);
}

void AppUsage::addKeyboardMouseBinding(const std::string& option,const std::string& explanation)
{
    _keyboardMouse[option]=explanation;
}

void AppUsage::getFormattedString(std::string& str, const UsageMap& um,unsigned int widthOfOutput,bool showDefaults,const UsageMap& ud)
{
    unsigned int maxNumCharsInOptions = 0;
    AppUsage::UsageMap::const_iterator citr;
    for(citr=um.begin();
        citr!=um.end();
        ++citr)
    {
        maxNumCharsInOptions = max(maxNumCharsInOptions,(unsigned int)citr->first.length());
    }

    unsigned int fullWidth = widthOfOutput;
    unsigned int optionPos = 2;
    unsigned int explanationPos = optionPos+maxNumCharsInOptions+2;

    double ratioOfExplanationToOutputWidth = float(explanationPos)/float(widthOfOutput);
    double maxRatioOfExplanationToOutputWidth = 0.25f;

    if (ratioOfExplanationToOutputWidth > maxRatioOfExplanationToOutputWidth)
    {
        explanationPos = static_cast<unsigned int>(maxRatioOfExplanationToOutputWidth*float(widthOfOutput));
    }

    unsigned int defaultPos = 0;
    if (showDefaults)
    {
        defaultPos = explanationPos;
        explanationPos = optionPos+8;
    }
    unsigned int explanationWidth = fullWidth-explanationPos;

    std::string line;

    for(citr=um.begin();
        citr!=um.end();
        ++citr)
    {
        line.assign(fullWidth,' ');
        line.replace(optionPos,citr->first.length(),citr->first);
        unsigned int currentEndPos = optionPos + citr->first.length();

        if (showDefaults)
        {

            UsageMap::const_iterator ditr = ud.find(citr->first);
            if (ditr != ud.end())
            {
                if (currentEndPos+1>=defaultPos)
                {
                    str += line; str += "\n";
                    line.assign(fullWidth,' ');
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
                line.assign(fullWidth,' ');

                currentEndPos =  0;
            }
        }

        const std::string& explanation = citr->second;
        std::string::size_type pos = 0;
        std::string::size_type offset = 0;
        bool firstInLine = true;
        if (!explanation.empty())
        {

            if (currentEndPos+1>explanationPos)
            {
                str += line; str += "\n";
                line.assign(fullWidth,' ');
            }

            while (pos<explanation.length())
            {
                if (firstInLine) offset = 0;

                // skip any leading white space.
                while (pos<explanation.length() && explanation[pos]==' ')
                {
                    if (firstInLine) ++offset;
                    ++pos;
                }

                firstInLine = false;

                std::string::size_type width = min((std::string::size_type)(explanation.length()-pos),(std::string::size_type)(explanationWidth-offset));
                std::string::size_type slashn_pos = explanation.find('\n',pos);

                unsigned int extraSkip = 0;
                bool concatinated = false;
                if (slashn_pos!=std::string::npos)
                {
                    if (slashn_pos<pos+width)
                    {
                        width = slashn_pos-pos;
                        ++extraSkip;
                        firstInLine = true;
                    }
                    else if (slashn_pos==pos+width)
                    {
                        ++extraSkip;
                        firstInLine = true;
                    }
                }

                if (pos+width<explanation.length())
                {
                    // now reduce width until we get a space or a return
                    // so that we ensure that whole words are printed.
                    while (width>0 &&
                           explanation[pos+width]!=' ' &&
                           explanation[pos+width]!='\n') --width;

                    if (width==0)
                    {
                        // word must be longer than a whole line so will need
                        // to concatenate it.
                        width = explanationWidth-1;
                        concatinated = true;
                    }
                }

                line.replace(explanationPos+offset,explanationWidth, explanation, pos, width);

                if (concatinated) { str += line; str += "-\n"; }
                else { str += line; str += "\n"; }

                // move to the next line of output.
                line.assign(fullWidth,' ');

                pos += width+extraSkip;

            }
        }
        else
        {
            str += line; str += "\n";
        }
    }
}

void AppUsage::write(std::ostream& output, const AppUsage::UsageMap& um,unsigned int widthOfOutput,bool showDefaults,const AppUsage::UsageMap& ud)
{
    std::string str;
    getFormattedString(str, um, widthOfOutput, showDefaults, ud);
    output << str << std::endl;
}

void AppUsage::write(std::ostream& output, unsigned int type, unsigned int widthOfOutput, bool showDefaults)
{

    output << "Usage: "<<getCommandLineUsage()<<std::endl;
    bool needspace = false;
    if ((type&COMMAND_LINE_OPTION) && !getCommandLineOptions().empty())
    {
        output << "Options";
        if (showDefaults) output << " [and default value]";
        output << ":"<<std::endl;
        write(output,getCommandLineOptions(),widthOfOutput,showDefaults,getCommandLineOptionsDefaults());
        needspace = true;
    }

    if ((type&ENVIRONMENTAL_VARIABLE) && !getEnvironmentalVariables().empty())
    {
        if (needspace) output << std::endl;
        output << "Environmental Variables";
        if (showDefaults) output << " [and default value]";
        output << ":"<<std::endl;
        write(output,getEnvironmentalVariables(),widthOfOutput,showDefaults,getEnvironmentalVariablesDefaults());
        needspace = true;
    }

    if ((type&KEYBOARD_MOUSE_BINDING) && !getKeyboardMouseBindings().empty())
    {
        if (needspace) output << std::endl;
        output << "Keyboard and Mouse Bindings:"<<std::endl;
        write(output,getKeyboardMouseBindings(),widthOfOutput);
        needspace = true;
    }
}

void AppUsage::writeEnvironmentSettings(std::ostream& output)
{
    output << "Current Environment Settings:"<<std::endl;

    unsigned int maxNumCharsInOptions = 0;
    AppUsage::UsageMap::const_iterator citr;
    for(citr=getEnvironmentalVariables().begin();
        citr!=getEnvironmentalVariables().end();
        ++citr)
    {
        std::string::size_type len = citr->first.find_first_of("\n\r\t ");
        if (len == std::string::npos) len = citr->first.size();
        maxNumCharsInOptions = max( maxNumCharsInOptions,static_cast<unsigned int>(len));
    }

    unsigned int optionPos = 2;
    std::string line;

    for(citr=getEnvironmentalVariables().begin();
        citr!=getEnvironmentalVariables().end();
        ++citr)
    {
        line.assign(optionPos+maxNumCharsInOptions+2,' ');
        std::string::size_type len = citr->first.find_first_of("\n\r\t ");
        if (len == std::string::npos) len = citr->first.size();
        line.replace(optionPos,len,citr->first.substr(0,len));
		size_t sz;
		char* buf = nullptr;
        if (_dupenv_s(&buf, &sz, citr->first.substr(0, len).c_str()) == 0 && buf != nullptr)
        {
            line += "[set]\n";
			free(buf);
        }
        else
        {
            line += "[not set]\n";
        }

        output << line;
    }
    output << std::endl;
}

}
