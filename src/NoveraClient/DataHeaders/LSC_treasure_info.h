#ifndef __LSC_treasure_info__H__
#define __LSC_treasure_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_treasure_info
//--------------------------------------------------
struct LSC_treasure_info
{
    DWORD    Index;
    DWORD    treasure_index;
    BYTE     treasure_stage;
    WORD     reward_turn;
    DWORD    reward_index;
    WORD     coordinate_x;
    WORD     coordinate_y;
    BYTE     reward_valuable;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_treasure_info_Manager
//--------------------------------------------------
class LSC_treasure_info_Manager : public BaseDataManager<DWORD, LSC_treasure_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00011d15;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_treasure_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_treasure_info*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_treasure_info__H__
