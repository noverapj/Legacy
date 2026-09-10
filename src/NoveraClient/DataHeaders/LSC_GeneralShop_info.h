#ifndef __LSC_GeneralShop_info__H__
#define __LSC_GeneralShop_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_GeneralShop_info
//--------------------------------------------------
struct LSC_GeneralShop_info
{
    WORD     sale_index;
    DWORD    general_goods_index;
    DWORD    buy_present_event_index;
    char     buy_present_event_msg[260];
    WORD     general_goods_order;
    char     general_goods_search_keyword[260];
    BYTE     general_goods_date_showtype;
    double    general_goods_start_date;
    double    general_goods_end_date;
    BYTE     plaza_sell;
    BYTE     active;
    BYTE     present_active;
    BYTE     package_type;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_GeneralShop_info_Manager
//--------------------------------------------------
class LSC_GeneralShop_info_Manager : public BaseDataManager<WORD, LSC_GeneralShop_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0002762d;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_GeneralShop_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_GeneralShop_info*>(pInfo->sale_index, pInfo));
            }
        }
    }
};


#endif //__LSC_GeneralShop_info__H__
