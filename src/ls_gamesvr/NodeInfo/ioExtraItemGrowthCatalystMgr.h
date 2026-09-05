#ifndef _ioExtraItemGrowthCatalystMgr_h_
#define _ioExtraItemGrowthCatalystMgr_h_

#include "../Util/IORandom.h"

class ioExtraItemGrowthCatalystMgr : public Singleton< ioExtraItemGrowthCatalystMgr >
{
protected:
	DWORDVec m_dwLevelUPRandList;           // 0lv ~ Nlv

	DWORD    m_dwMaxReinforceRand;
	DWORDVec m_dwReinforceRandList;         // +0 ~ +N
	IORandom m_ReinforceRandom;


protected:
	void ClearData();

public:
	bool IsGrowthCatalyst( int iGrowthValue );
	bool IsMortmainLevel( int iGrowthValue );
	int  GetGrowthCatalystReinforce( int iGrowthValue );

public:
	void ApplyLoadData( SP2Packet &rkPacket );

public:
	static ioExtraItemGrowthCatalystMgr& GetSingleton();

public:
	ioExtraItemGrowthCatalystMgr();
	virtual ~ioExtraItemGrowthCatalystMgr();
};
#define g_ExtraItemGrowthCatalystMgr ioExtraItemGrowthCatalystMgr::GetSingleton()
#endif