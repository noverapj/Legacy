#pragma once
#include "../ioDecorationPrice.h"
#include "../ioEventGoodsMgr.h"
#include "../ioSaleManager.h"
#include "../ioSpecialShopMgr.h"
#include "../ioEtcItem.h"
#include "../ioEtcItemManager.h"
#include "../ioExtraItemInfoManager.h"

enum 
{
	SHOP_MARK_TYPE_NONE							= 0,
	SHOP_MARK_TYPE_SALE							= 1,
	SHOP_MARK_TYPE_NEW							= 2,
	SHOP_MARK_TYPE_HIT							= 3,
	SHOP_MARK_TYPE_EVENT						= 4,
	SHOP_MARK_TYPE_BOUGHT						= 5,
	SHOP_MARK_TYPE_LIMIT_GOODS					= 6,
	SHOP_MARK_TYPE_SOLD_OUT						= 7,
	SHOP_MARK_TYPE_EXTRAITEM_GROWTH_CATALYST	= 8,
	SHOP_MARK_TYPE_GRADE_LIMIT_CLEAR			= 9,
	SHOP_MARK_TYPE_DATE							= 10,
	SHOP_MARK_TYPE_LIMIT_DAYS					= 11,
	SHOP_MARK_TYPE_BLACKFRIDAY					= 12,
};

enum 
{
	SHOP_TAB_TYPE_NONE		= 0,
	SHOP_TAB_TYPE_SOLDIER	= 1,
	SHOP_TAB_TYPE_EXTRA		= 2,
	SHOP_TAB_TYPE_DECO		= 3,
	SHOP_TAB_TYPE_ETC		= 4,
	SHOP_TAB_TYPE_COSTUME	= 5,
	SHOP_TAB_TYPE_LIMIT		= 6,
};

enum ShopCoinType
{
	SCT_GOLD,
	SCT_PESO,
	SCT_EVENT_CASH,
};

class NewShopWnd;

//-------------------------------------------------------------------------
typedef struct tagNewShopItemInfo
{
	int		m_iItemType;		// PRESENT_SOLDIER, 동일 = PRESENT_DEFAULT가 추가되었다. 상점에서 선물을 코인으로 판매함
	int		m_iSortPoint;		// 낮을 수록 순위가 높다.
	int		m_iShopMarkType;	// SHOP_MARK_TYPE_NONE
	int		m_bShowPeso;
	IntVec	m_vPeso;
	IntVec	m_vCash;
	IntVec	m_vStamp;
	IntVec	m_vTournamentCoin;
	DWORD	m_dwStampEtcItem;	// 사용할 코인 특별 아이템
	int		m_iNeedLevel;
	IntVec	m_vBonusPeso;		// etc 여러개의 보너스페소가 존재한다.
	IntVec	m_vManualIndex;		// etc 여러개의 매뉴얼을 소유한다.
	int		m_iValue0;			// PresetDefault : PresentType
	int		m_iValue1;			// PresetDefault : PresentValue1,  Soldier : class type     , extra : machine code    , deco : type  , etc : type
	int		m_iValue2;			// PresetDefault : PresentValue2,                           , extra : group           , deco : code  , etc : shop tap type
	IntVec	m_vValue3;			// PresetDefault : index,count                              , extra : multiply period , deco : sex   , etc : array

	//예외 한정수량
	int   m_iLimit;

	IntVec m_vSubscriptionType;
	int m_iSubscriptionType;

	ioHashString m_sName;
	ioHashString m_sIcon;
	ioHashString m_sSubIcon;

	//판매시간
	int m_iLimitDate;

	tagNewShopItemInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_iItemType			= 0;
		m_iSortPoint		= 0;    
		m_iShopMarkType		= SHOP_MARK_TYPE_NONE;
		m_bShowPeso			= false;
		m_dwStampEtcItem	= 0;
		m_iNeedLevel		= 0;
		m_iValue0			= 0;
		m_iValue1			= 0;
		m_iValue2			= 0;
		m_iLimit			= 0;
		m_iLimitDate		= 0;
		m_iSubscriptionType = SUBSCRIPTION_NONE;
		
		m_vPeso.clear();
		m_vCash.clear();
		m_vStamp.clear();
		m_vTournamentCoin.clear();

		m_vBonusPeso.clear();
		m_vManualIndex.clear();

		m_vValue3.clear();
		m_vSubscriptionType.clear();
	}

	int GetPeso( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int) m_vPeso.size() ) )
			return m_vPeso[iArray];

		return 0;
	}

	int GetCash( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int) m_vCash.size() ) )
			return m_vCash[iArray];

		return 0;
	}

	int GetStamp( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int)m_vStamp.size() ) )
			return m_vStamp[iArray];
		return 0;
	}

	int GetBonusPeso( int iArray )
	{
		if( COMPARE( iArray, 0, (int) m_vBonusPeso.size() ) )
			return m_vBonusPeso[iArray];

		return 0;
	}

	int GetTournamentCoin( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int)m_vTournamentCoin.size() ) )
			return m_vTournamentCoin[iArray];

		return 0;
	}

	int GetManualIndex( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int) m_vManualIndex.size() ) )
			return m_vManualIndex[iArray];

		return 0;
	}

	int GetValue3( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int) m_vValue3.size() ) )
			return m_vValue3[iArray];

		return 0;
	}

	int GetSubscriptionType( int iArray ) const
	{
		if( COMPARE( iArray, 0, (int) m_vSubscriptionType.size() ) )
			return m_vSubscriptionType[iArray];

		return 0;
	}

}NewShopItemInfo;
typedef std::vector<NewShopItemInfo> vNewShopItemInfoVec;

namespace NewShopItemHelp
{
	bool IsAvailableDate( DWORD dwDate );

	//상점 상품정보 생성
	bool CreateSoldierItemInfo( IN const ioSetItemInfo* pInfo, OUT NewShopItemInfo& Info );
	bool CreateExtraItemInfo( IN const RandomMachineInfo* pInfo, OUT NewShopItemInfo& Info );
	bool CreateDecoItemInfo( IN const DecoData *pInfo, IN int iSexType, IN int iDecoType, OUT NewShopItemInfo& Info );
	bool CreateEtcItemInfo( IN ioEtcItem *pEtcItem, IN int ArrayIndex, OUT NewShopItemInfo& kInfo );
	bool CreateCostumeInfo( IN CostumeShopInfo *pInfo, OUT NewShopItemInfo& kInfo );

	//탭분류에 맞게 데이터 파싱	
	void ParseItemInfoVec( OUT vNewShopItemInfoVec& rkVec, IN DWORD dwCurTab, IN DWORD dwCurSmallTab, IN NewShopWnd* pShop, IN int iDecoSexType = 0 );

	void ParseRecommendItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN int iDecoSexType );

	void ParseSoldierItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop );	
	void ParseExtraItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop );
	void ParseDecoItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN int iSexType );
	void ParseEtcItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN int iShopTabType );
	void ParseEventGoodsItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop, IN DWORD dwNeedEtcItemIdx );
	void ParseCostumeItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec, IN DWORD dwTabID, IN DWORD dwSmallTabID, IN NewShopWnd* pShop );

	void DevideItemListByCoin( IN vNewShopItemInfoVec& vItemList, IN ShopCoinType eType, IN int DevideCoin, OUT vNewShopItemInfoVec& vZeroCoinList );
}