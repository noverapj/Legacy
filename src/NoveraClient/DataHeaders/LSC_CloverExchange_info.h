#ifndef __LSC_CloverExchange_info__H__
#define __LSC_CloverExchange_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_CloverExchange_info
//--------------------------------------------------
struct LSC_CloverExchange_info
{
    DWORD    index;
    WORD     exchange_need_clover_count;
    DWORD    cloverexchange_reward_present;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_CloverExchange_info_Manager
//--------------------------------------------------
class LSC_CloverExchange_info_Manager : public BaseDataManager<DWORD, LSC_CloverExchange_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000c177;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_CloverExchange_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_CloverExchange_info*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_CloverExchange_info__H__
