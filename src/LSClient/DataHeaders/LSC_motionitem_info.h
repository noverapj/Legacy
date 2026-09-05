#ifndef __LSC_motionitem_info__H__
#define __LSC_motionitem_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_motionitem_info
//--------------------------------------------------
struct LSC_motionitem_info
{
    DWORD      item_code;
    char       name[64];
    char       motion_group[64];
    char       motion_hm[64];
    char       motion_hw[64];
    char       icon_name[64];
    char       inventory_manual[64];
    char       tooltip[64];
    DWORD      value_1;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_motionitem_info_Manager
//--------------------------------------------------
class LSC_motionitem_info_Manager : public BaseDataManager<DWORD, LSC_motionitem_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00014792;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_motionitem_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_motionitem_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_motionitem_info__H__
