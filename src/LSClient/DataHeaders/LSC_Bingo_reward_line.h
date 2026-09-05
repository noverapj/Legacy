#ifndef __LSC_Bingo_reward_line__H__
#define __LSC_Bingo_reward_line__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Bingo_reward_line
//--------------------------------------------------
struct LSC_Bingo_reward_line
{
    DWORD    index;
    DWORD    bingo_event_index;
    DWORD    bingo_line_reward;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Bingo_reward_line_Manager
//--------------------------------------------------
class LSC_Bingo_reward_line_Manager : public BaseDataManager<DWORD, LSC_Bingo_reward_line>
{
private:
    virtual int GetVersion()
    {
        return 0x00008de4;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Bingo_reward_line* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_Bingo_reward_line*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_Bingo_reward_line__H__
