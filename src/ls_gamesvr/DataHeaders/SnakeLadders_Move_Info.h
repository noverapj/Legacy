
#ifndef __SnakeLadders_Move_Info__H__
#define __SnakeLadders_Move_Info__H__

#include "BaseDataManager.h"

#pragma pack(push, 1)
struct SnakeLadders_Move
{
	INT			Index;
	SHORT		SnakeGroup; 
	SHORT		SnakeStart;
	SHORT		SnakeEnd;
};
#pragma pack(pop)

class SnakeLadders_Move_Info : public BaseDataManager<INT, SnakeLadders_Move>
{
private:
	virtual int GetVersion()
	{
		//return 0x00007304;
		return 0x000083e7;
	}

	virtual void CreateMapData()
	{
		int i;
		for (i = 0; i < m_nTotal; ++i)
		{
			SnakeLadders_Move* pInfo = GetAt(i);
			if (pInfo)
			{
				m_mapData.insert(std::pair<INT, SnakeLadders_Move*>(pInfo->Index, pInfo));
			}
		}
	}
};
#endif //__SnakeLadders_Move_Info__H__