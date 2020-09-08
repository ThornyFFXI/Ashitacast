#include "ashitacast.h"

struct equipItem_t
{
    uint8_t inventoryIndex;
    uint8_t equipmentSlot;
    uint8_t containerIndex;
    uint8_t unknown;

    equipItem_t()
    {
        inventoryIndex = 0;
        equipmentSlot  = 0;
        containerIndex = 0;
        unknown        = 0;
    }
    equipItem_t(equipData_t equip)
    {
        inventoryIndex = equip.index;
        equipmentSlot  = equip.slot;
        containerIndex = equip.container;
        unknown        = 0;
    }
};

struct pkEquipSingle_t //0x50
{
    uint32_t header;
    equipItem_t item;
};

struct pkEquipSet_t //0x51
{
    uint32_t header;
    uint8_t count;
    uint8_t unused[3];
    equipItem_t equip[16];
};

void ashitacast::equipAuto(equipData_t* equipment)
{
    std::list<equipData_t> equipList = parseInventoryForEquipment(equipment);
    if (equipList.size() < 9)
    {
        for (std::list<equipData_t>::iterator iter = equipList.begin(); iter != equipList.end(); iter++)
        {
            if (mConfig.mDebugEnabled)
                printDebug(*iter);
            pkEquipSingle_t packet                               = {0};
            packet.item                                          = equipItem_t(*iter);
            pVariables->mEquipOverrides[(*iter).slot].container  = (*iter).container;
            pVariables->mEquipOverrides[(*iter).slot].index      = (*iter).index;
            pVariables->mEquipOverrides[(*iter).slot].expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
            pPacket->addOutgoingPacket_s(0x50, sizeof(pkEquipSingle_t), &packet);
        }  
    }
    else
    {
        pkEquipSet_t packet = {0};
        for (std::list<equipData_t>::iterator iter = equipList.begin(); iter != equipList.end(); iter++)
        {
            if (mConfig.mDebugEnabled)
                printDebug(*iter);
            packet.equip[packet.count] = equipItem_t(*iter);
            packet.count++;
            pVariables->mEquipOverrides[(*iter).slot].container  = (*iter).container;
            pVariables->mEquipOverrides[(*iter).slot].index      = (*iter).index;
            pVariables->mEquipOverrides[(*iter).slot].expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
        }
        if (packet.count > 0)
            pPacket->addOutgoingPacket_s(0x51, sizeof(pkEquipSet_t), &packet);
    }
}

void ashitacast::equipSingles(equipData_t* equipment)
{
    std::list<equipData_t> equipList = parseInventoryForEquipment(equipment);
    for (std::list<equipData_t>::iterator iter = equipList.begin(); iter != equipList.end(); iter++)
    {
        if (mConfig.mDebugEnabled)
            printDebug(*iter);
        pkEquipSingle_t packet                               = {0};
        packet.item                                          = equipItem_t(*iter);
        pVariables->mEquipOverrides[(*iter).slot].container  = (*iter).container;
        pVariables->mEquipOverrides[(*iter).slot].index      = (*iter).index;
        pVariables->mEquipOverrides[(*iter).slot].expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
        pPacket->addOutgoingPacket_s(0x50, sizeof(pkEquipSingle_t), &packet);
    }
}

void ashitacast::equipSet(equipData_t* equipment)
{
    std::list<equipData_t> equipList = parseInventoryForEquipment(equipment);
    if (equipList.size() == 0)
        return;

    pkEquipSet_t packet = {0};
    for (std::list<equipData_t>::iterator iter = equipList.begin(); iter != equipList.end(); iter++)
    {
        if (mConfig.mDebugEnabled)
            printDebug(*iter);

        packet.equip[packet.count]                           = equipItem_t(*iter);
        packet.count++;
        pVariables->mEquipOverrides[(*iter).slot].container  = (*iter).container;
        pVariables->mEquipOverrides[(*iter).slot].index      = (*iter).index;
        pVariables->mEquipOverrides[(*iter).slot].expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
    }
    if (packet.count > 0)
        pPacket->addOutgoingPacket_s(0x51, sizeof(pkEquipSet_t), &packet);
}

std::list<equipData_t> ashitacast::parseInventoryForEquipment(equipData_t* equipment)
{
    IInventory* pInventory = m_AshitaCore->GetMemoryManager()->GetInventory();
    std::list<equipData_t> equipList;
    for (int x = 0; x < slotMax; x++)
    {
        //Skip slots that weren't set during the xml parse.
        if (equipment[x].name.length() < 1)
            continue;

        //Handle ignore tags
        if (equipment[x].name == "ignore")
            continue;

        //Handle displaced tags
        if (equipment[x].name == "displaced")
        {
            pVariables->mEquipOverrides[equipment[x].slot].container  = 0;
            pVariables->mEquipOverrides[equipment[x].slot].index      = 0;
            pVariables->mEquipOverrides[equipment[x].slot].expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
            continue;
        }

        //Set index for remove tags
        if (equipment[x].name == "remove")
        {
            equipRegistry_t equip  = pVariables->getEquippedItem(equipment[x].slot);
            if (equip.index < 1)
                continue;

            equipment[x].container = equip.container;
            equipment[x].index     = 0;
        }

        //Skip slots that are disabled or encumbered.
        equipRegistry_t data = pVariables->getEquippedItem(x);
        if ((data.encumbered) || (data.disabled))
            continue;

        //Skip slots that are already matching.
        if (data.index > 0)
        {
            Ashita::FFXI::item_t* pItem = pInventory->GetContainerItem(data.container, data.index);
            IItem* pResource            = m_AshitaCore->GetResourceManager()->GetItemById(pItem->Id);
            if ((equipment[x].ismatch(pResource)) && (equipment[x].augmatch(createAugmentData(pItem))))
                continue;
        }

        //Add slot to list.
        equipList.push_back(equipment[x]);
    }
    if (equipList.size() == 0)
        return equipList;

    //Sort list by priority.
    equipList.sort([](const equipData_t& a, const equipData_t& b) { return a.priority > b.priority; });

    //Iterate bags to find equipment.
    for (std::list<int32_t>::iterator bagIter = mConfig.mEquipBags.begin(); bagIter != mConfig.mEquipBags.end(); bagIter++)
    {
        if (!hasBag(*bagIter))
            continue;

        for (int indexIter = 1; indexIter < pInventory->GetContainerCountMax(*bagIter); indexIter++)
        {
            Ashita::FFXI::item_t* pItem = pInventory->GetContainerItem(*bagIter, indexIter);
            if (pItem->Id == 0)
                continue;

            IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(pItem->Id);
            if (!pResource)
                continue; //Skip item that can't be resolved to resource
            if ((pResource->Flags & 0x0800) == 0)
                continue; //Skip non-equippable item

            for (std::list<equipData_t>::iterator equipIter = equipList.begin(); equipIter != equipList.end(); equipIter++)
            {
                if ((*equipIter).index != -1)
                    continue;

                if (((*equipIter).ismatch(pResource)) && ((*equipIter).augmatch(createAugmentData(pItem))))
                {
                    bool canEquip = true;

                    //Remove item if it's in another slot already.
                    for (int slotIter = 0; slotIter < slotMax; slotIter++)
                    {
                        equipRegistry_t equip = pVariables->getEquippedItem(slotIter);
                        if ((equip.container == *bagIter) && (equip.index == indexIter))
                        {
                            //If item is disabled and in another slot, tough luck.
                            if ((pVariables->mEquipOverrides[slotIter].disabled) || (pVariables->mEquipOverrides[slotIter].encumbered))
                                canEquip = false;
                            else
                                removeEquip(equip.container, slotIter);
                        }
                    }

                    if (canEquip)
                    {
                        (*equipIter).index     = indexIter;
                        (*equipIter).container = *bagIter;
                    }
                    break;
                }
            }
        }
    }

    for (std::list<equipData_t>::iterator equipIter = equipList.begin(); equipIter != equipList.end();)
    {
        if ((*equipIter).index == -1)
            equipIter = equipList.erase(equipIter);
        else
            equipIter++;
    }
    return equipList;
}

void ashitacast::removeEquip(int equipSlot)
{
    equipRegistry_t equip = pVariables->getEquippedItem(equipSlot);
    removeEquip(equip.container, equipSlot);
}

void ashitacast::removeEquip(int containerIndex, int equipSlot)
{
    //Skip this if item is already empty, or we're encumbered.
    if ((pVariables->getEquippedItem(equipSlot).index < 1) || (pVariables->mEquipOverrides[equipSlot].encumbered))
        return;

    pkEquipSingle_t packet                            = {0};
    packet.item.containerIndex                        = containerIndex;
    packet.item.equipmentSlot                         = equipSlot;
    pVariables->mEquipOverrides[equipSlot].container  = 0;
    pVariables->mEquipOverrides[equipSlot].index      = 0;
    pVariables->mEquipOverrides[equipSlot].expiration = std::chrono::steady_clock::now() + std::chrono::milliseconds(2000);
    pPacket->addOutgoingPacket_s(0x50, sizeof(pkEquipSingle_t), &packet);
    if (mConfig.mDebugEnabled)
        pOutput->message_f("Removing %s equipment.", gSlotNames[equipSlot].c_str());
}
void ashitacast::printDebug(equipData_t equip)
{
    if (equip.index == 0)
    {
        pOutput->message_f("Removing %s equipment.", gSlotNames[equip.slot].c_str());
        return;
    }

    stringstream output;
    output << "Equipping " << gSlotNames[equip.slot] << ":  ";
    Ashita::FFXI::item_t* pItem = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(equip.container, equip.index);
    IItem* pResource            = m_AshitaCore->GetResourceManager()->GetItemById(pItem->Id);
    output << pResource->Name[0];
    augmentData_t augment = createAugmentData(pItem);
    if (augment.path > 0)
    {
        output << "(Path ";
        output << char(augment.path + 64);
        output << ")";
    }
    else if (augment.stringAugments.size() > 0)
    {
        output << "(";
        for (std::vector<string>::iterator iter = augment.stringAugments.begin(); iter != augment.stringAugments.end();)
        {
            output << *iter;
            iter++;
            if (iter != augment.stringAugments.end())
                output << ",";
        }
        output << ")";
    }

    output << " [" << equip.container << ":" << equip.index << "]";

    pOutput->message(output.str().c_str());
}