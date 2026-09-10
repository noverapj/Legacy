#ifndef __LSC_Bingo_reward_all__H__
#define __LSC_Bingo_reward_all__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Bingo_reward_all
//--------------------------------------------------
struct LSC_Bingo_reward_all
{
    DWORD    index;
    DWORD    bingo_event_index;
    DWORD    bingo_all_reward;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Bingo_reward_all_Manager
//--------------------------------------------------
class LSC_Bingo_reward_all_Manager : public BaseDataManager<DWORD, LSC_Bingo_reward_all>
{
private:
    virtual int GetVersion()
    {
        return 0x00008adb;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Bingo_reward_all* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_Bingo_reward_all*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_Bingo_reward_all__H__
