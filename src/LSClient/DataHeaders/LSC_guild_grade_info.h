#ifndef __LSC_guild_grade_info__H__
#define __LSC_guild_grade_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_guild_grade_info
//--------------------------------------------------
struct LSC_guild_grade_info
{
    DWORD    grade_index;
    char     grade_name[64];
    DWORD    grade_point;
    WORD     grade_bonus_rate;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_guild_grade_info_Manager
//--------------------------------------------------
class LSC_guild_grade_info_Manager : public BaseDataManager<DWORD, LSC_guild_grade_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000aa1f;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_guild_grade_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_guild_grade_info*>(pInfo->grade_index, pInfo));
            }
        }
    }
};


#endif //__LSC_guild_grade_info__H__
