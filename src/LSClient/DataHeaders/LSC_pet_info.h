#ifndef __LSC_pet_info__H__
#define __LSC_pet_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_pet_info
//--------------------------------------------------
struct LSC_pet_info
{
    WORD     item_code;
    char     name[64];
    char     icon_name [64];
    char     tooltip[64];
    char     ani_delay_name[64];
    char     ani_run_name[64];
    char     ani_shop_delay_name[64];
    char     model_name[64];
    WORD     stat_attack;
    WORD     stat_depense;
    WORD     stat_speed;
    WORD     stat_drop;
    WORD     stat_weapon;
    WORD     stat_armor;
    WORD     stat_cap;
    WORD     stat_cloak;
    DWORD    pet_exp;
    DWORD    sell_price;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_pet_info_Manager
//--------------------------------------------------
class LSC_pet_info_Manager : public BaseDataManager<WORD, LSC_pet_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00023d4e;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_pet_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_pet_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_pet_info__H__
