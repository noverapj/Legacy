#ifndef __LSC_New_Gashapon_info__H__
#define __LSC_New_Gashapon_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_New_Gashapon_info
//--------------------------------------------------
struct LSC_New_Gashapon_info
{
    DWORD    ItemIndex;
    DWORD    EtcItemCode;
    DWORD    CategoryIndex;
    DWORD    PackageIndex;
    DWORD    ElementIndex;
    WORD     CategoryType;
    DWORD    Category_Rand;
    DWORD    Category_Package_Rand;
    DWORD    Category_Package_Element_Type;
    DWORD    Category_Package_Element_Value1;
    DWORD    Category_Package_Element_Value2;
    DWORD    Period;
    BYTE     Package_Alarm;
    DWORD    Category_Package_Ment;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_New_Gashapon_info_Manager
//--------------------------------------------------
class LSC_New_Gashapon_info_Manager : public BaseDataManager<DWORD, LSC_New_Gashapon_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00025a26;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_New_Gashapon_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_New_Gashapon_info*>(pInfo->ItemIndex, pInfo));
            }
        }
    }
};


#endif //__LSC_New_Gashapon_info__H__
