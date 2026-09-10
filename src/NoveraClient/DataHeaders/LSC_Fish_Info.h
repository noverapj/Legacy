#ifndef __LSC_Fish_Info__H__
#define __LSC_Fish_Info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Fish_Info
//--------------------------------------------------
struct LSC_Fish_Info
{
    INT      Item_Index;
    char     Name[64];
    char     Info[64];
    char     Icon_Name[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Fish_Info_Manager
//--------------------------------------------------
class LSC_Fish_Info_Manager : public BaseDataManager<INT, LSC_Fish_Info>
{
private:
    virtual int GetVersion()
    {
        return 0x00007bfd;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Fish_Info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Fish_Info*>(pInfo->Item_Index, pInfo));
            }
        }
    }
};


#endif //__LSC_Fish_Info__H__
