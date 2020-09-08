#ifndef __ASHITA_ashitacastStrings_H_INCLUDED__
#define __ASHITA_ashitacastStrings_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define baseNodeMax 14
#define autoNodeMax 6
#define equipNodeMax 2
#define equipSetNodeMax 2
#define slotMax 16

const std::string gSlotNames[slotMax] = {
    "main",
    "sub",
    "range",
    "ammo",
    "head",
    "body",
    "hands",
    "legs",
    "feet",
    "neck",
    "waist",
    "ear1",
    "ear2",
    "ring1",
    "ring2",
    "back"};

const Ashita::FFXI::Enums::EquipmentSlot gOrderOfSlots[slotMax] = {
    Ashita::FFXI::Enums::EquipmentSlot::Main,
    Ashita::FFXI::Enums::EquipmentSlot::Sub,
    Ashita::FFXI::Enums::EquipmentSlot::Range,
    Ashita::FFXI::Enums::EquipmentSlot::Ammo,
    Ashita::FFXI::Enums::EquipmentSlot::Head,
    Ashita::FFXI::Enums::EquipmentSlot::Neck,
    Ashita::FFXI::Enums::EquipmentSlot::Ear1,
    Ashita::FFXI::Enums::EquipmentSlot::Ear2,
    Ashita::FFXI::Enums::EquipmentSlot::Body,
    Ashita::FFXI::Enums::EquipmentSlot::Hands,
    Ashita::FFXI::Enums::EquipmentSlot::Ring1,
    Ashita::FFXI::Enums::EquipmentSlot::Ring2,
    Ashita::FFXI::Enums::EquipmentSlot::Back,
    Ashita::FFXI::Enums::EquipmentSlot::Waist,
    Ashita::FFXI::Enums::EquipmentSlot::Legs,
    Ashita::FFXI::Enums::EquipmentSlot::Feet};

const std::string gBaseNodes[baseNodeMax] = {
    "sets",
    "load",
    "unload",
    "commands",
    "default",
    "preranged",
    "midranged",
    "precast",
    "midcast",
    "ability",
    "weaponskill",
    "item",
    "petskill",
    "petspell"};

const std::string gAutoNodes[autoNodeMax] = {
    "default",
    "ability",
    "weaponskill",
    "item",
    "petskill",
    "petspell"};

const std::string gEquipSetNodes[equipSetNodeMax] = {
    "midranged",
    "midcast"};

const std::string gEquipNodes[equipNodeMax] = {
    "preranged",
    "precast"};

const uint8_t gActionCompleteTypes[6] = {
    2, 3, 4, 5, 6, 14};

const uint8_t gPetActionCompleteTypes[3] = {
    4, 11, 13};
#endif