#pragma once


#include "ioComplexStringPrinter.h"

#define MAX_DESC_COLOR 10
#define MAX_DESC_LINE 2

class ioINILoader;
class ioGrowthUpInfo;
class ioPlayStage;

///////////////////////////////////////////////////////////////////////////////////////////

class ioGrowthInfo
{
protected:
	int m_iGrowthInfoNum;
	GrowthType m_GrowthType;
	ioHashString m_GrowthName;
	ioComplexStringPrinter m_MainDesc[MAX_DESC_LINE];

	ioGrowthUpInfo* m_pGrowthUpInfo;

public:
	void LoadProperty( ioINILoader &rkLoader );

protected:
	void ClearGrowthUpInfoList();
	ioGrowthUpInfo* CreateGrowthUpInfo();

public:
	inline int GetGrowthInfoNum() const { return m_iGrowthInfoNum; }
	inline int GetGrowthType() const { return (int)m_GrowthType; }

	const ioHashString& GetGrowthInfoName() const { return m_GrowthName; }
	ioComplexStringPrinter* GetGrowthInfoDesc() { return m_MainDesc; }


	ioGrowthUpInfo* GetGrowthUpInfo( int iLevel );

	bool CheckGrowthInfoType( int iType ) const;

	//void SetItemReinForceLv( int nLv );

public:
	ioGrowthInfo();
	virtual ~ioGrowthInfo();
};

typedef struct tagReinforceAddStat
{
	int m_nReinforceLv;
	int m_nAddStat;

	tagReinforceAddStat()
	{
		m_nReinforceLv = 0;
		m_nAddStat = 0;
	}
}ReinforceAddStat;

///////////////////////////////////////////////////////////////////////////////////////////

class ioGrowthInfoManager : public Singleton< ioGrowthInfoManager >
{
protected:
	typedef std::vector< ioGrowthInfo* > GrowthInfoList;
	typedef std::vector< ReinforceAddStat > ReinforceAddStatList;

	GrowthInfoList m_vGrowthInfoList;
	ReinforceAddStatList m_vReinforceAddStatList;

	GrowthTypeInfoVec m_vGrowthTypeInfoList;

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

	float m_fDiscountRate;
	float m_fReturnRate;
	float m_fReturnAllRate;

	float m_fRate;

	__int64 m_iGrowthInitPeso;

	int m_iGetGrowthPoint;
	int m_iCharGrowthUpPoint;
	int m_iItemGrowthUpPoint;
	int m_iWomanTotalGrowthPoint;

	ioHashString m_ZeroLevelDesc;

	//¼Û±â¿î¾¾ ¹ë·±½º Áö¿ø( À±ÅÂÃ¶ )
	bool m_bUseNextLevelUpCheck;

	//int m_nRebalanceNormalMaxLv;
	//std::vector<int> m_vRebalanceHightLevelValue;

public:
	void LoadGrowthInfoList();

protected:
	void ClearGrowthInfoList();

public:
	ioGrowthInfo* GetGrowthInfo( int iInfoNum );
	ioGrowthUpInfo* GetGrowthUpInfo( int iInfoNum, int iLevel );

	int GetGrowthUpNeedPoint( bool bChar );
	__int64 GetGrowthUpNeedPeso( bool bChar, int iLevel );
	__int64 GetGrowthReturnPeso( bool bChar, int iLevel );
	__int64 GetGrowthReturnAllPeso( bool bChar, int iLevel );

	DWORD GetTimeGrowthNeedTime( bool bChar, int iLevel );
	DWORD GetTimeGrowthConfirmTime( bool bChar, int iLevel );

	int GetMaxLevel();

	int GetTotalGrowthPoint( int iLevel );

	inline int GetLevelUpGrowthPoint() const { return m_iGetGrowthPoint; }
	inline __int64 GetGrowthInitPeso() const { return m_iGrowthInitPeso; }
	inline float GetDiscountRate() const { return m_fDiscountRate; }

	inline const ioHashString& GetZeroLevelDesc() const { return m_ZeroLevelDesc; }

	bool CheckGrowthInfoType( int iInfoNum, int iType );

	inline int GetTimeGrowthLimitLevel() const { return m_iTimeGrowthLimitLevel; }
	inline int GetTimeGrowthEnableCharCnt() const { return m_iTimeGrowthEnableCharCnt; }
	inline int GetTimeGrowthEnableTotalCnt() const { return m_iTimeGrowthEnableTotalCnt; }

	inline int GetWomanTotalGrowthPoint() const { return m_iWomanTotalGrowthPoint; }

	ioHashString GetGrowthTypeDesc( int iGrowthType );

	//int GetRebalanceHightLevelValue( int nLevel );

	//°­È­·¹º§¿¡ µû¸¥ Ãß°¡ ½ºÅÈ ¾ò±â
	int GetReinforceAddStat( int nReinforce );

	bool GetUseNextLvInfo(){ return m_bUseNextLevelUpCheck; }

public:
	static ioGrowthInfoManager& GetSingleton();

public:
	ioGrowthInfoManager();
	virtual ~ioGrowthInfoManager();
};

#define g_GrowthInfoMgr ioGrowthInfoManager::GetSingleton()

