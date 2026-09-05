#ifndef __LSC_ComposeRewardInfo__H__
#define __LSC_ComposeRewardInfo__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_ComposeRewardInfo
//--------------------------------------------------
struct LSC_ComposeRewardInfo
{
    WORD     compose_finish_item_type;
    DWORD    compose_finish_item;
    DWORD    compose_need_item;
    WORD     compose_need_item_min_rate;
    WORD     compose_need_item_max_rate;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_ComposeRewardInfo_Manager
//--------------------------------------------------
class LSC_ComposeRewardInfo_Manager : public BaseDataManager<WORD, LSC_ComposeRewardInfo>
{
private:
    virtual int GetVersion()
    {
        return 0x00012ea2;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_ComposeRewardInfo* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_ComposeRewardInfo*>(pInfo->compose_need_item, pInfo));
            }
        }
    }
};


#endif //__LSC_ComposeRewardInfo__H__
