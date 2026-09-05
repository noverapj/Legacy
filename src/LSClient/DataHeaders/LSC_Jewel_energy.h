#ifndef __LSC_Jewel_energy__H__
#define __LSC_Jewel_energy__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Jewel_energy
//--------------------------------------------------
struct LSC_Jewel_energy
{
    WORD     jewel_class;
    WORD     jewel_max_energy;
    WORD     jewel_energy_1;
    WORD     jewel_energy_2;
    char     jewel_grade_icon[64];
    DWORD    jewel_sell;
    DWORD    evolution_jewel_RB_index;
    DWORD    evolution_jewel_cost;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Jewel_energy_Manager
//--------------------------------------------------
class LSC_Jewel_energy_Manager : public BaseDataManager<WORD, LSC_Jewel_energy>
{
private:
    virtual int GetVersion()
    {
        return 0x00015a6f;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Jewel_energy* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_Jewel_energy*>(pInfo->jewel_class, pInfo));
            }
        }
    }
};


#endif //__LSC_Jewel_energy__H__
