#ifndef __LSC_Customizing_info__H__
#define __LSC_Customizing_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Customizing_info
//--------------------------------------------------
struct LSC_Customizing_info
{
    DWORD    Customizing_Code;
    WORD     Customizing_Hero_Code;
    WORD     Customizing_Type;
    char     Customizing_Icon[64];
    char     Customizing_Name[64];
    char     Customizing_Tooltip[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Customizing_info_Manager
//--------------------------------------------------
class LSC_Customizing_info_Manager : public BaseDataManager<DWORD, LSC_Customizing_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00013ade;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Customizing_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_Customizing_info*>(pInfo->Customizing_Code, pInfo));
            }
        }
    }
};


#endif //__LSC_Customizing_info__H__
