

#ifndef _ioExtraItemInfoManager_h_
#define _ioExtraItemInfoManager_h_

#include "../Util/Singleton.h"
#include "Item.h"

//////////////////////////////////////////////////////////////////////////////////////////

struct RandomItem
{
	int m_iRandomRate;
	int m_iItemCode;
	int m_iTradeTypeList;

	RandomItem()
	{
		m_iRandomRate = 0;
		m_iItemCode = 0;
		m_iTradeTypeList = 0;
	}
};
typedef std::vector< RandomItem > RandomItemList;

/////////////////////////////////////////////////////////////////////////////////////////

struct RandomPeriodTime
{
	int m_iRandomRate;
	int m_iPeriodTime;
	bool m_bAlarm;

	RandomPeriodTime()
	{
		m_iRandomRate = 0;
		m_iPeriodTime = 0;
		m_bAlarm      = false;
	}
};
typedef std::vector< RandomPeriodTime > RandomPeriodTimeList;

/////////////////////////////////////////////////////////////////////////////////////////

struct RandomReinforce
{
	int m_iRandomRate;
	int m_iReinforce;

	RandomReinforce()
	{
		m_iRandomRate = 0;
		m_iReinforce = 0;
	}
};
typedef std::vector< RandomReinforce > RandomReinforceList;

/////////////////////////////////////////////////////////////////////////////////////////

typedef struct tagRandomInfo
{
	RandomItemList m_vRandomItemList;
	RandomPeriodTimeList m_vPeriodList;
	RandomReinforceList m_vReinforceList;

	tagRandomInfo()
	{
		Init();
	}

	void Init()
	{
		m_vRandomItemList.clear();
		m_vPeriodList.clear();
		m_vReinforceList.clear();
	}

} RandomInfo;

//////////////////////////////////////////////////////////////////////////////////////////

#define TRADE_TYPE_MAX 3

struct RandomTradeType
{
	int m_iRandomRate[TRADE_TYPE_MAX];
	int m_iTradeType[TRADE_TYPE_MAX];

	DWORD m_dwTotalRate;

	RandomTradeType()
	{
		m_dwTotalRate = 0;

		for( int i=0; i < TRADE_TYPE_MAX; ++i )
		{
			m_iRandomRate[i] = 0;
			m_iTradeType[i] = 0;
		}
	}
};
typedef std::vector< RandomTradeType > RandomTradeTypeList;

/////////////////////////////////////////////////////////////////////////////////////////

struct ItemTestInfo
{
	int m_iCnt;
	int m_iTradeEnable;
	int m_iTradeNormal;
	int m_iTradeDisable;
	int m_iTypeList;

	ItemTestInfo()
	{
		m_iCnt = 0;
		m_iTradeEnable = 0;
		m_iTradeNormal = 0;
		m_iTradeDisable = 0;
		m_iTypeList = 0;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////

class ioRandomMachineGroup
{
public:
	enum MachineType  // array 순서 변경 금지 , 범위 체크하는 부분 있음
	{
		MT_NONE        = 0,
		MT_WEAPON      = 1,
		MT_ARMOR       = 2,
		MT_HELMET      = 3,
		MT_CLOAK       = 4,
		MT_ALL         = 5,
		MT_RARE_WEAPON = 6,
		MT_RARE_ARMOR  = 7,
		MT_RARE_HELMET = 8,
		MT_RARE_CLOAK  = 9,
		MT_RARE_ALL    = 10,
		MT_HIGH_EXTRA  = 11,
		MT_LIMITED_EXTRA = 12,
	};

public:
	friend class ioExtraItemInfoManager;

protected:
	int m_iMachineCode;
	IntVec m_vNeedPeso;
	IntVec m_vPeriod;
	int m_iNeedCash;
	int m_iBonusPeso;
	int m_iSellPeso;

	int m_iNeedLevel;
	bool m_bActive;
	bool m_bPackage;

	RandomItemList m_vRandomItemList;
	RandomPeriodTimeList m_vPeriodList;
	RandomReinforceList m_vReinforceList;
	RandomReinforceList m_vReinforceCashList;
	RandomTradeTypeList m_vTradeTypeList;

	DWORD m_dwTotalItemRate;
	DWORD m_dwTotalPeriodRate;
	DWORD m_dwTotalReinforceRate;
	DWORD m_dwTotalReinforceCashRate;

	MachineType  m_eDefaultType;

	int m_iSubscriptionType;
	
protected:
	void ClearAll();

public:
	void LoadMachineBasicInfo( ioINILoader &rkLoader );
	void LoadMachineRandomInfo( IN ioINILoader &rkLoader, IN bool bItemLoadAll );

	int GetNeedPeso( int iArray ) const;
	int GetPeriod( int iArray ) const;
	inline int GetSubscriptionType() const{ return m_iSubscriptionType; }
	inline int GetNeedCash() const{ return m_iNeedCash; }
	inline int GetBonusPeso() const{ return m_iBonusPeso; }
	inline int GetSellPeso() const{ return m_iSellPeso; }
	inline bool GetActive() const{ return m_bActive; }

	inline int GetNeedLevel() const { return m_iNeedLevel; }
	inline int GetMachineCode() const { return m_iMachineCode; }
	inline int GetTotalItemRate() const { return m_dwTotalItemRate; }
	inline int GetTotalPeriodRate() const { return m_dwTotalPeriodRate; }
	inline int GetTotalReinforceRate() const { return m_dwTotalReinforceRate; }
	inline int GetTotalReinforceCashRate() const { return m_dwTotalReinforceCashRate; }
	
	DWORD GetTotalTradeTypeRate( int iList );
	
	inline bool IsPackage() const { return m_bPackage; }

	int GetNeedPriceByMultiplePeso( int iPeriod );

	int GetRandomItemCode( IN int iItemRand, OUT int &iList, IN int iMachineCode);
	int GetRandomPeriod( int iPeriodRand );
	int GetRandomReinforce( int iReinforceRand );
	int GetRandomReinforceCash( int iReinforceCashRand );
	int GetRandomTradeType( int iList, int iTradeTypeRand );
	
	bool IsAlarm( int iPeriodTime );
	bool IsReinforceCash();

	void SetNeedPeso( int iNeedPeso, int iArray );
	void SetNeedCash( int iNeedCash );
	void SetActive( bool bActive );
	
	ioRandomMachineGroup::MachineType GetDefaultType() const { return m_eDefaultType; }

public:
	ioRandomMachineGroup();
	virtual ~ioRandomMachineGroup();
};

typedef std::vector< ioRandomMachineGroup* > RandomMachineGroupList;
typedef std::map< int, ioRandomMachineGroup* > RandomMachineGroupMap;

//////////////////////////////////////////////////////////////////////////////////////////


class ioExtraItemInfoManager : public Singleton< ioExtraItemInfoManager >
{
protected:
	RandomMachineGroupList m_vMachineGroupList;
	RandomMachineGroupMap  m_MachineGroupMap;
	RandomMachineGroupList m_vDefaultMachineGroupList;

	IORandom m_TotalRandom;

	IORandom m_ItemGroupRandom;
	IORandom m_ItemListRandom;
	IORandom m_PeriodTimeRandom;
	IORandom m_ReinforceRandom;
	IORandom m_TradeTypeRandom;

	int m_iLevelLimitConst;

	float m_fMortmainItemSell;
	float m_fTimeItemSell;
	float m_fItemSellConstRate;

	bool m_bTestRandomItem;

protected:
	int m_iDefaultExtraItemCount;		// 기본 장비 소지 수량

public:
	void CheckNeedReload();
	void LoadAllExtraItemInfo();
	void LoadRandomExtraItemInfo( ioINILoader &rkLoader );
	void CheckTestRandomItem();

protected:
	void ClearAllInfo();

	bool IsRightExtraItem( ioRandomMachineGroup::MachineType eType, int iItemCode );
	void CopyDefaultMachineBasicInfo( IN ioINILoader &rkLoader, OUT ioRandomMachineGroup *pGroup );
	bool CopyDefaultMachineRandomInfo( IN ioINILoader &rkLoader, OUT ioRandomMachineGroup *pGroup );

public:
	int GetSubscriptionType( int iMachineCode );

	int GetNeedPeso( int iMachineCode, int iArray );
	int GetNeedCash( int iMachineCode );
	int GetBonusPeso( int iMachineCode );
	int GetPeriod( int iMachineCode, int iArray );
	int GetSellPeso( int iMachineCode );
	bool GetActive( int iMachineCode );

	int GetNeedLevel( int iCode );
	int GetRandomItemCode( IN int iCode, OUT int &iList );
	int GetRandomPeriodTime( int iCode );
	int GetRandomReinforce( int iCode, bool bCash );
	int GetRandomTradeType( int iMachineCode, int iList );

	inline int GetLevelLimitConst() const { return m_iLevelLimitConst; }
	inline float GetItemSellConst() const { return m_fItemSellConstRate; }
	inline float GetMortmainItemSellPeso() const { return m_fMortmainItemSell; }
	inline float GetTimeItemSellPeso() const { return m_fTimeItemSell; }
	inline bool IsTestLOG() const { return m_bTestRandomItem; }

	int GetExtraItemExtendType(int iCode);

	bool IsAlarm( int iCode, int iPeriodTime );
	bool IsPackage( int iCode );

	void SetNeedCash( int iMachineCode, int iNeedCash );
	void SetNeedPeso( int iMachineCode, int iNeedPeso, int iArray );
	void SetActive( int iMachineCode, bool bActive );

	void CopyDefaultMachineAll( IN ioRandomMachineGroup::MachineType eType, OUT RandomInfo &rkInfo, OUT DWORD &rdwTotalItemRate, OUT DWORD &rdwTotalPeriodRate, OUT DWORD &rdwTotalReinforceRate, IN DWORD dwItemCode );

	int GetDefaultExtraItemCount() { return m_iDefaultExtraItemCount; }
	//int GetMaximumExtraItemCount() { return m_iMaximumExtraItemCount; }

public:
	static ioExtraItemInfoManager& GetSingleton();

public:
	ioExtraItemInfoManager();
	virtual ~ioExtraItemInfoManager();
};

#define g_ExtraItemInfoMgr ioExtraItemInfoManager::GetSingleton()

#endif
