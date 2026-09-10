#pragma once

#include "../ioShopManager.h"

class NewShopStartWnd : public ioWnd
{
public:
	enum
	{
		ID_AD_TEXT_WND			= 1,
		ID_AD_IMAGE_WND			= 2,

		ID_CATEGORY_PREV_BTN	= 3,
		ID_CATEGORY_NEXT_BTN	= 4,

		ID_PAGE_PREV_BTN		= 5,
		ID_PAGE_NEXT_BTN		= 6,

		ID_GOODS_01_BTN			= 10,
		ID_GOODS_02_BTN			= 11,
		ID_GOODS_03_BTN			= 12,
		ID_GOODS_04_BTN			= 13,
		ID_GOODS_05_BTN			= 14,
		ID_GOODS_06_BTN			= 15,
		ID_GOODS_07_BTN			= 16,
		ID_GOODS_BTN_MAX		= 17,
	};

	enum
	{
		CATEGORY_TITLE_OFFSET	= 23,
		CATEGORY_TITLE_X		= 375,
		CATEGORY_TITLE_Y		= 274,
		CATEGORY_TITLE_TEXT_Y	= 274,

		CATEGORY_PAGE_X			= 703,
		CATEGORY_PAGE_Y			= 274,
	};
protected:
	ioUIRenderFrame*	m_pProductFrm;

protected:
	int					m_CurrCategoryIndex;
	ioHashString		m_CurrCategoryName;
	ADProductList		m_CurrADProductList;

protected:
	int				m_iCurrPage;

protected:
	void ParseProduct();
	void ParseButton();
	void ParseCategoryButton();
	void ParsePageButton();

protected:
	int GetPageMax();

protected:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	NewShopStartWnd();
	virtual ~NewShopStartWnd();
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ioProductLinkButton : public ioButton
{
public:
	enum
	{
		ID_ICON_BTN = 1,
	};

protected:
	ioHashString		m_szName;
	ioHashString		m_szGold;
	ioHashString		m_szPeso;
	ADProductLink		m_CurrADProductLink;

protected:
	NewShopItemInfo		m_ShopItemInfo;

public:
	void Clear();

public:	
	void SetBlank();
	void SetProductInfo( ADProduct& Product );

protected:
	bool SetSoldierInfo( ADProduct& Product );
	bool SetExtraItemInfo( ADProduct& Product );
	bool SetDecoInfo( ADProduct& Product );
	bool SetEtcInfo( ADProduct& Product );
	bool SetCostumeInfo( ADProduct& Product );

protected:
	void ParseButton( const ioHashString& szIcon, const ioHashString& szSubIcon, int iMarkType, int nReinForce, int iLimitDate, bool bCostume, int nGradeType );

public:
	void MoveDirectLink();

public:
	const ADProductLink& GetADPrductLink(){ return m_CurrADProductLink; }
	const NewShopItemInfo& GetShopItemInfo(){ return m_ShopItemInfo; }

protected:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnProcess( float fTimePerSec );

	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	virtual void OnNormalRender( int iXPos, int iYPos );

public:
	ioProductLinkButton();
	virtual ~ioProductLinkButton();
};
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ioProductIconButton : public ioButton
{
public:
	enum
	{
		X_MARK_OFFSET   = -13,
		Y_MARK_OFFSET   = -5,
		X_TEXT_OFFSET   = 8,
		Y_TEXT_OFFSET   = -3,

		REINFORCE_Y		= 9,

		NUM_SIZE		= 20,
		PLUS_SIZE		= 14,

		NUM_GAP			= 4,
	};
protected:
	ioUIRenderImage*	m_pIcon;
	ioUIRenderImage*	m_pSubIcon;
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pIconEmpty;	
	ioUIRenderImage*	m_pNewMark;
	ioUIRenderImage*	m_pHotMark;
	ioUIRenderImage*	m_pEventMark;
	ioUIRenderImage*	m_pLimitMark;
	ioUIRenderImage*	m_pLimitDayMark;
	ioUIRenderImage*	m_pSoldOutMark;
	ioUIRenderImage*	m_pGradeLimitClearMark;
	ioUIRenderImage*	m_pNumText;
	ioUIRenderImage*	m_pPlusText;
	ioUIRenderImage*	m_pCostumeMark;
	ioUIRenderImage*	m_pDDayMark;

	ioUIRenderFrame*	m_pStateBlueFrm;
	ioUIRenderFrame*	m_pStatePinkFrm;

protected:
	int					m_iMarkType;
	int					m_iReinForceNumber;
	int					m_iLimitDay;

	bool				m_bCostume;
	bool				m_bScreen;

	DWORD				m_dwScreenTime;
	DWORD				m_dwScreenGapTime;
	DWORD				m_dwScreenCheckTime;

	int m_nGradeType;
	
public:
	void SetProductIconInfo( const ioHashString& szIconName, const ioHashString& szSubIcon, int iMarkType, int iReinforceNumber, int iLimitDate, bool bCostume, int nGradeType );	

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

	virtual void OnProcess( float fTimePerSec );

	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	void OnIconRender( int iXPos, int iYPos, UIRenderType eIconBackType, UIRenderType eIconType );
	void OnMarkRender( int iXPos, int iYPos );
	void OnNumberRender( int iXPos, int iYPos );
	void OnCostumeRender( int iXPos, int iYPos );

public:
	ioProductIconButton();
	virtual ~ioProductIconButton();
};