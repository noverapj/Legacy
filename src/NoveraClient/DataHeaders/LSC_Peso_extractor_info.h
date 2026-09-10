#ifndef __LSC_Peso_extractor_info__H__
#define __LSC_Peso_extractor_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Peso_extractor_info
//--------------------------------------------------
struct LSC_Peso_extractor_info
{
    INT      Mastery;
    DWORD    EXP;
    DWORD    MaxGold;
    DWORD    Duration;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Peso_extractor_info_Manager
//--------------------------------------------------
class LSC_Peso_extractor_info_Manager : public BaseDataManager<INT, LSC_Peso_extractor_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000797c;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Peso_extractor_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Peso_extractor_info*>(pInfo->Mastery, pInfo));
            }
        }
    }
};


#endif //__LSC_Peso_extractor_info__H__
