#ifndef __LSC_Ranking_Reward__H__
#define __LSC_Ranking_Reward__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Ranking_Reward
//--------------------------------------------------
struct LSC_Ranking_Reward
{
    BYTE     Ranking_class;
    DWORD    Ranking_reward_Edge;
    DWORD    Ranking_reward1;
    DWORD    Ranking_reward2;
    DWORD    Ranking_reward3;
    DWORD    Ranking_reward4;
    DWORD    Ranking_reward5;
    char     Ranking_Frame_Big[260];
    char     Ranking_Frame_Mid[260];
    char     Ranking_Frame_Small[260];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Ranking_Reward_Manager
//--------------------------------------------------
class LSC_Ranking_Reward_Manager : public BaseDataManager<BYTE, LSC_Ranking_Reward>
{
private:
    virtual int GetVersion()
    {
        return 0x0001bd12;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Ranking_Reward* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<BYTE, LSC_Ranking_Reward*>(pInfo->Ranking_class, pInfo));
            }
        }
    }
};


#endif //__LSC_Ranking_Reward__H__
