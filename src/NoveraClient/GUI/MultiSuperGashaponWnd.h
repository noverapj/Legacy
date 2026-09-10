#pragma once

#include "../ioMultiSuperGashaponMgr.h"

class MultiSuperGashaponWnd :	public ioWnd
{
public:
	enum 
	{
		ID_ROLLING_BTN	        = 1,

		//Shop
		ID_BUY_ADD_BTN          = 2,
		ID_GO_BTN               = 3,

		//Inven
		ID_VIEW_BTN             = 4,

		//Etc..
		ID_VERT_SCROLL			= 1000,
	};

	enum State
	{
		S_ROLLING			= 1,
		S_UPDATE_INFO		= 2,
		S_UP_SCALING		= 3,
		S_DOWN_SCALING		= 4,
		S_END				= 5,
	};

protected:
	enum
	{
		ICON_X_OFFSET			= 129, 
		ICON_Y_OFFSET			= 105,

		TITLE_X_OFFSET			= 129,
		TITLE_Y_OFFSET			= 152,
		TITLE_GAP				= 23,

		DESC_X_OFFSET			= 27,
		DESC_Y_OFFSET			= 206,
		DESC_GAP				= 18,

		GOODS_X_OFFSET			= 46,
		GOODS_Y_OFFSET			= 227,
		GOODS_GAP				= 42,

		BOUGHT_TEXT_X_OFFSET	= 128,
		BOUGHT_TEXT_Y_OFFSET	= 391,

		GOODS_TEXT_X_OFFSET		= 68,
		GOODS_TEXT_Y_OFFSET		= 212,
		GOODS_TEXT_BETWEEN		= 16,
		GOODS_TEXT_GAP			= 42,

		REINFORCE_X				= 126,
		REINFORCE_Y				= 124,
		REINFORCE_TEXT_SIZE		= 16,
		REINFORCE_TEXT_GAP		= 4,
	};

	enum
	{
		BUY_GOOD_MAX = 4
	};

public:
	enum
	{
		CATEGORY_TYPE_COMMON = 1,
		CATEGORY_TYPE_NORMAL,
		CATEGORY_TYPE_RARE,
		CATEGORY_TYPE_UNIQUE
	};

public:
	struct ShopBoughtInfo
	{
		short	m_iPresentType;
		int     m_iPresentValue1;
		int     m_iPresentValue2;
		int		m_iTradeType;
		DWORD	m_dwCategoryType;

		ShopBoughtInfo()
		{
			m_iPresentType		= 0;
			m_iPresentValue1	= 0;
			m_iPresentValue2	= 0;
			m_iTradeType		= 0;
			m_dwCategoryType	= 0;
		}
	};
	typedef std::vector<ShopBoughtInfo> ShopBoughtInfoVec;

	struct BuyGoods
	{
		ioUIRenderImage* m_pGoodsIcon;
		ioUIRenderImage* m_pSubIcon;
		ioHashString m_szTitle;
		ioHashString m_szSubTitle;
		bool m_bIsCostume;
		int m_nGradeType;
		DWORD m_dwCategoryType;

		BuyGoods()
		{
			m_pGoodsIcon = NULL;
			m_pSubIcon = NULL;
			m_bIsCostume = false;
			m_nGradeType = 0;
			m_dwCategoryType = 0;
		}
	};
	typedef std::vector<BuyGoods> BuyGoodsVec;

	typedef std::vector<ioHashStringVec> vEndEffect;

protected:	
	ShopBoughtInfoVec m_ShopBoughtInfoVec;
	BuyGoodsVec m_BuyGoodsVec;
	UINT m_CurrBoughtIndex;

protected:
	MultiSuperGashponUseType m_SuperGashponUseType;
	bool				m_bSound;

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pNum;
	ioUIRenderImage *m_pPlus;
	ioUIRenderImage *m_pEffect;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pResultIconBack;
	ioUIRenderImage *m_pGoodsNoneIcon;
	ioUIRenderImage *m_pGoodsBackIcon;
	ioUIRenderImage *m_pCostumeMark;
	ioUIRenderImage *m_pCostumeMarkSmall;
	ioUIRenderImage *m_pBackYellow;
	ioUIRenderImage *m_pBackViolet;

	ioUIRenderFrame *m_pStateOrangeFrm;
	ioUIRenderFrame *m_pStateGreenFrm;

	ioHashString     m_sTitle1;
	ioHashString     m_sTitle2;

	ioHashString     m_sRollingSound;
	ioHashString     m_sResultSound;

	ioHashStringVec  m_vDescOneLineVec;
	ioHashStringVec  m_vDescTwoLineVec;

	ioHashStringVec  m_vIconList;
	int              m_iCurIconArray;

	float			 m_fIconScale;
	DWORD            m_dwIconScaleTime;
	DWORD            m_dwRollingTime;
	DWORD            m_dwRollingTotalTime;

	DWORD            m_dwRollingCheckTime;
	DWORD            m_dwRollingEndTime;
	DWORD            m_dwScaleStartTime;
	DWORD            m_dwScaleEndTime;

	State            m_eState;

	ioHashStringVec  m_vRollingEffect;
	vEndEffect		 m_vEndEfffect;

	UINT			 m_iCurEffectType;
	UINT			 m_iCurEffectArray;
	DWORD			 m_dwRollingEffectTime;
	DWORD			 m_dwRollingEffectCheckTime;

	DWORD			m_dwCurVertPos;

	BoolVec			m_vCostume;
	bool			m_bIsCostume;

	int m_nGradeType;
	float m_fGradeScale;
	IntVec m_vGradeList;

	bool			m_bIsAllItemShow;

public:
	MultiSuperGashaponWnd::ShopBoughtInfo& GetBoughtInfo( UINT Index );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void ShowMultiSuperGashaponWnd( short iPresentType, int iPresentValue1, int iPresentValue2, MultiSuperGashponUseType UseType = MULTISUPERGASHPON_BUY );
	void ShowAddBuy();

	void AddBuyGoodsList( const ShopBoughtInfo& rkInfo, const ioHashString& szTitle, const ioHashString& szSubTitle );
	void AddResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iTradeType, DWORD dwCategoryType );

	void InitResultMultiSuperGashaponWnd();
	void ShowResultMultiSuperGashaponWnd( DWORD dwEtcItemCode, DWORD dwCategory, DWORD dwPackage, int iProductType );

	void DestoryInfo();	

	void UpdateAllItemInfo();

	bool IsAllItemShow();

public:
	bool IsRolling();

protected:
	void SetVertScroll();
	void ResetVertScroll();

protected:
	void ParseExtraEffectInfo( ioXMLElement &xElement );
	void ParseExtraEndEffectInfo( ioXMLElement &xElement );

protected:	
	virtual void OnProcess( float fTimePerSec );

	void ProcessRollingIcon( DWORD dwCurTime );
	void ProcessRollingEffect( float fTimePerSec );
	void ChangeRollingEffect( ioHashStringVec& vEffect, bool bLoop = false );
	void ProcessGoodsInfo(DWORD dwCurTime );

	void UpdateGoodsMainInfo( DWORD dwCurTime, const ShopBoughtInfo& rkInfo );
	void CheckRollingEnd( DWORD dwCurTime );

	void ProcessUpScale( DWORD dwCurTime );
	void ProcessDownScale( DWORD dwCurTime );	
	void ProcessScaleEndCheck( DWORD dwCurTime );

protected:
	virtual void OnRender();

	void OnIconRender( int iXPos, int iYPos );
	void OnExtraItemRender( int iXPos, int iYPos );
	void OnReinforceRender( int iXPos, int iYPos, int iReinforce );
	void OnEffectRender( int iXPos, int iYPos );
	void OnTtitleRender( int iXPos, int iYPos );
	void OnBuyGoodsRender( int iXPos, int iYPos );
	void OnMentBuyRender( int iXPos, int iYPos );
	void OnMentPresentRender( int iXPos, int iYPos );
	void OnCostumeRender( int iXPos, int iYPos );

public:
	MultiSuperGashaponWnd(void);
	virtual ~MultiSuperGashaponWnd(void);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class MultiSuperGashaponToastPopupWnd :	public ioWnd
{
protected:
	enum
	{
		MAX_ITEM_LIST	= 8
	};

	enum
	{
		START_Y_POS				= 235,

		FRAME_DEFAULT_WIDTH		= 211,
		FRAME_DEFAULT_HEIGHT	= 529,

		TITLE_START_OFFSET		= 12,
		TITLE_HEIGHT			= 28,
		TITLE_SLOT_Y_GAP		= 38,

		SLOT_START_X_POS		= 16,
		SLOT_START_Y_POS		= 38,
		SLOT_HEIGHT				= 60
	};

public:
	enum ArrowDirection
	{
		ARROW_LEFT,
		ARROW_RIGHT
	};

protected:
	DWORD m_dwOpenTime;
	DWORD m_dwShowTime;

	MultiSuperGashaponWnd::BuyGoodsVec m_BuyGoodsVec;

	int m_iIconYPosStart;

	int m_iOriginPosX;
	int m_iOriginPosY;

	ArrowDirection m_ArrowDirection;

protected:
	ioUIRenderImage *m_pBackGray;
	ioUIRenderImage *m_pBackYellow;
	ioUIRenderImage *m_pBackViolet;
	ioUIRenderImage *m_pGoodsNoneIcon;
	ioUIRenderImage *m_pGoodsBackIcon;
	ioUIRenderImage *m_pCostumeMark;
	ioUIRenderImage *m_pPopupArrow;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	//virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void OnProcess( float fTimePerSec );

	virtual void iwm_show();

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	void InitGoodsInfo( ArrowDirection arrow );
	void AddGoodsInfo( MultiSuperGashaponWnd::BuyGoods goods );

public:
	MultiSuperGashaponToastPopupWnd(void);
	virtual ~MultiSuperGashaponToastPopupWnd(void);
};