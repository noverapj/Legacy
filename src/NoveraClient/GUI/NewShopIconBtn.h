#pragma once

class NewShopIconBtn : public ioButton
{
public:
	enum TopSubIconType
	{
		TSIT_NONE,
		TSIT_COSTUME,
		TSIT_GUILD,
		TSIT_HOME,
	};

protected:
	TopSubIconType		m_TopSubIconType;

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
	ioUIRenderImage*	m_pGuildItemMark;
	ioUIRenderImage*	m_pHomeItemMark;
	ioUIRenderImage*	m_pDDayMark;
	ioUIRenderImage*	m_pBlackFriDay;

	ioUIRenderFrame*	m_pStateBlueFrm;
	ioUIRenderFrame*	m_pStatePinkFrm;
	ioUIRenderFrame*	m_pStatePinkLimitFrm;

	int					m_iMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	int					m_iLimitDay;	

	DWORD				m_dwScreenCheckTime;
	DWORD				m_dwScreenGapTime;
	DWORD				m_dwScreenTime;

	bool				m_bScreen;
	bool				m_bDisableSelfMouseOver;
	
	int					m_iNumber;
	int m_nGradeType;

protected:
	void RenderInfo( int iXPos, int iYPos, UIRenderType eType);
	void RenderCostume( int iXPos, int iYPos );
	void RenderGuildHQ( int iXPos, int iYPos );
	void RenderHome( int iXPos, int iYPos );
	void RenderMarkInfo( int iXPos, int iYPos );
	void RenderNumber( int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon );
	void SetSubIcon( ioUIRenderImage *pIcon );

	void SetTopSubIconType( NewShopIconBtn::TopSubIconType eType ){ m_TopSubIconType = eType; }

	void SetMarkType( int iMarkType )	{ m_iMarkType = iMarkType; } 
	void SetNumber( int iNumber )		{ m_iNumber = iNumber; }	
	void SetLimitDay( int iDay )		{ m_iLimitDay = iDay; }
	void SetGradeType( int nGradeType ) { m_nGradeType = nGradeType; }

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	NewShopIconBtn();
	virtual ~NewShopIconBtn();
};