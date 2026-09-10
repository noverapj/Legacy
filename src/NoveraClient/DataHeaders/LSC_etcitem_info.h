#ifndef __LSC_etcitem_info__H__
#define __LSC_etcitem_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_etcitem_info
//--------------------------------------------------
struct LSC_etcitem_info
{
    DWORD      item_code;
    char       name[64];
    char       etc_group[64];
    CN_TYPE    cn_itemtype;
    DWORD      cn_itemvalue;
    DWORD      cn_itemvalue_2;
    char       icon_name[64];
    char       icon_name_sub[64];
    char       basic_manual[64];
    char       tooltip[64];
    DWORD      value_1;
    BYTE       event;
    BYTE       use_type;
    BYTE       drop_able;
    BYTE       limit_lv;
    WORD       limit_count;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_etcitem_info_Manager
//--------------------------------------------------
class LSC_etcitem_info_Manager : public BaseDataManager<DWORD, LSC_etcitem_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0001fe66;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_etcitem_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_etcitem_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_etcitem_info__H__
