#include "ashitacast.h"

augmentData_t ashitacast::createAugmentData(Ashita::FFXI::item_t* item)
{
    augmentData_t currAugment = {0};

    if ((item->Extra[0] != 2) && (item->Extra[0] != 3))
        return currAugment;

    //Crafting shield
    if (item->Extra[1] & 0x08)
    {
        //Unlikely this will be used in ashitacast.
    }

    //Delve system
    else if (item->Extra[1] & 0x20)
    {
        currAugment.path = Ashita::BinaryData::UnpackBitsBE(item->Extra, 16, 2) + 1;
        currAugment.rank = Ashita::BinaryData::UnpackBitsBE(item->Extra, 18, 4);

        //Build raw augment vector
        for (int x = 0; x < 4; x++)
        {
            int32_t augmentId    = Ashita::BinaryData::UnpackBitsBE(item->Extra, (16 * x) + 48, 8);
            int32_t augmentValue = Ashita::BinaryData::UnpackBitsBE(item->Extra, (16 * x) + 56, 8);
            if (augmentId == 0)
                continue;

            singleAugment_t augment;
            augment.realId = augmentId;
            augment.table = 1;

            for (std::vector<augmentResource_t>::iterator iter = mAugmentData[augment.table][augment.realId].begin(); iter != mAugmentData[augment.table][augment.realId].end(); iter++)
            {
                augment.stat  = (*iter).stat;
                augment.value = (augmentValue + (*iter).offset) * (*iter).multiplier;
                augment.percent = (*iter).percent;

                //Merge like augments
                std::vector<singleAugment_t>::iterator iter2;
                for (iter2 = currAugment.rawAugments.begin(); iter2 != currAugment.rawAugments.end(); iter2++)
                {
                    if (((*iter2).table == augment.table) && (augment.stat == (*iter2).stat))
                    {
                        (*iter2).value += augment.value;
                        break;
                    }
                }

                //Add to table if no like augment
                if (iter2 == currAugment.rawAugments.end())
                    currAugment.rawAugments.push_back(augment);
            }
        }

        //Build string augment vector
        for (std::vector<singleAugment_t>::iterator iter = currAugment.rawAugments.begin(); iter != currAugment.rawAugments.end(); iter++)
        {
            stringstream stringAugment;
            stringAugment << (*iter).stat;
            if ((*iter).value != 0)
            {
                if ((*iter).value > 0)
                    stringAugment << "+";
                stringAugment << (*iter).value;
                if ((*iter).percent)
                    stringAugment << "%";
            }
            currAugment.stringAugments.push_back(stringAugment.str());
        }

        return currAugment;
    }

    //Dynamis system
    else if (item->Extra[1] == 131)
    {
        currAugment.path = Ashita::BinaryData::UnpackBitsBE(item->Extra, 32, 2) + 1;
        currAugment.rank = Ashita::BinaryData::UnpackBitsBE(item->Extra, 50, 5);        
        //pOutput->message_f("Path %d Rank %d", currAugment.path, currAugment.rank);
    }

    //Evolith system
    else if (item->Extra[1] & 0x80)
    {
        //Unsure how this will be implemented, if at all.
    }

    //Basic system
    else
    {
        int maxAugments = 5;
        if (item->Extra[1] & 0x40)
        {
            maxAugments             = 4;
            currAugment.trialNumber = Ashita::BinaryData::UnpackBitsBE(item->Extra, 80, 15);
            //currAugment.trialComplete = Ashita::BinaryData::UnpackBitsBE(item->Extra, 95, 1);  Not in use atm.
        }

        //Build raw augment vector
        for (int x = 0; x < maxAugments; x++)
        {
            int32_t augmentId    = Ashita::BinaryData::UnpackBitsBE(item->Extra, (16 * x) + 16, 11);
            int32_t augmentValue = Ashita::BinaryData::UnpackBitsBE(item->Extra, (16 * x) + 27, 5);
            if (augmentId == 0)
                continue;

            singleAugment_t augment;
            augment.realId = augmentId;
            augment.table  = 0;

            for (std::vector<augmentResource_t>::iterator iter = mAugmentData[augment.table][augment.realId].begin(); iter != mAugmentData[augment.table][augment.realId].end(); iter++)
            {
                augment.stat    = (*iter).stat;
                augment.value   = (augmentValue + (*iter).offset) * (*iter).multiplier;
                augment.percent = (*iter).percent;

                //Merge like augments
                std::vector<singleAugment_t>::iterator iter2;
                for (iter2 = currAugment.rawAugments.begin(); iter2 != currAugment.rawAugments.end(); iter2++)
                {
                    if (((*iter2).table == augment.table) && (augment.stat == (*iter2).stat))
                    {
                        (*iter2).value += augment.value;
                        break;
                    }
                }

                //Add to table if no like augment
                if (iter2 == currAugment.rawAugments.end())
                    currAugment.rawAugments.push_back(augment);
            }
        }

        //Build string augment vector
        for (std::vector<singleAugment_t>::iterator iter = currAugment.rawAugments.begin(); iter != currAugment.rawAugments.end(); iter++)
        {
            stringstream stringAugment;
            stringAugment << (*iter).stat;
            if ((*iter).value != 0)
            {
                if ((*iter).value > 0)
                    stringAugment << "+";
                stringAugment << (*iter).value;
                if ((*iter).percent)
                    stringAugment << "%";
            }
            currAugment.stringAugments.push_back(stringAugment.str());
        }
    }
    return currAugment;
}
equipData_t ashitacast::createEquipData(xml_node<>* equipNode, int equipSlot)
{
    equipData_t build;
    build.slot = equipSlot;
    build.name = pVariables->resolveVariables(equipNode->value());
    xml_attribute<>* attr = equipNode->first_attribute("augment");
    if (attr)
    {
        stringstream stream(pVariables->resolveVariables(attr->value()));
        while (stream.good())
        {
            string augment;
            getline(stream, augment, ',');
            build.augmentStrings.push_back(augment);
        }
    }
    attr = equipNode->first_attribute("augpath");
    if (attr)
        {
            if (string(attr->value()) == "A")
                build.augmentPath = 1;
            if (string(attr->value()) == "B")
                build.augmentPath = 2;
            if (string(attr->value()) == "C")
                build.augmentPath = 3;
            if (string(attr->value()) == "D")
                build.augmentPath = 4;
    }

    attr = equipNode->first_attribute("augrank");
    if (attr)
    {
        build.augmentRank = atoi(attr->value());
    }

    attr = equipNode->first_attribute("augtrial");
    if (attr)
    {
        build.augmentTrial = atoi(attr->value());
    }

    attr = equipNode->first_attribute("priority");
    if (attr)
    {
        build.priority = atoi(attr->value());
    }

    attr = equipNode->first_attribute("lock");
    build.locked = ((attr) && (_stricmp(attr->value(), "true") == 0));

    return build;
}
string ashitacast::createSet(string name)
{
    IInventory* pInvReader            = m_AshitaCore->GetMemoryManager()->GetInventory();
    IResourceManager* pResourceReader = m_AshitaCore->GetResourceManager();

    stringstream setContent;
    setContent << "<set name=\"" << name << "\">\r\n";
    for (int x = 0; x < slotMax; x++)
    {
        int equipIndex = x;
        if (!mConfig.mOrderByIndex)
            equipIndex = (int)gOrderOfSlots[x];
        std::string slotName = gSlotNames[equipIndex];

        int rawIndex       = pInvReader->GetEquippedItem(equipIndex)->Index;
        int containerIndex = Ashita::BinaryData::UnpackBitsBE((uint8_t*)&rawIndex, 8, 8);
        int slotIndex      = Ashita::BinaryData::UnpackBitsBE((uint8_t*)&rawIndex, 0, 8);
        if (slotIndex == 0)
        {
            if (mConfig.mIncludeEmptySlots)
            {
                setContent << "\t\t\t<" << slotName << ">";
                setContent << "ignore";
                setContent << "</" << slotName << ">\r\n";
            }
            continue;
        }

        Ashita::FFXI::item_t* item = pInvReader->GetContainerItem(containerIndex, slotIndex);
        setContent << "\t\t\t<" << slotName;

        augmentData_t augment = createAugmentData(item);
        if (augment.path != 0)
        {
            setContent << " augpath=\"";
            setContent << (char)(augment.path + 64);
            setContent << '\"';
        }
        else if (augment.stringAugments.size() > 0)
        {
            setContent << " augment=\"";
            for (vector<string>::iterator it = augment.stringAugments.begin(); it != augment.stringAugments.end(); it++)
            {
                if (it != augment.stringAugments.begin())
                    setContent << ',';
                setContent << *it;
            }
            setContent << '\"';
        }
        setContent << '>';
        setContent << pResourceReader->GetItemById(item->Id)->Name[0];
        setContent << "</" << slotName << ">";

        if ((augment.path != 0) && (augment.stringAugments.size() > 0))
        {
            setContent << " <!--";
            for (vector<string>::iterator it = augment.stringAugments.begin(); it != augment.stringAugments.end(); it++)
            {
                if (it != augment.stringAugments.begin())
                    setContent << ',';
                setContent << *it;
            }
            setContent << "-->";
        }
        setContent << "\r\n";
    }
    setContent << "\t\t</set>";

    return setContent.str();
}
int ashitacast::getEquipSlot(string name)
{
    for (int x = 0; x < slotMax; x++)
    {
        if (gSlotNames[x] == name)
            return x;
    }
    return -1;
}
bool ashitacast::hasBag(int bagIndex)
{
    if (mConfig.mEnableWardrobeCheck)
    {
        if (bagIndex == 11)
        {
            DWORD Memloc = Read32(pWardrobe, 1);
            Memloc       = Read32(Memloc, 0);
            return ((Read8(Memloc, 0xB4) & 0x04) != 0);
        }
        else if (bagIndex == 12)
        {
            DWORD Memloc = Read32(pWardrobe, 1);
            Memloc       = Read32(Memloc, 0);
            return ((Read8(Memloc, 0xB4) & 0x08) != 0);
        }
    }
    return true;
}