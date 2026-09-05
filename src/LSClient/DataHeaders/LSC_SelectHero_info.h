#ifndef __LSC_SelectHero_info__H__
#define __LSC_SelectHero_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SelectHero_info
//--------------------------------------------------
struct LSC_SelectHero_info
{
    DWORD    SelectHero_Group;
    char     SelectHero_Group_image[260];
    WORD     SelectHero_Code_1;
    WORD     SelectHero_Npc_Code_1;
    DWORD    SelectHero_Period_1;
    WORD     SelectHero_Code_2;
    WORD     SelectHero_Npc_Code_2;
    DWORD    SelectHero_Period_2;
    WORD     SelectHero_Code_3;
    WORD     SelectHero_Npc_Code_3;
    DWORD    SelectHero_Period_3;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SelectHero_info_Manager
//--------------------------------------------------
class LSC_SelectHero_info_Manager : public BaseDataManager<DWORD, LSC_SelectHero_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0002038f;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SelectHero_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_SelectHero_info*>(pInfo->SelectHero_Group, pInfo));
            }
        }
    }
};


#endif //__LSC_SelectHero_info__H__
