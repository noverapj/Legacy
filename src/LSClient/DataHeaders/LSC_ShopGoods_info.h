#ifndef __LSC_ShopGoods_info__H__
#define __LSC_ShopGoods_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_ShopGoods_info
//--------------------------------------------------
struct LSC_ShopGoods_info
{
    DWORD    goods_index;
    char     goods_name[64];
    char     goods_icon[260];
    char     goods_icon_sub[260];
    char     goods_video[260];
    BYTE     goods_main_tab;
    char     goods_sub_tab[64];
    BYTE     goods_mark_type;
    char      goods_manual[64];
    char     goods_tooltip[64];
    char     goods_tooltip2[64];
    BYTE     goods_main_price_type;
    BYTE     goods_sub_price_type;
    DWORD    goods_price_value;
    BYTE     goods_item1_main_type;
    BYTE     goods_item1_sub_type;
    DWORD    goods_item1_code;
    DWORD    goods_item1_count;
    DWORD    goods_item1_period;
    BYTE     goods_item1_reinforce;
    BYTE     goods_item2_main_type;
    BYTE     goods_item2_sub_type;
    DWORD    goods_item2_code;
    DWORD    goods_item2_count;
    DWORD    goods_item2_period;
    BYTE     goods_item2_reinforce;
    BYTE     goods_item3_main_type;
    BYTE     goods_item3_sub_type;
    DWORD    goods_item3_code;
    DWORD    goods_item3_count;
    DWORD    goods_item3_period;
    BYTE     goods_item3_reinforce;
    BYTE     goods_item4_main_type;
    BYTE     goods_item4_sub_type;
    DWORD    goods_item4_code;
    DWORD    goods_item4_count;
    DWORD    goods_item4_period;
    BYTE     goods_item4_reinforce;
    BYTE     goods_item5_main_type;
    BYTE     goods_item5_sub_type;
    DWORD    goods_item5_code;
    DWORD    goods_item5_count;
    DWORD    goods_item5_period;
    BYTE     goods_item5_reinforce;
    BYTE     goods_item6_main_type;
    BYTE     goods_item6_sub_type;
    DWORD    goods_item6_code;
    DWORD    goods_item6_count;
    DWORD    goods_item6_period;
    BYTE     goods_item6_reinforce;
    BYTE     goods_item7_main_type;
    BYTE     goods_item7_sub_type;
    DWORD    goods_item7_code;
    DWORD    goods_item7_count;
    DWORD    goods_item7_period;
    BYTE     goods_item7_reinforce;
    BYTE     active;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_ShopGoods_info_Manager
//--------------------------------------------------
class LSC_ShopGoods_info_Manager : public BaseDataManager<DWORD, LSC_ShopGoods_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0009caa5;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_ShopGoods_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_ShopGoods_info*>(pInfo->goods_index, pInfo));
            }
        }
    }
};


#endif //__LSC_ShopGoods_info__H__
