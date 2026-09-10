#ifndef __LSC_compound_dummy__H__
#define __LSC_compound_dummy__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_compound_dummy
//--------------------------------------------------
struct LSC_compound_dummy
{
    DWORD    index;
    DWORD    dummy_use_event_index;
    DWORD    compound_item_code1;
    DWORD    compound_item_code2;
    DWORD    compound_item_code3;
    DWORD    compound_item_code4;
    DWORD    compound_item_code5;
    DWORD    compound_item_code6;
    DWORD    compound_item_code7;
    DWORD    compound_item_code8;
    DWORD    compound_reward_present;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_compound_dummy_Manager
//--------------------------------------------------
class LSC_compound_dummy_Manager : public BaseDataManager<DWORD, LSC_compound_dummy>
{
private:
    virtual int GetVersion()
    {
        return 0x00020bd5;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_compound_dummy* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_compound_dummy*>(pInfo->index, pInfo));
            }
        }
    }
};


#endif //__LSC_compound_dummy__H__
