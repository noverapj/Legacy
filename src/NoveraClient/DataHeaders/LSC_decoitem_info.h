#ifndef __LSC_decoitem_info__H__
#define __LSC_decoitem_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_decoitem_info
//--------------------------------------------------
struct LSC_decoitem_info
{
    DWORD    item_code;
    char     name[64];
    BYTE     deco_type;
    char     icon_name[260];
    char     basic_manual[64];
    char     tooltip[64];
    DWORD    value_1;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_decoitem_info_Manager
//--------------------------------------------------
class LSC_decoitem_info_Manager : public BaseDataManager<DWORD, LSC_decoitem_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000e4ee;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_decoitem_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_decoitem_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_decoitem_info__H__
