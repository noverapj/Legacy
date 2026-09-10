#ifndef __LSC_MapGroup__H__
#define __LSC_MapGroup__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_MapGroup
//--------------------------------------------------
struct LSC_MapGroup
{
    INT      ID;
    char     MapSelectName[128];
    char     MapSelectImg[128];
    char     MapSelectOverImg[128];
    char     ButtonImg[128];
    char     ButtonOffImg[128];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_MapGroup_Manager
//--------------------------------------------------
class LSC_MapGroup_Manager : public BaseDataManager<INT, LSC_MapGroup>
{
private:
    virtual int GetVersion()
    {
        return 0x0000e769;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_MapGroup* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_MapGroup*>(pInfo->ID, pInfo));
            }
        }
    }
};


#endif //__LSC_MapGroup__H__
