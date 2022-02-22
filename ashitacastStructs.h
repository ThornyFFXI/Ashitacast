#ifndef __ASHITA_ashitacastStructs_H_INCLUDED__
#define __ASHITA_ashitacastStructs_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "C:\Ashita 4\plugins\sdk\Ashita.h"
#include "..\common\thirdparty\rapidxml.hpp"
#include <chrono>
#include <vector>

using namespace std;

struct cistringcmp
{
    bool operator()(const string& lhs, const string& rhs) const
    {
        return (_stricmp(lhs.c_str(), rhs.c_str()) < 0);
    }
};

struct commandPass_t
{
    bool isEcho;
    int commandDelay;
    int commandMode;
    std::string commandText;
    IAshitaCore* ashitaCore;
};

struct singleAugment_t
{
    int32_t table;
    int32_t realId;
    string stat;
    int32_t value;
    bool percent;
};

enum actionType
{
    Spell,
    Ability,
    Weaponskill,
    Ranged,
    Item,
    PetSpell,
    PetSkill_Ability,
    PetSkill_MobSkill
};

struct pkAction_t
{
    uint32_t header;
    uint32_t targetId;
    uint16_t targetIndex;
    uint16_t category;
    uint16_t param;
    uint16_t unknown;
    float_t posX;
    float_t posY;
    float_t posZ;
};
struct pkItem_t
{
    uint32_t header;
    uint32_t targetId;
    uint32_t unknown1;
    uint16_t targetIndex;
    uint8_t itemIndex;
    uint8_t unknown2;
    uint8_t itemContainer;
    uint8_t unknown3[3];
};

struct actionData_t
{
    bool active;
    bool cancel;
    bool postCast;
    actionType type;
    uint16_t target;
    uint16_t abilityId;
    pkAction_t raw;

    actionData_t()
    {}

    actionData_t(actionType type, uint16_t target, uint16_t abilityId)
        : type(type)
        , target(target)
        , abilityId(abilityId)
    {
        active = true;
        cancel   = false;
        postCast = false;
    }

    actionData_t(actionType type, pkAction_t raw)
        : type(type)
        , raw(raw)
    {
        active    = true;
        cancel    = false;
        postCast  = false;
        target    = raw.targetIndex;
        abilityId = raw.param;
    }
};

struct GearListEntry_t
{
    char Name[32];
    int32_t Quantity;
    int32_t AugPath;
    int32_t AugRank;
    int32_t AugTrial;
    int32_t AugCount;
    char AugString[10][100];

public:
    GearListEntry_t()
    {}

    GearListEntry_t(rapidxml::xml_node<>* node, IItem* pResource)
    {
        strcpy_s(Name, 32, node->value());
        Quantity       = 1;

        AugPath               = -1;
        rapidxml::xml_attribute<>* attr = node->first_attribute("augpath");
        if (attr)
        {
            if (attr->value()[0] == 'A')
                AugPath = 1;
            else if (attr->value()[0] == 'B')
                AugPath = 2;
            else if (attr->value()[0] == 'C')
                AugPath = 3;
            else if (attr->value()[0] == 'D')
                AugPath = 4;
        }

        AugRank = -1;
        attr    = node->first_attribute("augrank");
        if (attr)
        {
            AugRank = atoi(attr->value());
        }

        AugTrial = -1;
        attr     = node->first_attribute("augtrial");
        if (attr)
        {
            AugTrial = atoi(attr->value());
        }

        AugCount = 0;
        attr = node->first_attribute("augment");
        if ((attr) && (strlen(attr->value()) > 0))
        {
            stringstream stream(attr->value());
            while (stream.good())
            {
                string augment;
                getline(stream, augment, ',');
                strcpy_s(AugString[AugCount], 100, augment.c_str());
                AugCount++;
            }
        }

        if (_stricmp(node->name(), "item") == 0)
        {
            attr = node->first_attribute("quantity");
            if (attr)
            {
                if (_stricmp(attr->value(), "all") == 0)
                    Quantity       = -1;
                else
                    Quantity = atoi(attr->value());
            }
        }
    }

    bool operator==(const GearListEntry_t& other)
    {
        if (_stricmp(Name, other.Name) != 0)
            return false;

        if ((AugPath != other.AugPath) || (AugRank != other.AugRank) || (AugTrial != other.AugTrial))
            return false;

        if (AugCount != other.AugCount)
            return false;

        for (int x = 0; x < AugCount; x++)
        {
            if (strcmp(AugString[x], other.AugString[x]) != 0)
                return false;
        }

        return true;
    }
};

struct GearListEvent_t
{
    char Sender[256];
    int32_t EntryCount;
    GearListEntry_t Entries[1000];
};

struct buffRegistry_t
{
    int16_t buffId;
    bool active;
    std::chrono::time_point<std::chrono::steady_clock> expiration;
};
struct equipRegistry_t
{
    int container;
    int index;
    bool encumbered;
    bool disabled;
    std::chrono::time_point<std::chrono::steady_clock> expiration;

    equipRegistry_t()
    {
        container = 0;
        index     = 0;
        encumbered = false;
        disabled   = false;
        expiration = std::chrono::steady_clock::now() - std::chrono::minutes(1);
    }
};

struct characterState_t
{
    string lastName;
    int lastId;
    actionData_t mCurrentAction;
    std::chrono::time_point<std::chrono::steady_clock> gearLockDelay;
    std::chrono::time_point<std::chrono::steady_clock> playerActionDelay;
    bool petOverride;
    std::chrono::time_point<std::chrono::steady_clock> petActionDelay;
};

struct augmentData_t
{
    uint32_t path;
    uint32_t rank;
    uint32_t trialNumber;
    vector<string> stringAugments;
    vector<singleAugment_t> rawAugments;
};

struct augmentResource_t
{
    uint32_t id;
    uint32_t realid;
    string stat;
    int32_t offset;
    int32_t multiplier;
    bool percent;
};

struct equipData_t
{
    int32_t priority;
    int slot;
    bool locked;

    std::string name;
    vector<std::string> augmentStrings;
    int32_t augmentPath;
    int32_t augmentRank;
    int32_t augmentTrial;
    int32_t index;
    int32_t container;

    equipData_t()
        : augmentPath(-1)
        , augmentRank(-1)
        , augmentTrial(-1)
        , index(-1)
        , container(-1)
        , locked(false)
    {}

    bool ismatch(IItem* resource)
    {
        if ((!resource) || (_stricmp(resource->Name[0], name.c_str()) != 0))
            return false;

        if ((resource->Slots & (uint16_t)pow(2, slot)) == 0)
            return false;

        return true;
    }
    bool augmatch(augmentData_t augment)
    {
        for (vector<string>::iterator it = augmentStrings.begin(); it != augmentStrings.end(); it++)
        {
            if (std::find(augment.stringAugments.begin(), augment.stringAugments.end(), *it) == augment.stringAugments.end())
                return false;
        }

        if ((augmentPath != -1) && (augment.path != augmentPath))
            return false;
        if ((augmentRank != -1) && (augment.rank != augmentRank))
            return false;
        if ((augmentTrial != -1) && (augment.trialNumber != augmentTrial))
            return false;
        return true;
    }
};
#endif