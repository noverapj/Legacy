#ifndef __LSC_Wanted__H__
#define __LSC_Wanted__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Wanted
//--------------------------------------------------
struct LSC_Wanted
{
    BYTE     Index;
    char     TypeTitle[64];
    char     TypeDesc[64];
    char     Type_Image[64];
    char     Type_Icon_Title[64];
    char     Type_Icon[64];
    char     Wanted_Purpose[64];
    char     WantedImage[64];
    char     LoadingIcon[64];
    char     LoadingText[64];
    BYTE     Prev_Index;
    DWORD    Ready_Time;
    BYTE     Custom_Reward;
    DWORD    Reward1;
    DWORD    Reward2;
    DWORD    Reward3;
    WORD     MapIndex;
    char     INIPath[128];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Wanted_Manager
//--------------------------------------------------
class LSC_Wanted_Manager : public BaseDataManager<BYTE, LSC_Wanted>
{
private:
    virtual int GetVersion()
    {
        return 0x00022ce9;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Wanted* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<BYTE, LSC_Wanted*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_Wanted__H__
