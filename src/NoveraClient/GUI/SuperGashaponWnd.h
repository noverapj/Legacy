#pragma once
#include "../ioSuperGashaponMgr.h"

class SuperGashaponWnd : public ioWnd
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
	struct ShopBoughtInfo
	{
		short	m_iPresentType;
		int     m_iPresentValue1;
		int     m_iPresentValue2;
		int		m_iTradeType;

		ShopBoughtInfo()
		{
			m_iPresentType		= 0;
			m_iPresentValue1	= 0;
			m_iPresentValue2	= 0;
			m_iTradeType		= 0;
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

		BuyGoods()
		{
			m_pGoodsIcon = NULL;
			m_pSubIcon = NULL;
			m_bIsCostume = false;
			m_nGradeType = 0;
		}
	};
	typedef std::vector<BuyGoods> BuyGoodsVec;

protected:	
	ShopBoughtInfoVec m_ShopBoughtInfoVec;
	BuyGoodsVec m_BuyGoodsVec;
	UINT m_CurrBoughtIndex;

protected:
	SuperGashponUseType m_SuperGashponUseType;
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
	ioHashStringVec  m_vEndEfffect;

	UINT			 m_iCurEffectArray;
	DWORD			 m_dwRollingEffectTime;
	DWORD			 m_dwRollingEffectCheckTime;

	DWORD			m_dwCurVertPos;

	BoolVec			m_vCostume;
	bool			m_bIsCostume;

	int m_nGradeType;
	float m_fGradeScale;
	IntVec m_vGradeList;

public:
	SuperGashaponWnd::ShopBoughtInfo& GetBoughtInfo( UINT Index );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void ShowSuperGashaponWnd( short iPresentType, int iPresentValue1, int iPresentValue2, SuperGashponUseType UseType = SUPERGASHPON_BUY );
	void ShowAddBuy();

	void AddBuyGoodsList( const ShopBoughtInfo& rkInfo, const ioHashString& szTitle, const ioHashString& szSubTitle );
	void AddResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iTradeType );

	void ShowResultSuperGashaponWnd( DWORD dwEtcItemCode, DWORD dwPresentIndex, int iProductType );

	void DestoryInfo();	

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
	SuperGashaponWnd();
	virtual ~SuperGashaponWnd();
};