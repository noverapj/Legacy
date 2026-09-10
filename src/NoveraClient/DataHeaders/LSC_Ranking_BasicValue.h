#ifndef __LSC_Ranking_BasicValue__H__
#define __LSC_Ranking_BasicValue__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Ranking_BasicValue
//--------------------------------------------------
struct LSC_Ranking_BasicValue
{
    WORD     Ranking_limit_lv;
    BYTE     Mode1;
    BYTE     Mode2;
    BYTE     Mode3;
    BYTE     Mode4;
    BYTE     Mode5;
    DWORD    Pass_Score;
    DWORD    Fail_Score;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Ranking_BasicValue_Manager
//--------------------------------------------------
class LSC_Ranking_BasicValue_Manager : public BaseDataManager<WORD, LSC_Ranking_BasicValue>
{
private:
    virtual int GetVersion()
    {
        return 0x0000cdfb;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Ranking_BasicValue* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_Ranking_BasicValue*>(pInfo->Ranking_limit_lv, pInfo));
            }
        }
    }
};


#endif //__LSC_Ranking_BasicValue__H__
