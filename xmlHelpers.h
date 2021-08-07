#ifndef __ASHITA_xmlHelpers_H_INCLUDED__
#define __ASHITA_xmlHelpers_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <sstream>
#include <string>
using namespace std;

namespace XmlHelpers
{
    static string getXmlAttr(string tag, std::string name)
    {
        stringstream attr_buffer;
        attr_buffer << name << "=\"";
        size_t begin = tag.find(attr_buffer.str());
        if (begin == string::npos)
            return "";

        string substring = tag.substr(begin + attr_buffer.str().size());
        size_t end       = substring.find('"');
        if (end == string::npos)
            return "";

        return substring.substr(0, end);
    }
    static string getTagContent(string tag)
    {
        size_t tag_end = tag.find('>');
        if (tag_end == string::npos)
            return "";

        string tag_name    = tag.substr(1, (tag_end - 1));
        size_t first_space = tag.find(' ');
        if ((first_space != string::npos) && (first_space < tag_end))
        {
            tag_name = tag.substr(1, --first_space);
        }

        size_t selfterm = tag.find("/>");
        if (tag_end == ++selfterm)
            return "";

        stringstream tag_buffer;
        tag_buffer << "</" << tag_name << ">";
        size_t close_tag = tag.find(tag_buffer.str());
        if (close_tag == string::npos)
            return "";

        return tag.substr(tag_end + 1, close_tag - (tag_end + 1));
    }
    static string getNextXmlTag(string text, size_t* position)
    {
        *position = string::npos;
        stringstream tagBuffer;
        size_t tagBegin = text.find("<");
        if ((tagBegin == string::npos) || ((tagBegin + 1) >= text.length()))
            return "";

        while ((text[tagBegin + 1] == '/')
            || (text[tagBegin + 1] == '!'))
        {
            tagBegin = text.find("<", tagBegin + 1);
            if ((tagBegin == string::npos) || ((tagBegin + 1) >= text.length()))
                return "";
        }

        size_t tagNameLength = text.find_first_of(" />", tagBegin);
        if (tagNameLength == string::npos)
            return "";
        string tagName = text.substr(tagBegin + 1, (tagNameLength - tagBegin) - 1);


        //Check for self closing tag
        size_t tagEnd = text.find('>', tagBegin);
        if (tagEnd == string::npos)
            return "";

        if (text[tagEnd - 1] == '/')
        {
            *position = tagBegin;
            return text.substr(tagBegin, ++tagEnd - tagBegin);
        }

        tagBuffer.str(std::string());
        tagBuffer << "</" << tagName << ">";

        tagEnd = text.find(tagBuffer.str(), tagBegin);
        if (tagEnd == string::npos)
            return "";

        *position = tagBegin;
        return text.substr(tagBegin, (tagEnd + tagBuffer.str().length()) - tagBegin);
    }
    static string getNextXmlTag(string text, size_t* position, size_t offset)
    {
        string retvalue = getNextXmlTag(text.substr(offset), position);
        if (*position != string::npos)
            *position += offset;
        return retvalue;
    }
    static string getXmlTag(string text, std::string name, size_t* position)
    {
        *position = string::npos;
        stringstream tagBuffer;
        tagBuffer << "<" << name << " ";
        size_t tagBegin = text.find(tagBuffer.str());
        if (tagBegin == string::npos)
        {
            tagBuffer.str(string());
            tagBuffer << "<" << name << ">";
            tagBegin = text.find(tagBuffer.str());
            if (tagBegin == string::npos)
            {
                tagBuffer.str(string());
                tagBuffer << "<" << name << "/";
                tagBegin = text.find(tagBuffer.str());
                if (tagBegin == string::npos)
                {
                    return "";
                }
            }
        }

        //Check for self closing tag
        size_t tagEnd = text.find('>', tagBegin);
        if (tagEnd == string::npos)
            return "";

        if (text[tagEnd - 1] == '/')
        {
            *position = tagBegin;
            return text.substr(tagBegin, ++tagEnd - tagBegin);
        }

        tagBuffer.str(std::string());
        tagBuffer << "</" << name << ">";

        tagEnd = text.find(tagBuffer.str(), tagBegin);
        if (tagEnd == string::npos)
            return "";

        *position = tagBegin;
        return text.substr(tagBegin, (tagEnd + tagBuffer.str().length()) - tagBegin);
    }
    static string getXmlTag(string text, std::string name, size_t* position, size_t offset)
    {
        string retvalue = getXmlTag(text.substr(offset), name, position);
        if (*position != string::npos)
            *position += offset;
        return retvalue;
    }
    static string stripComments(string content)
    {        
        size_t comment = content.find("<!--");
        while (comment != string::npos)
        {
            size_t endComment = content.find("-->", comment);
            if (endComment == string::npos)
                return content;
            content = content.substr(0, comment) + content.substr(endComment + 3);
            comment = content.find("<!--");
        }
        return content;
    }
}


#endif