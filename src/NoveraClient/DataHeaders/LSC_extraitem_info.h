#ifndef __LSC_extraitem_info__H__
#define __LSC_extraitem_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_extraitem_info
//--------------------------------------------------
struct LSC_extraitem_info
{
    DWORD    item_code;
    BYTE     extra_group;
    char     name[64];
    char     tooltip[64];
    DWORD    value_1;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_extraitem_info_Manager
//--------------------------------------------------
class LSC_extraitem_info_Manager : public BaseDataManager<DWORD, LSC_extraitem_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000a5c8;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_extraitem_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_extraitem_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_extraitem_info__H__
