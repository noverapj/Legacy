#ifndef __LSC_guild_mark_info__H__
#define __LSC_guild_mark_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_guild_mark_info
//--------------------------------------------------
struct LSC_guild_mark_info
{
    WORD               guild_mark_index;
    GUILD_MARK_TYPE    guild_mark_type;
    BYTE               guild_mark_slot;
    BYTE               guild_mark_limit_lv;
    char               guild_mark_icon[64];
    char               guild_mark_small_icon[64];
    char               guild_mark_color[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_guild_mark_info_Manager
//--------------------------------------------------
class LSC_guild_mark_info_Manager : public BaseDataManager<WORD, LSC_guild_mark_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0001726c;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_guild_mark_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_guild_mark_info*>(pInfo->guild_mark_index, pInfo));
            }
        }
    }
};


#endif //__LSC_guild_mark_info__H__
