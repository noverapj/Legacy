#pragma once

class TradeItemBtn : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN			= 1,
		ID_CUSTOM_BTN		= 2,	
	};

	enum
	{
		MAX_DESC			= 2,
	};

protected:
	bool m_bWidthCutDescArray[MAX_DESC];
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	TradeItemInfo m_ItemInfo;

	ioUIRenderImage *m_pEmptyImg;

	int m_nGradeType;

protected:
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	void SetIsWidthCutDesc( int iArray , bool bWidthCut );
	bool IsWidthCutDesc( int iArray );
	bool SetExtraItemInfo();

public:
	bool IsCustomToolTip();	

public:
	void SetBlank();
	void SetInfo( TradeItemInfo &rkInfo );

	TradeItemInfo& GetInfo();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	TradeItemBtn();
	virtual ~TradeItemBtn();
};

//////////////////////////////////////////////////////////////////////////////

class TradeItemBuyWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN		= 1,
		ID_BUY_BTN		= 2,
		ID_CANCEL_BTN	= 3,
		ID_EXIT_BTN		= 4,

		MAX_DESC		= 3,
	};

protected:
	TradeItemInfo m_ItemInfo;
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ClearDesc();

public:
	void SetTradItemBuyInfo( TradeItemInfo &rkInfo );

public:
	TradeItemBuyWnd();
	virtual ~TradeItemBuyWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TradeItemBuyResultWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN		= 1,
		ID_ITEM_GO_BTN	= 2,
		ID_OK_BTN		= 3,
		ID_EXIT_BTN		= 4,
	};

protected:
	ioHashString m_szName;
	__int64 m_iPrice;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetBuyResultInfo( DWORD dwItemType, DWORD dwMagicCode, DWORD dwValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iPrice );

public:
	TradeItemBuyResultWnd();
	virtual ~TradeItemBuyResultWnd();
};

////////////////////////////////////////////////////////////////////////////////

class TradeMainWnd : public ioWnd
{
public:
	enum
	{
		ID_ITEM_REG_BTN			= 1,
		ID_SLOT_SORT_BTN		= 2,
		ID_GRADE_SORT_BTN		= 3,
		ID_PRICE_SORT_BTN		= 4,
		ID_SEARCH_BTN			= 5,
		ID_SEARCH_CLEAR_BTN		= 6,
		ID_SEARCH_EDIT			= 7,
		ID_REFRESH_BTN			= 8,

		ID_TRADE_ITEM_BTN1		= 101,
		ID_TRADE_ITEM_BTN2		= 102,
		ID_TRADE_ITEM_BTN3		= 103,
		ID_TRADE_ITEM_BTN4		= 104,
		ID_TRADE_ITEM_BTN5		= 105,
		ID_TRADE_ITEM_BTN6		= 106,
		ID_TRADE_ITEM_BTN7		= 107,
		ID_TRADE_ITEM_BTN8		= 108,
		ID_TRADE_ITEM_BTN9		= 109,
		ID_TRADE_ITEM_BTN10		= 110,
		ID_TRADE_ITEM_BTN11		= 111,
		ID_TRADE_ITEM_BTN12		= 112,
		ID_TRADE_ITEM_BTN13		= 113,
		ID_TRADE_ITEM_BTN14		= 114,
		ID_TRADE_ITEM_BTN15		= 115,
		ID_TRADE_ITEM_BTN16		= 116,
		ID_TRADE_ITEM_BTN17		= 117,
		ID_TRADE_ITEM_BTN18		= 118,
		ID_TRADE_ITEM_BTN19		= 119,
		ID_TRADE_ITEM_BTN20		= 120,

		ID_PRE_PAGE_BTN			= 201,
		ID_NEXT_PAGE_BTN		= 202,
		
		ID_SLOT_SORT_WND		= 301,
		ID_GRADE_SORT_WND		= 302,
		ID_PRICE_SORT_WND		= 303,
	};

	enum TradeItemType
	{
		TIT_NONE,
		TIT_NORMAL,		//사용되지 않음(아이템 그레이드와 등급을 동일하게 가져가기 위해 선언만 함)
		TIT_PRIMIUM,
		TIT_RARE,
		TIT_UNIQUE,
		TIT_LEGEND,
		TIT_IDOL,
	};

	enum
	{
		PRICE_NONE,
		PRICE_UP,
		PRICE_DOWN,
	};

protected:
	int						m_iCurPage;
	int						m_iMaxPage;

	TradeItemInfoList		m_vTradeItemInfoList;

	vNewShopPullDownItem	m_vSlotSortTypeList;
	vNewShopPullDownItem	m_vGradeSortTypeList;
	vNewShopPullDownItem	m_vPriceSortTypeList;

	ioEdit*					m_pPreEdit;
	ioHashString			m_CurrFindKeyWord;

	int						m_eCurrItemType;
	int						m_eGradeType;
	int						m_ePriceType;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void EnableEditFocus();
	void DisableEditFocus();
	void ProcessKeyWord();

	void CreatePullDown( ioWnd *pWnd, DWORD dwPullDownWndID, vNewShopPullDownItem& ItemList );

protected:
	void SetSortTypeList();
	void SetGradeSortTypeList();
	void SetPriceSortTypeList();

	bool ChangeCategory( IN ioWnd *pWnd, IN int Param, IN vNewShopPullDownItem& ItemList, OUT int& eType );
	void PrevPage();
	void NextPage();

	void UpdatePage();

	void InitTradeItemBtn();

public:
	void UpdateTradeItemList();
	void OnRefreshTradeItemList();

protected:
	void ProcessCustomBtnTooltip();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	TradeMainWnd();
	virtual ~TradeMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyTradeItemCustomBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pCustomImg;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	LobbyTradeItemCustomBtn();
	virtual ~LobbyTradeItemCustomBtn();
};
//////////////////////////////////////////////////////////////////////////
class LobbyTradeItemBtn : public ioButton
{
protected:
	enum
	{
		ID_ICON_BTN	= 1,
		ID_CUSTOM_BTN = 2,
		MAX_DESC	= 3,
	};
public:
	void SetBlank();

protected:
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	TradeItemInfo m_ItemInfo;
	ioUIRenderImage *m_pEmptyImg;

	int m_nGradeType;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

protected:
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

public:
	void SetTradeInfo( TradeItemInfo &rkInfo );
	TradeItemInfo& GetInfo();
	bool IsCustomToolTip();	

public:
	LobbyTradeItemBtn();
	virtual ~LobbyTradeItemBtn();
};
//////////////////////////////////////////////////////////////////////////
class LobbyOwnerTradeListWnd : public ioWnd
{
public:
	enum
	{
		ID_PRE_BTN		= 1,
		ID_NEXT_BTN		= 2,

		ID_TRADE_BTN1	= 10,
		ID_TRADE_BTN2	= 11,
		ID_TRADE_BTN3	= 12,
		ID_TRADE_BTN4	= 13,
		ID_TRADE_BTN5	= 14,
		ID_TRADE_BTN6	= 15,
		ID_TRADE_BTN7	= 16,
	};

	enum
	{
		OWNER_MAX_TRADE	= 7,
	};

	enum
	{
		QUICK_LOBBY_PAGE_Y = 422,
		LOBBY_PAGE_Y	   = 423,
	};
	
protected:
	int						m_iCurPage;
	int						m_iMaxPage;
	int						m_iPageYPos;

	DWORD					m_dwMaxTradeList;

	TradeItemInfoList		m_vTradeItemInfoList;
	ioComplexStringPrinter	m_PagePrinter;
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void InitTradeItemBtn();
	void PrevPage();
	void NextPage();

	void UpdatePage();

public:
	void PlazaUIMove( int nAddYPos = 0 );
	inline void SetMaxTradeList( DWORD dwMax ){ m_dwMaxTradeList = dwMax; }

public:
	void UpdateTradeItemList();
	void OnRefreshTradeItemList();

public:
	LobbyOwnerTradeListWnd();
	virtual ~LobbyOwnerTradeListWnd();
};

//////////////////////////////////////////////////////////////////////////

class LobbyTradeInfoWnd : public ioWnd
{
public:
	enum
	{
		ID_MY_INFO_BTN = 1,
		ID_NOVICE_GRADE_BTN			= 2,
		ID_NOVICE_GRADE_TOOTIP_BTN  = 3,
		ID_NOVICE_GRADE_TOOTIP		= 4,

		ID_OWNER_TRADE_LIST_WND	= 10,
		//ID_LOBBY_USER_LIST_WND	= 11,
	};

protected:
	bool m_bIsNoviceGrade;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void OnRefreshTradeItemList();

protected:
	void SetNamedTitleSparkle();

public:
	LobbyTradeInfoWnd();
	virtual ~LobbyTradeInfoWnd();
};

//////////////////////////////////////////////////////////////////////////////

class TradeItemCancelWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN			= 1,
		ID_OK_BTN			= 2,
		ID_ITEM_CANCEL_BTN	= 3,
		ID_EXIT_BTN			= 4,

		MAX_DESC		= 2,
	};

protected:
	TradeItemInfo m_ItemInfo;
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ClearDesc();

public:
	void SetTradItemCancelInfo( TradeItemInfo &rkInfo );

public:
	TradeItemCancelWnd();
	virtual ~TradeItemCancelWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class TradeItemCancelResultWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN		= 1,
		ID_ITEM_GO_BTN	= 2,
		ID_OK_BTN		= 3,
		ID_EXIT_BTN		= 4,
	};

protected:
	ioHashString m_szName;
	__int64 m_iPrice;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetCancelResultInfo( DWORD dwItemType, DWORD dwMagicCode, DWORD dwValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iPrice );

public:
	TradeItemCancelResultWnd();
	virtual ~TradeItemCancelResultWnd();
};

////////////////////////////////////////////////////////////////////////////////
