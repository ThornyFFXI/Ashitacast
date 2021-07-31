#include "ashitacastVariables.h"
#pragma comment(lib, "psapi.lib")
#include <psapi.h>

ashitacastVariables::ashitacastVariables(IAshitaCore* ashitaCore, actionData_t* actionData_t, OutputHelpers* output)
{
    m_AshitaCore   = ashitaCore;
    mCurrentAction = actionData_t;
    pOutput        = output;

    pEntity    = m_AshitaCore->GetMemoryManager()->GetEntity();
    pInventory = m_AshitaCore->GetMemoryManager()->GetInventory();
    pParty     = m_AshitaCore->GetMemoryManager()->GetParty();
    pPlayer    = m_AshitaCore->GetMemoryManager()->GetPlayer();
    pTarget    = m_AshitaCore->GetMemoryManager()->GetTarget();
    pResource  = m_AshitaCore->GetResourceManager();

    MODULEINFO mod = {0};
    ::GetModuleInformation(::GetCurrentProcess(), ::GetModuleHandle("FFXiMain.dll"), &mod, sizeof(MODULEINFO));
    pWeather  = Ashita::Memory::FindPattern((uintptr_t)mod.lpBaseOfDll, (uintptr_t)mod.SizeOfImage, "66A1????????663D????72", 0, 0);
    pVanaTime = Ashita::Memory::FindPattern((uintptr_t)mod.lpBaseOfDll, (uintptr_t)mod.SizeOfImage, "B0015EC390518B4C24088D4424005068", 0, 0);
    initializeResolverMap();
    for (int x = 0; x < 16; x++)
    {
        mEquipOverrides[x].expiration = std::chrono::steady_clock::now() - std::chrono::seconds(1);
        mEquipOverrides[x].disabled   = false;
        mEquipOverrides[x].encumbered = false;
    }
}

ashitacastVariables::~ashitacastVariables(void)
{
}

void ashitacastVariables::registerBuff(int16_t id, bool active, int duration)
{
    buffRegistry_t buff;
    buff.buffId     = id;
    buff.active     = active;
    buff.expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
    for (std::list<buffRegistry_t>::iterator it = mBuffOverrides.begin(); it != mBuffOverrides.end();)
    {
        if ((*it).buffId == id)
        {
            it = mBuffOverrides.erase(it);
        }
        else
            it++;
    }
    mBuffOverrides.push_back(buff);
}
void ashitacastVariables::clearBuff(int16_t id)
{
    for (std::list<buffRegistry_t>::iterator it = mBuffOverrides.begin(); it != mBuffOverrides.end();)
    {
        if ((*it).buffId == id)
        {
            it = mBuffOverrides.erase(it);
        }
        else
            it++;
    }
}

void ashitacastVariables::clearVariables()
{
    mVariableMap.clear();
}
void ashitacastVariables::clearVariable(std::string name)
{
    std::map<string, string>::iterator element = mVariableMap.find(name);
    while (element != mVariableMap.end())
    {
        mVariableMap.erase(element);
        element = mVariableMap.find(name);
    }
}
void ashitacastVariables::setVariable(std::string name, std::string value)
{
    mVariableMap[name] = value;
}

void ashitacastVariables::printEnvironmentalVariables()
{
    for (std::map<string, variableResolver>::iterator iter = mResolverMap.begin(); iter != mResolverMap.end(); iter++)
    {
        pOutput->message_f("%s : %s", iter->first.c_str(), ((this->*iter->second)()).c_str());
    }
}
void ashitacastVariables::printUserVariables()
{
    for (std::map<string, string>::iterator iter = mVariableMap.begin(); iter != mVariableMap.end(); iter++)
    {
        pOutput->message_f("%s : %s", iter->first.c_str(), iter->second.c_str());
    }
}
void ashitacastVariables::setLastPosition(float_t x, float_t z)
{
    mLastPosX = x;
    mLastPosZ = z;
}
bool ashitacastVariables::processAdvanced(std::string input)
{
    string working = resolveVariables(input);
    return (resolveAdvanced(working) == "true");
}
bool ashitacastVariables::processBasic(const char* attrName, const char* attrValue)
{
    string name    = attrName;
    string content = attrValue;

    if (content.find_first_of('|') != std::string::npos)
    {
        bool evaluate = false;

        stringstream stream(content);
        while (stream.good())
        {
            string comparer;
            getline(stream, comparer, '|');
            if (comparer.find_first_of("<>!") == std::string::npos)
                comparer = "$" + name + "=" + comparer;
            else
                comparer = "$" + name + comparer;

            string working = resolveVariables(comparer);
            if (resolveBasic(working) == "true")
            {
                evaluate = true;
                break;
            }
        }

        return evaluate;
    }
    else if (content.find_first_of('&') != std::string::npos)
    {
        stringstream stream(content);
        while (stream.good())
        {
            string comparer;
            getline(stream, comparer, '&');
            if (comparer.find_first_of("<>!") == std::string::npos)
                comparer = "$" + name + "=" + comparer;
            else
                comparer = "$" + name + comparer;

            string working = resolveVariables(comparer);
            if (resolveBasic(working) != "true")
                return false;
        }
        return true;
    }
    else
    {
        if (content.find_first_of("<>!") == std::string::npos)
            content = "$" + name + "=" + content;
        else
            content = "$" + name + content;
        string working = resolveVariables(content);
        return (resolveBasic(working) == "true");
    }
}
std::string ashitacastVariables::resolveVariables(std::string input)
{
    string working       = input;
    size_t firstVariable = working.rfind('$');
    while (firstVariable != string::npos)
    {
        working       = resolveVariable(working, firstVariable);
        firstVariable = working.rfind('$');
    }
    return working;
}
string ashitacastVariables::resolveVariable(string input, size_t offset)
{
    string subString = input.substr(offset + 1);

    if (subString.substr(0, 2) == "u_") //User variables start with u_.  Resolve accordingly.
    {
        subString = subString.substr(2);

        std::map<string, string>::iterator keep = mVariableMap.end();
        for (std::map<string, string>::iterator iter = mVariableMap.begin(); iter != mVariableMap.end(); iter++)
        {
            if (strncmp(subString.c_str(), iter->first.c_str(), iter->first.length()) == 0)
            {
                if ((keep == mVariableMap.end()) || (iter->first.length() > keep->first.length()))
                    keep = iter;
            }
        }
        if (keep != mVariableMap.end())
        {
            return input.replace(offset, keep->first.length() + 3, keep->second);
        }
        else
        {
            //We couldn't find a matching user variable, so eat the $ to prevent further attempts to resolve it.
            return input.replace(offset, 1, std::string());
        }
    }
    else if (subString.substr(0, 2) == "b_") //Buff checks start with b_.  Resolve accordingly.
    {
        subString = subString.substr(2);

        int numMatch = 0;
        for (int x = 0; x < 3; x++)
        {
            if (x >= subString.length())
                break;
            if (isdigit(subString[x]))
                numMatch++;
        }

        if (numMatch > 0)
        {
            std::vector<int16_t> buffs;
            buffs.push_back(atoi(subString.substr(0, numMatch).c_str()));
            int count = getBuffCount(buffs);
            return input.replace(offset, numMatch + 3, to_string(count));
        }

        std::vector<int16_t> matchingBuffs;
        int matchingLength = 0;
        for (int x = 0; x < 650; x++)
        {
            const char* buff = pResource->GetString("buffs", x);
            if (!buff)
                continue;
            int length = strlen(buff);

            if (_strnicmp(buff, subString.c_str(), length) == 0)
            {
                if (length == matchingLength)
                    matchingBuffs.push_back(x);
                else if (length > matchingLength)
                {
                    matchingBuffs.clear();
                    matchingBuffs.push_back(x);
                    matchingLength = length;
                }
            }
        }

        int count = getBuffCount(matchingBuffs);
        return input.replace(offset, matchingLength + 3, to_string(count));
    }
    else if (subString.substr(0, 3) == "arg") //$args $arg1 $arg2 ... $arg20 etc.
    {
        if (!mArgData.isActive)
        {
            //Args aren't active, so eat the $ to prevent further attempts to resolve it.
            return input.replace(offset, 1, std::string());
        }

        if (subString[3] == 's')
        {
            //We found $args, so replace with full arguments
            return input.replace(offset, 5, mArgData.fullargs);
        }

        //Find longest string that represents a valid argument.
        int argument      = atoi(subString.substr(3, 1).c_str());
        int replaceLength = 5;
        int length        = 1;
        while (isalnum(subString[3 + length]))
        {
            length++;
            int tempArg = atoi(subString.substr(3, length).c_str());
            if (tempArg < mArgData.argCount)
            {
                replaceLength = 4 + length;
                argument      = tempArg;
            }
            else
                break;
        }
        if (argument < mArgData.argCount)
        {
            return input.replace(offset, replaceLength, mArgData.args[argument]);
        }
        else
        {
            //Specified arg is higher than existing args, so eat the $ to prevent further attempts to resolve it.
            return input.replace(offset, 1, std::string());
        }
    }
    else //We know it's not a user variable or an argument, so check the resolver map for built in variables.
    {
        std::map<string, variableResolver>::iterator keep = mResolverMap.end();
        for (std::map<string, variableResolver>::iterator iter = mResolverMap.begin(); iter != mResolverMap.end(); iter++)
        {
            if (strncmp(subString.c_str(), iter->first.c_str(), iter->first.length()) == 0)
            {
                if ((keep == mResolverMap.end()) || (iter->first.length() > keep->first.length()))
                    keep = iter;
            }
        }
        if (keep != mResolverMap.end())
        {
            //We found the longest match, so resolve and replace.
            subString = (this->*keep->second)();
            return input.replace(offset, keep->first.length() + 1, subString);
        }
        else
        {
            //We couldn't find a matching variable, so eat the $ to prevent further attempts to resolve it.
            return input.replace(offset, 1, std::string());
        }
    }
}

string ashitacastVariables::resolveAdvanced(string input)
{
    string working          = input;
    size_t firstParentheses = working.rfind(')');
    while (firstParentheses != string::npos)
    {
        size_t begin = working.rfind('(', firstParentheses);
        if (begin == string::npos)
        {
            //Delete a closing parenthesis that has no opening parenthesis.
            working = working.replace(firstParentheses, 1, string());
        }
        else
        {
            //Process the contents of the parentheses.
            int length = firstParentheses - begin;
            working    = working.replace(begin, length + 1, resolveAdvanced(working.substr(begin + 1, length - 1)));
        }

        firstParentheses = working.rfind(')');
    }

    firstParentheses = working.rfind('(');
    while (firstParentheses != string::npos)
    {
        //Delete an opening parenthesis that has no closing parenthesis.
        working          = working.replace(firstParentheses, 1, string());
        firstParentheses = working.rfind('(');
    }

    //Parentheses all resolved.  Operands now?  Plz no.
    working = resolveExponents(working);
    working = resolveMultiplication(working);
    working = resolveAddition(working);
    working = resolveConditions(working);

    return working;
}
string ashitacastVariables::resolveBasic(string input)
{
    string working       = input;
    size_t firstOperator = working.find_first_of("<>=!");
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs                  = splitConditionBasic(working, firstOperator, &lhs, &offset, &length);
        bool isStatementTrue = false;

        if (working[firstOperator] == '=')
        {
            if (isNumericValue(lhs) || (isNumericValue(rhs)))
            {
                if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
                {
                    isStatementTrue = (atof(lhs.c_str()) == atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atoi(lhs.c_str()) == atoi(rhs.c_str()));
                }
            }
            else
            {
                isStatementTrue = isWildMatch(lhs.c_str(), rhs.c_str());
            }
        }
        else if (working[firstOperator] == '!')
        {
            if (isNumericValue(lhs) || (isNumericValue(rhs)))
            {
                if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
                {
                    isStatementTrue = !(atof(lhs.c_str()) == atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = !(atoi(lhs.c_str()) == atoi(rhs.c_str()));
                }
            }
            else
            {
                isStatementTrue = !isWildMatch(lhs.c_str(), rhs.c_str());
            }
        }
        else if (working[firstOperator] == '<')
        {
            if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atof(lhs.c_str()) <= atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atof(lhs.c_str()) < atof(rhs.c_str()));
                }
            }
            else
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atoi(lhs.c_str()) <= atoi(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atoi(lhs.c_str()) < atoi(rhs.c_str()));
                }
            }
        }
        else if (working[firstOperator] == '>')
        {
            if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atof(lhs.c_str()) >= atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atof(lhs.c_str()) > atof(rhs.c_str()));
                }
            }
            else
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atoi(lhs.c_str()) >= atoi(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atoi(lhs.c_str()) > atoi(rhs.c_str()));
                }
            }
        }

        if (isStatementTrue)
            working.replace(offset, length, "true");
        else
            working.replace(offset, length, "false");

        firstOperator = working.find_first_of("<>=!");
    }
    firstOperator = working.find_first_of("|&");
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs                  = splitCondition(working, firstOperator, &lhs, &offset, &length);
        bool isStatementTrue = false;

        if (working[firstOperator] == '|')
            isStatementTrue = ((lhs == "true") || (rhs == "true"));
        else if (working[firstOperator] == '&')
            isStatementTrue = ((lhs == "true") && (rhs == "true"));

        firstOperator = working.find_first_of("|&");
    }

    return working;
}
string ashitacastVariables::resolveExponents(string input)
{
    string working       = input;
    size_t firstOperator = working.find('^');
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs = splitCondition(working, firstOperator, &lhs, &offset, &length);

        if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
        {
            working.replace(offset, length, to_string(pow(atof(lhs.c_str()), atof(rhs.c_str()))));
        }
        else
        {
            working.replace(offset, length, to_string(pow(atoi(lhs.c_str()), atoi(rhs.c_str()))));
        }

        firstOperator = working.find('^');
    }
    return working;
}
string ashitacastVariables::resolveMultiplication(string input)
{
    string working       = input;
    size_t firstOperator = working.find_first_of("*/");
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs = splitCondition(working, firstOperator, &lhs, &offset, &length);

        if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
        {
            if (working[firstOperator] == '*')
                working.replace(offset, length, to_string(atof(lhs.c_str()) * atof(rhs.c_str())));
            else
                working.replace(offset, length, to_string(atof(lhs.c_str()) / atof(rhs.c_str())));
        }
        else
        {
            if (working[firstOperator] == '*')
                working.replace(offset, length, to_string(atoi(lhs.c_str()) * atoi(rhs.c_str())));
            else
                working.replace(offset, length, to_string(atoi(lhs.c_str()) / atoi(rhs.c_str())));
        }

        firstOperator = working.find_first_of("*/");
    }
    return working;
}
string ashitacastVariables::resolveAddition(string input)
{
    string working       = input;
    size_t firstOperator = working.find_first_of("+-");
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs = splitCondition(working, firstOperator, &lhs, &offset, &length);
        if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
        {
            if (working[firstOperator] == '+')
                working.replace(offset, length, to_string(atof(lhs.c_str()) + atof(rhs.c_str())));
            else
                working.replace(offset, length, to_string(atof(lhs.c_str()) - atof(rhs.c_str())));
        }
        else
        {
            if (working[firstOperator] == '+')
                working.replace(offset, length, to_string(atoi(lhs.c_str()) + atoi(rhs.c_str())));
            else
                working.replace(offset, length, to_string(atoi(lhs.c_str()) - atoi(rhs.c_str())));
        }

        firstOperator = working.find_first_of("+-");
    }
    return working;
}
string ashitacastVariables::resolveConditions(string input)
{
    string working       = input;
    size_t firstOperator = working.find_first_of("<>=!");
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs                  = splitCondition(working, firstOperator, &lhs, &offset, &length);
        bool isStatementTrue = false;

        if (working[firstOperator] == '=')
        {
            if (isNumericValue(lhs) || (isNumericValue(rhs)))
            {
                if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
                {
                    isStatementTrue = (atof(lhs.c_str()) == atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atoi(lhs.c_str()) == atoi(rhs.c_str()));
                }
            }
            else
            {
                isStatementTrue = (_stricmp(lhs.c_str(), rhs.c_str()) == 0);
            }
        }
        else if (working[firstOperator] == '!')
        {
            if (isNumericValue(lhs) || (isNumericValue(rhs)))
            {
                if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
                {
                    isStatementTrue = !(atof(lhs.c_str()) == atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = !(atoi(lhs.c_str()) == atoi(rhs.c_str()));
                }
            }
            else
            {
                isStatementTrue = !(_stricmp(lhs.c_str(), rhs.c_str()) == 0);
            }
        }
        else if (working[firstOperator] == '<')
        {
            if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atof(lhs.c_str()) <= atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atof(lhs.c_str()) < atof(rhs.c_str()));
                }
            }
            else
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atoi(lhs.c_str()) <= atoi(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atoi(lhs.c_str()) < atoi(rhs.c_str()));
                }
            }
        }
        else if (working[firstOperator] == '>')
        {
            if ((lhs.find_first_of('.') != string::npos) || (rhs.find_first_of('.') != string::npos))
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atof(lhs.c_str()) >= atof(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atof(lhs.c_str()) > atof(rhs.c_str()));
                }
            }
            else
            {
                if (((firstOperator + 1) < working.length()) && ((working[firstOperator + 1]) == '='))
                {
                    isStatementTrue = (atoi(lhs.c_str()) >= atoi(rhs.c_str()));
                }
                else
                {
                    isStatementTrue = (atoi(lhs.c_str()) > atoi(rhs.c_str()));
                }
            }
        }

        if (isStatementTrue)
            working.replace(offset, length, "true");
        else
            working.replace(offset, length, "false");

        firstOperator = working.find_first_of("<>=!");
    }
    firstOperator = working.find_first_of("|&");
    while (firstOperator != string::npos)
    {
        string lhs, rhs;
        size_t offset, length;
        rhs                  = splitCondition(working, firstOperator, &lhs, &offset, &length);
        bool isStatementTrue = false;

        if (working[firstOperator] == '|')
            isStatementTrue = ((lhs == "true") || (rhs == "true"));
        else if (working[firstOperator] == '&')
            isStatementTrue = ((lhs == "true") && (rhs == "true"));

        if (isStatementTrue)
            working.replace(offset, length, "true");
        else
            working.replace(offset, length, "false");

        firstOperator = working.find_first_of("|&");
    }

    return working;
}
string ashitacastVariables::splitCondition(string input, size_t offset, string* lhs, size_t* replaceOffset, size_t* replaceLength)
{
    size_t leftbound = input.substr(0, offset).find_last_of("^/*+-|&<>=!");

    if (((offset + 1) < input.length()) && (input[offset + 1] == '='))
    {
        offset++;
    }

    size_t rightbound = input.substr(offset + 1).find_first_of("^/*+-|&<>=!");

    if (leftbound == string::npos)
        *lhs = trimWhiteSpace(input.substr(0, offset));
    else
        *lhs = trimWhiteSpace(input.substr(leftbound + 1, (offset - (leftbound + 1))));

    if (leftbound == string::npos)
        *replaceOffset = 0;
    else
        *replaceOffset = leftbound + 1;

    if (rightbound == string::npos)
        *replaceLength = input.size() - *replaceOffset;
    else
        *replaceLength = (rightbound + offset + 1) - *replaceOffset;

    if (rightbound == string::npos)
        return trimWhiteSpace(input.substr(offset + 1));
    else
        return trimWhiteSpace(input.substr(offset + 1, rightbound));
}
string ashitacastVariables::splitConditionBasic(string input, size_t offset, string* lhs, size_t* replaceOffset, size_t* replaceLength)
{
    size_t leftbound = input.substr(0, offset).find_last_of("<>=!");

    if (((offset + 1) < input.length()) && (input[offset + 1] == '='))
    {
        offset++;
    }

    size_t rightbound = input.substr(offset + 1).find_first_of("<>=!");

    if (leftbound == string::npos)
        *lhs = trimWhiteSpace(input.substr(0, offset));
    else
        *lhs = trimWhiteSpace(input.substr(leftbound + 1, (offset - (leftbound + 1))));

    if (leftbound == string::npos)
        *replaceOffset = 0;
    else
        *replaceOffset = leftbound + 1;

    if (rightbound == string::npos)
        *replaceLength = input.size() - *replaceOffset;
    else
        *replaceLength = (rightbound + offset + 1) - *replaceOffset;

    if (rightbound == string::npos)
        return trimWhiteSpace(input.substr(offset + 1));
    else
        return trimWhiteSpace(input.substr(offset + 1, rightbound));
}
string ashitacastVariables::trimWhiteSpace(string input)
{
    string working = input;
    size_t start   = working.find_first_not_of(' ');
    if (start == string::npos)
        return string();
    else
        working = working.substr(start);

    size_t end = working.find_last_not_of(' ');
    return working.substr(0, end + 1);
}
bool ashitacastVariables::isNumericValue(string input)
{
    bool decimalfound = false;
    for (int x = 0; x < input.length(); x++)
    {
        if (isdigit(input[x]))
            continue;
        else if (input[x] == '.')
        {
            if (decimalfound)
                return false;
            decimalfound = true;
        }
        else
            return false;
    }
    return true;
}
bool ashitacastVariables::isWildMatch(const char* compare, const char* wc)
{
    while (*wc)
    {
        if (wc[0] == '*')
        {
            if (wc[1] == 0)
                return true;
            while (*compare)
            {
                if (isWildMatch(compare, (wc + 1)))
                {
                    return true;
                }
                compare++;
            }
            return false;
        }
        if (*compare == 0)
            break;
        if ((wc[0] | 32) != (compare[0] | 32))
            return false;
        wc++;
        compare++;
    }
    return ((*compare == 0) && (*wc == 0));
}

ffxiDateTime ashitacastVariables::getTimeStamp()
{
    DWORD pointer    = Read32(pVanaTime, 0x34);
    uint64_t rawTime = (uint64_t)Read32(pointer, 0x0C) + (uint64_t)92514960;
    ffxiDateTime retValue;
    retValue.day    = (uint32_t)(rawTime / (uint64_t)3456);
    retValue.hour   = (uint8_t)((rawTime / (uint64_t)144) % 24);
    rawTime         = rawTime % (uint64_t)144;
    retValue.minute = (uint8_t)((float_t)rawTime / 2.4);
    return retValue;
}
uint8_t ashitacastVariables::getWeather()
{
    DWORD pointer = Read32(pWeather, 0x02);
    return Read8(pointer, 0);
}
int ashitacastVariables::getBuffCount(std::vector<int16_t> buffs)
{
    for (std::list<buffRegistry_t>::iterator iter = mBuffOverrides.begin(); iter != mBuffOverrides.end();)
    {
        if (std::chrono::steady_clock::now() > (*iter).expiration)
            iter = mBuffOverrides.erase(iter);
        else
        {
            if (std::find(buffs.begin(), buffs.end(), (*iter).buffId) != buffs.end())
            {
                return (*iter).active;
            }
            iter++;
        }
    }

    int count = 0;
    for (std::vector<int16_t>::iterator iter = buffs.begin(); iter != buffs.end(); iter++)
    {
        for (int x = 0; x < 32; x++)
        {
            if (pPlayer->GetBuffs()[x] == *iter)
                count++;
        }
    }
    return count;
}
equipRegistry_t ashitacastVariables::getEquippedItem(int slot)
{
    if (std::chrono::steady_clock::now() < mEquipOverrides[slot].expiration)
    {
        return mEquipOverrides[slot];
    }
    else
    {
        equipRegistry_t equip = mEquipOverrides[slot];
        Ashita::FFXI::equipmententry_t* equipData = pInventory->GetEquippedItem(slot);
        equip.index                               = Read8(&equipData->Index, 0);
        equip.container                           = Read8(&equipData->Index, 1);
        return equip;
    }
}