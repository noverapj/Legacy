#ifndef __LSC_exchange_dummy__H__
#define __LSC_exchange_dummy__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_exchange_dummy
//--------------------------------------------------
struct LSC_exchange_dummy
{
    DWORD    index;
    DWORD    dummy_use_event_index;
    DWORD    exchange_item_code;
    WORD     exchange_need_item_count;
    DWORD    exchange_reward_present;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_exchange_dummy_Manager
//--------------------------------------------------
class LSC_exchange_dummy_Manager : public BaseDataManager<DWORD, LSC_exchange_dummy>
{
private:
    virtual int GetVersion()
    {
        return 0x000106fd;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_exchange_dummy* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_exchange_dummy*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_exchange_dummy__H__
