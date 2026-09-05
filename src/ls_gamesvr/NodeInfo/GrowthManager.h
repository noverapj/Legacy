
#ifndef _GrowthManager_h_
#define _GrowthManager_h_

#include "../Util/Singleton.h"

class User;

class GrowthManager : public Singleton< GrowthManager >
{
protected:
	IntVec m_vGrowthInfoList;

	int m_iMaxLevel;

	int m_iCharNeedPesoA;
	int m_iCharNeedPesoB;
	int m_iItemNeedPesoA;
	int m_iItemNeedPesoB;

	float m_fTimeGrowthConstA;
	float m_fTimeGrowthConstB;
	float m_fTimeGrowthConstC;

	int m_iTimeGrowthLimitLevel;
	int m_iTimeGrowthEnableCharCnt;
	int m_iTimeGrowthEnableTotalCnt;

	float m_fConfirmConst;

	__int64 m_iGrowthInitPeso;

	int m_iGetGrowthPoint;
	int m_iCharGrowthUpPoint;
	int m_iItemGrowthUpPoint;
	int m_iWomanTotalGrowthPoint;
	
	float m_fDiscountRate;
	float m_fReturnRate;
	float m_fReturnAllRate;
	__int64  m_iReturnAllLimitPeso;

public:
	void LoadGrowthInfo();
 
protected:
	void ClearAllInfo();

public:
	int CheckUpLevel( int iInfoNum, int iLevel );

	int CheckCurTotalGrowthPoint( int iLevel );

	int GetGrowthUpNeedPoint( bool bChar );
	int GetGrowthUpUsePoint( bool bChar, int iLevel );
	
	__int64 GetGrowthUpNeedPeso( bool bChar, int iLevel );
	__int64 GetGrowthReturnPeso( bool bChar, int iLevel );
	__int64 GetGrowthReturnAllPeso( bool bChar, int iLevel );

	DWORD GetTimeGrowthNeedTime( bool bChar, int iLevel );
	DWORD GetTimeGrowthConfirmTime( bool bChar, int iLevel );

	inline int GetLevelUpGrowthPoint() const { return m_iGetGrowthPoint; }
	inline __int64 GetGrowthInitPeso() const { return m_iGrowthInitPeso; }
	inline __int64 GetReturnAllLimitPeso() const { return m_iReturnAllLimitPeso; }

	inline int GetMaxLevel() const { return m_iMaxLevel; }

	inline int GetTimeGrowthLimitLevel() const { return m_iTimeGrowthLimitLevel; }
	inline int GetTimeGrowthEnableCharCnt() const { return m_iTimeGrowthEnableCharCnt; }
	inline int GetTimeGrowthEnableTotalCnt() const { return m_iTimeGrowthEnableTotalCnt; }

	inline int GetWomanTotalGrowthPoint() const { return m_iWomanTotalGrowthPoint; }

public:
	static GrowthManager& GetSingleton();

public:
	GrowthManager();
	virtual ~GrowthManager();
};

#define g_GrowthMgr GrowthManager::GetSingleton()

#endif
