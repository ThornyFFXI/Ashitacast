#include "ashitacast.h"

void ashitacast::handleIncomingPacket0x0A(uint16_t id, uint32_t size, const uint8_t* data)
{
    uint32_t currentId = Read32(data, 4);
    std::string currentName((const char*)data + 0x84);
    if (currentName.length() > 16)
        currentName = currentName.substr(0, 16);

    if ((currentName != mCharacterState.lastName) || (currentId != mCharacterState.lastId))
    {
        mCharacterState.lastName = currentName;
        mCharacterState.lastId   = currentId;
        crawlSection("unload");
        pProfile->load(mCharacterState.lastName.c_str(), mCharacterState.lastId, Read8(data, 0xB4));
        pVariables->clearVariables();
        crawlSection("load");
    }
}
void ashitacast::handleIncomingPacket0x1B(uint16_t id, uint32_t size, const uint8_t* data)
{
    for (int x = 0; x < slotMax; x++)
    {
        pVariables->mEquipOverrides[x].encumbered = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 0x60, x, 1);
    }
}
void ashitacast::handleIncomingPacket0x28(uint16_t id, uint32_t size, const uint8_t* data)
{
    uint32_t userId    = Read32(data, 5);
    uint8_t actionType = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 82, 4);

    if (userId == m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberServerId(0))
    {
        //Any action complete by character should reset actiondelay and allow idle to process.
        if (std::find(std::begin(gActionCompleteTypes), std::end(gActionCompleteTypes), actionType) != std::end(gActionCompleteTypes))
        {
            mCharacterState.playerActionDelay = std::chrono::steady_clock::now() - std::chrono::seconds(1);
        }

        else if ((actionType == 8) || (actionType == 12))
        {
            //Ranged or magic interrupt will also reset actiondelay so idle can process.
            if (Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 0, 86, 16) == 28787)
            {
                mCharacterState.playerActionDelay = std::chrono::steady_clock::now() - std::chrono::seconds(1);
            }
        }
    }

    uint16_t myIndex    = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberTargetIndex(0);
    uint16_t myPetIndex = m_AshitaCore->GetMemoryManager()->GetEntity()->GetPetTargetIndex(myIndex);
    if (myPetIndex == 0)
        return;

    uint32_t myPetId = m_AshitaCore->GetMemoryManager()->GetEntity()->GetServerId(myPetIndex);
    if (userId == myPetId)
    {
        if (std::find(std::begin(gPetActionCompleteTypes), std::end(gPetActionCompleteTypes), actionType) != std::end(gPetActionCompleteTypes))
        {
            mCharacterState.petOverride    = false;
            mCharacterState.petActionDelay = std::chrono::steady_clock::now() - std::chrono::seconds(1);
            return;
        }

        if (actionType == 7) //Pet WS
        {
            uint16_t actionId       = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 213, 10);
            uint16_t actionMessage  = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 230, 10);
            uint32_t actionTargetId = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 18, 6, 32);
            uint16_t actionTarget   = 0;
            for (int x = 0; x < 0x900; x++)
            {
                if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetServerId(x) == actionTargetId)
                {
                    actionTarget = x;
                    break;
                }
            }

            mCharacterState.mCurrentAction.cancel = true;
            mCharacterState.petOverride           = false;
            mCharacterState.petActionDelay        = std::chrono::steady_clock::now() - std::chrono::seconds(1);
            if (actionMessage == 43)
                mCharacterState.mCurrentAction = actionData_t(actionType::PetSkill_MobSkill, actionTarget, actionId);
            else
                mCharacterState.mCurrentAction = actionData_t(actionType::PetSkill_Ability, actionTarget, actionId);
            crawlSection("petskill");
            mCharacterState.mCurrentAction.active = false;

            if (!mCharacterState.mCurrentAction.cancel)
            {
                mCharacterState.petActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.mPetSkillDelay);
            }
        }

        if (actionType == 8) //Pet Spell
        {
            uint16_t actionId       = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 213, 10);
            uint32_t actionTargetId = Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 18, 6, 32);
            uint16_t actionTarget   = 0;
            for (int x = 0; x < 0x900; x++)
            {
                if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetServerId(x) == actionTargetId)
                {
                    actionTarget = x;
                    break;
                }
            }

            if (Ashita::BinaryData::UnpackBitsBE((uint8_t*)data, 0, 86, 16) != 28787) //Interrupt
            {
                ISpell* pResource = m_AshitaCore->GetResourceManager()->GetSpellById(actionId);
                if (!pResource)
                    return;

                mCharacterState.mCurrentAction.cancel = true;
                mCharacterState.petOverride           = false;
                mCharacterState.petActionDelay        = std::chrono::steady_clock::now() - std::chrono::seconds(1);
                mCharacterState.mCurrentAction        = actionData_t(actionType::PetSpell, actionTarget, actionId);
                crawlSection("petspell");
                mCharacterState.mCurrentAction.active = false;

                if (!mCharacterState.mCurrentAction.cancel)
                {
                    int baseCast                   = pResource->CastTime * 250;
                    int actualDelay                = baseCast + mConfig.mPetSpellOffset;
                    mCharacterState.petActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.mPetSkillDelay);
                }
            }
        }
    }
}

void ashitacast::handleOutgoingPacket0x15(uint32_t sizeChunk, const uint8_t* dataChunk)
{
    //I decided to trigger delayed gear here because 0x15 only tick when you're actively sending packets and in a zone.
    if ((std::chrono::steady_clock::now() > mPackerDelay) && (mPackerDelayXml))
    {
        mEventBuffer.document = mPackerDelayXml;
        m_AshitaCore->GetPluginManager()->RaiseEvent("packer_gear", &mEventBuffer, sizeof(packerPluginEvent_t));
        mPackerDelayXml = NULL;
    }

    bool actionPacketFound = false;

    auto offset = 0;
    auto packet = dataChunk;
    while (offset < sizeChunk)
    {
        const auto size = (*(uint16_t*)packet >> 0x09) * 0x04;
        const auto pid  = (uint16_t)(*(uint16_t*)packet & 0x01FF);
        if (pid == 0x1A)
            actionPacketFound = true;
        offset += size;
        packet += size;
    }

    bool parseDefault = !actionPacketFound;

    if (std::chrono::steady_clock::now() < mCharacterState.playerActionDelay)
        parseDefault = false;
    if (std::chrono::steady_clock::now() < mCharacterState.petActionDelay)
        parseDefault = false;

    if (parseDefault)
    {
        //Make sure inappropriate cancel or change tags are recognized.
        mCharacterState.mCurrentAction.cancel   = false;
        mCharacterState.mCurrentAction.postCast = true;
        crawlSection("default");
    }

    pVariables->setLastPosition(ReadFloat(dataChunk, 0x04), ReadFloat(dataChunk, 0x08));
}
bool ashitacast::handleOutgoingPacket0x1A(uint16_t id, uint32_t size, const uint8_t* data)
{
    pkAction_t* packet = (pkAction_t*)data;
    if (packet->category == 3) //Spell
    {
        //Populate necessary data and process precast swaps.
        mCharacterState.mCurrentAction = actionData_t(actionType::Spell, *packet);
        crawlSection("precast");

        //If spell was canceled, end here.
        if (mCharacterState.mCurrentAction.cancel)
        {
            mCharacterState.mCurrentAction.active = false;
            return true;
        }

        //Set action delay
        ISpell* pResource                 = m_AshitaCore->GetResourceManager()->GetSpellById(mCharacterState.mCurrentAction.abilityId);
        double baseCast                   = pResource->CastTime * 250;
        double postFc                     = baseCast * ((100 - mConfig.mFastCast) / 100);
        int actualDelay                   = (int)floor(postFc) + mConfig.mSpellOffset;
        mCharacterState.playerActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(actualDelay);

        //Reinject our action packet so it ends up after precast swaps.
        pPacket->addOutgoingPacket_s(0x1A, sizeof(pkAction_t), &mCharacterState.mCurrentAction.raw);

        //Update variable and process midcast swaps.
        mCharacterState.mCurrentAction.postCast = true;
        crawlSection("midcast");
        mCharacterState.mCurrentAction.active = false;
        return true;
    }

    else if (packet->category == 7) //WS
    {
        //Populate necessary data and process precast swaps.
        mCharacterState.mCurrentAction = actionData_t(actionType::Weaponskill, *packet);
        crawlSection("weaponskill");
        mCharacterState.mCurrentAction.active = false;

        //If ability was canceled, end here.
        if (mCharacterState.mCurrentAction.cancel)
            return true;

        //Set action delay
        mCharacterState.playerActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.mWeaponskillDelay);

        //Reinject our action packet so it ends up after swaps.
        pPacket->addOutgoingPacket_s(0x1A, sizeof(pkAction_t), &mCharacterState.mCurrentAction.raw);
        return true;
    }

    else if (packet->category == 9) //JA
    {
        //Populate necessary data and process precast swaps.
        mCharacterState.mCurrentAction = actionData_t(actionType::Ability, *packet);
        crawlSection("ability");
        mCharacterState.mCurrentAction.active = false;

        //If ability was canceled, end here.
        if (mCharacterState.mCurrentAction.cancel)
            return true;

        //Set action delay
        mCharacterState.playerActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mConfig.mJobAbilityDelay);

        //Reinject our action packet so it ends up after swaps.
        pPacket->addOutgoingPacket_s(0x1A, sizeof(pkAction_t), &mCharacterState.mCurrentAction.raw);
        return true;
    }

    else if (packet->category == 10) //Ranged
    {
        //Populate necessary data and process precast swaps.
        mCharacterState.mCurrentAction = actionData_t(actionType::Ranged, *packet);
        crawlSection("preranged");

        //If ranged was canceled, end here.
        if (mCharacterState.mCurrentAction.cancel)
        {
            mCharacterState.mCurrentAction.active = false;
            return true;
        }

        //Reinject our action packet so it ends up after precast swaps.
        pPacket->addOutgoingPacket_s(0x1A, sizeof(pkAction_t), &mCharacterState.mCurrentAction.raw);

        IInventory* pInv             = m_AshitaCore->GetMemoryManager()->GetInventory();
        equipRegistry_t equip        = pVariables->getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Range);
        Ashita::FFXI::item_t* pRange = pInv->GetContainerItem(equip.container, equip.index);
        IItem* pResource             = m_AshitaCore->GetResourceManager()->GetItemById(pRange->Id);

        double baseCast                   = (double)pResource->Delay / (double).120;
        double postSnap                   = baseCast * ((100 - mConfig.mSnapShot) / 100);
        int actualDelay                   = (int)floor(postSnap) + mConfig.mRangedOffset;
        mCharacterState.playerActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(actualDelay);

        //Update variable and process midcast swaps.
        mCharacterState.mCurrentAction.postCast = true;
        crawlSection("midranged");
        mCharacterState.mCurrentAction.active = false;
        return true;
    }

    return false;
}
bool ashitacast::handleOutgoingPacket0x37(uint16_t id, uint32_t size, const uint8_t* data)
{
    pkItem_t* packet            = (pkItem_t*)data;
    Ashita::FFXI::item_t* pItem = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(packet->itemContainer, packet->itemIndex);

    //Populate necessary data and process precast swaps.
    mCharacterState.mCurrentAction = actionData_t(actionType::Item, packet->targetIndex, pItem->Id);
    crawlSection("item");

    //If item was canceled, end here.
    if (mCharacterState.mCurrentAction.cancel)
        return true;

    //Set action delay
    IItem* pResource                  = m_AshitaCore->GetResourceManager()->GetItemById(mCharacterState.mCurrentAction.abilityId);
    if (pResource)
        mCharacterState.playerActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds((pResource->CastTime * 250) + mConfig.mItemOffset);
    else
        mCharacterState.playerActionDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(2800 + mConfig.mItemOffset);

    //Reinject our item usage packet so it ends up after precast swaps.
    pPacket->addOutgoingPacket_s(0x37, sizeof(pkItem_t), packet);
    return true;
}
void ashitacast::handleOutgoingPacket0x100(uint16_t id, uint32_t size, const uint8_t* data)
{
    if (Read8(data, 4) == 0)
        return;

    crawlSection("unload");
    pProfile->load(mCharacterState.lastName.c_str(), mCharacterState.lastId, Read8(data, 4));
    crawlSection("load");
}