#include "ashitacast.h"
#include <filesystem>
#include <fstream>

void ashitacast::handleCommands(vector<string> args, int argcount)
{
    auto iter = mCommandMap.find(args[1]);
    if (iter == mCommandMap.end())
    {
        pOutput->error_f("Command not recognized. [$H%s$R]", args[1].c_str());
        return;
    }
    (this->*(iter->second.handler))(args, argcount, iter->second.help);
}

void ashitacast::handleEval(const char* command)
{
    const char* offset = strstr(command, "eval");
    if (offset != NULL)
    {
        offset += 4;
        while (offset[0] == ' ')
            offset++;
        if (strlen(offset) > 0)
            m_AshitaCore->GetChatManager()->QueueCommand(0, pVariables->resolveVariables(offset).c_str());
    }
}

void ashitacast::handleLoad(vector<string> args, int argcount, commandHelp help)
{
    crawlSection("unload");

    if (argcount == 2)
    {
        pProfile->load(mCharacterState.lastName.c_str(), mCharacterState.lastId, m_AshitaCore->GetMemoryManager()->GetPlayer()->GetMainJob());
    }
    else
    {
        char fileName[1024];
        sprintf_s(fileName, 1024, "%sconfig\\ashitacast\\%s_%d\\%s%s",
            m_AshitaCore->GetInstallPath(), mCharacterState.lastName.c_str(), mCharacterState.lastId, args[2].c_str(),
            ((args[2].length() < 4) || (_stricmp(args[2].substr(args[2].length() - 4).c_str(), ".xml") != 0)) ? ".xml" : "");
        if (!std::filesystem::exists(fileName))
        {
            sprintf_s(fileName, 1024, "%sconfig\\ashitacast\\%s%s",
                m_AshitaCore->GetInstallPath(), args[2].c_str(),
                ((args[2].length() < 4) || (_stricmp(args[2].substr(args[2].length() - 4).c_str(), ".xml") != 0)) ? ".xml" : "");
        }
        if (std::filesystem::exists(fileName))
        {
            pProfile->load(fileName);
        }
        else
        {
            pOutput->error_f("Could not find an XML matching %s.", args[2].c_str());
            return;
        }
    }

    pVariables->clearVariables();
    crawlSection("load");
}
void ashitacast::handleReload(vector<string> args, int argcount, commandHelp help)
{
    if (!pProfile->mIsLoaded)
    {
        pOutput->error("Could not reload XML.  No XML was loaded.");
        return;
    }

    crawlSection("unload");
    pProfile->reload();
    pVariables->clearVariables();
    crawlSection("load");
}
void ashitacast::handleUnload(vector<string> args, int argcount, commandHelp help)
{
    crawlSection("unload");
    pProfile->unload();
    pVariables->clearVariables();
}
void ashitacast::handleNaked(vector<string> args, int argcount, commandHelp help)
{
    for (int x = 0; x < 16; x++)
    {        
        removeEquip(x);
        pVariables->mEquipOverrides[x].disabled = true;
    }
}
void ashitacast::handleSet(vector<string> args, int argcount, commandHelp help)
{
    if (argcount < 3)
    {
        printHelpText(help, true);
        return;
    }
    
    int delay = 5000;
    if (argcount >= 4)
    {
        delay = atoi(args[3].c_str());
        if (delay > 3600000)
            delay = 3600000;
        if (delay < 1)
        {
            pOutput->error("Duration must be at least 1ms.");
            printHelpText(help, true);
            return;
        }
    }

    mCharacterState.gearLockDelay = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    equipData_t currEquip[slotMax];
    processEquipSet(args[2], currEquip, NULL);
    equipSet(currEquip);
    mCharacterState.gearLockDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay);
}
void ashitacast::handleEnable(vector<string> args, int argcount, commandHelp help)
{
    if (argcount == 2)
    {
        for (int x = 0; x < 16; x++)
        {
            pVariables->mEquipOverrides[x].disabled = false;
        }
        pOutput->message("All slots enabled.");
        return;
    }
    int slot = getEquipSlot(args[2]);
    if (slot != -1)
    {
        pVariables->mEquipOverrides[slot].disabled = false;
        pOutput->message_f("$H%s$R enabled.", gSlotNames[slot].c_str());
    }
}
void ashitacast::handleDisable(vector<string> args, int argcount, commandHelp help)
{
    if (argcount == 2)
    {
        for (int x = 0; x < 16; x++)
        {
            pVariables->mEquipOverrides[x].disabled = true;
        }
        pOutput->message("All slots disabled.");
        return;
    }
    int slot = getEquipSlot(args[2]);
    if (slot != -1)
    {
        pVariables->mEquipOverrides[slot].disabled = true;
        pOutput->message_f("%s disabled.", gSlotNames[slot].c_str());
    }
}
void ashitacast::handleHelp(vector<string> args, int argcount, commandHelp help)
{
    if (argcount > 2)
    {
        auto iter = mCommandMap.find(args[2]);
        if (iter != mCommandMap.end())
        {
            printHelpText(iter->second.help, true);
            return;
        }
    }

    pOutput->message("Command List");
    for (auto iter = mCommandMap.begin(); iter != mCommandMap.end(); iter++)
    {
        printHelpText(iter->second.help, false);
    }
}
void ashitacast::handlePrint(vector<string> args, int argcount, commandHelp help)
{
    if (argcount < 3)
    {
        printHelpText(help, true);
        return;
    }

    if (args[2] == "vars")
        pVariables->printEnvironmentalVariables();

    else if (args[2] == "uvars")
        pVariables->printUserVariables();

    else
    {
        printHelpText(help, true);
        return;
    }
}
void ashitacast::handleAddSet(vector<string> args, int argcount, commandHelp help)
{
    if (argcount < 3)
    {
        printHelpText(help, true);
        return;
    }
    pProfile->addSet(args[2], createSet(args[2]));
}

void ashitacast::handleNewXml(vector<string> args, int argcount, commandHelp help)
{
    char fileName[1024];
    if (argcount > 2)
    {
        sprintf_s(fileName, 1024, "%sconfig\\ashitacast\\%s_%d\\%s%s",
            m_AshitaCore->GetInstallPath(), mCharacterState.lastName.c_str(), mCharacterState.lastId, args[2].c_str(),
            ((args[2].length() < 4) || (_stricmp(args[2].substr(args[2].length() - 4).c_str(), ".xml") != 0)) ? ".xml" : "");
    }
    else
    {
        sprintf_s(fileName, 1024, "%sconfig\\ashitacast\\%s_%d\\%s.xml",
            m_AshitaCore->GetInstallPath(), mCharacterState.lastName.c_str(), mCharacterState.lastId,
            m_AshitaCore->GetResourceManager()->GetString("jobs_abbr", m_AshitaCore->GetMemoryManager()->GetPlayer()->GetMainJob()));
    }

    if (std::filesystem::exists(fileName))
    {
        pOutput->error_f("File already exists: $H%s$R", fileName);
        return;
    }

    //Ensure directories exist, making them if not.
    string makeDirectory(fileName);
    size_t nextDirectory = makeDirectory.find("\\");
    nextDirectory        = makeDirectory.find("\\", nextDirectory + 1);
    while (nextDirectory != string::npos)
    {
        string currentDirectory = makeDirectory.substr(0, nextDirectory + 1);
        if ((!CreateDirectory(currentDirectory.c_str(), NULL)) && (ERROR_ALREADY_EXISTS != GetLastError()))
        {
            pOutput->error_f("Could not find or create folder. [$H%s$R]", currentDirectory.c_str());
            return;
        }
        nextDirectory = makeDirectory.find("\\", nextDirectory + 1);
    }

    std::ofstream xmlWriter(fileName, std::ofstream::binary);
    if (!xmlWriter.is_open())
    {
        pOutput->error_f("File could not be created: $H%s$R", fileName);
        return;
    }

    xmlWriter << "<ashitacast>\n";
    for (int x = 0; x < baseNodeMax; x++)
    {
        xmlWriter << "\t<" << gBaseNodes[x] << ">\n";
        xmlWriter << "\t</" << gBaseNodes[x] << ">\n";
    }
    xmlWriter << "</ashitacast>";
    xmlWriter.close();
    if (pProfile->load(fileName))
    {
        crawlSection("load");
    }
}
void ashitacast::handleSetVar(vector<string> args, int argcount, commandHelp help)
{
    if (argcount < 4)
    {
        printHelpText(help, true);
        return;    
    }
    pVariables->setVariable(args[2], pVariables->resolveVariables(args[3]));
}
void ashitacast::handleClearVar(vector<string> args, int argcount, commandHelp help)
{
    if (argcount < 3)
    {
        printHelpText(help, true);
        return;
    }
    pVariables->clearVariable(args[2]);
}
void ashitacast::handleClearVars(vector<string> args, int argcount, commandHelp help)
{
    pVariables->clearVariables();
}
void ashitacast::handleDebug(vector<string> args, int argcount, commandHelp help)
{
    if ((argcount > 2) && (args[2] == "on"))
        mConfig.mDebugEnabled = true;
    else if ((argcount > 2) && (args[2] == "off"))
        mConfig.mDebugEnabled = false;
    else
        mConfig.mDebugEnabled = !mConfig.mDebugEnabled;

    pOutput->message_f("Debug $H%s$R.", mConfig.mDebugEnabled ? "enabled" : "disabled");
}
void ashitacast::handleValidate(vector<string> args, int argcount, commandHelp help)
{
    if (!m_AshitaCore->GetPluginManager()->IsLoaded("Packer"))
    {
        pOutput->error("Could not activate packer.  Packer plugin is not loaded.");
        return;
    }
    if (!pProfile->mIsLoaded)
    {
        pOutput->error("Could not activate packer.  No XML was loaded.");
        return;
    }
    
    mEventBuffer.document = &pProfile->mXmlDocument;
    m_AshitaCore->GetPluginManager()->RaiseEvent("packer_validate", &mEventBuffer, sizeof(packerPluginEvent_t));
}
void ashitacast::handleGear(vector<string> args, int argcount, commandHelp help)
{
    if (!m_AshitaCore->GetPluginManager()->IsLoaded("Packer"))
    {
        pOutput->error("Could not activate packer.  Packer plugin is not loaded.");
        return;    
    }
    if (!pProfile->mIsLoaded)
    {
        pOutput->error("Could not activate packer.  No XML was loaded.");
        return;
    }

    for (int x = 0; x < 16; x++)
    {
        pVariables->mEquipOverrides[x].disabled = true;
    }
    if (mConfig.mNakedForPacker)
    {
        pOutput->message("Removing all equipment for Packer.");
        for (int x = 0; x < 16; x++)
        {
            removeEquip(x);
        }

        //This triggers on outgoing 0x15 once 2 seconds have passed, so inventory has time to reflect that the gear has been removed.
        mPackerDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
        mPackerDelayXml = &pProfile->mXmlDocument;
    }
    else
    {
        mEventBuffer.document = &pProfile->mXmlDocument;
        m_AshitaCore->GetPluginManager()->RaiseEvent("packer_gear", &mEventBuffer, sizeof(packerPluginEvent_t));
    }
}
void ashitacast::handleImport(vector<string> args, int argcount, commandHelp help)
{
    if (argcount < 3)
    {
        printHelpText(help, true);
        return;
    }

    stringstream file(args[2]);
    ifstream file_reader(file.str().c_str(), ios::in | ios::binary | ios::ate);
    if (!file_reader.is_open())
    {
        file.str("");
        file << m_AshitaCore->GetInstallPath() << "config\\ashitacast\\";
        file << args[2];
        file_reader = ifstream(file.str().c_str(), ios::in | ios::binary | ios::ate);
        if (!file_reader.is_open())
        {
            file << ".xml";
            file_reader = ifstream(file.str().c_str(), ios::in | ios::binary | ios::ate);
            if (!file_reader.is_open())
            {
                pOutput->error_f("Failed to import XML.  Could not locate file. [$H%s$R]", args[2].c_str());
                return;
            }
        }
    }

    string baseFile;
    baseFile.resize(file_reader.tellg());
    file_reader.seekg(0, ios::beg);
    file_reader.read(&baseFile[0], baseFile.size());
    file_reader.close();

    string newFile = file.str();
    size_t offset  = newFile.find("_v3.xml");
    if (offset == string::npos)
    {
        pOutput->error_f("Failed to create new XML to hold import data.  Filename of imported xml doesn't include _v3.xml. [$H%s$R]", newFile.c_str());
        return;
    }
    newFile = newFile.replace(offset, 7, ".xml");


    performImport(baseFile, newFile);
}
void ashitacast::handleBench(vector<string> args, int argcount, commandHelp help)
{
    for (int x = 1; x < 81; x++)
    {
        Ashita::FFXI::item_t* item = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(0, x);
        if (item->Id == 26258)
        {
            augmentData_t aug = createAugmentData(item);
            int y             = 0;
            for (std::vector<string>::iterator augStringIter = aug.stringAugments.begin(); augStringIter != aug.stringAugments.end(); augStringIter++)
            {
                pOutput->message_f("Augment %d: %s", y, (*augStringIter).c_str());
                y++;
            }
        }    
    }
}

void ashitacast::printHelpText(commandHelp help, bool description)
{
    pOutput->message_f("$H%s", help.command.c_str());
    if (description)
        pOutput->message(help.description);
}