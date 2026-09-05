

#ifndef _ioItemInfoManager_h_
#define _ioItemInfoManager_h_

class User;
class ioItem;

#include "../Util/Singleton.h"
#include "../Util/IORandom.h"

struct BaseCharInfo
{
	float m_fDefaultHP;
	float m_fDefaultSpeed;

	DWORD m_dwBlowProtectionTime;
	DWORD m_dwNoInputDelayTime;
	DWORD m_dwStartProtectionTime;
	DWORD m_dwStartProtectionTime2;

	float m_fDefaultRecoveryGauge;
	DWORD m_dwDefaultRecoveryGaugeTic;
	float m_fDelayRunGaugeRate;
	float m_fEtcGaugeRate;

	float m_fDefaultRecover;
	DWORD m_dwDefaultRecoveryTick;
};

struct SkillInfo
{
	float m_fNeedGauge;
};

typedef std::map< ioHashString, const SkillInfo* > SkillInfoNameMap;

struct ItemInfo
{
	ioHashString m_Name;

	int m_iItemCode;
	int m_iGroupIndex;
	int m_iCreateMaxLimit;

	DWORD m_dwSetCode;

	bool m_bNotDeleteItem;
	int  m_iCrownItemType;
	int  m_iItemTeamType;

	int m_iEnableClass;
	int m_iDefaultReinforceMin;
	int m_iDefaultReinforceMax;
	int m_iGradeType;

	float m_fBaseMaxGauge;
	float m_fBaseArmorClass;
	float m_fBaseSpeedClass;

private:
	IntVec m_AddExpertList;
	IntVec m_ReachExpertList;
	IntVec m_LevelUpPesoList;

public:
	int GetAddExpert(int iLevel) const
	{
		int iSize = m_AddExpertList.size();
		int iLevelInList = iLevel - 1;
		if(COMPARE(iLevelInList, 0, iSize))
			return m_AddExpertList[iLevelInList];
		else
			return -1;
	}
	int GetReachExpert(int iLevel) const
	{
		int iSize = m_ReachExpertList.size();
		int iLevelInList = iLevel - 1;
		if(COMPARE(iLevelInList, 0, iSize))
			return m_ReachExpertList[iLevelInList];
		else
			return -1;
	}
	int GetLevelUpPeso(int iLevel) const
	{
		int iSize = m_LevelUpPesoList.size();
		int iLevelInList = iLevel - 1;
		if(COMPARE(iLevelInList, 0, iSize))
			return m_LevelUpPesoList[iLevelInList];
		else
			return -1;
	}
	void ReserveAddExpert(int iReserve) {m_AddExpertList.reserve(iReserve);}
	void ReserveReachExpert(int iReserve) {m_ReachExpertList.reserve(iReserve);}
	void ReserveLevelUpPeso(int iReserve) {m_LevelUpPesoList.reserve(iReserve);}

	void ClearAddExpert() {m_AddExpertList.clear();}
	void ClearReachExpert() {m_ReachExpertList.clear();}
	void ClearLevelUpPeso() {m_LevelUpPesoList.clear();}

	void PushBackAddExpert(int iValue) {m_AddExpertList.push_back(iValue);}
	void PushBackPeachExpert(int iValue) {m_ReachExpertList.push_back(iValue);}
	void PushBackLevelUpPeso(int iValue) {m_LevelUpPesoList.push_back(iValue);}
};

typedef std::vector< ItemInfo* > InfoVector;
typedef std::vector< const ItemInfo* > ConstInfoVector;

class ioItemMaker;

class ioItemInfoManager : public Singleton< ioItemInfoManager >
{
protected:
	typedef std::map< ioHashString, const ItemInfo* > InfoNameMap;
	typedef std::map< int, const ItemInfo* > InfoCodeMap;
	typedef std::map< int, ConstInfoVector* > GroupInfoMap;

	InfoCodeMap m_CodeMap;		// 검색용
	InfoNameMap m_NameMap;		// 검색용
	GroupInfoMap m_GroupMap;	// 검색용 ( 그룹단위 )
	InfoVector m_InfoList;		// 메모리관리풀

	IORandom m_ReinforceRandom;

	BaseCharInfo m_BaseCharInfo;
	SkillInfoNameMap m_SkillInfo;

public:
	void LoadItemInfo( const char *szFileName );
	
	// LoadItemInfo 이후 실행
	void LoadBaseCharInfo();
	void LoadBaseItemInfo();
	void LoadBaseSkillInfo();

	bool CheckBaseCharInfo( User *pUser,
							float fDefaultHP,
							float fDefaultSpeed,
							DWORD dwBlowProtectionTime,
							DWORD dwNoInputDelayTime,
							DWORD dwStartProtectionTime,
							DWORD dwStartProtectionTime2,
							float fDefaultRecoveryGauge,
							DWORD dwDefaultRecoveryGaugeTic,
							float fDelayRunGaugeRate,
							float fEtcGaugeRate,
							float fDefaultRecover,
							DWORD dwDefaultRecoveryTick );

	bool CheckBaseItemInfo( User *pUser,
							DWORD dwCode,
							float fBaseMaxGauge,
							float fBaseArmorClass,
							float fBaseSpeedClass );

	bool CheckBaseSkillInfo( User *pUser,
							 const ioHashString &szSkillName,
							 float fNeedGauge );
 
protected:
	void AddItemInfo( ioINILoader &rkLoader );
	void LoadBaseItemInfo( ioINILoader &rkLoader );

	ItemInfo* GetItemInfoByPool( int iItemCode );

	void ClearAllInfo();

public:
	const ItemInfo* GetItemInfo( int iItemCode ) const;
	const ItemInfo* GetItemInfo( const ioHashString &rkName ) const;
	const ConstInfoVector* GetInfoVector( int iGroupIdx ) const;

	const SkillInfo* GetSkillInfo( const ioHashString &rkName ) const;

	DWORDVec GetSetItemList( DWORD dwSetCode ) const;

	ioItemMaker* CreateItemMaker();

	int GetDefaultReinforce( int iItemCode );

public:
	static ioItemInfoManager& GetSingleton();

public:
	ioItemInfoManager();
	virtual ~ioItemInfoManager();
};

#define g_ItemInfoMgr ioItemInfoManager::GetSingleton()

class ioItemMaker
{
private:	// ioItemInfoManager 만이 생성 가능
	friend class ioItemInfoManager;

private:
	ioItemInfoManager *m_pCreator;
	DWORD m_dwNextCreateIndex;

	typedef std::map< int, int > ItemCreateMap;
	ItemCreateMap m_ItemCreateMap;	// 생성된 녀석들 검사용

public:
	ioItem* CreateItem( int iItemCode );
	ioItem* CreateItem( const ioHashString &rkName );

private:
	ioItem* AddNewItem( const ItemInfo *pInfo );
	DWORD GetNextCreateIndex();

public:
	void NotifyItemDestroyed( int iItemCode );
	void IncreaseItemCreateCnt( int iItemCode );
	int GetCreatedCnt( int iItemCode ) const;

private:
	ioItemMaker( ioItemInfoManager *pCreator );

public:
	virtual ~ioItemMaker();
};

class ioItemPriceManager : public Singleton< ioItemPriceManager >
{
public:
	enum
	{
		DB_LOAD_COUNT       = 150,             // 한 라인에 100개의 용병 가격 로드
		DEFAULT_RESELL_PESO = 3750,
	};

	enum PriceType
	{
		PT_NORMAL  = 0,
		PT_PREMIUM = 1,
		PT_RARE    = 2,
	};

private:	
	struct PriceData
	{
		int   m_iSetCode;              
		bool  m_bActive;
		bool  m_bPcRoomActive;
		bool  m_bFreeDayHero;
		int   m_iBuyPeso;
		int   m_iBuyCash;
		int   m_iBonusPeso;
		PriceType  m_eType;
#ifdef HERO_POSSIBLE_CHECK
		bool  m_bHeroPossibleUse;
#endif
		int m_iSubscriptionType;
		
		__int64 m_iPcRoomStartDate;
		__int64 m_iPcRoomEndDate;

		PriceData()
		{
			m_iSetCode			= 0;
			m_bActive			= false;
			m_bPcRoomActive		= false;
			m_bFreeDayHero		= false;
			m_iBuyPeso			= 0;
			m_iBuyCash			= 0;
			m_iBonusPeso		= 0;
			m_eType				= PT_NORMAL;
			m_iSubscriptionType =  SUBSCRIPTION_NONE;
			m_iPcRoomStartDate	= 0;
			m_iPcRoomEndDate	= 0;
#ifdef HERO_POSSIBLE_CHECK
			m_bHeroPossibleUse	= true;
#endif
		}
	};

	typedef std::vector< PriceData* > vPriceData; // vector을 다른것으로 대체할때 INI Reload 개선 필요

	class PriceDataSort
	{
	public:
		bool operator()( const PriceData *lhs , const PriceData *rhs ) const
		{
			if( lhs->m_iBuyPeso < rhs->m_iBuyPeso )
			{
				return true;
			}
			else if( lhs->m_iBuyPeso == rhs->m_iBuyPeso )
			{
				if( lhs->m_iSetCode < rhs->m_iSetCode )
				{
					return true;
				}
			}
			return false;
		}
	};
	vPriceData m_vPriceList;

	struct LimitData
	{
		int   m_iLimitDate;
		float m_fLimitPricePer;

		LimitData()
		{
			m_iLimitDate	 = 0;			
			m_fLimitPricePer = 1.0f;
		}
	};

	typedef std::vector< LimitData* > vLimitData; // vector을 다른것으로 대체할때 INI Reload 개선 필요
	vLimitData m_vPesoLimitDataList;
	vLimitData m_vCashLimitDataList;
	vLimitData m_vPremiumCashLimitDataList;
	vLimitData m_vRareCashLimitDataList;

	DWORD m_dwFirstHireLimit;                //최초 용병 고용 기간
	DWORD m_dwDefaultLimit;                  //기본 용병 고용 기간
	DWORD m_dwBankruptcyLimit;               //파산 용병 고용 기간

	int   m_iBankruptcyCount;                //파산시 지급 용병 범위 정렬된 리스트의 0 ~
	float m_fLimitExtendDiscount;            //재고용 할인가격
	int   m_iBankruptcyPeso;                 //파산시 지급하는 페소

	float m_fMortmainCharMultiply;           //무제한 용병 판매가격 곱하기 >> (2시간 고용가격) x m_fMortmainCharMultiply
	float m_fTimeCharDivision;               //기간제 용병 되팔기 보상 나누기 >> { (해당용병의 현재 2시간 고용가격 / 7200) x 해당 용병의 남은 보유시간 } / m_fTimeCharDivision
	float m_fMortmainCharDivision;           //무제한 용병 되팔기 보상 나누기 >> { (2시간 고용가격) x m_fMortmainCharMultiply } / m_fMortmainCharDivision
	float m_fMortmainCharMultiplyCash;       //무제한 용병의 캐쉬 판매가격 곱하기 >> cash x m_fMortmainCharMultiplyCash
	float m_fMortmainPremiumCharMultiplyCash;//무제한 프리미엄 용병의 캐쉬 판매가격 곱하기 >> cash x m_fMortmainPremiumCharMultiplyCash
	float m_fMortmainRareCharMultiplyCash;   //무제한 레어 용병의 캐쉬 판매가격 곱하기 >> cash x m_fMortmainRareCharMultiplyCash

	DWORD m_dwCurrentTime;
	void ClearAllInfo();
	
public:
	bool LoadPriceInfo( const char *szFileName, bool bCreateLoad = true );
	//void SetWeekBuyCntAndPeso( int iBuyPeso[ioItemPriceManager::DB_LOAD_COUNT] );
	void SetWeekBuyCntAndPeso( boost::unordered_map<int, int>& mClassPrice );

public:
	int GetSubscriptionType( int iClassType );
	int  GetClassBuyPeso( int iClassType, int iLimitDate, bool bResell = false );
	int  GetClassBuyCash( int iClassType, int iLimitDate );
	int GetBonusPeso( int iClassType, bool bMortmain );
	int  GetMortmainCharPeso( int iClassType, bool bResell = false );
	int  GetMortmainCharCash( int iClassType );
	int  GetLowPesoClass( int iLimitDate );
	bool IsCompareLimitDatePeso( int iClassType, int iLimitDate );
	bool IsCompareLimitDateCash( int iClassType, int iLimitDate );
	bool IsCompareBankruptcy( int iClassType, int iTutorialType );

	DWORD GetFirstHireLimit(){ return m_dwFirstHireLimit; }
	DWORD GetDefaultLimit(){ return m_dwDefaultLimit; }
	DWORD GetBankruptcyLimit(){ return m_dwBankruptcyLimit; }

	float GetLimitExtendDiscount(){ return m_fLimitExtendDiscount; }
	float GetCharDivision(){return m_fTimeCharDivision;}
	int   GetBankruptcyPeso() const { return m_iBankruptcyPeso; }

	bool IsCashOnly( int iClassType );
	bool IsPesoOnly( int iClassType );
	bool IsCashPeso( int iClassType );
	bool IsActive( int iClassType );
	bool IsPcRoomActive( int iClassType );

	int  GetTimeCharResellPeso( int iClassType, int iRemainTime );
	int  GetMortmainCharResellPeso( int iClassType );

public:
	int   GetMaxClassInfo();
	int   GetMaxActiveClass();
	int   GetMaxInActiveClass();
	bool  GetArrayClassActive( int iArray );
	DWORD GetArrayClassCode( int iArray );

public:
	int  GetCash( int iClassType );
	void SetCash( int iClassType, int iCash );
	void SetActive( int iClassType, bool bActive );
	
public:
	void FillClassBuyPriceInfo( SP2Packet &rkPacket );
	void SendClassBuyPriceInfo( User *pUser );

protected:
	PriceData *GetPriceData( int iArray );
	PriceData *GetPriceDataToSetCode( int iSetCode );
	LimitData *GetPesoLimitData( int iArray );
	LimitData *GetCashLimitData( int iArray );
	LimitData *GetPremiumCashLimitData( int iArray );
	LimitData *GetRareCashLimitData( int iArray );

public:
	void Process();

	/************************************************************************/
	/* 용병 가격을 DB에서 자동으로 책정하기 위해 구매 / 플레이에 대한 정보를*/
	/* 취합하여 하루에 한번 전송한다                                        */
	/************************************************************************/
protected:
	typedef std::map< int, __int64 > CollectedMap;
	CollectedMap m_ClassCollectedMap;
	CollectedMap m_GradeCollectedMap;
	DWORD        m_dwUpdatePriceCollectedTime;
	int          m_iQuestSoldierCollectedTime;
	DWORD        m_dwUpdateCollectedTime;

public:
	void SetQuestSoldierCollected( int iClassType );
	void SetBuySoldierCollected( int iClassType, int iClassTime );
	void SetGradePlayTimeCollected( int iGradeLevel, int iPlayTime );
	void UpdatePriceCollected();

	bool GetPcRoomActiveDate(char* szDate, __int64& iDate);
	bool IsActivePcRoomHero(int iClassType);
	bool GetPcRoomHeroActiveDate(int iClassType, __int64& iStartDate, __int64& iEndDate);
	bool IsActiveBuyHero(int iClassType);

#ifdef HERO_POSSIBLE_CHECK
	bool IsPossibleHero(int iClassType);
#endif

public:
	static ioItemPriceManager& GetSingleton();

public:
	ioItemPriceManager();
	virtual ~ioItemPriceManager();
};

#define g_ItemPriceMgr ioItemPriceManager::GetSingleton()

#endif