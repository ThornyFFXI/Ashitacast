#include "ashitacastVariables.h"
#include "variableConstants.h"

void ashitacastVariables::initializeResolverMap()
{
    mResolverMap.insert(std::make_pair("a_actiontarget", &ashitacastVariables::var_a_actiontarget));
    mResolverMap.insert(std::make_pair("a_count", &ashitacastVariables::var_a_count));
    mResolverMap.insert(std::make_pair("a_inally", &ashitacastVariables::var_a_inally));
    mResolverMap.insert(std::make_pair("a_target", &ashitacastVariables::var_a_target));
    mResolverMap.insert(std::make_pair("ad_casttime", &ashitacastVariables::var_ad_casttime));
    mResolverMap.insert(std::make_pair("ad_element", &ashitacastVariables::var_ad_element));
    mResolverMap.insert(std::make_pair("ad_id", &ashitacastVariables::var_ad_id));
    mResolverMap.insert(std::make_pair("ad_mpaftercast", &ashitacastVariables::var_ad_mpaftercast));
    mResolverMap.insert(std::make_pair("ad_mpcost", &ashitacastVariables::var_ad_mpcost));
    mResolverMap.insert(std::make_pair("ad_mppaftercast", &ashitacastVariables::var_ad_mppaftercast));
    mResolverMap.insert(std::make_pair("ad_name", &ashitacastVariables::var_ad_name));
    mResolverMap.insert(std::make_pair("ad_recast", &ashitacastVariables::var_ad_recast));
    mResolverMap.insert(std::make_pair("ad_skill", &ashitacastVariables::var_ad_skill));
    mResolverMap.insert(std::make_pair("ad_type", &ashitacastVariables::var_ad_type));
    mResolverMap.insert(std::make_pair("at_distance", &ashitacastVariables::var_at_distance));
    mResolverMap.insert(std::make_pair("at_hpp", &ashitacastVariables::var_at_hpp));
    mResolverMap.insert(std::make_pair("at_id", &ashitacastVariables::var_at_id));
    mResolverMap.insert(std::make_pair("at_index", &ashitacastVariables::var_at_index));
    mResolverMap.insert(std::make_pair("at_name", &ashitacastVariables::var_at_name));
    mResolverMap.insert(std::make_pair("at_status", &ashitacastVariables::var_at_status));
    mResolverMap.insert(std::make_pair("at_type", &ashitacastVariables::var_at_type));
    mResolverMap.insert(std::make_pair("e_area", &ashitacastVariables::var_e_area));
    mResolverMap.insert(std::make_pair("e_day", &ashitacastVariables::var_e_day));
    mResolverMap.insert(std::make_pair("e_dayelement", &ashitacastVariables::var_e_dayelement));
    mResolverMap.insert(std::make_pair("e_moon", &ashitacastVariables::var_e_moon));
    mResolverMap.insert(std::make_pair("e_moonpct", &ashitacastVariables::var_e_moonpct));
    mResolverMap.insert(std::make_pair("e_time", &ashitacastVariables::var_e_time));
    mResolverMap.insert(std::make_pair("e_weather", &ashitacastVariables::var_e_weather));
    mResolverMap.insert(std::make_pair("e_weatherelement", &ashitacastVariables::var_e_weatherelement));
    mResolverMap.insert(std::make_pair("eq_ammo", &ashitacastVariables::var_eq_ammo));
    mResolverMap.insert(std::make_pair("eq_back", &ashitacastVariables::var_eq_back));
    mResolverMap.insert(std::make_pair("eq_body", &ashitacastVariables::var_eq_body));
    mResolverMap.insert(std::make_pair("eq_ear1", &ashitacastVariables::var_eq_ear1));
    mResolverMap.insert(std::make_pair("eq_ear2", &ashitacastVariables::var_eq_ear2));
    mResolverMap.insert(std::make_pair("eq_feet", &ashitacastVariables::var_eq_feet));
    mResolverMap.insert(std::make_pair("eq_hands", &ashitacastVariables::var_eq_hands));
    mResolverMap.insert(std::make_pair("eq_head", &ashitacastVariables::var_eq_head));
    mResolverMap.insert(std::make_pair("eq_legs", &ashitacastVariables::var_eq_legs));
    mResolverMap.insert(std::make_pair("eq_main", &ashitacastVariables::var_eq_main));
    mResolverMap.insert(std::make_pair("eq_neck", &ashitacastVariables::var_eq_neck));
    mResolverMap.insert(std::make_pair("eq_range", &ashitacastVariables::var_eq_range));
    mResolverMap.insert(std::make_pair("eq_ring1", &ashitacastVariables::var_eq_ring1));
    mResolverMap.insert(std::make_pair("eq_ring2", &ashitacastVariables::var_eq_ring2));
    mResolverMap.insert(std::make_pair("eq_sub", &ashitacastVariables::var_eq_sub));
    mResolverMap.insert(std::make_pair("eq_waist", &ashitacastVariables::var_eq_waist));
    mResolverMap.insert(std::make_pair("pet_active", &ashitacastVariables::var_pet_active));
    mResolverMap.insert(std::make_pair("pet_hpp", &ashitacastVariables::var_pet_hpp));
    mResolverMap.insert(std::make_pair("pet_tp", &ashitacastVariables::var_pet_tp));
    mResolverMap.insert(std::make_pair("pet_status", &ashitacastVariables::var_pet_status));
    mResolverMap.insert(std::make_pair("pet_name", &ashitacastVariables::var_pet_name));
    mResolverMap.insert(std::make_pair("pet_distance", &ashitacastVariables::var_pet_distance));
    mResolverMap.insert(std::make_pair("p_attack", &ashitacastVariables::var_p_attack));
    mResolverMap.insert(std::make_pair("p_darkresist", &ashitacastVariables::var_p_darkresist));
    mResolverMap.insert(std::make_pair("p_defense", &ashitacastVariables::var_p_defense));
    mResolverMap.insert(std::make_pair("p_earthresist", &ashitacastVariables::var_p_earthresist));
    mResolverMap.insert(std::make_pair("p_fireresist", &ashitacastVariables::var_p_fireresist));
    mResolverMap.insert(std::make_pair("p_hp", &ashitacastVariables::var_p_hp));
    mResolverMap.insert(std::make_pair("p_hpmax", &ashitacastVariables::var_p_hpmax));
    mResolverMap.insert(std::make_pair("p_hpp", &ashitacastVariables::var_p_hpp));
    mResolverMap.insert(std::make_pair("p_iceresist", &ashitacastVariables::var_p_iceresist));
    mResolverMap.insert(std::make_pair("p_ismoving", &ashitacastVariables::var_p_ismoving));
    mResolverMap.insert(std::make_pair("p_job", &ashitacastVariables::var_p_job));
    mResolverMap.insert(std::make_pair("p_level", &ashitacastVariables::var_p_level));
    mResolverMap.insert(std::make_pair("p_lightningresist", &ashitacastVariables::var_p_lightningresist));
    mResolverMap.insert(std::make_pair("p_lightresist", &ashitacastVariables::var_p_lightresist));
    mResolverMap.insert(std::make_pair("p_mainjob", &ashitacastVariables::var_p_mainjob));
    mResolverMap.insert(std::make_pair("p_mp", &ashitacastVariables::var_p_mp));
    mResolverMap.insert(std::make_pair("p_mpmax", &ashitacastVariables::var_p_mpmax));
    mResolverMap.insert(std::make_pair("p_mpp", &ashitacastVariables::var_p_mpp));
    mResolverMap.insert(std::make_pair("p_name", &ashitacastVariables::var_p_name));
    mResolverMap.insert(std::make_pair("p_status", &ashitacastVariables::var_p_status));
    mResolverMap.insert(std::make_pair("p_subjob", &ashitacastVariables::var_p_subjob));
    mResolverMap.insert(std::make_pair("p_subjoblevel", &ashitacastVariables::var_p_subjoblevel));
    mResolverMap.insert(std::make_pair("p_tp", &ashitacastVariables::var_p_tp));
    mResolverMap.insert(std::make_pair("p_waterresist", &ashitacastVariables::var_p_waterresist));
    mResolverMap.insert(std::make_pair("p_windresist", &ashitacastVariables::var_p_windresist));
    mResolverMap.insert(std::make_pair("pt_actiontarget", &ashitacastVariables::var_pt_actiontarget));
    mResolverMap.insert(std::make_pair("pt_count", &ashitacastVariables::var_pt_count));
    mResolverMap.insert(std::make_pair("pt_inparty", &ashitacastVariables::var_pt_inparty));
    mResolverMap.insert(std::make_pair("pt_target", &ashitacastVariables::var_pt_target));
    mResolverMap.insert(std::make_pair("t_distance", &ashitacastVariables::var_t_distance));
    mResolverMap.insert(std::make_pair("t_hpp", &ashitacastVariables::var_t_hpp));
    mResolverMap.insert(std::make_pair("t_id", &ashitacastVariables::var_t_id));
    mResolverMap.insert(std::make_pair("t_index", &ashitacastVariables::var_t_index));
    mResolverMap.insert(std::make_pair("t_name", &ashitacastVariables::var_t_name));
    mResolverMap.insert(std::make_pair("t_status", &ashitacastVariables::var_t_status));
    mResolverMap.insert(std::make_pair("t_type", &ashitacastVariables::var_t_type));
}

string ashitacastVariables::var_a_actiontarget()
{
    if (mCurrentAction->active)
    {
        for (int x = 0; x < 18; x++)
        {
            if (pParty->GetMemberTargetIndex(x) == mCurrentAction->target)
                return "true";
        }
    }
    return "false";
}

string ashitacastVariables::var_a_count()
{
    int count = 0;
    for (int x = 0; x < 18; x++)
    {
        if (pParty->GetMemberIsActive(x))
            count++;
    }
    return std::to_string(count);
}

string ashitacastVariables::var_a_inally()
{
    for (int x = 6; x < 18; x++)
    {
        if (pParty->GetMemberIsActive(x))
            return "true";
    }
    return "false";
}

string ashitacastVariables::var_a_target()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "false";

    for (int x = 0; x < 18; x++)
    {
        if ((pParty->GetMemberIsActive(x)) && (pParty->GetMemberTargetIndex(x) == targetIndex))
            return "true";
    }

    return "false";
}

string ashitacastVariables::var_ad_casttime()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            return to_string(spellResource->CastTime * 250);
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_element()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            map<uint16_t, string>::iterator find = gSpellElements.find(spellResource->Element);
            if (find == gSpellElements.end())
                return "ELEMENT_INDEX_ERROR";
            return find->second;
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_id()
{
    if (mCurrentAction->active)
    {
        return to_string(mCurrentAction->abilityId);
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_mpaftercast()
{
    if (mCurrentAction->active)
    {
        if (mCurrentAction->type == actionType::Spell)
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            return to_string(pParty->GetMemberMP(0) - spellResource->ManaCost);
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_mpcost()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            return to_string(spellResource->ManaCost);
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_mppaftercast()
{
    if (mCurrentAction->active)
    {
        if (mCurrentAction->type == actionType::Spell)
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            int remainingMp = pParty->GetMemberMP(0) - spellResource->ManaCost;
            int percent     = (remainingMp * 100) / pPlayer->GetMPMax();
            return to_string(percent);
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_name()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            return spellResource->Name[0];
        }
        else if ((mCurrentAction->type == actionType::Ability) || (mCurrentAction->type == actionType::PetSkill_Ability))
        {
            IAbility* abilityResource = pResource->GetAbilityById(mCurrentAction->abilityId + 0x200);
            if (!abilityResource)
                return "UNRESOLVED_RESOURCE";
            return abilityResource->Name[0];
        }
        else if (mCurrentAction->type == actionType::Weaponskill)
        {
            IAbility* wsResource = pResource->GetAbilityById(mCurrentAction->abilityId);
            if (!wsResource)
                return "UNRESOLVED_RESOURCE";
            return wsResource->Name[0];
        }
        else if (mCurrentAction->type == actionType::Item)
        {
            IItem* itemResource = pResource->GetItemById(mCurrentAction->abilityId);
            if (!itemResource)
                return "UNRESOLVED_RESOURCE";
            return itemResource->Name[0];
        }
        else if (mCurrentAction->type == actionType::PetSkill_MobSkill)
        {
            const char* mobSkillName = pResource->GetString("monsters.abilities", mCurrentAction->abilityId);
            if (mobSkillName == NULL)
                return "UNRESOLVED_STRING";
            return mobSkillName;
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_recast()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            return to_string(spellResource->RecastDelay * 250);
        }
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_skill()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            map<uint16_t, string>::iterator find = gSpellSkills.find(spellResource->Skill);
            if (find == gSpellSkills.end())
                return "SKILL_INDEX_ERROR";
            return find->second;
        }
    }

    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_ad_type()
{
    if (mCurrentAction->active)
    {
        if ((mCurrentAction->type == actionType::Spell) || (mCurrentAction->type == actionType::PetSpell))
        {
            ISpell* spellResource = pResource->GetSpellById(mCurrentAction->abilityId);
            if (!spellResource)
                return "UNRESOLVED_RESOURCE";
            map<uint16_t, string>::iterator find = gSpellTypes.find(spellResource->Type);
            if (find == gSpellTypes.end())
                return "SPELL_TYPE_INDEX_ERROR";
            return find->second;
        }
        else if ((mCurrentAction->type == actionType::Ability) || (mCurrentAction->type == actionType::PetSkill_Ability))
        {
            IAbility* abilityResource = pResource->GetAbilityById(mCurrentAction->abilityId + 0x200);
            if (!abilityResource)
                return "UNRESOLVED_RESOURCE";

            map<uint16_t, string>::iterator find = gAbilTypes.find(abilityResource->RecastTimerId);
            if (find == gAbilTypes.end())
                return "ABIL_TYPE_INDEX_ERROR";
            return find->second;
        }
        else if (mCurrentAction->type == actionType::PetSkill_MobSkill)
        {
            IAbility* abilityResource = NULL;
            const char* mobSkillName  = pResource->GetString("monsters.abilities", mCurrentAction->abilityId);
            for (int x = 0; x < ABILITY_MAX; x++)
            {
                IAbility* tempResource = pResource->GetAbilityById(x + 0x200);
                if ((tempResource) && (strcmp(tempResource->Name[0], mobSkillName) == 0))
                {
                    abilityResource = tempResource;
                    break;
                }
            }
            if (!abilityResource)
                return "UNUNRESOLVED_RESOURCE";

            map<uint16_t, string>::iterator find = gAbilTypes.find(abilityResource->RecastTimerId);
            if (find == gAbilTypes.end())
                return "ABIL_TYPE_INDEX_ERROR";
            return find->second;
        }
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_distance()
{
    if (mCurrentAction->active)
    {
        double actionTargetDistance = pEntity->GetDistance(mCurrentAction->target);
        return to_string(sqrt(actionTargetDistance));
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_hpp()
{
    if (mCurrentAction->active)
    {
        return to_string(pEntity->GetHPPercent(mCurrentAction->target));
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_id()
{
    if (mCurrentAction->active)
    {
        return to_string(pEntity->GetServerId(mCurrentAction->target));
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_index()
{
    if (mCurrentAction->active)
    {
        return to_string(mCurrentAction->target);
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_name()
{
    if (mCurrentAction->active)
    {
        return pEntity->GetName(mCurrentAction->target);
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_status()
{
    if (mCurrentAction->active)
    {
        map<uint32_t, string>::iterator find = gEntityStatus.find(pEntity->GetStatus(mCurrentAction->target));
        if (find == gEntityStatus.end())
            return "ENTITY_STATUS_ERROR";
        return find->second;
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_at_type()
{
    if (mCurrentAction->active)
    {
        if (mCurrentAction->target == pParty->GetMemberTargetIndex(0))
            return "self";
        
        uint32_t type = pEntity->GetSpawnFlags(mCurrentAction->target);
        map<uint32_t, string>::iterator find = gSpawnFlags.find(type);
        if (find == gSpawnFlags.end())
            return "SPAWN_FLAGS_ERROR";
        return find->second;
    }
    return "UNRESOLVED_VARIABLE";
}

string ashitacastVariables::var_e_area()
{
    return pResource->GetString("zones.names", pParty->GetMemberZone(0));
}

string ashitacastVariables::var_e_day()
{
    uint8_t weekDay = getTimeStamp().day % 8;

    map<uint8_t, string>::iterator find = gWeekDay.find(weekDay);
    if (find == gWeekDay.end())
    {
        return "WEEK_DAY_ERROR";
    }
    return find->second;
}

string ashitacastVariables::var_e_dayelement()
{
    uint8_t weekDay = getTimeStamp().day % 8;

    map<uint8_t, string>::iterator find = gWeekDayElement.find(weekDay);
    if (find == gWeekDayElement.end())
    {
        return "WEEK_DAY_ELEMENT_ERROR";
    }
    return find->second;
}

string ashitacastVariables::var_e_moon()
{
    int32_t moonPhase                    = (getTimeStamp().day + 26) % 84;
    map<int32_t, string>::iterator find = gMoonPhase.find(moonPhase);
    if (find == gMoonPhase.end())
    {
        return "MOON_PHASE_ERROR";
    }
    return find->second;
}

string ashitacastVariables::var_e_moonpct()
{
    int32_t  moonPhase = (getTimeStamp().day + 26) % 84;
    map<int32_t, uint8_t>::iterator find = gMoonPhasePct.find(moonPhase);
    if (find == gMoonPhasePct.end())
    {
        return "MOON_PHASE_PERCENT_ERROR";
    }
    return to_string(find->second);
}

string ashitacastVariables::var_e_time()
{
    ffxiDateTime currTime = getTimeStamp();
    stringstream output;
    output << setw(2) << std::setfill('0') << to_string(currTime.hour);
    output << '.';
    output << setw(2) << std::setfill('0') << to_string(currTime.minute);
    return output.str();
}

string ashitacastVariables::var_e_weather()
{
    for (int x = 0; x < 32; x++)
    {
        map<int16_t, string>::iterator find = gStormWeather.find(pPlayer->GetBuffs()[x]);
        if (find != gStormWeather.end())
            return find->second;
    }

    map<uint8_t, string>::iterator find = gWeather.find(getWeather());
    if (find == gWeather.end())
        return "WEATHER_ERROR";
    return find->second;
}

string ashitacastVariables::var_e_weatherelement()
{
    for (int x = 0; x < 32; x++)
    {
        map<int16_t, string>::iterator find = gStormWeatherElement.find(pPlayer->GetBuffs()[x]);
        if (find != gStormWeatherElement.end())
            return find->second;
    }

    map<uint8_t, string>::iterator find = gWeatherElement.find(getWeather());
    if (find == gWeatherElement.end())
        return "WEATHER_ELEMENT_ERROR";
    return find->second;
}

string ashitacastVariables::var_eq_ammo()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Ammo);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_back()
{
    equipRegistry_t equip      = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Back);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_body()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Body);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_ear1()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Ear1);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_ear2()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Ear2);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_feet()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Feet);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_hands()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Hands);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_head()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Head);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_legs()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Legs);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_main()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Main);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_neck()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Neck);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_range()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Range);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_ring1()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Ring1);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_ring2()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Ring2);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_sub()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Sub);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_eq_waist()
{
    equipRegistry_t equip = getEquippedItem((int)Ashita::FFXI::Enums::EquipmentSlot::Waist);
    if (equip.index == 0)
        return "empty";
    Ashita::FFXI::item_t* item = pInventory->GetContainerItem(equip.container, equip.index);
    IItem* resource            = pResource->GetItemById(item->Id);
    if (!resource)
        return "ITEM_RESOURCE_ERROR";
    return resource->Name[0];
}

string ashitacastVariables::var_pet_active()
{
    int myIndex = pParty->GetMemberTargetIndex(0);
    int petIndex = pEntity->GetPetTargetIndex(myIndex);
    if ((!petIndex) || (pEntity->GetRawEntity(petIndex) == NULL))
        return "false";
    return "true";
}

string ashitacastVariables::var_pet_hpp()
{
    int myIndex  = pParty->GetMemberTargetIndex(0);
    int petIndex = pEntity->GetPetTargetIndex(myIndex);
    if ((!petIndex) || (pEntity->GetRawEntity(petIndex) == NULL))
        return "PET_DOESNT_EXIST";
    return to_string(pEntity->GetHPPercent(petIndex));
}

string ashitacastVariables::var_pet_tp()
{
    int myIndex  = pParty->GetMemberTargetIndex(0);
    int petIndex = pEntity->GetPetTargetIndex(myIndex);
    if ((!petIndex) || (pEntity->GetRawEntity(petIndex) == NULL))
        return "PET_DOESNT_EXIST";
    return to_string(pPlayer->GetPetTP());
}

string ashitacastVariables::var_pet_status()
{
    int myIndex  = pParty->GetMemberTargetIndex(0);
    int petIndex = pEntity->GetPetTargetIndex(myIndex);
    if ((!petIndex) || (pEntity->GetRawEntity(petIndex) == NULL))
        return "PET_DOESNT_EXIST";

    map<uint32_t, string>::iterator find = gEntityStatus.find(pEntity->GetStatus(petIndex));
    if (find == gEntityStatus.end())
        return "ENTITY_STATUS_ERROR";
    return find->second;
}

string ashitacastVariables::var_pet_name()
{
    int myIndex  = pParty->GetMemberTargetIndex(0);
    int petIndex = pEntity->GetPetTargetIndex(myIndex);
    if ((!petIndex) || (pEntity->GetRawEntity(petIndex) == NULL))
        return "PET_DOESNT_EXIST";
    return pEntity->GetName(petIndex);
}

string ashitacastVariables::var_pet_distance()
{
    int myIndex  = pParty->GetMemberTargetIndex(0);
    int petIndex = pEntity->GetPetTargetIndex(myIndex);
    if ((!petIndex) || (pEntity->GetRawEntity(petIndex) == NULL))
        return "PET_DOESNT_EXIST";
    return to_string(sqrt(pEntity->GetDistance(petIndex)));
}

string ashitacastVariables::var_p_attack()
{
    return to_string(pPlayer->GetAttack());
}

string ashitacastVariables::var_p_darkresist()
{
    return to_string(pPlayer->GetResist(7));
}

string ashitacastVariables::var_p_defense()
{
    return to_string(pPlayer->GetDefense());
}

string ashitacastVariables::var_p_earthresist()
{
    return to_string(pPlayer->GetResist(3));
}

string ashitacastVariables::var_p_fireresist()
{
    return to_string(pPlayer->GetResist(0));
}

string ashitacastVariables::var_p_hp()
{
    return to_string(pParty->GetMemberHP(0));
}

string ashitacastVariables::var_p_hpmax()
{
    return to_string(pPlayer->GetHPMax());
}

string ashitacastVariables::var_p_hpp()
{
    return to_string(pParty->GetMemberHPPercent(0));
}

string ashitacastVariables::var_p_iceresist()
{
    return to_string(pPlayer->GetResist(1));
}

string ashitacastVariables::var_p_ismoving()
{
    int myIndex = pParty->GetMemberTargetIndex(0);
    if ((pEntity->GetLocalPositionX(myIndex) != mLastPosX) || (pEntity->GetLocalPositionZ(myIndex) != mLastPosZ))
        return "true";
    return "false";
}

string ashitacastVariables::var_p_job()
{
    stringstream ret;
    ret << pResource->GetString("jobs.names_abbr", pPlayer->GetMainJob());
    ret << "/";
    ret << pResource->GetString("jobs.names_abbr", pPlayer->GetSubJob());
    return ret.str();
}

string ashitacastVariables::var_p_level()
{
    return to_string(pPlayer->GetMainJobLevel());
}

string ashitacastVariables::var_p_lightningresist()
{
    return to_string(pPlayer->GetResist(4));
}

string ashitacastVariables::var_p_lightresist()
{
    return to_string(pPlayer->GetResist(6));
}

string ashitacastVariables::var_p_mainjob()
{
    return pResource->GetString("jobs.names_abbr", pPlayer->GetMainJob());
}

string ashitacastVariables::var_p_mp()
{
    return to_string(pParty->GetMemberMP(0));
}

string ashitacastVariables::var_p_mpmax()
{
    return to_string(pPlayer->GetMPMax());
}

string ashitacastVariables::var_p_mpp()
{
    return to_string(pParty->GetMemberMPPercent(0));
}

string ashitacastVariables::var_p_name()
{
    return pParty->GetMemberName(0);
}

string ashitacastVariables::var_p_status()
{
    int myIndex = pParty->GetMemberTargetIndex(0);
    map<uint32_t, string>::iterator find = gEntityStatus.find(pEntity->GetStatus(myIndex));
    if (find == gEntityStatus.end())
        return "ENTITY_STATUS_ERROR";
    return find->second;
}

string ashitacastVariables::var_p_subjob()
{
    return pResource->GetString("jobs.names_abbr", pPlayer->GetSubJob());
}

string ashitacastVariables::var_p_subjoblevel()
{
    return to_string(pPlayer->GetSubJobLevel());
}

string ashitacastVariables::var_p_tp()
{
    return to_string(pParty->GetMemberTP(0));
}

string ashitacastVariables::var_p_waterresist()
{
    return to_string(pPlayer->GetResist(5));
}

string ashitacastVariables::var_p_windresist()
{
    return to_string(pPlayer->GetResist(2));
}

string ashitacastVariables::var_pt_actiontarget()
{
    if (mCurrentAction->active)
    {
        for (int x = 0; x < 6; x++)
        {
            if (pParty->GetMemberTargetIndex(x) == mCurrentAction->target)
                return "true";
        }
    }
    return "false";
}

string ashitacastVariables::var_pt_count()
{
    int count = 0;
    for (int x = 0; x < 6; x++)
    {
        if (pParty->GetMemberIsActive(x))
            count++;
    }
    return std::to_string(count);
}

string ashitacastVariables::var_pt_inparty()
{
    for (int x = 1; x < 6; x++)
    {
        if (pParty->GetMemberIsActive(x))
            return "true";
    }
    return "false";
}

string ashitacastVariables::var_pt_target()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "false";

    for (int x = 0; x < 6; x++)
    {
        if ((pParty->GetMemberIsActive(x)) && (pParty->GetMemberTargetIndex(x) == targetIndex))
            return "true";
    }

    return "false";
}

string ashitacastVariables::var_t_distance()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";

    return to_string(sqrt(pEntity->GetDistance(targetIndex)));
}

string ashitacastVariables::var_t_hpp()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";
    return to_string(pEntity->GetHPPercent(targetIndex));
}

string ashitacastVariables::var_t_id()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";
    return to_string(pEntity->GetServerId(targetIndex));
}

string ashitacastVariables::var_t_index()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";
    return to_string(targetIndex);
}

string ashitacastVariables::var_t_name()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";
    return pEntity->GetName(targetIndex);
}

string ashitacastVariables::var_t_status()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";

    map<uint32_t, string>::iterator find = gEntityStatus.find(pEntity->GetStatus(targetIndex));
    if (find == gEntityStatus.end())
        return "ENTITY_STATUS_ERROR";
    return find->second;
}

string ashitacastVariables::var_t_type()
{
    int targetIndex = pTarget->GetTargetIndex(pTarget->GetIsSubTargetActive());
    if (targetIndex == 0)
        return "NO_TARGET";

    uint32_t type                        = pEntity->GetSpawnFlags(targetIndex);
    map<uint32_t, string>::iterator find = gSpawnFlags.find(type);
    if (find == gSpawnFlags.end())
        return "SPAWN_FLAGS_ERROR";
    return find->second;
}