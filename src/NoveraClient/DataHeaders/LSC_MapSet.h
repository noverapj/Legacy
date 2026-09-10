#ifndef __LSC_MapSet__H__
#define __LSC_MapSet__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_MapSet
//--------------------------------------------------
struct LSC_MapSet
{
    INT      ID;
    BYTE     MapSet_ID;
    BYTE     MapInfo_ID;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_MapSet_Manager
//--------------------------------------------------
class LSC_MapSet_Manager : public BaseDataManager<INT, LSC_MapSet>
{
private:
    virtual int GetVersion()
    {
        return 0x000047f8;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_MapSet* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_MapSet*>(pInfo->ID, pInfo));
            }
        }
    }
};


#endif //__LSC_MapSet__H__
