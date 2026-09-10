#ifndef __LSC_SnakeLadders_Move__H__
#define __LSC_SnakeLadders_Move__H__


#include "BaseDataManager.h"


#pragma pack(push, 1)

//--------------------------------------------------
// LSC_SnakeLadders_Move
//--------------------------------------------------
struct LSC_SnakeLadders_Move
{
	INT      Index;
	SHORT    SnakeGroup;
	SHORT    SnakeStart;
	SHORT    SnakeEnd;
};

#pragma pack(pop)


//--------------------------------------------------
// LSC_SnakeLadders_Move_Manager
//--------------------------------------------------
class LSC_SnakeLadders_Move_Manager : public BaseDataManager<INT, LSC_SnakeLadders_Move>
{
private:
	virtual int GetVersion()
	{
		return 0x000083e7;
	}

	virtual void CreateMapData()
	{
		int i;
		for (i = 0; i < m_nTotal; ++i)
		{
			LSC_SnakeLadders_Move* pInfo = GetAt(i);
			if (pInfo)
			{
				m_mapData.insert(std::pair<INT, LSC_SnakeLadders_Move*>(pInfo->Index, pInfo));
			}
		}
	}
};


#endif //__LSC_SnakeLadders_Move__H__
