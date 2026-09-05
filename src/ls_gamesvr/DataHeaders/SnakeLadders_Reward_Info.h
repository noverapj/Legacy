
#ifndef __SnakeLadders_Reward_Info__H__
#define __SnakeLadders_Reward_Info__H__

#include "BaseDataManager.h"

#pragma pack(push, 1)
struct Reward
{
	SHORT	 RewardType;
	INT		 RewardValue1;
	INT		 RewardValue2;
};
struct SnakeLadders_Reward
{
	INT			Index;
	SHORT		RewardGroup;
	Reward		Item_Info[10];
};

struct SnakeLadders_Reward_data
{
	INT		Index;
	SHORT	RewardGroup;
	SHORT	Reward1Type;
	INT		Reward1Value1;
	INT		Reward1Value2;
	SHORT	Reward2Type;
	INT		Reward2Value1;
	INT		Reward2Value2;
	SHORT	Reward3Type;
	INT		Reward3Value1;
	INT		Reward3Value2;
	SHORT	Reward4Type;
	INT		Reward4Value1;
	INT		Reward4Value2;
	SHORT	Reward5Type;
	INT		Reward5Value1;
	INT		Reward5Value2;
	SHORT	Reward6Type;
	INT		Reward6Value1;
	INT		Reward6Value2;
	SHORT	Reward7Type;
	INT		Reward7Value1;
	INT		Reward7Value2;
	SHORT	Reward8Type;
	INT		Reward8Value1;
	INT		Reward8Value2;
	SHORT	Reward9Type;
	INT		Reward9Value1;
	INT		Reward9Value2;
	SHORT	Reward10Type;
	INT		Reward10Value1;
	INT		Reward10Value2;
};








#pragma pack(pop)

class SnakeLadders_Reward_Info : public BaseDataManager<INT, SnakeLadders_Reward_data>
{
private:
	virtual int GetVersion()
	{
		//return 0x0001dea1;
		return 0x0003baf8;
	}

	virtual void CreateMapData()
	{
		int i;
		for (i = 0; i < m_nTotal; ++i)
		{
			SnakeLadders_Reward_data* pInfo = GetAt(i);
			if (pInfo)
			{
				m_mapData.insert(std::pair<INT, SnakeLadders_Reward_data*>(pInfo->Index, pInfo));
			}
		}
	}
};
#endif //__SnakeLadders_Reward_info__H__
