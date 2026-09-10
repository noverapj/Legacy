#ifndef __LSC_PresentReward_info__H__
#define __LSC_PresentReward_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_PresentReward_info
//--------------------------------------------------
struct LSC_PresentReward_info
{
    DWORD    TableNo;
    BYTE     ItemType;
    DWORD    ItemCode;
    DWORD    PresentValue;
    BYTE     PeriodType;
    DWORD    PresentPeriod;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_PresentReward_info_Manager
//--------------------------------------------------
class LSC_PresentReward_info_Manager : public BaseDataManager<DWORD, LSC_PresentReward_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000c7be;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_PresentReward_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_PresentReward_info*>(pInfo->TableNo, pInfo));
            }
        }
    }
};


#endif //__LSC_PresentReward_info__H__
