#ifndef __LSC_LV_Reward_info__H__
#define __LSC_LV_Reward_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_LV_Reward_info
//--------------------------------------------------
struct LSC_LV_Reward_info
{
    DWORD    Index;
    BYTE     stage_no;
    DWORD    reward_index_1;
    DWORD    reward_index_2;
    DWORD    reward_index_3;
    DWORD    extra_reward_index;
    DWORD    special_reward_index;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_LV_Reward_info_Manager
//--------------------------------------------------
class LSC_LV_Reward_info_Manager : public BaseDataManager<DWORD, LSC_LV_Reward_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00010f32;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_LV_Reward_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_LV_Reward_info*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_LV_Reward_info__H__
