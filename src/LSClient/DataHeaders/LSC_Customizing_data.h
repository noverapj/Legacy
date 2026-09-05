#ifndef __LSC_Customizing_data__H__
#define __LSC_Customizing_data__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Customizing_data
//--------------------------------------------------
struct LSC_Customizing_data
{
    DWORD    Index;
    DWORD    Customizing_Code;
    BYTE     Customizing_Hero_Sex;
    WORD     Customizing_Type;
    DWORD    Customizing_ItemCode_1;
    DWORD    Customizing_ItemCode_2;
    DWORD    Customizing_ItemCode_3;
    DWORD    Customizing_ItemCode_4;
    DWORD    Customizing_ItemCode_5;
    DWORD    Customizing_ItemCode_6;
    DWORD    Customizing_ItemCode_7;
    DWORD    Customizing_ItemCode_8;
    DWORD    Customizing_ItemCode_9;
    DWORD    Customizing_ItemCode_10;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Customizing_data_Manager
//--------------------------------------------------
class LSC_Customizing_data_Manager : public BaseDataManager<DWORD, LSC_Customizing_data>
{
private:
    virtual int GetVersion()
    {
        return 0x0002b103;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Customizing_data* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_Customizing_data*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_Customizing_data__H__
