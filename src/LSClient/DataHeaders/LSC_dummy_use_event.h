#ifndef __LSC_dummy_use_event__H__
#define __LSC_dummy_use_event__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_dummy_use_event
//--------------------------------------------------
struct LSC_dummy_use_event
{
    DWORD    index;
    DWORD    dummy_use_event_index;
    BYTE     dummy_use_type;
    char     dummy_use_event_title[64];
    char     dummy_use_event_substance[128];
    char     dummy_use_event_icon[64];
    char     dummy_use_evnet_icon_sub[64];
    WORD     dummy_use_event_start_year;
    BYTE     dummy_use_event_start_month;
    BYTE     dummy_use_event_start_day;
    BYTE     dummy_use_event_start_hour;
    WORD     dummy_use_event_end_year;
    BYTE     dummy_use_event_end_month;
    BYTE     dummy_use_event_end_day;
    BYTE     dummy_use_event_end_hour;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_dummy_use_event_Manager
//--------------------------------------------------
class LSC_dummy_use_event_Manager : public BaseDataManager<DWORD, LSC_dummy_use_event>
{
private:
    virtual int GetVersion()
    {
        return 0x000358ac;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_dummy_use_event* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_dummy_use_event*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_dummy_use_event__H__
