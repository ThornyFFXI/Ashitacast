#include "ashitacast.h"


GearListEvent_t* ashitacast::CreateGearEvent()
{
    GearListEvent_t* gearEvent = new GearListEvent_t();
    gearEvent->EntryCount = 0;
    sprintf_s(gearEvent->Sender, 256, "Ashitacast4_%d", static_cast<int>(floor(this->GetVersion() * 100.0)));
    if (!pProfile->mIsLoaded)
    {
        pOutput->error("Could not create equipment list.  No XML was loaded.");
        delete gearEvent;
        return nullptr;
    }

    std::list<GearListEntry_t> entries;
    xml_node<>* node = pProfile->mXmlDocument.first_node("ashitacast");
    if (!node)
    {
        pOutput->error("Could not evaluate ashitacast profile.  Root ashitacast node not found.");
        delete gearEvent;
        return nullptr;
    }
    node = node->first_node("sets");
    if (!node)
    {
        pOutput->error("Could not evaluate ashitacast profile.  Sets node not found.");
        delete gearEvent;
        return nullptr;
    }

    for (node = node->first_node(); node; node = node->next_sibling())
    {
        if (_stricmp(node->name(), "set") == 0)
        {
            string setName        = "Unknown";
            xml_attribute<>* name = node->first_attribute("name");
            if (name)
                setName = name->value();
            list<int> foundSlots;

            list<GearListEntry_t> tempItems;
            for (xml_node<>* child = node->first_node(); child; child = child->next_sibling())
            {
                //Check that node is a valid equipment slot, and has not already been used in this set.
                int slot = getEquipSlot(child->name());
                if (slot == -1)
                {
                    pOutput->error_f("Unrecognized node found.  It will be ignored.  [Set:$H%s$R] [Node:$H%s$R]", setName.c_str(), child->name());
                    continue;
                }
                if (find(foundSlots.begin(), foundSlots.end(), slot) != foundSlots.end())
                {
                    pOutput->error_f("Same slot was used twice in a set.  Packer will only honor the first instance.  [Set:$H%s$R] [Node:$H%s$R]", setName.c_str(), child->name());
                    continue;
                }
                foundSlots.push_back(slot);

                //Skip blank nodes, keyword nodes, and nodes with a variable in their value.
                if ((strlen(child->value()) == 0) || (_stricmp(child->value(), "ignore") == 0) || (_stricmp(child->value(), "displaced") == 0) || (_stricmp(child->value(), "remove") == 0) || (strstr(child->value(), "$")))
                    continue;

                //Check that item name matches a valid node.
                IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemByName(child->value(), 0);
                if (!pResource)
                {
                    pOutput->error_f("Invalid item node found.  It will be ignored.  [Set:$H%s$R] [Node:$H%s$R] [Item:$H%s$R]", setName.c_str(), child->name(), child->value());
                    continue;
                }
                else if (!(pResource->Flags & 0x0800))
                {
                    pOutput->error_f("Unequippable item node found.  It will be ignored.  [Set:$H%s$R] [Node:$H%s$R] [Item:$H%s$R]", setName.c_str(), child->name(), child->value());
                    continue;
                }

                //Combine adds counts.  Ensures that if the same item is in a set multiple times, it gets counted appropriately.
                combineOrder(&tempItems, GearListEntry_t(child, pResource));
            }

            for (list<GearListEntry_t>::iterator iter = tempItems.begin(); iter != tempItems.end(); iter++)
            {
                //Integrate keeps highest count, we don't want to add every instance of the item being in any set.
                integrateOrder(&entries, *iter);
            }
        }
        else if (_stricmp(node->name(), "packer") == 0)
        {
            for (xml_node<>* child = node->first_node(); child; child = child->next_sibling())
            {
                if (_stricmp(child->name(), "item") == 0)
                {
                    IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemByName(child->value(), 0);
                    if (!pResource)
                    {
                        pOutput->error_f("Invalid item node found.  It will be ignored.  [Packer Section] [Node:$H%s$R] [Item:$H%s$R]", child->name(), child->value());
                        continue;
                    }

                    //Integrate keeps highest count, we don't want to add include to sets in case item is in both.
                    integrateOrder(&entries, GearListEntry_t(child, pResource));
                }
                else
                {
                    pOutput->error_f("Unrecognized node found.  It will be ignored.  [Packer Section] [Node:$H%s$R]", child->name());
                    continue;
                }
            }
        }
        else
        {
            pOutput->error_f("Unrecognized node found.  It will be ignored.  [Sets Section] [Node:$H%s$R]", node->name());
            continue;
        }
    }

    for (auto iter = entries.begin(); iter != entries.end(); iter++)
    {
        gearEvent->Entries[gearEvent->EntryCount] = *iter;
        gearEvent->EntryCount++;
    }

    return gearEvent;
}
void ashitacast::combineOrder(list<GearListEntry_t>* list, GearListEntry_t order)
{
    for (std::list<GearListEntry_t>::iterator iOrder = list->begin(); iOrder != list->end(); iOrder++)
    {
        if (*iOrder == order)
        {
            //Items match, so merge.
            if ((iOrder->Quantity == -1) || (order.Quantity == -1))
                iOrder->Quantity = -1;
            else
                iOrder->Quantity += order.Quantity;
            return;
        }
    }
    list->push_back(order);
}
void ashitacast::integrateOrder(list<GearListEntry_t>* list, GearListEntry_t order)
{
    for (std::list<GearListEntry_t>::iterator iOrder = list->begin(); iOrder != list->end(); iOrder++)
    {
        if (*iOrder == order)
        {
            //Items match, so take higher quantity.
            if ((iOrder->Quantity == -1) || (order.Quantity == -1))
                iOrder->Quantity = -1;
            else
                iOrder->Quantity = max(iOrder->Quantity, order.Quantity);
            return;
        }
    }
    list->push_back(order);
}