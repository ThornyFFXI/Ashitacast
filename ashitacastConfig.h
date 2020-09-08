#ifndef __ASHITA_ashitacastConfig_H_INCLUDED__
#define __ASHITA_ashitacastConfig_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

struct ashitacastConfig
{
public:
    std::list<int32_t> mEquipBags;
    bool mEnableWardrobeCheck;
    bool mOrderByIndex; //Determines if addset should be written in order of index or visible order.
    bool mIncludeEmptySlots; //Determines if addset should write in a slot if it is empty.
    bool mGenerateMergedFiles; //Determines if a hard copy of merged files should be written when using includes.
    bool mNakedForPacker; //Determines if AC will use naked command and a delay prior to running packer.
    bool mDebugEnabled;

    int mPetSkillDelay; //Determines the length of time gear will be locked for a petskill event if the end skill WS packet is not received.
    int mWeaponskillDelay; //Determines the length of time gear will be locked for a weaponskill event if the end WS packet is not received.
    int mJobAbilityDelay; //Determines the length of time gear will be locked for a job ability event if the end JA packet is not received
    int mSpellOffset; //Adds extra time to calculated spell length when determining the length of time gear will be locked for a spell event if the end spell packet is not received.
    int mPetSpellOffset; //Adds extra time to calculated spell length when determining the length of time gear will be locked for a petspell event if the end spell packet is not received.
    int mRangedOffset; //Adds extra time to calculated ranged length when determining the length of time gear will be locked for a ranged event if the end ranged packet is not received.
    int mItemOffset; //Adds extra time to calculated item length when determining the length of time gear will be locked for an item event if the end item packet is not received.
    int mFastCast; //Allows you to set a fast cast value which will modify calculated spell casttimes for spell events.
    int mSnapShot; //Allows you to set a snapshot value which will modify calculated ranged duration for ranged events.
    ashitacastConfig()
        : mEquipBags({0, 8, 10, 11, 12})
        , mEnableWardrobeCheck(true)
        , mOrderByIndex(false)
        , mIncludeEmptySlots(false)
        , mGenerateMergedFiles(true)
        , mDebugEnabled(false)
        , mNakedForPacker(true)
        , mPetSkillDelay(1500)
        , mWeaponskillDelay(1500)
        , mJobAbilityDelay(1500)
        , mSpellOffset(500)
        , mPetSpellOffset(500)
        , mRangedOffset(500)
        , mItemOffset(500)
        , mFastCast(0)
        , mSnapShot(0)
    {}
};

#endif