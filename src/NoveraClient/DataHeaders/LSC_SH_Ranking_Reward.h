#ifndef __LSC_SH_Ranking_Reward__H__
#define __LSC_SH_Ranking_Reward__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SH_Ranking_Reward
//--------------------------------------------------
struct LSC_SH_Ranking_Reward
{
    BYTE     Reward_Num;
    BYTE     Reward_Cycle;
    WORD     Rank;
    DWORD    Rank_Reward1;
    DWORD    Rank_Reward2;
    DWORD    Rank_Reward3;
    DWORD    Rank_Reward4;
    DWORD    Rank_Reward5;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SH_Ranking_Reward_Manager
//--------------------------------------------------
class LSC_SH_Ranking_Reward_Manager : public BaseDataManager<BYTE, LSC_SH_Ranking_Reward>
{
private:
    virtual int GetVersion()
    {
        return 0x0001011c;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SH_Ranking_Reward* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<BYTE, LSC_SH_Ranking_Reward*>(pInfo->Reward_Num, pInfo));
            }
        }
    }
};


#endif //__LSC_SH_Ranking_Reward__H__
