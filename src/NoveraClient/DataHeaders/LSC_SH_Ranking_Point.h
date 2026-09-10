#ifndef __LSC_SH_Ranking_Point__H__
#define __LSC_SH_Ranking_Point__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SH_Ranking_Point (구조체를 1~10번에서 배열로 수동으로 수정
//--------------------------------------------------
struct LSC_SH_Ranking_Point
{
	BYTE     floor;
	WORD     Floor_Rank[10];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SH_Ranking_Point_Manager
//--------------------------------------------------
class LSC_SH_Ranking_Point_Manager : public BaseDataManager<BYTE, LSC_SH_Ranking_Point>
{
private:
    virtual int GetVersion()
    {
        return 0x00014151;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_SH_Ranking_Point* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<BYTE, LSC_SH_Ranking_Point*>(pInfo->floor, pInfo));
            }
        }
    }
};


#endif //__LSC_SH_Ranking_Point__H__
