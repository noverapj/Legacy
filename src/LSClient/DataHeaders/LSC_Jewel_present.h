#ifndef __LSC_Jewel_present__H__
#define __LSC_Jewel_present__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Jewel_present
//--------------------------------------------------
struct LSC_Jewel_present
{
    DWORD    jewel_RB_index;
    DWORD    jewel_RB_class;
    char     jewel_icon[64];
    char     jewel_name[64];
    char     jewel_tooltip[64];
    char     jewel_manual[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Jewel_present_Manager
//--------------------------------------------------
class LSC_Jewel_present_Manager : public BaseDataManager<DWORD, LSC_Jewel_present>
{
private:
    virtual int GetVersion()
    {
        return 0x000100fa;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Jewel_present* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_Jewel_present*>(pInfo->jewel_RB_index, pInfo));
            }
        }
    }
};


#endif //__LSC_Jewel_present__H__
