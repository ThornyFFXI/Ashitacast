#include "ashitacast.h"
#include "xmlHelpers.h"
#include <fstream>

#define BAD_NODE(a) "<!-- ATTN: Invalid node(" << a << ") found.  Node was not imported. -->\n"
#define BAD_ATTR(a, b) "<!-- ATTN: The node used to construct the following " << a << " node had an invalid attribute(" << b << ").  The attribute was discarded. -->\n"
#define MULTIPLE_ATTR(a, b) "<!-- ATTN: The node used to construct the following " << a << " node had multiple " << b << " attributes.  Only the first was imported. -->\n"

using namespace std;
using namespace XmlHelpers;

struct attribute_t
{
    string nodeName;
    string attrName;

    attribute_t(string nodeName, string attrName)
        : nodeName(nodeName)
        , attrName(attrName)
    {}

    bool operator==(const attribute_t& b)
    {
        return ((nodeName == b.nodeName) && (attrName == b.attrName));
    }

    bool operator==(const std::pair<xml_node<>*, xml_attribute<>*>& b)
    {
        return ((_stricmp(nodeName.c_str(), b.first->name()) == 0) && (_stricmp(attrName.c_str(), b.second->name()) == 0));
    }
};

struct conversion_t
{
    string oldString;
    string newString;

    conversion_t(string oldString, string newString)
        : oldString(oldString)
        , newString(newString)
    {}

    bool operator==(const string& b)
    {
        return (_stricmp(oldString.c_str(), b.c_str()) == 0);
    }
};

std::list<attribute_t> gAttributesWithVariables =
    {
        attribute_t("equip", "set"),
        attribute_t("main", "augment"),
        attribute_t("sub", "augment"),
        attribute_t("range", "augment"),
        attribute_t("ammo", "augment"),
        attribute_t("head", "augment"),
        attribute_t("neck", "augment"),
        attribute_t("lear", "augment"),
        attribute_t("ear1", "augment"),
        attribute_t("rear", "augment"),
        attribute_t("ear2", "augment"),
        attribute_t("body", "augment"),
        attribute_t("hands", "augment"),
        attribute_t("lring", "augment"),
        attribute_t("ring1", "augment"),
        attribute_t("rring", "augment"),
        attribute_t("ring2", "augment"),
        attribute_t("back", "augment"),
        attribute_t("waist", "augment"),
        attribute_t("legs", "augment"),
        attribute_t("feet", "augment"),
        attribute_t("setvar", "value"),
        attribute_t("change", "name")};

std::list<conversion_t> gBaseNodeConversion =
    {
        conversion_t("idlegear", "default"),
        conversion_t("preranged", "preranged"),
        conversion_t("midranged", "midranged"),
        conversion_t("premagic", "precast"),
        conversion_t("midmagic", "midcast"),
        conversion_t("jobability", "ability"),
        conversion_t("weaponskill", "weaponskill"),
        conversion_t("petskill", "petskill"),
        conversion_t("petspell", "petspell")};

std::list<conversion_t> gEquipNodeConversion =
    {
        conversion_t("main", "main"),
        conversion_t("sub", "sub"),
        conversion_t("range", "range"),
        conversion_t("ammo", "ammo"),
        conversion_t("head", "head"),
        conversion_t("neck", "neck"),
        conversion_t("lear", "ear1"),
        conversion_t("ear1", "ear1"),
        conversion_t("rear", "ear2"),
        conversion_t("ear2", "ear2"),
        conversion_t("body", "body"),
        conversion_t("hands", "hands"),
        conversion_t("lring", "ring1"),
        conversion_t("ring1", "ring1"),
        conversion_t("rring", "ring2"),
        conversion_t("ring2", "ring2"),
        conversion_t("back", "back"),
        conversion_t("waist", "waist"),
        conversion_t("legs", "legs"),
        conversion_t("feet", "feet")};

void ashitacast::performImport(std::string content, std::string destinationFile)
{
    char* tempCompleteFile = new char[content.length() + 1];
    strcpy_s(tempCompleteFile, content.length() + 1, content.c_str());

    xml_document<>* document = new xml_document<>();
    try
    {
        document->parse<0>(tempCompleteFile);
    }
    catch (const rapidxml::parse_error& e)
    {
        int line = static_cast<long>(std::count(tempCompleteFile, e.where<char>(), '\n') + 1);
        stringstream error;
        error << "Could not import XML.  Parse error[$H" << e.what() << "$R] at line " << line << ".";
        pOutput->error(error.str().c_str());
        delete[] tempCompleteFile;
        delete document;
        return;
    }
    catch (...)
    {
        pOutput->error("Could not import XML.  Unknown error during XML parsing.");
        delete[] tempCompleteFile;
        delete document;
        return;
    }

    pOutput->error("Document parsed.");

    xml_node<>* rootNode = document->first_node("ashitacast");
    if (!rootNode)
    {
        pOutput->error("Could not import XML.  Ashitacast node was not found.");
        delete[] tempCompleteFile;
        delete document;
        return;
    }

    ofstream fileOut = ofstream(destinationFile.c_str(), ios::binary);
    if (!fileOut.is_open())
    {
        pOutput->error_f("Failed to create new XML to hold import data.  Could not create file. [$H%s$R]", destinationFile.c_str());
        delete[] tempCompleteFile;
        delete document;
        return;
    }

    stringstream output;
    output << "<!-- Automatically converted file for use with Ashitacast For Ashita4. -->\n";
    output << "<!-- Prior to using, please ctrl-f for '<!-- ATTN:' and review all notes. -->\n\n";
    output << "<ashitacast>\n\n";

    xml_node<>* setsNode = rootNode->first_node("sets");
    if (setsNode)
    {
        checkAttributes(setsNode, "sets", std::list<string>(), &output, 1);

        output << "\t<sets>\n";
        output << importSets(setsNode);
        output << "\t</sets>\n\n";
    }

    xml_node<>* commandsNode = rootNode->first_node("inputcommands");
    if (commandsNode)
    {
        checkAttributes(commandsNode, "commands", std::list<string>(), &output, 1);

        output << "\t<commands>\n";
        output << importCommands(commandsNode);
        output << "\t</commands>\n\n";
    }

    xml_node<>* initNode = rootNode->first_node("init");
    xml_node<>* varsNode = rootNode->first_node("variables");
    if ((initNode) || (varsNode))
    {
        if (initNode)
        {
            checkAttributes(initNode, "load", std::list<string>(), &output, 1);
        }
        if (varsNode)
        {
            checkAttributes(varsNode, "load", std::list<string>(), &output, 1);
        }

        output << "\t<load>\n";
        if (varsNode)
        {
            output << importVariables(varsNode);
            pOutput->error("Variables parsed.");
        }
        if (initNode)
        {
            output << importFlow("load", initNode, 2);
            pOutput->error("Load parsed.");
        }
        output << "\t</load>\n\n";
    }

    for (std::list<conversion_t>::iterator iter = gBaseNodeConversion.begin(); iter != gBaseNodeConversion.end(); iter++)
    {
        xml_node<>* node = rootNode->first_node((*iter).oldString.c_str());
        if (node)
        {
            checkAttributes(node, (*iter).newString, std::list<string>(), &output, 1);

            output << "\t<" << (*iter).newString << ">\n";
            output << importFlow((*iter).newString, node, 2);
            output << "\t</" << (*iter).newString << ">\n\n";
        }
    }

    output << "</ashitacast>";
    fileOut << output.str();
    fileOut.close();
    pOutput->message_f("XML import complete. [$H%s$R]", destinationFile.c_str());
    delete document;
}

string ashitacast::importCommands(xml_node<>* baseNode)
{
    stringstream output;
    for (xml_node<>* node = baseNode->first_node(); node; node = node->next_sibling())
    {
        if (_stricmp(node->name(), "cmd") == 0)
        {
            output << importCmdNode(node);
        }
        else
        {
            output << "\t\t" << BAD_NODE(node->name());
        }
    }
    return output.str();
}
string ashitacast::importCmdNode(xml_node<>* node)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "command", std::list<string>{"input"}, &output, 2);

    std::map<string, xml_attribute<>*>::iterator input = attrMap.find("input");
    if (input != attrMap.end())
    {
        output << "\t\t<command input=\"" << input->second->value() << "\">\n";
        output << importFlow("commands", node, 3);
        output << "\t\t</command>\n";
        return output.str();
    }
    else
    {
        return "\t\t<!-- ATTN: Cmd node found without an input attribute.  Node was not imported. -->\n";
    }
}

string ashitacast::importSets(xml_node<>* baseNode)
{
    stringstream output;
    for (xml_node<>* node = baseNode->first_node(); node; node = node->next_sibling())
    {
        if (_stricmp(node->name(), "set") == 0)
        {
            output << importSetNode(node);
        }
        else if (_stricmp(node->name(), "include") == 0)
        {
            output << importIncludeNode(node);
        }
        else
        {
            output << "\t\t" << BAD_NODE(node->name());
        }
    }
    return output.str();
}
string ashitacast::importSetNode(xml_node<>* baseNode)
{
    xml_attribute<>* name = baseNode->first_attribute("name");
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(baseNode, "set", std::list<string>{"name", "baseset", "lock", "priority"}, &output, 2);

    std::map<string, xml_attribute<>*>::iterator attr = attrMap.find("lock");
    if (attr != attrMap.end())
    {
        output << "\t\t<!-- ATTN: The following set node had a lock attribute.  Lock attributes can no longer be used on a per-set basis, so it was removed. -->\n";
    }
    attr = attrMap.find("priority");
    if (attr != attrMap.end())
    {
        output << "\t\t<!-- ATTN: The following set node had a priority attribute.  Priority attributes can no longer be used on a per-set basis, so it was removed. -->\n";
    }

    attr = attrMap.find("name");
    if (attr == attrMap.end())
    {
        return "\t\t<!-- ATTN: Sets node found without a name attribute.  Node was not imported. -->\n";
    }

    output << "\t\t<set name=\"" << attr->second->value() << "\"";

    attr = attrMap.find("baseset");
    if (attr != attrMap.end())
        output << " baseset=\"" << attr->second->value() << "\"";
    output << ">\n";

    for (xml_node<>* equip = baseNode->first_node(); equip; equip = equip->next_sibling())
    {
        std::list<conversion_t>::iterator iter = std::find(gEquipNodeConversion.begin(), gEquipNodeConversion.end(), string(equip->name()));

        if (iter == gEquipNodeConversion.end())
        {
            output << "\t\t\t" << BAD_NODE(equip->name());
            continue;
        }

        else
        {
            output << importEquipSetSlotNode((*iter).newString, equip);
        }
    }

    output << "\t\t</set>\n";
    return output.str();
}
string ashitacast::importIncludeNode(xml_node<>* baseNode)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(baseNode, "packer", std::list<string>(), &output, 2);

    output << "\t\t<packer>\n";
    for (xml_node<>* equip = baseNode->first_node(); equip; equip = equip->next_sibling())
    {
        if (_stricmp(equip->name(), "item") == 0)
        {
            output << importItemNode(equip);
        }
        else
        {
            output << "\t\t\t" << BAD_NODE(equip->name());
        }
    }

    output << "\t\t</packer>\n";
    return output.str();
}
string ashitacast::importEquipSetSlotNode(string slotName, xml_node<>* node)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(node, slotName, std::list<string>{"augment", "lock", "priority"}, &output, 3);

    IItem* pItem = m_AshitaCore->GetResourceManager()->GetItemByName(node->value(), 0);

    //If no item resource, check if item has variables in it..
    if ((!pItem) && (strstr(node->value(), "$") == NULL) && (strstr(node->value(), "%") == NULL))
    {
        //If no item resource, and item doesn't have variables in it, check if it's a keyword..
        if ((_stricmp(node->value(), "displaced") != 0) && (_stricmp(node->value(), "remove") != 0) && (_stricmp(node->value(), "ignore") != 0))
            //Item is invalid, so error instead of making a new node.
            return "\t\t\t<!-- ATTN: " + string(node->name()) + " node had an invalid piece of equipment(" + string(node->value()) + ").  Node was not imported. -->\n";
    }

    pOutput->error("Checkpoint 1");
    string augmentString                              = " augment=\"unknown\"";
    string commentString                              = "";
    std::map<string, xml_attribute<>*>::iterator attr = attrMap.find("augment");
    if (attr != attrMap.end())
    {
        if ((!pItem) || (!resolveAugment(&augmentString, pItem->Name[0], attr->second->value(), &commentString)))
        {
            output << "\t\t\t<!-- ATTN: The node used to construct the following " << slotName << " node had augments that could not be resolved. -->\n";
        }
    }

    pOutput->error("Checkpoint 2");

    output << "\t\t\t<" << slotName;

    if (attr != attrMap.end())
        output << augmentString;

    attr = attrMap.find("priority");
    if (attr != attrMap.end())
        output << " priority=\"" << attr->second->value() << "\"";

    attr = attrMap.find("lock");
    if (attr != attrMap.end())
        output << "lock=\"" << attr->second->value() << "\"";

    if (pItem)
        output << ">" << pItem->Name[0] << "</" << slotName << ">";
    else
    {
        string item = node->value();
        item        = replace(item, "$", "$u_");
        item        = replace(item, "%", "$");
        output << ">" << item << "</" << slotName << ">";
    }
    output << commentString << '\n';

    pOutput->error("Checkpoint 3");

    return output.str();
}
string ashitacast::importItemNode(xml_node<>* node)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "item", std::list<string>{"augment", "quantity"}, &output, 3);

    IItem* pItem = m_AshitaCore->GetResourceManager()->GetItemByName(node->value(), 0);
    if (!pItem)
    {
        return "\t\t\t<!-- ATTN: Item node had an invalid item(" + string(node->value()) + ").  Node was not imported. -->\n";
    }

    string augmentString = " augment=\"unknown\"";
    string commentString = "";

    std::map<string, xml_attribute<>*>::iterator attr = attrMap.find("augment");
    if (attr != attrMap.end())
    {
        if (!resolveAugment(&augmentString, pItem->Name[0], attr->second->value(), &commentString))
        {
            output << "\t\t\t<!-- ATTN: The node used to construct the following item node had augments that could not be resolved. -->\n";
        }
    }

    output << "\t\t\t<item";
    if (attr != attrMap.end())
        output << augmentString;

    attr = attrMap.find("quantity");
    if (attr != attrMap.end())
        output << " quantity=\"" << attr->second->value() << "\"";

    output << ">" << pItem->Name[0] << "</item>";
    output << commentString << '\n';

    return output.str();
}

string ashitacast::importVariables(xml_node<>* baseNode)
{
    stringstream output;
    for (xml_node<>* node = baseNode->first_node(); node; node = node->next_sibling())
    {
        if (_stricmp(node->name(), "var") == 0)
        {
            map<string, xml_attribute<>*> attrMap = checkAttributes(node, "setvar", std::list<string>{"name"}, &output, 2);

            std::map<string, xml_attribute<>*>::iterator attr = attrMap.find("name");
            if (attr != attrMap.end())
            {
                output << "\t\t<setvar name=\"" << attr->second->value() << "\" value=\"" << node->value() << "\" />\n";
            }
            else
            {
                output << "\t\t<!-- ATTN: The variables section used to populate this area has a var node without a name attribute.  Node was not imported.";
            }
        }
        else
        {
            output << "\t\t<!-- ATTN: The variables section used to populate this area has an invalid node(" << node->name() << ").  Node was not imported.";
        }
    }
    return output.str();
}
string ashitacast::importFlow(string section, xml_node<>* baseNode, int depth)
{
    stringstream output;

    for (xml_node<>* node = baseNode->first_node(); node; node = node->next_sibling())
    {
        std::map<string, importHandler>::iterator find = mImportMap.find(string(node->name()));
        if (find != mImportMap.end())
        {
            output << (this->*(find->second))(section, node, depth);
        }
        else
        {
            output << std::string(depth, '\t') << BAD_NODE(node->name());
        }
    }

    return output.str();
}
string ashitacast::importIfNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    output << importConditions(node, depth, &output);
    output << importFlow(section, node, depth + 1);
    output << std::string(depth, '\t') << "</if>\n";

    return output.str();
}
string ashitacast::importElseIfNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    output << importConditions(node, depth, &output);
    output << importFlow(section, node, depth + 1);
    output << std::string(depth, '\t') << "</elseif>\n";

    return output.str();
}
string ashitacast::importElseNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    checkAttributes(node, "else", std::list<string>(), &output, depth);

    output << std::string(depth, '\t') << "<else>\n";
    output << importFlow(section, node, depth + 1);
    output << std::string(depth, '\t') << "</else>\n";

    return output.str();
}
string ashitacast::importCommandNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "command", std::list<string>{"delay"}, &output, depth);

    output << std::string(depth, '\t') << "<command";

    std::map<string, xml_attribute<>*>::iterator attr = attrMap.find("delay");
    if (attr != attrMap.end())
        output << " delay=\"" << attr->second->value() << "\"";

    output << ">";

    string command = node->value();
    command        = replace(command, "$", "$u_");
    command        = replace(command, "%", "$");
    output << command << "</command>\n";

    return output.str();
}
string ashitacast::importClearVarsNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    checkAttributes(node, "clearvar", std::list<string>(), &output, depth);

    output << std::string(depth, '\t') << "<clearvar all=\"true\" />\n";

    return output.str();
}
string ashitacast::importSetVarNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "setvar", std::list<string>{"name", "value"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator name  = attrMap.find("name");
    std::map<string, xml_attribute<>*>::iterator value = attrMap.find("value");

    if (name == attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A setvar node was found without a name attribute.  It was not imported. -->\n";

    if (value == attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A setvar node was found without a value attribute.  It was not imported. -->\n";

    output << std::string(depth, '\t') << "<setvar name=\"" << name->second->value() << "\" value=\"" << value->second->value() << "\" />\n";
    return output.str();
}
string ashitacast::importIncVarNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "setvar", std::list<string>{"name", "value"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator name  = attrMap.find("name");
    std::map<string, xml_attribute<>*>::iterator value = attrMap.find("value");

    if (name == attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A incvar node was found without a name attribute.  It was not imported. -->\n";

    output << std::string(depth, '\t') << "<setvar name=\"" << name->second->value() << "\" value=\"";
    output << "$u_" << name->second->value() << " + ";

    if (value == attrMap.end())
        output << "1";
    else
        output << value->second->value();
    output << "\" />\n";

    return output.str();
}
string ashitacast::importDecVarNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "setvar", std::list<string>{"name", "value"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator name  = attrMap.find("name");
    std::map<string, xml_attribute<>*>::iterator value = attrMap.find("value");

    if (name == attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A decvar node was found without a name attribute.  It was not imported. -->\n";

    output << std::string(depth, '\t') << "<setvar name=\"" << name->second->value() << "\" value=\"";
    output << "$u_" << name->second->value() << " - ";

    if (value == attrMap.end())
        output << "1";
    else
        output << value->second->value();
    output << "\" />\n";

    return output.str();
}
string ashitacast::importChangeNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "change", std::list<string>{"id", "type", "name"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator id   = attrMap.find("id");
    std::map<string, xml_attribute<>*>::iterator type = attrMap.find("type");
    std::map<string, xml_attribute<>*>::iterator name = attrMap.find("name");

    if (type != attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A change node was found with a type attribute.  It was not imported, as ACv4 does not support type changes. -->\n";

    if ((id == attrMap.end()) && (name == attrMap.end()))
        return std::string(depth, '\t') + "<!-- ATTN: A change node was found with neither an ID nor a name attribute.  It was not imported, as one is required. -->\n";

    output << std::string(depth, '\t') << "<change";
    if (id != attrMap.end())
        output << " id=\"" << id->second->value() << "\"";
    if (name != attrMap.end())
        output << " name=\"" << name->second->value() << "\"";
    output << " />\n";

    return output.str();
}
string ashitacast::importAddToChatNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "echo", std::list<string>{"color"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator color = attrMap.find("color");

    output << std::string(depth, '\t') << "<echo";
    if (color != attrMap.end())
        output << " mode=\"" << color->second->value() << "\"";
    output << ">";

    string text = node->value();
    text        = replace(text, "$", "$u_");
    text        = replace(text, "%", "$");
    output << text << "</echo>\n";

    return output.str();
}
string ashitacast::importAddToEchoNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "echo", std::list<string>{"color"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator color = attrMap.find("color");

    output << std::string(depth, '\t') << "<echo";
    if (color != attrMap.end())
        output << " color=\"" << color->second->value() << "\"";
    output << ">";

    string text = node->value();
    text        = replace(text, "$", "$u_");
    text        = replace(text, "%", "$");
    output << text << "</echo>\n";

    return output.str();
}
string ashitacast::importGearLockNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "gearlock", std::list<string>{"length"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator length = attrMap.find("length");

    output << std::string(depth, '\t') << "<gearlock";
    if (length != attrMap.end())
    {
        float value = (atof(length->second->value()) * 1000.0f);
        int outval  = (int)floor(value);
        output << " length=\"" << outval << "\"";
    }
    output << " />\n";

    return output.str();
}
string ashitacast::importCancelNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    checkAttributes(node, "cancel", std::list<string>(), &output, depth);
    output << std::string(depth, '\t') << "<cancel />\n";

    return output.str();
}
string ashitacast::importReturnNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    checkAttributes(node, "return", std::list<string>(), &output, depth);
    output << std::string(depth, '\t') << "<return />\n";

    return output.str();
}
string ashitacast::importDoIdleGearNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    output << std::string(depth, '\t') << "<!-- ATTN: A doidlegear node was here.  Doidlegear is no longer a thing, simply update your variables and wait for default section to reparse.  No speed is lost. -->\n";

    return output.str();
}
string ashitacast::importRegisterBuffNode(string section, xml_node<>* node, int depth)
{
    stringstream output;
    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "registerbuff", std::list<string>{"id", "active"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator id     = attrMap.find("id");
    std::map<string, xml_attribute<>*>::iterator active = attrMap.find("active");

    if (id == attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A registerbuff node was found with no ID attribute.  It was not imported, as an ID attribute is required. -->\n";

    output << std::string(depth, '\t') << "<registerbuff id=\"";
    output << id->second->value() << "\"";

    if (active != attrMap.end())
        output << " active=\"" << active->second->value() << "\"";
    output << " />\n";

    return output.str();
}
string ashitacast::importClearBuffNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "clearbuff", std::list<string>{"id"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator id = attrMap.find("id");

    if (id == attrMap.end())
        return std::string(depth, '\t') + "<!-- ATTN: A clearbuff node was found with no ID attribute.  It was not imported, as an ID attribute is required. -->\n";

    output << std::string(depth, '\t') << "<clearbuff id=\"";
    output << id->second->value() << "\"";
    output << " />\n";

    return output.str();
}
string ashitacast::importEquipNode(string section, xml_node<>* node, int depth)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(node, "equip", std::list<string>{"set", "priority", "lock"}, &output, depth);

    std::map<string, xml_attribute<>*>::iterator set      = attrMap.find("set");
    std::map<string, xml_attribute<>*>::iterator priority = attrMap.find("priority");
    std::map<string, xml_attribute<>*>::iterator lock     = attrMap.find("lock");

    output << std::string(depth, '\t') << "<equip";
    if (set != attrMap.end())
    {
        string text = set->second->value();
        text        = replace(text, "$", "$u_");
        text        = replace(text, "%", "$");
        output << " set=\"" << text << "\"";
    }

    if (priority != attrMap.end())
        output << " priority=\"" << priority->second->value() << "\"";
    if (lock != attrMap.end())
        output << " lock=\"" << lock->second->value() << "\"";

    if (node->first_node())
    {
        output << ">\n";

        for (xml_node<>* equip = node->first_node(); equip; equip = equip->next_sibling())
        {
            conversion_t* equipNodeName = NULL;
            for (std::list<conversion_t>::iterator iter = gEquipNodeConversion.begin(); iter != gEquipNodeConversion.end(); iter++)
            {
                if (_stricmp(equip->name(), (*iter).oldString.c_str()) == 0)
                {
                    equipNodeName = &(*iter);
                    break;
                }
            }

            if (!equipNodeName)
            {
                output << std::string(depth + 1, '\t') << BAD_NODE(equip->name());
                continue;
            }

            else
            {
                output << importEquipSlotNode(section, equipNodeName->newString, equip, depth + 1);
            }
        }

        output << std::string(depth, '\t') << "</equip>\n";
    }
    else
    {
        output << " />\n";
    }

    return output.str();
}
string ashitacast::importEquipSlotNode(string section, string slotName, xml_node<>* node, int depth)
{
    stringstream output;

    map<string, xml_attribute<>*> attrMap = checkAttributes(node, slotName, std::list<string>{"augment", "lock", "priority"}, &output, depth);

    IItem* pItem = m_AshitaCore->GetResourceManager()->GetItemByName(node->value(), 0);

    //If no item resource, check if item has variables in it..
    if ((!pItem) && (strstr(node->value(), "$") == NULL) && (strstr(node->value(), "%") == NULL))
    {
        //If no item resource, and item doesn't have variables in it, check if it's a keyword..
        if ((_stricmp(node->value(), "displaced") != 0) && (_stricmp(node->value(), "remove") != 0) && (_stricmp(node->value(), "ignore") == 0))
            //Item is invalid, so error instead of making a new node.
            return "\t\t\t<!-- ATTN: " + string(node->name()) + " node had an invalid piece of equipment(" + string(node->value()) + ").  Node was not imported. -->\n";
    }

    string augmentString                              = " augment=\"unknown\"";
    string commentString                              = "";
    std::map<string, xml_attribute<>*>::iterator attr = attrMap.find("augment");
    if (attr != attrMap.end())
    {
        if ((!pItem) || (!resolveAugment(&augmentString, pItem->Name[0], attr->second->value(), &commentString)))
        {
            output << std::string(depth, '\t') << "<!-- ATTN: The node used to construct the following " << slotName << " node had augments that could not be resolved. -->\n";
        }
    }

    output << std::string(depth, '\t') << "<" << slotName;

    if (attr != attrMap.end())
        output << augmentString;

    attr = attrMap.find("priority");
    if (attr != attrMap.end())
        output << " priority=\"" << attr->second->value() << "\"";

    attr = attrMap.find("lock");
    if (attr != attrMap.end())
        output << "lock=\"" << attr->second->value() << "\"";

    if (pItem)
        output << ">" << pItem->Name[0] << "</" << slotName << ">";
    else
    {
        string item = node->value();
        item        = replace(item, "$", "$u_");
        item        = replace(item, "%", "$");
        output << ">" << item << "</" << slotName << ">";
    }
    output << commentString << '\n';

    return output.str();
}
string ashitacast::importConditions(xml_node<>* node, int depth, stringstream* baseOutput)
{
    stringstream output;

    output << std::string(depth, '\t');
    if (_stricmp(node->name(), "if") == 0)
        output << "<if";
    else
        output << "<elseif";

    for (xml_attribute<>* attr = node->first_attribute(); attr; attr = attr->next_attribute())
    {
        if (_stricmp(attr->name(), "buffactive") == 0)
        {
            output << importBuffActive(attr, depth, baseOutput);
        }
        else
        {
            output << " " << attr->name() << "=\"";
            string value = attr->value();
            value        = replace(value, "$", "$u_");
            value        = replace(value, "%", "$");
            output << value << "\"";
        }
    }
    output << ">\n";

    return output.str();
}

string ashitacast::importBuffActive(xml_attribute<>* attr, int depth, stringstream* baseOutput)
{
    stringstream output;

    string rawString = attr->value();
    std::vector<string> rawStrings;
    size_t location = rawString.find_first_of("|&");
    while (location != string::npos)
    {
        rawStrings.push_back(rawString.substr(0, location));
        rawString = rawString.substr(location);
        location  = rawString.find_first_of("|&", 1);
    }
    rawStrings.push_back(rawString);

    if (rawStrings.size() == 1)
    {
        bool negate = false;
        if (rawString[0] == '!')
        {
            negate    = true;
            rawString = rawString.substr(1);
        }

        int count    = 1;
        size_t lastX = rawString.find_last_of('x');
        if (lastX != string::npos)
        {
            bool isCount = true;
            for (int x = lastX + 1; x < rawString.length(); x++)
            {
                if (!isdigit(rawString[x]))
                {
                    isCount = false;
                    break;
                }
            }

            if (isCount)
            {
                count     = atoi(rawString.substr(lastX + 1).c_str());
                rawString = rawString.substr(0, lastX);
            }
        }

        output << " b_";

        if (rawString.find(' ') != string::npos)
        {
            *baseOutput << std::string(depth, '\t') << "<!-- ATTN: The buffactive attribute used to construct the following node used a buff name with a space in it.  Please convert it to buff ID. -->\n";
        }

        output << rawString;
        output << "=\"";

        if (negate)
            output << "<";
        else
            output << ">=";

        output << count;
        output << "\"";
    }
    else
    {
        *baseOutput << std::string(depth, '\t') << "<!-- ATTN: The buffactive attribute used to construct the following node was using '|' and &'.  These are ambiguous, please review the new node for accuracy. -->\n";
        output << " advanced=\"";

        for (std::vector<string>::iterator iter = rawStrings.begin(); iter != rawStrings.end(); iter++)
        {
            if (((*iter)[0] == '|') || ((*iter)[0] == '&'))
            {
                output << (*iter)[0];
                (*iter) = (*iter).substr(1);
            }
            output << "($b_";

            bool negate = false;
            if ((*iter)[0] == '!')
            {
                negate  = true;
                (*iter) = (*iter).substr(1);
            }

            int count    = 1;
            size_t lastX = (*iter).find_last_of('x');
            if (lastX != string::npos)
            {
                bool isCount = true;
                for (int x = lastX + 1; x < (*iter).length(); x++)
                {
                    if (!isdigit((*iter)[x]))
                    {
                        isCount = false;
                        break;
                    }
                }

                if (isCount)
                {
                    count   = atoi((*iter).substr(lastX + 1).c_str());
                    (*iter) = (*iter).substr(0, lastX);
                }
            }

            output << *iter;
            if (negate)
                output << "<";
            else
                output << ">=";

            output << count;
            output << ")";
        }
        output << "\"";
    }

    return output.str();
}
map<string, xml_attribute<>*> ashitacast::checkAttributes(xml_node<>* node, string nodeName, std::list<string> validAttributes, stringstream* output, int depth)
{
    map<string, xml_attribute<>*> resultMap;

    for (xml_attribute<>* attr = node->first_attribute(); attr; attr = attr->next_attribute())
    {
        bool isvalid = false;
        for (std::list<string>::iterator iter = validAttributes.begin(); iter != validAttributes.end(); iter++)
        {
            if (_stricmp(attr->name(), (*iter).c_str()) == 0)
            {
                isvalid = true;

                if (resultMap.find(*iter) == resultMap.end())
                {
                    resultMap[*iter] = attr;
                    if (strstr(attr->value(), "%") || (strstr(attr->value(), "$")))
                    {
                        if (std::find(gAttributesWithVariables.begin(), gAttributesWithVariables.end(), std::make_pair(node, attr)) == gAttributesWithVariables.end())
                        {
                            *output << std::string(depth, '\t') << "<!-- ATTN: The node used to construct the following " << nodeName << " node had variables in the " << attr->name() << " attribute.  Ashitacast4 doesn't support variables in this attribute, so they will not be resolved. -->\n";
                        }
                    }
                }
                else
                    *output << std::string(depth, '\t') << MULTIPLE_ATTR(nodeName, *iter);
            }
        }
        if (!isvalid)
        {
            *output << std::string(depth, '\t') << BAD_ATTR(nodeName, attr->name());
        }
    }

    return resultMap;
}

bool ashitacast::resolveAugment(string* augmentString, const char* itemName, const char* augmentCode, string* comment)
{
    IInventory* pInv = m_AshitaCore->GetMemoryManager()->GetInventory();

    for (int x = 0; x < 13; x++)
    {
        if (x == 3)
            continue;

        uint32_t containerMax = pInv->GetContainerCountMax(x);
        if (containerMax == 4294967295)
            continue;

        for (int y = 1; y < containerMax; y++)
        {
            if (x == 5)
                break;
            Ashita::FFXI::item_t* item = pInv->GetContainerItem(x, y);
            if ((item == NULL) || (item->Id < 1) || (item->Count < 1))
                continue;

            IItem* resource = m_AshitaCore->GetResourceManager()->GetItemById(item->Id);
            if (!resource)
                continue;

            if (_stricmp(resource->Name[0], itemName) == 0)
            {
                if (getLegacyAugment(item->Extra) == string(augmentCode))
                {
                    augmentData_t augment = createAugmentData(item);
                    if (augment.path != 0)
                    {
                        stringstream output;
                        output << " augpath=\"";
                        output << (char)(augment.path + 64);
                        output << "\"";
                        *augmentString = output.str();

                        if (augment.stringAugments.size() > 0)
                        {
                            stringstream makeComment;
                            makeComment << " <!--";
                            for (vector<string>::iterator it = augment.stringAugments.begin(); it != augment.stringAugments.end(); it++)
                            {
                                if (it != augment.stringAugments.begin())
                                    makeComment << ',';
                                makeComment << *it;
                            }
                            makeComment << "-->";
                            *comment = makeComment.str();
                        }
                        return true;
                    }
                    else if (augment.stringAugments.size() > 0)
                    {
                        stringstream output;
                        output << " augment=\"";
                        for (vector<string>::iterator it = augment.stringAugments.begin(); it != augment.stringAugments.end(); it++)
                        {
                            if (it != augment.stringAugments.begin())
                                output << ',';
                            output << *it;
                        }
                        output << '\"';
                        *augmentString = output.str();
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
string ashitacast::getLegacyAugment(unsigned char Input[])
{
    if (Input[0] == 2) //Augmented Item
    {
        if (Input[1] == 3) //Standard Augment
        {
            string Builder = "S";
            Builder += to_string(Ashita::BinaryData::UnpackBitsBE(Input, 2, 0, 48));
            int Value = Ashita::BinaryData::UnpackBitsBE(Input, 8, 0, 32);
            if (Value > 0)
                Builder += to_string(Value);
            return Builder;
        }
        else if (Input[1] == 67) //Magian Augment
        {
            string Builder = "M";
            Builder += to_string(Ashita::BinaryData::UnpackBitsBE(Input, 6, 0, 32));
            return Builder;
        }
        else if (Input[1] == 35) //Delve Augment
        {
            string Builder = "D";
            Builder += to_string(Ashita::BinaryData::UnpackBitsBE(Input, 6, 0, 48));
            return Builder;
        }
    }
    else if (Input[0] == 3) //Dynamis Augments
    {
        string Builder = "O";
        Builder += to_string(Ashita::BinaryData::UnpackBitsBE(Input, 2, 0, 48));
        int Value = Ashita::BinaryData::UnpackBitsBE(Input, 8, 0, 32);
        if (Value > 0)
            Builder += to_string(Value);
        return Builder;
    }
    return "X";
}
string ashitacast::replace(string text, string find, string replace)
{
    size_t offset   = 0;
    size_t location = text.find(find);
    while (location != string::npos)
    {
        text     = text.replace(location, find.length(), replace);
        offset   = location + replace.length();
        location = text.find(find, offset);
    }
    return text;
}