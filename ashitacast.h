#ifndef __ASHITA_Ashitacast_H_INCLUDED__
#define __ASHITA_Ashitacast_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "C:\Ashita 4\plugins\sdk\Ashita.h"
#include "..\common\Output.h"
#include "ashitacastGlobals.h"
#include "ashitacastStructs.h"
#include "ashitacastConfig.h"
#include "ashitacastProfile.h"
#include "ashitacastVariables.h"
#include "..\common\safePacketInjector.h"
#include <map>

class ashitacast : IPlugin
{
private:
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t m_PluginId;

public:
    const char* GetName(void) const override
    {
        return u8"Ashitacast";
    }
    const char* GetAuthor(void) const override
    {
        return u8"Thorny";
    }
    const char* GetDescription(void) const override
    {
        return u8"A plugin to handle your equipment swaps automatically as you play.";
    }
    const char* GetLink(void) const override
    {
        return u8"https://github.com/Lolwutt/ashitacast";
    }
    double GetVersion(void) const override
    {
        return 0.22f;
    }
    int32_t GetPriority(void) const override
    {
        return 0;
    }
    uint32_t GetFlags(void) const override
    {
        return ((uint32_t)Ashita::PluginFlags::UseCommands | (uint32_t)Ashita::PluginFlags::UsePackets | (uint32_t)Ashita::PluginFlags::UsePluginEvents);
    }

    bool Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) override;
    void Release(void) override;
    bool HandleCommand(int32_t mode, const char* command, bool injected) override;
    bool HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
    bool HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
    void HandleEvent(const char* eventName, const void* eventData, const uint32_t eventSize) override;

private:
    typedef void (ashitacast::*xmlNodeHandler)(xml_node<>*);
    struct commandHelp
    {
        string command;
        string description;
    };
    typedef void (ashitacast::*commandHandler)(vector<string>, int, commandHelp help);
    struct commandCombo
    {
        commandHandler handler;
        commandHelp help;
    };
    typedef string (ashitacast::*importHandler)(string, xml_node<>*, int);
    std::map<string, commandCombo, cistringcmp> mCommandMap;
    std::map<string, xmlNodeHandler, cistringcmp> mNodeMap;
    std::map<string, importHandler, cistringcmp> mImportMap;
    vector<augmentResource_t> mAugmentData[3][0x800];
    std::chrono::time_point<std::chrono::steady_clock> mPackerDelay;
    rapidxml::xml_document<>* mPackerDelayXml;

    ashitacastConfig mConfig;
    characterState_t mCharacterState;
    OutputHelpers* pOutput;
    ashitacastProfile* pProfile;
    ashitacastVariables* pVariables;
    safePacketInjector* pPacket;
    DWORD pWardrobe;
    packerPluginEvent_t mEventBuffer;

    //init.cpp
    void initHandlerMaps();
    void initAugmentData();

    //handleCommands.cpp
    void handleCommands(vector<string> args, int argcount);
    void handleEval(const char* command);
    void handleImport(vector<string> args, int argcount, commandHelp help);
    void handleLoad(vector<string> args, int argcount, commandHelp help);
    void handleReload(vector<string> args, int argcount, commandHelp help);
    void handleUnload(vector<string> args, int argcount, commandHelp help);
    void handleNaked(vector<string> args, int argcount, commandHelp help);
    void handleSet(vector<string> args, int argcount, commandHelp help);
    void handleEnable(vector<string> args, int argcount, commandHelp help);
    void handleDisable(vector<string> args, int argcount, commandHelp help);
    void handleHelp(vector<string> args, int argcount, commandHelp help);
    void handlePrint(vector<string> args, int argcount, commandHelp help);
    void handleAddSet(vector<string> args, int argcount, commandHelp help);
    void handleNewXml(vector<string> args, int argcount, commandHelp help);
    void handleSetVar(vector<string> args, int argcount, commandHelp help);
    void handleClearVar(vector<string> args, int argcount, commandHelp help);
    void handleClearVars(vector<string> args, int argcount, commandHelp help);
    void handleDebug(vector<string> args, int argcount, commandHelp help);
    void handleValidate(vector<string> args, int argcount, commandHelp help);
    void handleGear(vector<string> args, int argcount, commandHelp help);
    void handleBench(vector<string> args, int argcount, commandHelp help);
    void printHelpText(commandHelp help, bool description);

    //handlePackets.cpp
    void handleIncomingPacket0x0A(uint16_t id, uint32_t size, const uint8_t* data);
    void handleIncomingPacket0x1B(uint16_t id, uint32_t size, const uint8_t* data);
    void handleIncomingPacket0x28(uint16_t id, uint32_t size, const uint8_t* data);

    void handleOutgoingPacket0x15(uint32_t sizeChunk, const uint8_t* dataChunk);
    bool handleOutgoingPacket0x1A(uint16_t id, uint32_t size, const uint8_t* data);
    bool handleOutgoingPacket0x37(uint16_t id, uint32_t size, const uint8_t* data);
    void handleOutgoingPacket0x100(uint16_t id, uint32_t size, const uint8_t* data);

    //handleXml.cpp
    bool checkInputCommands(const char* text);
    void crawlSection(string section);
    bool crawlNodes(xml_node<>* parent, equipData_t* currEquip);
    void crawlEquipTags(xml_node<>* parent, equipData_t* currEquip, xml_node<>* equipTag);
    bool evaluateNode(xml_node<>* node);
    void processNode(xml_node<>* node, equipData_t* currEquip);
    void processEquipNode(xml_node<>* parent, equipData_t* currEquip);
    void processEquipSet(string set, equipData_t* currEquip, xml_node<>* equipTag);
    void processCommand(xml_node<>* node);
    void processSetVar(xml_node<>* node);
    void processClearVar(xml_node<>* node);
    void processChange(xml_node<>* node);
    void processEcho(xml_node<>* node);
    void processGearLock(xml_node<>* node);
    void processRegisterBuff(xml_node<>* node);
    void processClearBuff(xml_node<>* node);
    void processCancel(xml_node<>* node);
    void processOverride(xml_node<>* node);

    //equip.cpp
    void equipAuto(equipData_t* equipment);
    void equipSingles(equipData_t* equipment);
    void equipSet(equipData_t* equipment);
    std::list<equipData_t> parseInventoryForEquipment(equipData_t* equipment);
    void removeEquip(int equipSlot);
    void removeEquip(int containerIndex, int equipSlot);
    void printDebug(equipData_t equip);

    //helpers.cpp
    string createSet(string name);
    augmentData_t createAugmentData(Ashita::FFXI::item_t* item);
    equipData_t createEquipData(xml_node<>* equipNode, int equipSlot);
    int getEquipSlot(string name);
    bool hasBag(int bagIndex);

    //import.cpp
    void performImport(std::string content, std::string destinationFile);

    string importCommands(xml_node<>* baseNode);
    string importCmdNode(xml_node<>* node);

    string importSets(xml_node<>* baseNode);
    string importSetNode(xml_node<>* baseNode);
    string importIncludeNode(xml_node<>* baseNode);
    string importEquipSetSlotNode(string slotName, xml_node<>* node);
    string importItemNode(xml_node<>* node);

    string importVariables(xml_node<>* baseNode);

    string importFlow(string section, xml_node<>* baseNode, int depth);
    string importIfNode(string section, xml_node<>* node, int depth);
    string importElseIfNode(string section, xml_node<>* node, int depth);
    string importElseNode(string section, xml_node<>* node, int depth);
    string importCommandNode(string section, xml_node<>* node, int depth);
    string importClearVarsNode(string section, xml_node<>* node, int depth);
    string importSetVarNode(string section, xml_node<>* node, int depth);
    string importIncVarNode(string section, xml_node<>* node, int depth);
    string importDecVarNode(string section, xml_node<>* node, int depth);
    string importChangeNode(string section, xml_node<>* node, int depth);
    string importAddToChatNode(string section, xml_node<>* node, int depth);
    string importAddToEchoNode(string section, xml_node<>* node, int depth);
    string importGearLockNode(string section, xml_node<>* node, int depth);
    string importCancelNode(string section, xml_node<>* node, int depth);
    string importReturnNode(string section, xml_node<>* node, int depth);
    string importDoIdleGearNode(string section, xml_node<>* node, int depth);
    string importRegisterBuffNode(string section, xml_node<>* node, int depth);
    string importClearBuffNode(string section, xml_node<>* node, int depth);
    string importEquipNode(string section, xml_node<>* node, int depth);
    string importEquipSlotNode(string section, string slotName, xml_node<>* node, int depth);
    string importConditions(xml_node<>* node, int depth, stringstream* baseOutput);
    string importBuffActive(xml_attribute<>* attr, int depth, stringstream* baseOutput);
    string replace(string text, string find, string replace);

    map<string, xml_attribute<>*> checkAttributes(xml_node<>* node, string nodeName, std::list<string> validAttributes, stringstream* output, int depth);

    bool resolveAugment(string* augmentString, const char* itemName, const char* augmentCode, string* comment);
    string getLegacyAugment(unsigned char Input[]);
};
#endif