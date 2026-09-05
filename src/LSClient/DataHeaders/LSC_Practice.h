#ifndef __LSC_Practice__H__
#define __LSC_Practice__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Practice
//--------------------------------------------------
struct LSC_Practice
{
    INT      Index;
    INT      Type;
    char     TypeTitle[64];
    char     TypeDesc[64];
    DWORD    BriefingTime;
    char     Guide_1[64];
    char     Guide_1_Desc[64];
    char     Guide_2[64];
    char     Guide_2_Desc[64];
    char     Guide_3[64];
    char     Guide_3_Desc[64];
    char     ListImage[64];
    char     ListImage_Over[64];
    char     LoadingText[64];
    INT      TimeLimit;
    bool     Countdown;
    INT      GradeA;
    DWORD    RewardA;
    INT      GradeB;
    DWORD    RewardB;
    INT      GradeC;
    DWORD    RewardC;
    char     GradeDesc[64];
    INT      UseClass;
    char     UseName[64];
    INT      MapIndex;
    char     INIPath[128];
	INT      FreeAdmission;
	INT      AdmissionType;
	INT      AdmissionMoney;
	char     SendName[64];
	short	 RewardAType;
	INT      RewardAValue;
	INT      RewardACount;
	short	 RewardBType;
	INT      RewardBValue;
	INT      RewardBCount;
	short	 RewardCType;
	INT      RewardCValue;
	INT      RewardCCount;
	short	 PresentMent;
	short	 PresentTime;

};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Practice_Manager
//--------------------------------------------------
class LSC_Practice_Manager : public BaseDataManager<INT, LSC_Practice>
{
private:
    virtual int GetVersion()
    {
        return 0x00048a72;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Practice* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Practice*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_Practice__H__
