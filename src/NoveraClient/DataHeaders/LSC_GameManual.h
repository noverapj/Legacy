#ifndef __LSC_GameManual__H__
#define __LSC_GameManual__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_GameManual
//--------------------------------------------------
struct LSC_GameManual
{
    INT      ID;
    INT      MainIndex;
    char     MainTitle[64];
    char     Icon[64];
    INT      SubIndex;
    char     SubTitle[64];
    char     Image[64];
    char     Desc[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_GameManual_Manager
//--------------------------------------------------
class LSC_GameManual_Manager : public BaseDataManager<INT, LSC_GameManual>
{
private:
    virtual int GetVersion()
    {
        return 0x0000d069;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_GameManual* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_GameManual*>(pInfo->ID, pInfo));
            }
        }
    }
};


#endif //__LSC_GameManual__H__
