#ifndef __LSC_heroitem_info__H__
#define __LSC_heroitem_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_heroitem_info
//--------------------------------------------------
struct LSC_heroitem_info
{
    DWORD    item_code;
    char     name[64];
    char     char_group[64];
    BYTE     hero_group;
    BYTE     hero_ranking;
    BYTE     preset_gender;
    WORD     preset_man_face;
    WORD     preset_man_hair;
    WORD     preset_man_skincolor;
    WORD     preset_man_haircolor;
    WORD     preset_man_underwear;
    WORD     preset_woman_face;
    WORD     preset_woman_hair;
    WORD     preset_woman_skincolor;
    WORD     preset_woman_haircolor;
    WORD     preset_woman_underwear;
    WORD     voice_type;
    char     tag_char_slot[260];
    char     tag_char_back[260];
    char     tag_char_gauge[260];
    char     icon_name[260];
    char     result_image[260];
    char     illust[260];
    char     background[260];
    char     leader_img[260];
    char     command_img[260];
    char     tooltip[64];
    char     hero_info_desc[64];
    char     name_image[260];
    char     big_illust[260];
    DWORD    value_1;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_heroitem_info_Manager
//--------------------------------------------------
class LSC_heroitem_info_Manager : public BaseDataManager<DWORD, LSC_heroitem_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0004b467;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_heroitem_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_heroitem_info*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_heroitem_info__H__
