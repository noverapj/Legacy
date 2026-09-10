#ifndef __LSC_Reinforce_info__H__
#define __LSC_Reinforce_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Reinforce_info
//--------------------------------------------------
struct LSC_Reinforce_info
{
    INT      Lv_Gap;
    FLOAT    Reinforce_Rate_1;
    FLOAT    Reinforce_Rate_2;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Reinforce_info_Manager
//--------------------------------------------------
class LSC_Reinforce_info_Manager : public BaseDataManager<INT, LSC_Reinforce_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00007ceb;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Reinforce_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Reinforce_info*>(pInfo->Lv_Gap, pInfo));
            }
        }
    }
};


#endif //__LSC_Reinforce_info__H__
