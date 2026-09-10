#ifndef __LSC_Growthbooster_info__H__
#define __LSC_Growthbooster_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Growthbooster_info
//--------------------------------------------------
struct LSC_Growthbooster_info
{
    INT      BoosterIndex;
    char     BoosterName[64];
    char     Icon[64];
    char     BG[64];
    DWORD    Duration;
    DWORD    EXP;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Growthbooster_info_Manager
//--------------------------------------------------
class LSC_Growthbooster_info_Manager : public BaseDataManager<INT, LSC_Growthbooster_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000bbc5;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Growthbooster_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Growthbooster_info*>(pInfo->BoosterIndex, pInfo));
            }
        }
    }
};


#endif //__LSC_Growthbooster_info__H__
