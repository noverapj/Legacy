#ifndef __LSC_MapInfo__H__
#define __LSC_MapInfo__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_MapInfo
//--------------------------------------------------
struct LSC_MapInfo
{
    INT      ID;
    char     Path[64];
    BYTE     Type;
    BYTE     GroupID;
    char     MapName[128];
    char     Description[128];
    char     LoadingImg[128];
    char     ResultImg[128];
    char     IconImg[128];
    char     Bgm[128];
    BYTE     Active;
    BYTE     Difficulty;
    BYTE     MinUserCnt;
    BYTE     MaxUserCnt;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_MapInfo_Manager
//--------------------------------------------------
class LSC_MapInfo_Manager : public BaseDataManager<INT, LSC_MapInfo>
{
private:
    virtual int GetVersion()
    {
        return 0x000168db;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_MapInfo* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_MapInfo*>(pInfo->ID, pInfo));
            }
        }
    }
};


#endif //__LSC_MapInfo__H__
