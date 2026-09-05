#ifndef __LSC_SH_Phase_Reward__H__
#define __LSC_SH_Phase_Reward__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SH_Phase_Reward
//--------------------------------------------------
struct LSC_SH_Phase_Reward
{
    INT      Phase;
    DWORD    Exp;
    DWORD    Gold;
    DWORD    Reward_Code;
    char     Icon1_name[64];
    char     Icon1_sub_name[64];
    char     Icon1_Desc[64];
    char     Icon2_name[64];
    char     Icon2_sub_name[64];
    char     Icon2_Desc[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SH_Phase_Reward_Manager
//--------------------------------------------------
class LSC_SH_Phase_Reward_Manager : public BaseDataManager<INT, LSC_SH_Phase_Reward>
{
private:
    virtual int GetVersion()
    {
        return 0x00014251;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SH_Phase_Reward* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_SH_Phase_Reward*>(pInfo->Phase, pInfo));
            }
        }
    }
};


#endif //__LSC_SH_Phase_Reward__H__
