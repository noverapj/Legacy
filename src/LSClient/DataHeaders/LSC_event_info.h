#ifndef __LSC_event_info__H__
#define __LSC_event_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_event_info
//--------------------------------------------------
struct LSC_event_info
{
    DWORD    event_index;
    char     event_title[126];
    char     event_summary[127];
    char     event_icon[128];
    char     event_icon_detail[128];
    char     event_sub_icon[128];
    BYTE     event_start_year;
    BYTE     event_start_month;
    BYTE     event_start_day;
    BYTE     event_start_hour;
    BYTE     event_start_min;
    BYTE     event_end_year;
    BYTE     event_end_month;
    BYTE     event_end_day;
    BYTE     event_end_hour;
    BYTE     event_end_min;
    BYTE     event_day_rotation;
    WORD     event_class_type;
    BYTE     event_mode1;
    BYTE     event_mode2;
    BYTE     event_mode3;
    BYTE     event_mode4;
    BYTE     event_mode5;
    WORD     event_reward_peso_buffvalue;
    WORD     event_reward_exp_buffvalue;
    DWORD    event_reward_present1;
    DWORD    event_reward_present2;
    DWORD    event_reward_present3;
    DWORD    event_reward_present4;
    DWORD    event_reward_present5;
    BYTE     event_min_summoner_level;
    BYTE     event_max_summoner_level;
    DWORD    event_value1;
    DWORD    event_value2;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_event_info_Manager
//--------------------------------------------------
class LSC_event_info_Manager : public BaseDataManager<DWORD, LSC_event_info>
{
private:
    virtual int GetVersion()
    {
        return 0x00058f67;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_event_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_event_info*>(pInfo->event_index, pInfo));
            }
        }
    }
};


#endif //__LSC_event_info__H__
