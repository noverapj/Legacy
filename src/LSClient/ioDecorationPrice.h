#pragma once

#include "GameEnumType.h"
#include "CharacterInfo.h"

class ioINILoader;

enum 
{
	ORDER_DEFAULT    = 999999999,
};

enum
{
	UID_FACE		= 1,
	UID_HAIR		= 2,
	UID_SKIN_COLOR	= 3,
	UID_HAIR_COLOR	= 4,
	UID_KINDRED     = 5,
	UID_CLASS       = 6,
	UID_UNDERWEAR   = 7,
	UID_WEAPON       = 10,
	UID_ARMOR        = 11,
	UID_HELM         = 12,
	UID_CLOAK        = 13,
};

#define MAX_DECO_UI_LINE           4
typedef struct tagDecoData
{
	ioHashString	m_szName;
	ioHashString	m_szIcon;
	ioHashString	m_szIconEx;
	int				m_iDecoCode;
	int				m_iPeso;
	int				m_iCash;
	int				m_iBonusPeso;
	int				m_iSellPeso;
	bool			m_bShowPeso;				// true : 상점에서 페소가격을 보여줌 , false : 상점에서 캐쉬가격을 보여줌.
	bool			m_bActive;
	bool			m_bPackageActive;
	int				m_iLimitLevel;
	int				m_iManual;					// sp2_deco_manual.ini [Manual숫자] 값
	int				m_iInvenManual;			// sp2_deco_inven_manual.ini [Manual숫자] 값
	int				m_iShopOrder;				// 상점에서 정렬 순서
	int				m_iRecommendedShopOrder;   // 상점의 금주의 상품에서 정렬 순서
	int				m_iShopMarkType;			// SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	int				m_iLimitDate;

	int m_iSubscriptionType;

	tagDecoData()
	{
		m_iDecoCode				= 999999999;
		m_iPeso					= 0;
		m_iCash					= 0;
		m_iBonusPeso			= 0;
		m_iSellPeso				= 0;
		m_bShowPeso				= true;
		m_bActive				= false;
		m_bPackageActive		= false;
		m_iLimitLevel			= 0;
		m_iManual				= 0;
		m_iInvenManual			= 0;
		m_iShopOrder			= ORDER_DEFAULT;
		m_iRecommendedShopOrder	= ORDER_DEFAULT;
		m_iShopMarkType			= 0;			 // SHOP_MARK_TYPE_NONE
		m_iLimitDate			= 0;

		m_iSubscriptionType		= SUBSCRIPTION_NONE;
	}
}DecoData;
typedef std::vector< DecoData > vDecoData;
typedef struct tagDecoList
{
	ioHashString m_szTitle;
	int          m_iDecoType;
	int			 m_iPackageKeepPeso;
	vDecoData    m_vList;
	tagDecoList()
	{
		m_iDecoType = m_iPackageKeepPeso = 0;
	}

}DecoList;
typedef std::vector< DecoList > vDecoList;	

typedef struct tagDefaultList
{
	int    m_iDecoType;
	IntVec m_vDecoCodeList;
	tagDefaultList()
	{
		m_iDecoType = 0;
	}
}DefaultList;
typedef std::vector< DefaultList > vDefaultList;

class ioDecorationPrice : public Singleton< ioDecorationPrice >
{

private:
	struct SexDecoList
	{		
		ioHashString m_szName;
		ioHashString m_szINI;
		int          m_iSex; // 0 : 남자 , 1 : 여자

		vDecoList    m_vList;
		vDefaultList m_vDefaultList;
		SexDecoList()
		{	
			m_iSex = 0;
		}
	};
	typedef std::vector< SexDecoList > vSexDecoList;	
	vSexDecoList m_vSexList;

	enum 
	{
		MAX_RAND_TABLE = 500,
	};
	static int m_RandTable[MAX_RAND_TABLE];
protected:
	void  ClearSex();
	void  LoadDecoInfo();
	void  LoadDefaultDecoCodeList();

	DWORD GetRandTableValue( DWORD dwRandSeed );
public:
	void  LoadPriceInfo();	

public:
	const DecoData &GetDecoData( int iSexType, int iDecoType, int iDecoCode );
	const DecoData *GetDecoDataPtr( int iSexType, int iDecoType, int iDecoCode );
	const int GetSexType( int iSexArray ) const;		
	const int GetSexSize() const;

	const int GetDecoCode( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;
	const int GetDecoType( int iSexArray, int iDecoTypeArray ) const;
	const int GetDecoTypeSize( int iSexArray ) const;             
	const int GetDecoSize( int iSexArray, int iDecoTypeArray ) const;
	const int GetShopOrder( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;
	const int GetRecommandedShopOrder( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;
	const int GetShopMarkType( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;
	const int GetLimitDate( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;
	const bool GetActive( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;
	const bool GetPackageActive( int iSexArray, int iDecoTypeArray, int iDecoArray ) const;

	int  GetDefaultDecoCode( int iSexType, int iDecoType, DWORD dwRandSeed ); 	

	int  GetDecoPackageKeepPeso( int iSexArray, int iDecoType );
	void GetPackageBuyDefaultList( CHARACTER &rkCharInfo, ITEMSLOTVec &rkDecoList );
	ITEMSLOT GetItemSlot( CHARACTER &rkCharInfo, int iDecoType );

	const int GetShopOrderByType( int iType, int iCode );
	const int GetRecommendedShopOrderByType( int iType, int iCode );

	const bool GetActiveByType( int iType, int iCode );
	const bool GetPackageActiveByType( int iType, int iCode );
	const int GetDecoCashByType( int iType, int iCode );
	const int GetDecoPesoByType( int iType, int iCode );
	const int GetShopMarkTypeByType( int iType, int iCode );

	void SetDecoCashByType( int iType, int iCode, int iCash );
	void SetDecoPesoByType( int iType, int iCode, int iPeso );
	void SetActiveByType( int iType, int iCode, bool bActive );
	void SetPackageActiveByType( int iType, int iCode, bool bActive );

	void SetShopOrderByType( int iType, int iCode, int iShopOrder );
	void SetRecommendedShopOrderByType( int iType, int iCode, int iRecommendShopOrder );

	void SetShopMarkTypeByType( int iType, int iCode, int iShopMarkType );

	int GetDecoRecommandedCnt();

public:
	static ioDecorationPrice& GetSingleton();

public:
	ioDecorationPrice();
	virtual ~ioDecorationPrice();
};

#define g_DecorationPrice ioDecorationPrice::GetSingleton()

