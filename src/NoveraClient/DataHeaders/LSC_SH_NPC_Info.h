#ifndef __LSC_SH_NPC_Info__H__
#define __LSC_SH_NPC_Info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SH_NPC_Info
//--------------------------------------------------
struct LSC_SH_NPC_Info
{
    WORD     index;
    FLOAT    add_hp_rate;
    FLOAT    add_attack_rate;
    FLOAT    add_defense_rate;
    FLOAT    add_speed_rate;
    FLOAT    npc_drop_damage;
    FLOAT    npc_fall_damage;
    FLOAT    npc_float_damage_rate;
    FLOAT    npc_blow_damage_rate;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SH_NPC_Info_Manager
//--------------------------------------------------
class LSC_SH_NPC_Info_Manager : public BaseDataManager<WORD, LSC_SH_NPC_Info>
{
private:
    virtual int GetVersion()
    {
        return 0x000163a8;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SH_NPC_Info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_SH_NPC_Info*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_SH_NPC_Info__H__
