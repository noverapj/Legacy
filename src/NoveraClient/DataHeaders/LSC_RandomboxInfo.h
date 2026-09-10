#ifndef __LSC_RandomboxInfo__H__
#define __LSC_RandomboxInfo__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_RandomboxInfo
//--------------------------------------------------
struct LSC_RandomboxInfo
{
    DWORD    RB_Index;
    DWORD    RB_etcitem_code;
    BYTE     RB_open_type;
    DWORD    RB_item_group;
    DWORD    item_code;
    BYTE     item_type;
    BYTE     item_multi_count;
    DWORD    item_count_min;
    DWORD    item_count_max;
    WORD     item_reinfos_min;
    WORD     item_reinfos_max;
    BYTE     item_period_type;
    DWORD    item_time_min;
    DWORD    item_time_max;
    BYTE     Icon_Display;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_RandomboxInfo_Manager
//--------------------------------------------------
class LSC_RandomboxInfo_Manager : public BaseDataManager<DWORD, LSC_RandomboxInfo>
{
private:
    virtual int GetVersion()
    {
        return 0x00021736;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_RandomboxInfo* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_RandomboxInfo*>(pInfo->RB_Index, pInfo));
            }
        }
    }
};


#endif //__LSC_RandomboxInfo__H__
