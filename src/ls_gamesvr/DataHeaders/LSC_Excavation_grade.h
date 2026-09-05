// 2018-01-15 by bckim, ≈ΩªÁ »Æ¿Â

#ifndef __LSC_Excavation_grade__H__
#define __LSC_Excavation_grade__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_Excavation_grade
//--------------------------------------------------
struct LSC_Excavation_grade
{
    INT      GradeIndex;
    DWORD    GradeRate;
    FLOAT    GradeValue;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_Excavation_grade_Manager
//--------------------------------------------------
class LSC_Excavation_grade_Manager : public BaseDataManager<INT, LSC_Excavation_grade>
{
private:
    virtual int GetVersion()
    {
        return 0x00007304;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_Excavation_grade* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<INT, LSC_Excavation_grade*>(pInfo->GradeIndex, pInfo));
            }
        }
    }
};

#endif //__LSC_Excavation_grade__H__
