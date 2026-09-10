#ifndef __LSC_MsgInfo__H__
#define __LSC_MsgInfo__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_MsgInfo
//--------------------------------------------------
struct LSC_MsgInfo
{
    char     MsgType[64];
    char     Msg[64];
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_MsgInfo_Manager
//--------------------------------------------------
class LSC_MsgInfo_Manager : public BaseDataManager<ioHashString, LSC_MsgInfo>
{
private:
    virtual int GetVersion()
    {
        return 0x00004585;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_MsgInfo* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<ioHashString, LSC_MsgInfo*>(pInfo->MsgType, pInfo));
            }
        }
    }
};


#endif //__LSC_MsgInfo__H__
