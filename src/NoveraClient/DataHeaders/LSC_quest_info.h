#ifndef __LSC_quest_info__H__
#define __LSC_quest_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_quest_info
//--------------------------------------------------
struct LSC_quest_info
{
    DWORD                     Quest_main_index;
    char                      Quest_title[64];
    char                      Quest_substance[64];
    char                      Quest_icon[64];
    char                      Quest_icon_sub[64];
    char                      Quest_progress[64];
    QUEST_QUICK_START_TYPE    Quest_quick_start_type;
    BYTE                      Quest_repeat_style;
    char                      Quest_clear[64];
    BYTE                      Quest_is_event;
    BYTE                      Quest_perform_type;
    BYTE                      Quest_display_list_type;
    BYTE                      Quest_Guide_ID;
    BYTE                      Quest_progress_type;
    BYTE                      Quest_check_server_client;
    BYTE                      Quest_auto_type;
    BYTE                      Quest_Giveup_type;
    DWORD                     Quest_occur_value_no;
    char                      Quest_occur_value[64];
    BYTE                      Quest_occur_login;
    DWORD                     Quest_complete_value_no;
    char                      Quest_complete_value[64];
    WORD                      Quest_start_year;
    BYTE                      Quest_start_month;
    BYTE                      Quest_start_date;
    BYTE                      Quest_start_hour;
    WORD                      Quest_end_year;
    BYTE                      Quest_end_month;
    BYTE                      Quest_end_date;
    BYTE                      Quest_end_hour;
    BYTE                      Quest_weekly;
    DWORD                     Quest_occur_previous_quest;
    DWORD                     Quest_occur_overlap_quest;
    DWORD                     Quest_occur_quest_link;
    BYTE                      Quest_occur_pre_incomplete;
    DWORD                     Quest_occur_min_summoner_lv;
    DWORD                     Quest_occur_max_summoner_lv;
    DWORD                     Quest_occur_item;
    DWORD                     Quest_occur_item_count;
    DWORD                     Quest_occur_hero;
    BYTE                      Quest_occur_growth;
    DWORD                     Quest_occur_hero_lv;
    DWORD                     Quest_progress_Level;
    DWORD                     Quest_progress_mode1;
    DWORD                     Quest_progress_mode2;
    DWORD                     Quest_progress_mode3;
    DWORD                     Quest_progress_mode4;
    DWORD                     Quest_progress_mode5;
    DWORD                     Quest_progress_mode6;
    DWORD                     Quest_progress_mode7;
    DWORD                     Quest_progress_mode8;
    DWORD                     Quest_progress_mode9;
    DWORD                     Quest_progress_mode10;
    DWORD                     Quest_progress_hero;
    DWORD                     Quest_progress_hero_lv;
    DWORD                     class_name_no;
    char                      class_name[64];
    DWORD                     Quest_custom_value1;
    DWORD                     Quest_custom_value2;
    DWORD                     Quest_custom_value3;
    DWORD                     Quest_custom_value4;
    DWORD                     Quest_custom_value5;
    DWORD                     Quest_custom_value6;
    DWORD                     Quest_reward_present1;
    DWORD                     Quest_reward_present2;
    DWORD                     Quest_reward_present3;
    DWORD                     Quest_reward_present4;
    DWORD                     Quest_reward_present5;
    char                      Quest_sound_accept[256];
    char                      Quest_sound_complete[256];
    DWORD                     etc1;
    DWORD                     etc2;
    DWORD                     etc3;

	char* GetQuestIcon();
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_quest_info_Manager
//--------------------------------------------------
class LSC_quest_info_Manager : public BaseDataManager<DWORD, LSC_quest_info>
{
private:
    virtual int GetVersion()
    {
        return 0x000d0dcd;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_quest_info* pInfo = GetAt(i);
            if (pInfo)
            {
				auto it = m_mapData.find(pInfo->Quest_main_index);
				if(it != m_mapData.end())
				{
					LOG.PrintTimeAndLog(0, "Qst Overlap -%d-", pInfo->Quest_main_index);
					continue;
					//delete(pInfo);
				}
				else if(pInfo->class_name_no == 0)
				{			
					LOG.PrintTimeAndLog(0, "Qst NoUse -%d-", pInfo->class_name_no == 0);
					continue;
				}

                m_mapData.insert(std::pair<DWORD, LSC_quest_info*>(pInfo->Quest_main_index, pInfo));
            }
        }
    }
};


#endif //__LSC_quest_info__H__
