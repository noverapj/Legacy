#ifndef __LSC_pet_upgrade__H__
#define __LSC_pet_upgrade__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_pet_upgrade
//--------------------------------------------------
struct LSC_pet_upgrade
{
    WORD     item_code;
    WORD     upgrade_item_code;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_pet_upgrade_Manager
//--------------------------------------------------
class LSC_pet_upgrade_Manager : public BaseDataManager<WORD, LSC_pet_upgrade>
{
private:
    virtual int GetVersion()
    {
        return 0x00005e6c;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_pet_upgrade* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<WORD, LSC_pet_upgrade*>(pInfo->item_code, pInfo));
            }
        }
    }
};


#endif //__LSC_pet_upgrade__H__
