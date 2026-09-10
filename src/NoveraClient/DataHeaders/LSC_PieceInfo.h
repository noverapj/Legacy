#ifndef __LSC_PieceInfo__H__
#define __LSC_PieceInfo__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_PieceInfo
//--------------------------------------------------
struct LSC_PieceInfo
{
    DWORD    piece_index;
    char     piece_name[64];
    char     piece_icon_name[64];
    DWORD    piece_sell_peso;
    DWORD    piece_max_count;
    char     piece_inventory_sub_manual[64];
    char     tooltip[64];
    BYTE     piece_change_state;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_PieceInfo_Manager
//--------------------------------------------------
class LSC_PieceInfo_Manager : public BaseDataManager<DWORD, LSC_PieceInfo>
{
private:
    virtual int GetVersion()
    {
        return 0x00015e4d;
    }

    virtual void CreateMapData()
    {
        int i;
        for (i = 0; i < m_nTotal; ++i)
        {
            LSC_PieceInfo* pInfo = GetAt(i);
            if (pInfo)
            {
                m_mapData.insert(std::pair<DWORD, LSC_PieceInfo*>(pInfo->piece_index, pInfo));
            }
        }
    }
};


#endif //__LSC_PieceInfo__H__
