#include "StdAfx.h"

#include "NewShopIconBtn.h"

NewShopIconBtn::NewShopIconBtn()
{
	m_pIcon					= NULL;
	m_pSubIcon = NULL;
	m_pIconBack				= NULL;
	m_pIconEmpty			= NULL;
	m_pNewMark				= NULL;
	m_pHotMark				= NULL;
	m_pEventMark			= NULL;
	m_pLimitMark			= NULL;
	m_pLimitDayMark			= NULL;
	m_pSoldOutMark			= NULL;
	m_pGradeLimitClearMark	= NULL;
	m_pNumText				= NULL;
	m_pPlusText				= NULL;
	m_pCostumeMark			= NULL;
	m_pGuildItemMark		= NULL;
	m_pHomeItemMark			= NULL;

	m_pStateBlueFrm			= NULL;
	m_pStatePinkFrm			= NULL;
	m_pStatePinkLimitFrm	= NULL;
	m_pDDayMark				= NULL;
	m_pBlackFriDay			= NULL;

	m_iMarkType				= 0; // SHOP_MARK_TYPE_NONE

	m_bScreen				= false;
	m_bDisableSelfMouseOver	= false;
	m_TopSubIconType		= TSIT_NONE;

	m_dwScreenTime			= 200;
	m_dwScreenGapTime		= 100;
	m_dwScreenCheckTime		= 0;

	m_iNumber				= 0;
	m_iLimitDay				= 0;
	m_nGradeType = 0;
}

NewShopIconBtn::~NewShopIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconEmpty );
	SAFEDELETE( m_pNewMark );
	SAFEDELETE( m_pHotMark );
	SAFEDELETE( m_pEventMark );
	SAFEDELETE( m_pLimitMark );
	SAFEDELETE( m_pLimitDayMark );
	SAFEDELETE( m_pSoldOutMark );
	SAFEDELETE( m_pGradeLimitClearMark );
	SAFEDELETE( m_pStateBlueFrm );
	SAFEDELETE( m_pStatePinkFrm );
	SAFEDELETE( m_pStatePinkLimitFrm );
	SAFEDELETE( m_pDDayMark );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pCostumeMark );
	SAFEDELETE( m_pGuildItemMark );
	SAFEDELETE( m_pHomeItemMark );
	SAFEDELETE( m_pBlackFriDay );
}

void NewShopIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else if( szType == "NewMark" )
	{
		SAFEDELETE( m_pNewMark );
		m_pNewMark = pImage;
	}
	else if( szType == "HotMark" )
	{
		SAFEDELETE( m_pHotMark );
		m_pHotMark = pImage;
	}
	else if( szType == "EventMark" )
	{
		SAFEDELETE( m_pEventMark );
		m_pEventMark = pImage;
	}
	else if( szType == "LimitMark" )
	{
		SAFEDELETE( m_pLimitMark );
		m_pLimitMark = pImage;
	}
	else if( szType == "LimitDayMark" )
	{
		SAFEDELETE( m_pLimitDayMark );
		m_pLimitDayMark = pImage;
	}
	else if( szType == "SoldOutMark" )
	{
		SAFEDELETE( m_pSoldOutMark );
		m_pSoldOutMark = pImage;
	}
	else if( szType == "GradeLimitClearMark" )
	{
		SAFEDELETE( m_pGradeLimitClearMark );
		m_pGradeLimitClearMark = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else if( szType == "CostumeMark" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else if( szType == "GuildMark" )
	{
		SAFEDELETE( m_pGuildItemMark );
		m_pGuildItemMark = pImage;
	}
	else if( szType == "HomeMark" )
	{
		SAFEDELETE( m_pHomeItemMark );
		m_pHomeItemMark = pImage;
	}
	else if( szType == "DDayMark" )
	{
		SAFEDELETE( m_pDDayMark );
		m_pDDayMark = pImage;
	}
	else if( szType == "BlackFridayMark" )
	{
		SAFEDELETE( m_pBlackFriDay );
		m_pBlackFriDay = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void NewShopIconBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateBlueFrm" )
		{
			SAFEDELETE( m_pStateBlueFrm );
			m_pStateBlueFrm = pFrame;
		}
		else if( szType == "StatePinkFrm" )
		{
			SAFEDELETE( m_pStatePinkFrm );
			m_pStatePinkFrm = pFrame;
		}
		else if( szType == "StatePinkLimitFrm" )
		{
			SAFEDELETE( m_pStatePinkLimitFrm );
			m_pStatePinkLimitFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void NewShopIconBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_dwScreenGapTime		= xElement.GetIntAttribute_e( "ScreenGapTime" );
	m_dwScreenTime			= xElement.GetIntAttribute_e( "ScreenDuration" );
	m_bDisableSelfMouseOver	= xElement.GetBoolAttribute_e( "DisableSelfOver" );
}

void NewShopIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderMarkInfo( iXPos, iYPos );
	RenderNumber( iXPos, iYPos );

	switch( m_TopSubIconType )
	{
	case TSIT_COSTUME:
		RenderCostume( iXPos, iYPos );
		break;
	case TSIT_GUILD:
		RenderGuildHQ( iXPos, iYPos );
		break;
	case TSIT_HOME:
		RenderHome( iXPos, iYPos );
		break;
	}
}

void NewShopIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderInfo( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderMarkInfo( iXPos, iYPos );
	RenderNumber( iXPos, iYPos );

	switch( m_TopSubIconType )
	{
	case TSIT_COSTUME:
		RenderCostume( iXPos, iYPos );
		break;
	case TSIT_GUILD:
		RenderGuildHQ( iXPos, iYPos );
		break;
	case TSIT_HOME:
		RenderHome( iXPos, iYPos );
		break;
	}
}

void NewShopIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderInfo( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderMarkInfo( iXPos, iYPos );
	RenderNumber( iXPos, iYPos );

	switch( m_TopSubIconType )
	{
	case TSIT_COSTUME:
		RenderCostume( iXPos, iYPos );
		break;
	case TSIT_GUILD:
		RenderGuildHQ( iXPos, iYPos );
		break;
	case TSIT_HOME:
		RenderHome( iXPos, iYPos );
		break;
	}
}

void NewShopIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );
	if( m_pIconEmpty )
		m_pIconEmpty->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_MULTIPLY );
}

void NewShopIconBtn::RenderInfo( int iXPos, int iYPos, UIRenderType eType )
{
	if( m_pIconBack )
		m_pIconBack->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_MULTIPLY );

	if ( eType != UI_RENDER_SCREEN )
		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + GetWidth()/2, iYPos + GetHeight()/2, FLOAT1, eType );

	if( m_pIcon )
		m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eType );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eType );
}

void NewShopIconBtn::RenderCostume( int iXPos, int iYPos )
{
	// UI内胶片 包访 (内胶片 付农)
	if( m_pCostumeMark )
		m_pCostumeMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL );
}

void NewShopIconBtn::RenderGuildHQ( int iXPos, int iYPos )
{
	if( m_pGuildItemMark )
		m_pGuildItemMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL );
}
void NewShopIconBtn::RenderHome( int iXPos, int iYPos )
{
	if( m_pHomeItemMark )
		m_pHomeItemMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL );
}

void NewShopIconBtn::RenderMarkInfo( int iXPos, int iYPos )
{
	enum 
	{
		X_MARK_OFFSET   = -8,
		Y_MARK_OFFSET   = -10,
		X_TEXT_OFFSET   = 13,
		Y_TEXT_OFFSET   = -8,
		X_TIME_OFFSET   = 14,
		Y_TIME_OFFSET   = -9,
	};

	if( m_iMarkType == SHOP_MARK_TYPE_NEW )
	{
		if( m_pNewMark )
			m_pNewMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );		
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_HIT )
	{
		if( m_pHotMark )
			m_pHotMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_SALE )
	{
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_EVENT )
	{
		if( m_pEventMark )
		{
			m_pEventMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );

			if( m_bScreen )
				m_pEventMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_BOUGHT )
	{
		if( m_pStateBlueFrm )
			m_pStateBlueFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + X_TEXT_OFFSET, iYPos + Y_TEXT_OFFSET, FONT_SIZE_11, STR(1) );
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_LIMIT_GOODS )
	{
		if( m_pLimitMark )
		{
			m_pLimitMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );

			if( m_bScreen )
				m_pLimitMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_LIMIT_DAYS )
	{
		if( m_pLimitDayMark )
		{
			m_pLimitDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );

			if( m_bScreen )
				m_pLimitDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_SOLD_OUT )
	{
		if( m_pSoldOutMark )
		{
			m_pSoldOutMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_EXTRAITEM_GROWTH_CATALYST )
	{
		int iCenterX = (iXPos + GetWidth()/2);
		int iCenterY = (iYPos + GetHeight()/2);
		if( m_pStatePinkFrm )
		{
			m_pStatePinkFrm->Render( iCenterX - 45, iCenterY - 41 );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iCenterX - 20, iCenterY - 40, FONT_SIZE_11, STR(2), m_iNumber );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_GRADE_LIMIT_CLEAR )
	{
		if( m_pGradeLimitClearMark )
		{
			m_pGradeLimitClearMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );

			if( m_bScreen )
				m_pGradeLimitClearMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_DATE )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );

		if( m_iLimitDay == 0 )
		{
			if( m_pDDayMark )
			{
				m_pDDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
				if( m_bScreen )
					m_pDDayMark->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
			}
		}
		else
		{
			if( m_pStatePinkLimitFrm )
			{
				m_pStatePinkLimitFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );
				if( m_bScreen )
					m_pStatePinkLimitFrm->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
			}

			g_FontMgr.PrintText( iXPos + X_TIME_OFFSET, iYPos + Y_TIME_OFFSET, FONT_SIZE_11, "D-%d", m_iLimitDay );
			g_FontMgr.PrintText( iXPos + X_TIME_OFFSET - 1, iYPos + Y_TIME_OFFSET, FONT_SIZE_11, "D-%d", m_iLimitDay );
		}
	}
	else if( m_iMarkType == SHOP_MARK_TYPE_BLACKFRIDAY )
	{
		if( m_pBlackFriDay )
		{
			m_pBlackFriDay->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_NORMAL );

			if( m_bScreen )
				m_pBlackFriDay->Render( iXPos + X_MARK_OFFSET, iYPos + Y_MARK_OFFSET, UI_RENDER_SCREEN );
		}
	}
}

void NewShopIconBtn::RenderNumber( int iXPos, int iYPos )
{
	if( m_iNumber <= 0 )
		return;

	if( !m_pNumText || !m_pPlusText )
		return;

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	int iNewXPos = iXPos + GetWidth()/2;
	int iNewYPos = iYPos + GetHeight()/2;
	int iCurPos = 0;
	int iDefaultPos = iNewXPos;

	int iCurYPos = iNewYPos + REINFORCE_Y;

	int iTotalSize = 0;
	if( m_iNumber < 10 )
		iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
	else if( m_iNumber < 100 )
		iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
	else
		iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

	iCurPos = iDefaultPos - (iTotalSize / 2);

	m_pPlusText->Render( iCurPos, iCurYPos );

	iCurPos += (PLUS_SIZE-NUM_GAP);

	m_pNumText->RenderNum( iCurPos, iCurYPos, m_iNumber, -NUM_GAP );
}

bool NewShopIconBtn::SetIcon( ioUIRenderImage *pIcon )
{
	if( !pIcon )
	{
		LOG.PrintTimeAndLog( 0, "%s pImg == NULL.", __FUNCTION__ );
		return false;
	}
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );	
	m_pIcon = pIcon;

	m_bScreen = true;
	m_dwScreenCheckTime = FRAMEGETTIME();
	m_TopSubIconType = TSIT_NONE;
	m_nGradeType = 0;

	return true;
}

void NewShopIconBtn::SetSubIcon( ioUIRenderImage *pIcon )
{
	SAFEDELETE( m_pSubIcon );
	m_pSubIcon = pIcon;
}

void NewShopIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !m_pParent )
		return;

	if( m_dwScreenCheckTime > 0 && m_dwScreenGapTime > 0 && m_dwScreenTime > 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_bScreen )
		{
			if( m_dwScreenCheckTime+m_dwScreenTime < dwCurTime )
			{
				m_bScreen = false;
				m_dwScreenCheckTime = dwCurTime;
			}
		}
		else
		{
			if( m_dwScreenCheckTime+m_dwScreenGapTime < dwCurTime )
			{
				m_bScreen = true;
				m_dwScreenCheckTime = dwCurTime;
			}
		}
	}

	if( m_bOver && !m_bClicked && !m_bDisableSelfMouseOver )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( m_pParent->GetID() );
	}	
}