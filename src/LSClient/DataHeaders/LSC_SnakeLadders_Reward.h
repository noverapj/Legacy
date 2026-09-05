#ifndef __LSC_SnakeLadders_Reward__H__
#define __LSC_SnakeLadders_Reward__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SnakeLadders_Reward
//--------------------------------------------------
struct LSC_SnakeLadders_Reward
{
    INT      Index;
    SHORT    RewardGroup;
    char     Reward1name[64];
    char     Reward1icon[128];
    char     Reward2name[64];
    char     Reward2icon[128];
    char     Reward3name[64];
    char     Reward3icon[128];
    char     Reward4name[64];
    char     Reward4icon[128];
    char     Reward5name[64];
    char     Reward5icon[128];
    char     Reward6name[64];
    char     Reward6icon[128];
    char     Reward7name[64];
    char     Reward7icon[128];
    char     Reward8name[64];
    char     Reward8icon[128];
    char     Reward9name[64];
    char     Reward9icon[128];
    char     Reward10name[64];
    char     Reward10icon[128];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SnakeLadders_Reward_Manager
//--------------------------------------------------
class LSC_SnakeLadders_Reward_Manager : public BaseDataManager<INT, LSC_SnakeLadders_Reward>
{
private:
    virtual int GetVersion()
    {
        return 0x0003322e;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SnakeLadders_Reward* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_SnakeLadders_Reward*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_SnakeLadders_Reward__H__
