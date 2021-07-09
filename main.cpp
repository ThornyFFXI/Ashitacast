#include "ashitacast.h"

#pragma comment(lib, "psapi.lib")
#include <psapi.h>

__declspec(dllexport) IPlugin* __stdcall expCreatePlugin(const char* args)
{
    UNREFERENCED_PARAMETER(args);

    return (IPlugin*)(new ashitacast());
}

__declspec(dllexport) double __stdcall expGetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}

bool ashitacast::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id)
{
    m_AshitaCore = core;
    m_LogManager = logger;
    m_PluginId   = id;

    mPackerDelay   = std::chrono::steady_clock::now() - std::chrono::milliseconds(1000);
    MODULEINFO mod = {0};
    ::GetModuleInformation(::GetCurrentProcess(), ::GetModuleHandle("FFXiMain.dll"), &mod, sizeof(MODULEINFO));
    pWardrobe = Ashita::Memory::FindPattern((uintptr_t)mod.lpBaseOfDll, (uintptr_t)mod.SizeOfImage, "A1????????568BF1578B88????????C1E902F6C101", 0, 0);

    pOutput    = new OutputHelpers(core, logger, "Ashitacast");

    initAugmentData();
    initHandlerMaps();

    pPacket    = new safePacketInjector(core->GetPacketManager());
    pProfile   = new ashitacastProfile(core, &mConfig, pOutput);
    pVariables = new ashitacastVariables(core, &mCharacterState.mCurrentAction, pOutput);

    strcpy_s(mEventBuffer.returnEvent, 256, "packer_ashitacast_response");

    if (m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberIsActive(0))
    {
        mCharacterState.lastName = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberName(0);
        mCharacterState.lastId  = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberServerId(0);
        pProfile->load(mCharacterState.lastName.c_str(), mCharacterState.lastId, m_AshitaCore->GetMemoryManager()->GetPlayer()->GetMainJob());
        crawlSection("load");
    }

    return true;
}
void ashitacast::Release(void)
{
    delete pPacket;
    delete pProfile;
    delete pVariables;
}
bool ashitacast::HandleCommand(int32_t mode, const char* command, bool injected)
{
    if (checkInputCommands(command))
        return true;

    std::vector<string> args;
    int argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (argcount < 2)
        return false;

    if ((args[0] == "/ac") || (args[0] == "/ashitacast"))
    {
        if (args[1] == "eval")
        {
            handleEval(command);
            return true;
        }

        handleCommands(args, argcount);
        return true;
    }

    return false;
}
bool ashitacast::HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    //Avoid processing anything we injected ourselves.
    if ((injected) && (pPacket->checkIncomingSelfInjected(id, size, data)))
        return false;

    if (id == 0x0A)
    {
        handleIncomingPacket0x0A(id, size, data);
    }

    if (id == 0x1B)
    {
        handleIncomingPacket0x1B(id, size, data);
    }

    if (id == 0x28)
    {
        handleIncomingPacket0x28(id, size, data);    
    }

    return false;
}
bool ashitacast::HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    //Avoid processing anything we injected ourselves.
    if ((injected) && (pPacket->checkOutgoingSelfInjected(id, size, data)))
        return false;

    if ((id == 0x15) && (!injected))
    {
        handleOutgoingPacket0x15(sizeChunk, dataChunk);
    }

    if ((id == 0x1A) && (!blocked))
    {
        return handleOutgoingPacket0x1A(id, size, data);
    }

    if ((id == 0x37) && (!blocked))
    {
        return handleOutgoingPacket0x37(id, size, data);
    }

    if ((id == 0x100) && (!blocked))
    {
        handleOutgoingPacket0x100(id, size, data);
    }

    return false;
}


void ashitacast::HandleEvent(const char* eventName, const void* eventData, const uint32_t eventSize)
{
    if (strncmp(eventName, mEventBuffer.returnEvent, strlen(mEventBuffer.returnEvent)) == 0)
    {
        const char* eventText = (const char*)eventName + strlen(mEventBuffer.returnEvent) + 1;
        if (strcmp(eventText, "GEAR_STARTED") != 0)
        {
            for (int x = 0; x < 16; x++)
            {
                pVariables->mEquipOverrides[x].disabled = false;
            }
        }
        m_LogManager->Logf((uint32_t)Ashita::LogLevel::Debug, "Ashitacast", "Packer Event: %s", eventText);
    }
}