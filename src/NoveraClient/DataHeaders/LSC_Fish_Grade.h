#ifndef __LSC_Fish_Grade__H__
#define __LSC_Fish_Grade__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Fish_Grade
//--------------------------------------------------
struct LSC_Fish_Grade
{
    INT      Grade_Index;
    char     Name[64];
    char     Info[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Fish_Grade_Manager
//--------------------------------------------------
class LSC_Fish_Grade_Manager : public BaseDataManager<INT, LSC_Fish_Grade>
{
private:
    virtual int GetVersion()
    {
        return 0x00005e9f;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Fish_Grade* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Fish_Grade*>(pInfo->Grade_Index, pInfo));
            }
        }
    }
};


#endif //__LSC_Fish_Grade__H__
