#ifndef __LSC_RankingLevel_info__H__
#define __LSC_RankingLevel_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_RankingLevel_info
//--------------------------------------------------
struct LSC_RankingLevel_info
{
    BYTE     RankingClass;
    BYTE     RankingStep;
    DWORD    RankingTotalPoint;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_RankingLevel_info_Manager
//--------------------------------------------------
class LSC_RankingLevel_info_Manager : public BaseDataManager<int, LSC_RankingLevel_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00008977;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_RankingLevel_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<int, LSC_RankingLevel_info*>(i, pInfo));
            }
        }
    }
};


#endif //__LSC_RankingLevel_info__H__
