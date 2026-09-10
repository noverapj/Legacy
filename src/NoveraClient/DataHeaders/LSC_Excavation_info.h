#ifndef __LSC_Excavation_info__H__
#define __LSC_Excavation_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Excavation_info
//--------------------------------------------------
struct LSC_Excavation_info
{
	INT      RewardType;
	INT      ItemIndex;
	BYTE     ItemType;
	char     ItemName[64];
	char     ItemInfo[64];
	char     ItemIcon[64];
    WORD     AppointMap01;
    WORD     AppointMap02;
    WORD     AppointMap03;
    WORD     AppointMap04;
    WORD     AppointMap05;
    WORD     AppointMap06;
    WORD     AppointMap07;
    WORD     AppointMap08;
    WORD     AppointMap09;
    WORD     AppointMap10;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Excavation_info_Manager
//--------------------------------------------------
class LSC_Excavation_info_Manager : public BaseDataManager<INT, LSC_Excavation_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0001dec9;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Excavation_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Excavation_info*>(pInfo->ItemIndex, pInfo));
            }
        }
    }
};


#endif //__LSC_Excavation_info__H__
