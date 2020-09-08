#include "ashitacast.h"

DWORD WINAPI callthread(LPVOID lpParameter)
{
    commandPass_t* data = (commandPass_t*)lpParameter;
    ::Sleep(data->commandDelay);

    if (data->isEcho)
    {
        data->ashitaCore->GetChatManager()->AddChatMessage(data->commandMode, false, data->commandText.c_str());
    }
    else
    {
        data->ashitaCore->GetChatManager()->QueueCommand(data->commandMode, data->commandText.c_str());
    }
    delete data;
    return NULL;
}

bool ashitacast::checkInputCommands(const char* text)
{
    //Attempt to pull the section from profile.
    xml_node<>* sectionNode = pProfile->getSection("commands");
    if (sectionNode == NULL)
        return false;

    xml_node<>* goal = NULL;
    xml_attribute<>* attr = NULL;

    for (xml_node<>* iter = sectionNode->first_node(); iter; iter = iter->next_sibling())
    {
        xml_attribute<>* tempAttr = iter->first_attribute("input");
        if ((tempAttr) && (_strnicmp(tempAttr->value(), text, strlen(tempAttr->value())) == 0))
        {
            if ((!goal) || (strlen(tempAttr->value()) < strlen(attr->value())))
            {
                goal = iter;
                attr = tempAttr;
            }
        }
    }

    if (!goal) return false;

    equipData_t currEquip[slotMax];
    pVariables->mArgData.args.clear();
    pVariables->mArgData.argCount = Ashita::Commands::GetCommandArgs(text, &(pVariables->mArgData.args));
    const char* args              = text + strlen(attr->value());
    while (args[0] == ' ')
        args++;
    pVariables->mArgData.fullargs = args;
    pVariables->mArgData.isActive = true;
    crawlNodes(goal, currEquip);
    pVariables->mArgData.isActive = false;
    return true;
}

void ashitacast::crawlSection(string section)
{
    //Attempt to pull the section from profile.
    xml_node<>* sectionNode = pProfile->getSection(section);
    if (sectionNode == NULL)
        return;
    else if ((section == "petspell") || (section == "petskill"))
        mCharacterState.mCurrentAction.cancel = false;

    //Create an array of pointer to nodes to hold goal equipment, then traverse nodes to fill it out.
    equipData_t currEquip[slotMax];
    crawlNodes(sectionNode, currEquip);

    //If the action will be cancelled, we don't want to bother equipping anything for it.
    if (mCharacterState.mCurrentAction.cancel)
        return;

    //NOTE: Legitimate client cannot send equip single on same slot twice in same packet interval.
    //So, precast/preranged will always use equip single, and midcast/midranged will always use equipset.
    //Everything else will use equipset if 9 or more pieces, or singles if less, to consume the least packet space.
    if (std::find(std::begin(gAutoNodes), std::end(gAutoNodes), section) != std::end(gAutoNodes))
    {
        equipAuto(currEquip);
    }
    else if (std::find(std::begin(gEquipNodes), std::end(gEquipNodes), section) != std::end(gEquipNodes))
    {
        equipSingles(currEquip);
    }
    else if (std::find(std::begin(gEquipSetNodes), std::end(gEquipSetNodes), section) != std::end(gEquipSetNodes))
    {
        equipSet(currEquip);
    }
}
bool ashitacast::crawlNodes(xml_node<>* parentNode, equipData_t* currEquip)
{
    bool inElseChain  = false;
    bool lastNodeIsIf = false;

    for (xml_node<>* activeNode = parentNode->first_node(); activeNode; activeNode = activeNode->next_sibling())
    {
        if (_stricmp(activeNode->name(), "if") == 0)
        {
            lastNodeIsIf = true;
            if (evaluateNode(activeNode))
            {
                inElseChain = false;
                if (crawlNodes(activeNode, currEquip))
                    return true;
            }
            else
                inElseChain = true;
        }
        else if (_stricmp(activeNode->name(), "elseif") == 0)
        {
            if (!lastNodeIsIf)
            {
                pOutput->error("Elseif node must follow an if or elseif node.");
            }

            lastNodeIsIf = true;
            if ((inElseChain) && (evaluateNode(activeNode)))
            {
                inElseChain = false;
                if (crawlNodes(activeNode, currEquip))
                    return true;
            }
        }
        else if (_stricmp(activeNode->name(), "else") == 0)
        {
            if (!lastNodeIsIf)
            {
                pOutput->error("Else node must follow an if or elseif node.");
            }

            lastNodeIsIf = false;
            if (inElseChain)
            {
                inElseChain = false;
                if (crawlNodes(activeNode, currEquip))
                    return true;
            }
        }
        else if (_stricmp(activeNode->name(), "return") == 0)
            return true;
        else
        {
            inElseChain  = false;
            lastNodeIsIf = false;
            processNode(activeNode, currEquip);
        }
    }

    return false;
}
bool ashitacast::evaluateNode(xml_node<>* node)
{
    xml_attribute<>* attr = node->first_attribute("mode");
    bool orMode           = ((attr) && (_stricmp(attr->value(), "OR") == 0));

    for (xml_attribute<>* iter = node->first_attribute(); iter; iter = iter->next_attribute())
    {
        if (_stricmp(iter->name(), "mode") == 0)
            continue;

        bool evaluate = false;

        if (_stricmp(iter->name(), "advanced") != 0)
        {
            evaluate = pVariables->processBasic(iter->name(), iter->value());
        }

        else
        {
            evaluate = pVariables->processAdvanced(iter->value());
        }

        if ((orMode) && (evaluate))
            return true;

        if ((!orMode) && (!evaluate))
            return false;
    }

    return !orMode;
}
void ashitacast::processNode(xml_node<>* node, equipData_t* currEquip)
{
    if (_stricmp(node->name(), "equip") == 0)
    {
        processEquipNode(node, currEquip);
        return;
    }

    std::map<string, xmlNodeHandler>::iterator iter = mNodeMap.find(node->name());
    if (iter == mNodeMap.end())
    {
        pOutput->error_f("Node not recognized. [$H%s$R]", node->name());
        return;
    }
    (this->*iter->second)(node);
}
void ashitacast::processEquipNode(xml_node<>* parent, equipData_t* currEquip)
{
    xml_attribute<>* equipSet = parent->first_attribute("set");
    if (equipSet)
        processEquipSet(pVariables->resolveVariables(equipSet->value()), currEquip, parent);
    crawlEquipTags(parent, currEquip, parent);
}
void ashitacast::processEquipSet(string set, equipData_t* currEquip, xml_node<>* equipTag)
{
    xml_node<>* sectionNode = pProfile->getSection("sets");
    if (sectionNode == NULL)
        return;

    for (xml_node<>* activeNode = sectionNode->first_node(); activeNode; activeNode = activeNode->next_sibling())
    {
        xml_attribute<>* name = activeNode->first_attribute("name");
        if ((name) && (_stricmp(name->value(), set.c_str()) == 0))
        {
            //Look for baseset parameter and equip if so.
            xml_attribute<>* baseSet = activeNode->first_attribute("baseset");
            if (baseSet)
                processEquipSet(baseSet->value(), currEquip, equipTag);
            crawlEquipTags(activeNode, currEquip, equipTag);
            break;
        }
    }
}
void ashitacast::crawlEquipTags(xml_node<>* parent, equipData_t* currEquip, xml_node<>* equipTag)
{
    //Don't bother reading equip tags if we have a gearlock or /ac set in place.
    if (std::chrono::steady_clock::now() < mCharacterState.gearLockDelay)
        return;
    //Don't read equip tags if we've got a pet override in place.
    if ((mCharacterState.petOverride) && (std::chrono::steady_clock::now() < mCharacterState.petActionDelay))
        return;

    bool lock                    = false;
    int priority                 = 0;
    if (equipTag)
    {
        xml_attribute<>* lockTag = equipTag->first_attribute("lock");
        if (lockTag)
            lock = (_stricmp(lockTag->value(), "true") == 0);
        xml_attribute<>* priorityTag = equipTag->first_attribute("priority");
        if (priorityTag)
            priority = atoi(priorityTag->value());
    }

    for (xml_node<>* equipNode = parent->first_node(); equipNode; equipNode = equipNode->next_sibling())
    {
        int slot = getEquipSlot(equipNode->name());
        if (slot == -1)
            continue;

        equipData_t equipTemp = createEquipData(equipNode, slot);

        //If the individual piece does not have a priority or lock setting, defer to the equip tag.
        if (equipNode->first_attribute("lock") == NULL)
            equipTemp.locked = lock;

        if (equipNode->first_attribute("priority") == NULL)
            equipTemp.priority = priority;

        //Only overwrite the active piece if it is not locked, or this piece is also locked.
        if ((!currEquip[slot].locked) || (equipTemp.locked))
            currEquip[slot] = equipTemp;
    }
}
void ashitacast::processCommand(xml_node<>* node)
{
    int mode              = -1;
    xml_attribute<>* attr = node->first_attribute("mode");
    if (attr)
        mode = atoi(attr->value());

    std::string content = pVariables->resolveVariables(node->value());

    attr = node->first_attribute("delay");
    if (attr)
    {
        int delay = atoi(attr->value());
        if (delay < 1)
            delay = 1;
        commandPass_t* command = new commandPass_t();
        command->isEcho        = false;
        command->commandMode   = mode;
        command->commandDelay  = delay;
        command->commandText   = content;
        command->ashitaCore    = m_AshitaCore;
        HANDLE thread          = CreateThread(NULL, 0, callthread, command, 0, 0);
    }
    else
    {
        m_AshitaCore->GetChatManager()->QueueCommand(mode, content.c_str());
    }
}
void ashitacast::processSetVar(xml_node<>* node)
{
    xml_attribute<>* name  = node->first_attribute("name");
    xml_attribute<>* value = node->first_attribute("value");
    if (name && value)
        pVariables->setVariable(name->value(), pVariables->resolveVariables(value->value()));
}
void ashitacast::processClearVar(xml_node<>* node)
{
    xml_attribute<>* name = node->first_attribute("name");
    xml_attribute<>* all  = node->first_attribute("all");
    if ((all) && (_stricmp(all->value(), "true") == 0))
        pVariables->clearVariables();
    else
        pVariables->clearVariable(name->value());
}
void ashitacast::processChange(xml_node<>* node)
{
    if ((mCharacterState.mCurrentAction.active) && (!mCharacterState.mCurrentAction.postCast))
    {
        if (mCharacterState.mCurrentAction.type == actionType::Ability)
        {
            xml_attribute<>* nameTag = node->first_attribute("name");
            if (nameTag)
            {
                IAbility* abilityResource = m_AshitaCore->GetResourceManager()->GetAbilityByName(pVariables->resolveVariables(nameTag->value()).c_str(), 0);
                if (abilityResource)
                {
                    mCharacterState.mCurrentAction.abilityId = abilityResource->Id - 0x200;
                    mCharacterState.mCurrentAction.raw.param = mCharacterState.mCurrentAction.abilityId;
                }
                else
                {
                    pOutput->error_f("Could not process change tag.  Ability not found [$H%s$R].", nameTag->value());
                }
            }

            xml_attribute<>* idTag = node->first_attribute("id");
            if (idTag)
            {
                int id                    = atoi(idTag->value());
                IAbility* abilityResource = m_AshitaCore->GetResourceManager()->GetAbilityById(id + 0x200);
                if (abilityResource)
                {
                    mCharacterState.mCurrentAction.abilityId = id;
                    mCharacterState.mCurrentAction.raw.param = mCharacterState.mCurrentAction.abilityId;
                }
                else
                {
                    pOutput->error_f("Could not process change tag.  Ability not found [$HID:%d$R].", id);
                }
            }
        }
        else if (mCharacterState.mCurrentAction.type == actionType::Spell)
        {
            xml_attribute<>* nameTag = node->first_attribute("name");
            if (nameTag)
            {
                ISpell* spellResource = m_AshitaCore->GetResourceManager()->GetSpellByName(pVariables->resolveVariables(nameTag->value()).c_str(), 0);
                if (spellResource)
                {
                    mCharacterState.mCurrentAction.abilityId = spellResource->Id;
                    mCharacterState.mCurrentAction.raw.param = mCharacterState.mCurrentAction.abilityId;
                }
                else
                {
                    pOutput->error_f("Could not process change tag.  Spell not found [$H%s$R].", nameTag->value());
                }
            }

            xml_attribute<>* idTag = node->first_attribute("id");
            if (idTag)
            {
                int id                = atoi(idTag->value());
                ISpell* spellResource = m_AshitaCore->GetResourceManager()->GetSpellById(id);
                if (spellResource)
                {
                    mCharacterState.mCurrentAction.abilityId = id;
                    mCharacterState.mCurrentAction.raw.param = mCharacterState.mCurrentAction.abilityId;
                }
                else
                {
                    pOutput->error_f("Could not process change tag.  Spell not found [$HID:%d$R].", id);
                }
            }
        }
        else if (mCharacterState.mCurrentAction.type == actionType::Weaponskill)
        {
            xml_attribute<>* nameTag = node->first_attribute("name");
            if (nameTag)
            {
                IAbility* abilityResource = m_AshitaCore->GetResourceManager()->GetAbilityByName(pVariables->resolveVariables(nameTag->value()).c_str(), 0);
                if (abilityResource)
                {
                    mCharacterState.mCurrentAction.abilityId = abilityResource->Id;
                    mCharacterState.mCurrentAction.raw.param = mCharacterState.mCurrentAction.abilityId;
                }
                else
                {
                    pOutput->error_f("Could not process change tag.  Weaponskill not found [$H%s$R].", nameTag->value());
                }
            }

            xml_attribute<>* idTag = node->first_attribute("id");
            if (idTag)
            {
                int id                    = atoi(idTag->value());
                IAbility* abilityResource = m_AshitaCore->GetResourceManager()->GetAbilityById(id);
                if (abilityResource)
                {
                    mCharacterState.mCurrentAction.abilityId = id;
                    mCharacterState.mCurrentAction.raw.param = mCharacterState.mCurrentAction.abilityId;
                }
                else
                {
                    pOutput->error_f("Could not process change tag.  Weaponskill not found [$HID:%d$R].", id);
                }
            }
        }
        else
        {
            pOutput->error("Invalid use of change tag.");
        }
    }
    else
    {
        pOutput->error("Invalid use of change tag.");
    }
}
void ashitacast::processEcho(xml_node<>* node)
{
    int mode              = 0;
    xml_attribute<>* attr = node->first_attribute("mode");
    if (attr)
        mode = atoi(attr->value());

    std::string content = pVariables->resolveVariables(node->value());

    attr = node->first_attribute("color");
    if (attr)
    {
        int color = min(0, max(511, atoi(attr->value())));
        if (color < 256)
            content = Ashita::Chat::Color1((uint8_t)color, content);
        else
            content = Ashita::Chat::Color2((uint8_t)(color & 0xFF), content);
    }

    attr = node->first_attribute("delay");
    if (attr)
    {
        int delay = atoi(attr->value());
        if (delay < 1)
            delay = 1;
        commandPass_t* command = new commandPass_t();
        command->isEcho        = true;
        command->commandMode   = mode;
        command->commandDelay  = delay;
        command->commandText   = content;
        command->ashitaCore    = m_AshitaCore;
        HANDLE thread          = CreateThread(NULL, 0, callthread, command, 0, 0);
    }
    else
    {
        m_AshitaCore->GetChatManager()->AddChatMessage(mode, false, content.c_str());
    }
}
void ashitacast::processGearLock(xml_node<>* node)
{
    xml_attribute<>* length = node->first_attribute("length");
    if (length)
    {
        int duration = atoi(length->value());
        if (duration < 0)
            return;
        if (duration > 3600000)
            duration = 3600000;
        mCharacterState.gearLockDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
    }
}
void ashitacast::processRegisterBuff(xml_node<>* node)
{
    int16_t buffId = -1;

    xml_attribute<>* nameTag = node->first_attribute("name");
    if (nameTag)
    {
        buffId = m_AshitaCore->GetResourceManager()->GetString("buffs", nameTag->value());
    }

    xml_attribute<>* idTag = node->first_attribute("id");
    if (idTag)
    {
        int id = atoi(idTag->value());
        if ((id >= 0) && (id < 1000))
            buffId = id;
    }

    bool active                = true;
    xml_attribute<>* activeTag = node->first_attribute("active");
    if ((activeTag) && (_stricmp(activeTag->value(), "false") == 0))
        active = false;

    if (buffId != -1)
        pVariables->registerBuff(buffId, active, 2000);
}
void ashitacast::processClearBuff(xml_node<>* node)
{
    int16_t buffId = -1;

    xml_attribute<>* nameTag = node->first_attribute("name");
    if (nameTag)
    {
        buffId = m_AshitaCore->GetResourceManager()->GetString("buffs", nameTag->value());
    }

    xml_attribute<>* idTag = node->first_attribute("id");
    if (idTag)
    {
        int id = atoi(idTag->value());
        if ((id >= 0) && (id < 1000))
            buffId = id;
    }

    if (buffId != -1)
        pVariables->clearBuff(buffId);
}
void ashitacast::processCancel(xml_node<>* node)
{
    if ((mCharacterState.mCurrentAction.type == actionType::PetSkill_Ability) || (mCharacterState.mCurrentAction.type == actionType::PetSkill_MobSkill) || (mCharacterState.mCurrentAction.type == actionType::PetSpell))
    {
        mCharacterState.mCurrentAction.cancel = true;
    }
    else if ((mCharacterState.mCurrentAction.active) && (!mCharacterState.mCurrentAction.postCast))
    {
        mCharacterState.mCurrentAction.cancel = true;
    }
    else
    {
        pOutput->error("Invalid use of cancel tag.");
    }
}
void ashitacast::processOverride(xml_node<>* node)
{
    if ((mCharacterState.mCurrentAction.type == actionType::PetSkill_Ability) || (mCharacterState.mCurrentAction.type == actionType::PetSkill_MobSkill) || (mCharacterState.mCurrentAction.type == actionType::PetSpell))
    {
        mCharacterState.petOverride = true;
    }
    else
    {
        pOutput->error("Override tag can only be used during a pet action.");
    }
}