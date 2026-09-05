#ifndef __LSC_RB_item_info__H__
#define __LSC_RB_item_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_RB_item_info
//--------------------------------------------------
struct LSC_RB_item_info
{
    DWORD    Item_index;
    BYTE     Item_group;
    DWORD    Item_Code;
    BYTE     Icon_Display;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_RB_item_info_Manager
//--------------------------------------------------
class LSC_RB_item_info_Manager : public BaseDataManager<DWORD, LSC_RB_item_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000890f;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_RB_item_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_RB_item_info*>(pInfo->Item_index, pInfo));
            }
        }
    }
};


#endif //__LSC_RB_item_info__H__
