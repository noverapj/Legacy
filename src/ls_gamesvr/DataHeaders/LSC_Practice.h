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
    INT      TimeLimit;
    INT      MinTime;
    INT      GradeA;
    DWORD    RewardA;
    INT      GradeB;
    DWORD    RewardB;
    INT      GradeC;
    DWORD    RewardC;
    INT      UseClass;
    char     UseName[64];
    BYTE     Gender;
    INT      Face;
    INT      Hair;
    INT      HairColor;
    INT      SkinColor;
    INT      Underwear;
    INT      CloakIndex;
    INT      HelmetIndex;
    INT      ArmorIndex;
    INT      WeaponIndex;
    INT      MapIndex;
    INT      FreeAdmission;
    INT      AdmissionType;
    INT      AdmissionMoney;
    char     SendName[64];
    SHORT    RewardAType;
    INT      RewardAValue;
    INT      RewardACount;
    SHORT    RewardBType;
    INT      RewardBValue;
    INT      RewardBCount;
    SHORT    RewardCType;
    INT      RewardCValue;
    INT      RewardCCount;
    SHORT    PRESENTMENT;
    SHORT    PresentTime;
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
		return 0x00038a1b;
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
