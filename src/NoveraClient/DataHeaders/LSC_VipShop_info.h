#ifndef __LSC_VipShop_info__H__
#define __LSC_VipShop_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_VipShop_info
//--------------------------------------------------
struct LSC_VipShop_info
{
    WORD     vip_sale_index;
    DWORD    vip_goods_index;
    DWORD    buy_present_event_index;
    char     buy_present_event_msg[260];
    WORD     vip_goods_order;
    char     vip_goods_search_keyword[260];
    BYTE     vip_goods_date_showtype;
    double    vip_goods_start_date;
    double    vip_goods_end_date;
    BYTE     active;
    BYTE     vip_present_active;
    BYTE     vip_package_type;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_VipShop_info_Manager
//--------------------------------------------------
class LSC_VipShop_info_Manager : public BaseDataManager<WORD, LSC_VipShop_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0002422e;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_VipShop_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_VipShop_info*>(pInfo->vip_sale_index, pInfo));
            }
        }
    }
};


#endif //__LSC_VipShop_info__H__
