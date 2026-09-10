#ifndef __LSC_lobby_banner_info__H__
#define __LSC_lobby_banner_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_lobby_banner_info
//--------------------------------------------------
struct LSC_lobby_banner_info
{
    BYTE                Index;
    char                Image[64];
    BANNER_LINK_TYPE    banner_link_type;
    char                value[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_lobby_banner_info_Manager
//--------------------------------------------------
class LSC_lobby_banner_info_Manager : public BaseDataManager<BYTE, LSC_lobby_banner_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000ae85;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_lobby_banner_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<BYTE, LSC_lobby_banner_info*>(pInfo->Index, pInfo));
            }
        }
    }
};


#endif //__LSC_lobby_banner_info__H__
