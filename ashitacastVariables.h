#ifndef __ASHITA_AshitacastVariables_H_INCLUDED__
#define __ASHITA_AshitacastVariables_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "C:\Program Files (x86)\Ashita 4\plugins\sdk\Ashita.h"
#include "Output.h"
#include "ashitacastStructs.h"
#include <map>

#define RBUFP(p, pos) (((uint8_t*)(p)) + (pos))
#define Read8(p, pos) (*(uint8_t*)RBUFP((p), (pos)))
#define Read16(p, pos) (*(uint16_t*)RBUFP((p), (pos)))
#define Read32(p, pos) (*(uint32_t*)RBUFP((p), (pos)))
#define Read64(p, pos) (*(uint64_t*)RBUFP((p), (pos)))
#define ReadFloat(p, pos) (*(float_t*)RBUFP((p), (pos)))

using namespace std;

struct argData
{
    bool isActive;
    std::string fullargs;
    std::vector<string> args;
    int argCount;
};

struct ffxiDateTime
{
    uint32_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

class ashitacastVariables
{
public:
    equipRegistry_t mEquipOverrides[16];

    ashitacastVariables(IAshitaCore* ashitaCore, actionData_t* actionData_t, OutputHelpers* output);
    ~ashitacastVariables(void);

    actionData_t* mCurrentAction;
    argData mArgData;

    void registerBuff(int16_t id, bool active, int duration);
    void clearBuff(int16_t id);

    void clearVariables();
    void clearVariable(string name);
    void printEnvironmentalVariables();
    void printUserVariables();
    void setLastPosition(float_t x, float_t z);
    void setVariable(string name, string value);

    bool processAdvanced(string input);
    bool processBasic(const char* attrName, const char* attrValue);
    string resolveVariables(string input);
    string resolveAdvanced(string input);
    string resolveBasic(string input);
    equipRegistry_t getEquippedItem(int slot);

private:
    IAshitaCore* m_AshitaCore;
    IEntity* pEntity;
    IInventory* pInventory;
    IParty* pParty;
    IPlayer* pPlayer;
    ITarget* pTarget;
    IResourceManager* pResource;
    OutputHelpers* pOutput;

    DWORD pWeather;
    DWORD pVanaTime;

    typedef string (ashitacastVariables::*variableResolver)(void);
    std::map<string, variableResolver> mResolverMap;
    std::map<string, string> mVariableMap;
    std::list<buffRegistry_t> mBuffOverrides;
    float_t mLastPosX;
    float_t mLastPosZ;

    ffxiDateTime getTimeStamp();
    uint8_t getWeather();
    int getBuffCount(std::vector<int16_t> buffs);

    void initializeResolverMap();
    string resolveVariable(string input, size_t offset);
    string resolveExponents(string input);
    string resolveMultiplication(string input);
    string resolveAddition(string input);
    string resolveConditions(string input);
    string splitCondition(string input, size_t offset, string* lhs, size_t* replaceOffset, size_t* replaceLength);
    string splitConditionBasic(string input, size_t offset, string* lhs, size_t* replaceOffset, size_t* replaceLength);
    string trimWhiteSpace(string input);
    bool isNumericValue(string input);
    bool isWildMatch(const char* lhs, const char* rhs);

    string var_a_actiontarget(void);
    string var_a_count(void);
    string var_a_inally(void);
    string var_a_target(void);
    string var_ad_casttime(void);
    string var_ad_element(void);
    string var_ad_id(void);
    string var_ad_mpaftercast(void);
    string var_ad_mpcost(void);
    string var_ad_mppaftercast(void);
    string var_ad_name(void);
    string var_ad_recast(void);
    string var_ad_skill(void);
    string var_ad_type(void);
    string var_at_distance(void);
    string var_at_hpp(void);
    string var_at_id(void);
    string var_at_index(void);
    string var_at_name(void);
    string var_at_status(void);
    string var_at_type(void);
    string var_e_area(void);
    string var_e_day(void);
    string var_e_dayelement(void);
    string var_e_moon(void);
    string var_e_moonpct(void);
    string var_e_time(void);
    string var_e_weather(void);
    string var_e_weatherelement(void);
    string var_eq_ammo(void);
    string var_eq_back(void);
    string var_eq_body(void);
    string var_eq_ear1(void);
    string var_eq_ear2(void);
    string var_eq_feet(void);
    string var_eq_hands(void);
    string var_eq_head(void);
    string var_eq_legs(void);
    string var_eq_main(void);
    string var_eq_neck(void);
    string var_eq_range(void);
    string var_eq_ring1(void);
    string var_eq_ring2(void);
    string var_eq_sub(void);
    string var_eq_waist(void);
    string var_pet_active(void);
    string var_pet_hpp(void);
    string var_pet_tp(void);
    string var_pet_status(void);
    string var_pet_name(void);
    string var_pet_distance(void);
    string var_p_attack(void);
    string var_p_darkresist(void);
    string var_p_defense(void);
    string var_p_earthresist(void);
    string var_p_fireresist(void);
    string var_p_hp(void);
    string var_p_hpmax(void);
    string var_p_hpp(void);
    string var_p_iceresist(void);
    string var_p_ismoving(void);
    string var_p_job(void);
    string var_p_level(void);
    string var_p_lightningresist(void);
    string var_p_lightresist(void);
    string var_p_mainjob(void);
    string var_p_mp(void);
    string var_p_mpmax(void);
    string var_p_mpp(void);
    string var_p_name(void);
    string var_p_status(void);
    string var_p_subjob(void);
    string var_p_subjoblevel(void);
    string var_p_tp(void);
    string var_p_waterresist(void);
    string var_p_windresist(void);
    string var_pt_actiontarget(void);
    string var_pt_count(void);
    string var_pt_inparty(void);
    string var_pt_target(void);
    string var_t_distance(void);
    string var_t_hpp(void);
    string var_t_id(void);
    string var_t_index(void);
    string var_t_name(void);
    string var_t_status(void);
    string var_t_type(void);
};
#endif