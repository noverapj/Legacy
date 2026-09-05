#ifndef __LSC_event_list__H__
#define __LSC_event_list__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_event_list
//--------------------------------------------------
struct LSC_event_list
{
    DWORD    Index;
    DWORD    Event_index;
    BYTE     Event_type;
    BYTE     Period_type;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_event_list_Manager
//--------------------------------------------------
class LSC_event_list_Manager : public BaseDataManager<DWORD, LSC_event_list>
{
private:
    virtual int GetVersion()
    {
        return 0x00008559;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_event_list* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_event_list*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_event_list__H__
