#ifndef __LSC_motionoption_info__H__
#define __LSC_motionoption_info__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_motionoption_info
//--------------------------------------------------
struct LSC_motionoption_info
{
    DWORD    type;
    char     title[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_motionoption_info_Manager
//--------------------------------------------------
class LSC_motionoption_info_Manager : public BaseDataManager<DWORD, LSC_motionoption_info>
{
private:
    virtual int GetVersion()
    {
        return 0x0000549f;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_motionoption_info* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_motionoption_info*>(pInfo->type, pInfo));
            }
        }
    }
};


#endif //__LSC_motionoption_info__H__
