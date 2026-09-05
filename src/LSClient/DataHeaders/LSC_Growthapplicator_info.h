#ifndef __LSC_Growthapplicator_info__H__
#define __LSC_Growthapplicator_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Growthapplicator_info
//--------------------------------------------------
struct LSC_Growthapplicator_info
{
    INT      Mastery;
    DWORD    EXP;
    BYTE     MaxSlot;
    DWORD    Booster;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Growthapplicator_info_Manager
//--------------------------------------------------
class LSC_Growthapplicator_info_Manager : public BaseDataManager<INT, LSC_Growthapplicator_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00007a33;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Growthapplicator_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Growthapplicator_info*>(pInfo->Mastery, pInfo));
            }
        }
    }
};


#endif //__LSC_Growthapplicator_info__H__
