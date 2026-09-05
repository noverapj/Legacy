#ifndef __LSC_SH_Phase_Data__H__
#define __LSC_SH_Phase_Data__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SH_Phase_Data
//--------------------------------------------------
struct LSC_SH_Phase_Data
{
    INT      phase;
    BYTE     event_type;
    DWORD    playtime;
    FLOAT    max_hp_rate;
    FLOAT    recovery_gauge_rate;
    FLOAT    drop_damage;
    FLOAT    fall_damage;
    FLOAT    float_damage_rate;
    FLOAT    blow_damage_rate;
    BYTE     phase_type;
    BYTE     phase_value;
    BYTE     keep_npc_count;
    DWORD    terms;
    SHORT    max_spawn;
    BYTE     spawn_count;
    DWORD    boss_code;
    WORD     boss_info;
    DWORD    subboss1_code;
    WORD     subboss1_info;
    DWORD    subboss2_code;
    WORD     subboss2_info;
    DWORD    subboss3_code;
    WORD     subboss3_info;
    DWORD    subboss4_code;
    WORD     subboss4_info;
    DWORD    subboss5_code;
    WORD     subboss5_info;
    DWORD    monster1_code;
    WORD     monster1_info;
    FLOAT    monster2_code;
    WORD     monster2_info;
    DWORD    monster3_code;
    WORD     monster3_info;
    DWORD    monster4_code;
    WORD     monster4_info;
    DWORD    monster5_code;
    WORD     monster5_info;
    DWORD    monster6_code;
    WORD     monster6_info;
    DWORD    monster7_code;
    WORD     monster7_info;
    DWORD    monster8_code;
    WORD     monster8_info;
    DWORD    monster9_code;
    WORD     monster9_info;
    DWORD    monster10_code;
    WORD     monster10_info;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SH_Phase_Data_Manager
//--------------------------------------------------
class LSC_SH_Phase_Data_Manager : public BaseDataManager<INT, LSC_SH_Phase_Data>
{
private:
    virtual int GetVersion()
    {
        return 0x0005fc49;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SH_Phase_Data* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_SH_Phase_Data*>(pInfo->phase, pInfo));
            }
        }
    }
};


#endif //__LSC_SH_Phase_Data__H__
