#ifndef __LSC_costum_item_info__H__
#define __LSC_costum_item_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_costum_item_info
//--------------------------------------------------
struct LSC_costum_item_info
{
    DWORD    item_code;
    char     name[64];
    char     tooltip[64];
    char     item_equip_effect[64];
    char     item_equip_effect_hw[64];
    char     spring_biped[64];
    char     icon_name [64];
    char     icon_sub_name [64];
    char     mesh_name_01[64];
    char     mesh_name_02[64];
    BYTE     face_change_hm;
    BYTE     face_change_hw;
    BYTE     hair_change_hm;
    BYTE     hair_change_hw;
    BYTE     invisible_face;
    DWORD    sell_price;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_costum_item_info_Manager
//--------------------------------------------------
class LSC_costum_item_info_Manager : public BaseDataManager<DWORD, LSC_costum_item_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0002715e;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_costum_item_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_costum_item_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_costum_item_info__H__
