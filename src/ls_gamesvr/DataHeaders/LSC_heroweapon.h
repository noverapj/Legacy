#ifndef __LSC_heroweapon__H__
#define __LSC_heroweapon__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_heroweapon
//--------------------------------------------------
struct LSC_heroweapon
{
    DWORD    skillid;
    DWORD    weaponID;
    FLOAT    armor_class;
    FLOAT    speed_class;
    FLOAT    maxgauge;
    WORD     range ;
    BYTE     attack_count;
    BYTE     special_attackcount;
    FLOAT    special_attacktime;
    FLOAT    skill_gauge_rate;
    BYTE     bullet_count;
    DWORD    bullet_time;
    char     skill_name [127];
    DWORD    skill_keep_time;
    DWORD    push_item;
    BYTE     pushstruct_count;
    BYTE     pushstruct_time;
    FLOAT    activegauge;
    BYTE     blowstate;
    BYTE     down_blowstate;
    BYTE     air_blowstate;
    BYTE     panicstate;
    BYTE     skill_defence_ignore;
    BYTE     skill_defence_break;
    BYTE     skill_attackcount;
    FLOAT    skill_attacktime;
    BYTE     skill_active_type;
    FLOAT    skill_move_range;
    FLOAT    charge_gauge;
    char     buff_name1[64];
    char     buff_name2[64];
    char     buff_name3[64];
    char     buff_name4[64];
    char     buff_name5[64];
    char     buff_name6[64];
    char     buff_name7[64];
    char     buff_name8[64];
    char     buff_name9[64];
    char     buff_name10[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_heroweapon_Manager
//--------------------------------------------------
class LSC_heroweapon_Manager : public BaseDataManager<DWORD, LSC_heroweapon>
{
private:
    virtual int GetVersion()
    {
        return 0x00054d28;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_heroweapon* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_heroweapon*>(pInfo->skillid, pInfo));
            }
        }
    }
};


#endif //__LSC_heroweapon__H__
