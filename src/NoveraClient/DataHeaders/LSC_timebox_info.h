#ifndef __LSC_timebox_info__H__
#define __LSC_timebox_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_timebox_info
//--------------------------------------------------
struct LSC_timebox_info
{
    DWORD                     Index;
    DWORD                     etcitem_code;
    BYTE                      timebox_type;
    WORD                      reward_time;
    BYTE                      start_reward_time_type;
    DWORD                     reward_index;
    DWORD                     random_reward_index;
    BYTE                      max_use_count;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_timebox_info_Manager
//--------------------------------------------------
class LSC_timebox_info_Manager : public BaseDataManager<DWORD, LSC_timebox_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00013345;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_timebox_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_timebox_info*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_timebox_info__H__
