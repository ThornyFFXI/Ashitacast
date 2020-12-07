#include "ashitacast.h"
#include "thirdparty\rapidxml.hpp"
#include <fstream>
#include <stdio.h>

using namespace rapidxml;

void ashitacast::initHandlerMaps()
{
    commandCombo combo;

    combo.handler          = &ashitacast::handleImport;
    combo.help.command     = "/ac import [required: filename]";
    combo.help.description = "Attempt to convert an Acv3 XML to Acv4. Filename must be in the format Charname_JOB_v3.xml. Converted XML will be created as Charname_JOB.xml. For more details, see quickstart.html in the ashita/docs/ashitacast folder.";
    mCommandMap.insert(std::make_pair("import", combo));

    combo.handler          = &ashitacast::handleLoad;
    combo.help.command     = "/ac load [optional: filename]";
    combo.help.description = "Attempt to load an XML.  Ashitacast will first check for an absolute path, then check ashita/config/ashitacast.  If you do not specify filename, ashitacast will use your current CharacterName_JOB.xml.";
    mCommandMap.insert(std::make_pair("load", combo));

    combo.handler          = &ashitacast::handleReload;
    combo.help.command     = "/ac reload";
    combo.help.description = "Reload the currently loaded XML from hard drive, to reflect any changes made.";
    mCommandMap.insert(std::make_pair("reload", combo));

    combo.handler          = &ashitacast::handleUnload;
    combo.help.command     = "/ac unload";
    combo.help.description = "Unload the currently loaded XML, resulting in no XML loaded.";
    mCommandMap.insert(std::make_pair("unload", combo));

    combo.handler          = &ashitacast::handleNaked;
    combo.help.command     = "/ac naked";
    combo.help.description = "Remove all equipment and disable all slots.";
    mCommandMap.insert(std::make_pair("naked", combo));

    combo.handler          = &ashitacast::handleSet;
    combo.help.command     = "/ac set [required: set name] [optional: duration]";
    combo.help.description = "Equip a set from your current XML manually.  It will be locked in for the specified duration, in milliseconds.  If duration is not specified, it will be locked in for 5000ms(5 seconds).";
    mCommandMap.insert(std::make_pair("set", combo));

    combo.handler          = &ashitacast::handleEnable;
    combo.help.command     = "/ac enable [optional: slot]";
    combo.help.description = "Enable a slot that has been disabled so that it will be swapped again.  If no slot is specified, all slots will be enabled.";
    mCommandMap.insert(std::make_pair("enable", combo));

    combo.handler          = &ashitacast::handleDisable;
    combo.help.command     = "/ac disable [optional: slot]";
    combo.help.description = "Temporarily disable a slot from being swapped by ashitacast.  If no slot is specified, all slots will be disabled.";
    mCommandMap.insert(std::make_pair("disable", combo));

    combo.handler          = &ashitacast::handleHelp;
    combo.help.command     = "/ac help [optional: command]";
    combo.help.description = "Print information on a command.  If no command is specified, a list of commands will be printed.";
    mCommandMap.insert(std::make_pair("help", combo));

    combo.handler          = &ashitacast::handlePrint;
    combo.help.command     = "/ac print [required: vars|uvars]";
    combo.help.description = "If vars is specified, print a list of all currently accessible integrated variables and their values.  If uvars is specified, print a list of all user-defined variables and their current values.";
    mCommandMap.insert(std::make_pair("print", combo));

    combo.handler          = &ashitacast::handleAddSet;
    combo.help.command     = "/ac addset [required: set name]";
    combo.help.description = "Add your currently equipped armor to your XML as a set.  If a set with the listed name already exists, it will be replaced.  Your XML will be reloaded afterwards, so the set will take effect immediately.";
    mCommandMap.insert(std::make_pair("addset", combo));

    combo.handler          = &ashitacast::handleNewXml;
    combo.help.command     = "/ac newxml [optional: filename]";
    combo.help.description = "Create a new empty XML in ashita/config/ashitacast/.  If filename is not specified, ashitacast will use your current CharacterName_JOB.xml.  Will fail if the file already exists.";
    mCommandMap.insert(std::make_pair("newxml", combo));

    combo.handler          = &ashitacast::handleSetVar;
    combo.help.command     = "/ac setvar [required: name] [required: value]";
    combo.help.description = "Set a variable.  You may use math and reference other variables in value, by the same rules as advanced tags.";
    mCommandMap.insert(std::make_pair("setvar", combo));

    combo.handler          = &ashitacast::handleClearVar;
    combo.help.command     = "/ac clearvar [required: name]";
    combo.help.description = "Clear a variable.";
    mCommandMap.insert(std::make_pair("clearvar", combo));

    combo.handler          = &ashitacast::handleClearVars;
    combo.help.command     = "/ac clearvars";
    combo.help.description = "Clear all variables.";
    mCommandMap.insert(std::make_pair("clearvars", combo));

    combo.handler          = &ashitacast::handleDebug;
    combo.help.command     = "/ac debug [optional: on/off]";
    combo.help.description = "Set debug mode.  When debug mode is enabled, all swaps will be printed to the chat log.  If on or off is not specified, mode will toggle.";
    mCommandMap.insert(std::make_pair("debug", combo));

    combo.handler          = &ashitacast::handleValidate;
    combo.help.command     = "/ac validate";
    combo.help.description = "Use the plugin Packer to confirm all equipment in your sets section is currently in your equip-friendly bags.";
    mCommandMap.insert(std::make_pair("validate", combo));

    combo.handler          = &ashitacast::handleGear;
    combo.help.command     = "/ac gear";
    combo.help.description = "Use the plugin Packer to collect all equipment in your sets section according to your configured settings in Packer.  Note that all slots will be disabled while Packer executes, and if you have 'nakedforpacker' enabled they will also be emptied.  In both cases, slots will be enabled as soon as Packer completes.";
    mCommandMap.insert(std::make_pair("gear", combo));

    combo.handler          = &ashitacast::handleBench;
    combo.help.command     = "/ac bench";
    combo.help.description = "Benchmark command for developer use only.";
    mCommandMap.insert(std::make_pair("bench", combo));

    combo.handler          = NULL;
    combo.help.command     = "/ac eval [Content]";
    combo.help.description = "Sends content back through the command handler with variables resolved.  See readme for more details and an example.";
    mCommandMap.insert(std::make_pair("eval", combo));

    mNodeMap.insert(std::make_pair("command", &ashitacast::processCommand));
    mNodeMap.insert(std::make_pair("setvar", &ashitacast::processSetVar));
    mNodeMap.insert(std::make_pair("clearvar", &ashitacast::processClearVar));
    mNodeMap.insert(std::make_pair("change", &ashitacast::processChange));
    mNodeMap.insert(std::make_pair("echo", &ashitacast::processEcho));
    mNodeMap.insert(std::make_pair("gearlock", &ashitacast::processGearLock));
    mNodeMap.insert(std::make_pair("registerbuff", &ashitacast::processRegisterBuff));
    mNodeMap.insert(std::make_pair("clearbuff", &ashitacast::processClearBuff));
    mNodeMap.insert(std::make_pair("cancel", &ashitacast::processCancel));
    mNodeMap.insert(std::make_pair("override", &ashitacast::processOverride));

    mImportMap.insert(std::make_pair("if", &ashitacast::importIfNode));
    mImportMap.insert(std::make_pair("elseif", &ashitacast::importElseIfNode));
    mImportMap.insert(std::make_pair("else", &ashitacast::importElseNode));
    mImportMap.insert(std::make_pair("command", &ashitacast::importCommandNode));
    mImportMap.insert(std::make_pair("clearvars", &ashitacast::importClearVarsNode));
    mImportMap.insert(std::make_pair("setvar", &ashitacast::importSetVarNode));
    mImportMap.insert(std::make_pair("incvar", &ashitacast::importIncVarNode));
    mImportMap.insert(std::make_pair("decvar", &ashitacast::importDecVarNode));
    mImportMap.insert(std::make_pair("change", &ashitacast::importChangeNode));
    mImportMap.insert(std::make_pair("addtochat", &ashitacast::importAddToChatNode));
    mImportMap.insert(std::make_pair("addtoecho", &ashitacast::importAddToEchoNode));
    mImportMap.insert(std::make_pair("gearlock", &ashitacast::importGearLockNode));
    mImportMap.insert(std::make_pair("cancel", &ashitacast::importCancelNode));
    mImportMap.insert(std::make_pair("return", &ashitacast::importReturnNode));
    mImportMap.insert(std::make_pair("doidlegear", &ashitacast::importDoIdleGearNode));
    mImportMap.insert(std::make_pair("registerbuff", &ashitacast::importRegisterBuffNode));
    mImportMap.insert(std::make_pair("clearbuff", &ashitacast::importClearBuffNode));
    mImportMap.insert(std::make_pair("equip", &ashitacast::importEquipNode));
}

void ashitacast::initAugmentData()
{
    stringstream file;
    file << m_AshitaCore->GetInstallPath() << "resources\\ashitacast\\augments.xml";
    ifstream fileReader(file.str().c_str(), ios::in | ios::binary | ios::ate);
    if (fileReader.is_open())
    {
        long size    = fileReader.tellg();
        char* buffer = new char[size + 1];
        fileReader.seekg(0, ios::beg);
        fileReader.read(buffer, size);
        fileReader.close();
        buffer[size] = 0x00;

        xml_document<>* doc = new xml_document<>();
        try
        {
            doc->parse<0>(buffer);
        }
        catch (...)
        {
            delete[] buffer;
            delete doc;
            return;
        }
        xml_node<>* node = doc->first_node("augmentdata");
        for (node = node->first_node("augments"); node; node = node->next_sibling("augments"))
        {
            int attr = atoi(node->first_attribute("table")->value());
            for (xml_node<>* child = node->first_node("augment"); child; child = child->next_sibling("augment"))
            {
                int id = atoi(child->first_attribute("id")->value());
                for (xml_node<>* subchild = child->first_node("params"); subchild; subchild = subchild->next_sibling("params"))
                {
                    augmentResource_t temp;
                    temp.id                  = id;
                    temp.realid              = id;
                    temp.multiplier          = 1;
                    temp.percent             = false;
                    temp.offset              = 0;
                    temp.stat                = "";
                    xml_attribute<>* subattr = subchild->first_attribute("stat");
                    if (subattr)
                        temp.stat = subattr->value();
                    subattr = subchild->first_attribute("offset");
                    if (subattr)
                        temp.offset = atoi(subattr->value());
                    subattr = subchild->first_attribute("multiplier");
                    if (subattr)
                        temp.multiplier = atoi(subattr->value());
                    subattr = subchild->first_attribute("percent");
                    if (subattr)
                        temp.percent = subattr->value();
                    for (int x = 0; x < id; x++)
                    {
                        for (std::vector<augmentResource_t>::iterator it = mAugmentData[attr][x].begin(); it != mAugmentData[attr][x].end(); it++)
                        {
                            if (((*it).stat == temp.stat) && ((*it).multiplier == temp.multiplier))
                                temp.realid = id;
                        }
                    }
                    mAugmentData[attr][id].push_back(temp);
                }
            }
        }
        delete[] buffer;
        delete doc;
    }
}