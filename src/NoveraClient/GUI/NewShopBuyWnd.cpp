#include "StdAfx.h"

#include "../ioSoldierAwakeManager.h"
#include "../ioPowerUpManager.h"
#include "../ioShopManager.h"
#include "../ioExtendSoundManager.h"
#include "../ioDateHelp.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../Housing/BlockDefine.h"

#include "ioExtraItemListWnd.h"
#include "ioDecoItemListWnd.h"
#include "NewShopBuyWnd.h"
#include "NewShopItemInfoWnd.h"
#include "BillingWantWnd.h"
#include "SelectExtraGashaponWnd.h"
#include "SelectGashaponWnd.h"
#include "GlobalWnd.h"
#include "UISoldierSelectWnd.h"
#include "NewShopBoughtWnd.h"
#include "SendPresentWnd.h"
#include "ShopItemMovieWnd.h"
#include "ioCostumeListWnd.h"
#include "NewShopItemPurchaseLeedWnd.h"
#include "ioHousingBlockInvenWnd.h"
//////////////////////////////////////////////////////////////////////////
NewShopPullDownWnd::NewShopPullDownWnd()
{
	m_iStartHGap  = 0;
	m_iEndHGap    = 0;
	m_iItemHSize  = 0;
	m_iItemMaxPage= 0;
	m_iCurPage    = 0;
	m_pOpenBtn    = NULL;
	m_pScrollWnd  = NULL;
	m_pDarkLine   = NULL;
	m_pLightLine  = NULL;
	m_pOverLine   = NULL;
	m_pStatePinkFrm = NULL;

	m_fFontSize   = FONT_SIZE_12;
}

NewShopPullDownWnd::~NewShopPullDownWnd()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine );
	SAFEDELETE( m_pStatePinkFrm );
	m_PullDownItem.clear();
}

void NewShopPullDownWnd::CreatePullDown( ioWnd *pOpenBtn, int iXPos, int iYPos )
{
	m_pOpenBtn      = pOpenBtn;
	SetWndPos( iXPos, iYPos );

	m_PullDownItem.clear();
}

void NewShopPullDownWnd::InsertItem( NewShopPullDownItem &rkItem )
{	
	m_PullDownItem.push_back( rkItem );	
}

void NewShopPullDownWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iStartHGap	= xElement.GetIntAttribute_e( "StartHGap" );
	m_iEndHGap		= xElement.GetIntAttribute_e( "EndHGap" );
	m_iWidthGap		= xElement.GetIntAttribute_e( "WidthGap" );
	m_iItemHSize	= xElement.GetIntAttribute_e( "ItemHSize" );
	m_iItemMaxPage	= xElement.GetIntAttribute_e( "ItemMaxPage" );
	m_fFontSize		= xElement.GetFloatAttribute_e( "FontSize" );
	m_fWidthCutSize	= xElement.GetFloatAttribute_e( "WidthCut" );
}

void NewShopPullDownWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void NewShopPullDownWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StatePinkFrm" )
	{
		SAFEDELETE( m_pStatePinkFrm );
		m_pStatePinkFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void NewShopPullDownWnd::iwm_show()
{
	if( m_PullDownItem.empty() )
	{
		HideWnd();
		return;
	}

	m_pScrollWnd = FindChildWnd( ID_VERT_SCROLL );

	// 스크롤 UI
	m_iCurPage = 0;
	int iItemSize = m_PullDownItem.size();
	if( iItemSize > m_iItemMaxPage )
	{
		ioScroll *pScroll = (ioScroll*)m_pScrollWnd;
		if( pScroll )
		{
			int iTotalList = (int)iItemSize - ( m_iItemMaxPage - 1 );
			pScroll->SetScrollRange( 0, iTotalList );
			pScroll->SetScrollPos( m_iCurPage );
			pScroll->ShowWnd();
		}
	}
	else if( m_pScrollWnd )
	{
		m_pScrollWnd->HideWnd();
	}

	// 윈도우 사이즈
	SetSize( GetWidth(), m_iStartHGap + m_iEndHGap + ( m_iItemHSize * min( m_iItemMaxPage, iItemSize ) ) );
	SetCheckParentOutSide( true );
}

void NewShopPullDownWnd::iwm_hide()
{	
	SetCheckParentOutSide( false );
}

void NewShopPullDownWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	m_iCurPage = curPos;
}

void NewShopPullDownWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		ioScroll *pScroll = (ioScroll*)m_pScrollWnd;
		if( !pScroll ) return;
		if( !pScroll->IsShow() ) return;

		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPage - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPage + 1 );
		}
	}
}

bool NewShopPullDownWnd::PrevClickCheck()
{
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( !pPreClick ) return false;
	if( pPreClick == m_pOpenBtn ) return false;
	if( pPreClick == this ) return false;
	if( pPreClick->GetParent() == m_pScrollWnd ) return false;
	if( pPreClick->GetParent() == this ) return false;

	return true;
}

void NewShopPullDownWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	m_iOverListIndex = -1;
	// 다른 곳에서 클릭이 일어났는지 확인
	if( PrevClickCheck() )
	{
		HideWnd();
		return;
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	int iMaxItem = m_PullDownItem.size();
	int iStart   = m_iCurPage;
	for(int i = 0;i < m_iItemMaxPage;i++,iStart++)
	{
		if( iStart >= iMaxItem ) continue;

		int iWidth = GetWidth();
		if( iMaxItem > m_iItemMaxPage )
		{	
			iWidth -= 33;
		}
		else
		{	
			iWidth -= 8;
		}

		RECT rcNameRect;
		rcNameRect.left = iXPos + 4;
		rcNameRect.top  = iYPos + m_iStartHGap + i * m_iItemHSize;
		rcNameRect.right  = rcNameRect.left + iWidth;
		rcNameRect.bottom = rcNameRect.top + m_iItemHSize;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_iOverListIndex = iStart;

			if( pMouse->IsLBtnUp() )
			{
				GetParent()->iwm_command( this, IOBN_BTNUP, iStart );
			}
			else
			{
				GetParent()->iwm_command( this, IOWN_OVERED, iStart );
			}
			break;
		}
	}
}

void NewShopPullDownWnd::OnRender()
{
	ioWnd::OnRender();	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	bool bDarkRender = false;
	int iMaxItem     = m_PullDownItem.size();
	int iStart       = m_iCurPage;
	bool bOver		 = false;
	bool bShowScroll = false;
	if( iMaxItem > m_iItemMaxPage )
		bShowScroll = true;

	for(int i = 0;i < m_iItemMaxPage;i++,iStart++)
	{
		if( iStart >= iMaxItem ) continue;

		bOver = false;
		NewShopPullDownItem &rkItem = m_PullDownItem[iStart];
		g_FontMgr.SetTextColor( rkItem.m_dwTextColor );
		g_FontMgr.SetAlignType( TAT_LEFT );
		
		bDarkRender = !bDarkRender;
		// 오버 
		if( m_iOverListIndex == iStart  )
		{
			if( bShowScroll )
				m_pOverLine->SetSize( GetWidth() - 33, m_iItemHSize );
			else
				m_pOverLine->SetSize( GetWidth() - 8, m_iItemHSize );
		
			m_pOverLine->Render( iXPos + 4, iYPos + m_iStartHGap + ( i * m_iItemHSize ) , UI_RENDER_MULTIPLY );
			g_FontMgr.SetTextColor( 255, 255, 255 );
			bOver = true;
		}
		else if( bDarkRender )
		{
			if( bShowScroll )
				m_pDarkLine->SetSize( GetWidth() - 33, m_iItemHSize );
			else
				m_pDarkLine->SetSize( GetWidth() - 8, m_iItemHSize );

			m_pDarkLine->Render( iXPos + 4, iYPos + m_iStartHGap + ( i * m_iItemHSize ) , UI_RENDER_MULTIPLY );
		}
		else
		{
			if( bShowScroll )
				m_pLightLine->SetSize( GetWidth() - 33, m_iItemHSize );
			else
				m_pLightLine->SetSize( GetWidth() - 8, m_iItemHSize );

			m_pLightLine->Render( iXPos + 4, iYPos + m_iStartHGap + ( i * m_iItemHSize ) , UI_RENDER_MULTIPLY );
		}

		// 아이템 이름
		int iNameWidth = 0;
		if( bShowScroll )
		{
			if( m_fWidthCutSize == 0 )
				iNameWidth = min( GetWidth() - 46, g_FontMgr.GetTextWidth( rkItem.m_szName.c_str(), TS_OUTLINE_FULL_2X, m_fFontSize ) + 1 );
			else
				iNameWidth = min( m_fWidthCutSize, g_FontMgr.GetTextWidth( rkItem.m_szName.c_str(), TS_OUTLINE_FULL_2X, m_fFontSize ) + 1 );
		}
		else
		{
			if( m_fWidthCutSize == 0 )
				iNameWidth = min( GetWidth() - 34, g_FontMgr.GetTextWidth( rkItem.m_szName.c_str(), TS_OUTLINE_FULL_2X, m_fFontSize ) + 1 );
			else
				iNameWidth = min( m_fWidthCutSize, g_FontMgr.GetTextWidth( rkItem.m_szName.c_str(), TS_OUTLINE_FULL_2X, m_fFontSize ) + 1 );
		}
		
		if ( rkItem.m_bCount )
		{			
			if ( bShowScroll )
			{
				if ( rkItem.m_bPetLevel )
				{
					g_FontMgr.PrintText( iXPos + 13 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, "Lv" );

					if ( !bOver )
						g_FontMgr.SetTextColor( rkItem.m_dwPetLevelTextColor );
					g_FontMgr.PrintText( iXPos + 28 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, rkItem.m_szPetLevel.c_str() );

					if ( !bOver )
						g_FontMgr.SetTextColor( rkItem.m_dwTextColor );
					g_FontMgr.PrintTextWidthCut( iXPos + 58 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, 100, rkItem.m_szName.c_str() );
				}
				else
					g_FontMgr.PrintTextWidthCut( iXPos + 13 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, 100, rkItem.m_szName.c_str() );

				if ( !bOver )
					g_FontMgr.SetTextColor( rkItem.m_dwCntTextColor );
				g_FontMgr.SetAlignType( TAT_RIGHT );
				g_FontMgr.PrintText( iXPos + 164 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, rkItem.m_szCnt.c_str() );
			}
			else
			{
				if ( rkItem.m_bPetLevel )
				{
					g_FontMgr.PrintText( iXPos + 13 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, "Lv" );

					if ( !bOver )
						g_FontMgr.SetTextColor( rkItem.m_dwPetLevelTextColor );
					g_FontMgr.PrintText( iXPos + 28 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, rkItem.m_szPetLevel.c_str() );

					if ( !bOver )
						g_FontMgr.SetTextColor( rkItem.m_dwTextColor );
					g_FontMgr.PrintTextWidthCut( iXPos + 58 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, 120, rkItem.m_szName.c_str() );
				}
				else
					g_FontMgr.PrintTextWidthCut( iXPos + 13 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, 120, rkItem.m_szName.c_str() );

				if ( !bOver )
					g_FontMgr.SetTextColor( rkItem.m_dwCntTextColor );
				g_FontMgr.SetAlignType( TAT_RIGHT );
				g_FontMgr.PrintText( iXPos + 191 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, m_fFontSize, rkItem.m_szCnt.c_str() );
			}
		}
		else if ( rkItem.m_iGrowthCatalystLevel )
		{
			// 성장 
			g_FontMgr.PrintTextWidthCut( iXPos + 58 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 2, m_fFontSize, 133.0f, rkItem.m_szName.c_str() );
			if( m_pStatePinkFrm )
			{
				m_pStatePinkFrm->Render( iXPos + 5, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 2 );

				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 30, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 3, FONT_SIZE_11, STR(1),rkItem.m_iGrowthCatalystLevel );
			}
		}
		else 
			g_FontMgr.PrintTextWidthCut( iXPos + 17 + m_iWidthGap, iYPos + m_iStartHGap + ( i * m_iItemHSize ) + 2, m_fFontSize, iNameWidth, rkItem.m_szName.c_str() );
	}
}

bool NewShopPullDownWnd::GetOverItem( OUT NewShopPullDownItem &rItem )
{
	if( COMPARE( m_iOverListIndex, 0, (int)m_PullDownItem.size() ) )
	{
		rItem = m_PullDownItem[m_iOverListIndex];
		return true;
	}

	return false;
}
//-------------------------------------------------------------------------
NewShopHelpControlBtn::NewShopHelpControlBtn()
{
	m_pBtn             = NULL;
}

NewShopHelpControlBtn::~NewShopHelpControlBtn()
{
	SAFEDELETE( m_pBtn );
}

void NewShopHelpControlBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( strcmp(szType.c_str(), "Btn") == 0 )
	{
		SAFEDELETE( m_pBtn );
		m_pBtn = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void NewShopHelpControlBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
}

void NewShopHelpControlBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderInfo( iXPos, iYPos , UI_RENDER_SCREEN );
}

void NewShopHelpControlBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	RenderInfo( iXPos, iYPos+2 , UI_RENDER_NORMAL );
	RenderInfo( iXPos, iYPos+2 , UI_RENDER_SCREEN );
}

void NewShopHelpControlBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_GRAY );
}

void NewShopHelpControlBtn::RenderInfo( int iXPos, int iYPos, UIRenderType eType )
{
	if( m_pBtn )
		m_pBtn->Render( iXPos, iYPos, eType );
}

void NewShopHelpControlBtn::SetBtnAlphaRate( int iAlphaRate )
{
	if( m_pBtn )
		m_pBtn->SetAlpha( (BYTE) iAlphaRate );
}
//-------------------------------------------------------------------------
NewShopHelpControlWnd::NewShopHelpControlWnd()
{
	InitAniState();
}

NewShopHelpControlWnd::~NewShopHelpControlWnd()
{
}

void NewShopHelpControlWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_LEFT_ROTATE_BTN:
	case ID_RIGHT_ROTATE_BTN:
	case ID_DEFAULT_POS_BTN:
		if( cmd == IOBN_BTNUP || cmd == IOBN_BTNDOWN )
		{
			if( ioWnd::GetParent() )
				ioWnd::GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void NewShopHelpControlWnd::SetDefaultPosTitle( const char* szTitle )
{
	ioWnd *pWnd = FindChildWnd( ID_DEFAULT_POS_BTN );
	if( pWnd )
		pWnd->SetTitleText( szTitle );
}

void NewShopHelpControlWnd::InitAniState()
{
	enum { DELAY_ALPHA = 500, };

	FSMState *pState = new FSMState(AT_NONE);
	if( pState )
		pState->Add( AT_ALPHA_IN,  AT_ALPHA_IN );
	m_AniState.Add( pState );

	pState = new FSMState(AT_ALPHA_IN);
	if( pState )
	{
		pState->Add( AT_ALPHA_IN, AT_NORMAL, DELAY_ALPHA );
		pState->Add( AT_ALPHA_OUT, AT_ALPHA_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_NORMAL);
	if( pState )
	{
		pState->Add( AT_ALPHA_OUT, AT_ALPHA_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_ALPHA_OUT);
	if( pState )
	{
		pState->Add( AT_ALPHA_OUT, AT_HIDE, DELAY_ALPHA );
	}
	m_AniState.Add( pState );	

	m_AniState.SetCurState( AT_NONE );
}

void NewShopHelpControlWnd::iwm_show()
{
	m_AniState.SetCurState( AT_ALPHA_IN );
	HideChildWnd( ID_DEFAULT_POS_BTN );
}

void NewShopHelpControlWnd::iwm_hide()
{
	m_AniState.SetCurState( AT_NONE );
}

void NewShopHelpControlWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	m_AniState.UpdateTime();

	if( m_AniState.GetCurState() == AT_ALPHA_IN )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * fRate;

		SetChildAlpha( iAlpha );
	}
	else if( m_AniState.GetCurState() == AT_ALPHA_OUT )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * ( FLOAT1 - fRate );

		SetChildAlpha( iAlpha );
	}
	else if( m_AniState.GetCurState() == AT_HIDE )
	{
		HideWnd();
	}
	else
	{
		SetChildAlpha( 255 );
	}
}

void NewShopHelpControlWnd::SetChildAlpha( int iAlphaRate )
{
	for (int i = 0; i < MAX_BTN ; i++)
	{
		NewShopHelpControlBtn *pBtn = dynamic_cast<NewShopHelpControlBtn*> ( FindChildWnd( ID_LEFT_ROTATE_BTN + i ) );
		if( !pBtn )
			continue;
		pBtn->SetBtnAlphaRate( iAlphaRate );
	}
}

void NewShopHelpControlWnd::HideWndAnimation()
{
	if( !IsShow() )
		return;
	m_AniState.ChangeState( AT_ALPHA_OUT );
}
//-------------------------------------------------------------------------
NewShopHelpWnd::NewShopHelpWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar	  = NULL;
	
	m_p3DBack = NULL;

	m_fRotateYaw  = 0.0f;
	m_fPreYaw     = 0.0f;
	m_iClassType  = 0;
	m_fXMLRotateYaw = 0.0f;
	m_fZPosition = FLOAT500;

	m_bLeftRotateOn  = false;
	m_bRightRotateOn = false;
	m_bReleaseHelm	 = false;
	m_bReleaseAllItem= false;
	m_bShopMotion    = false;
	m_bUserInfoToolTip = false;
}

NewShopHelpWnd::~NewShopHelpWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_p3DBack );
}

void NewShopHelpWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "shop_cam" );
	
	m_fZPosition = FLOAT500;
}

void NewShopHelpWnd::ShowNoneControlWnd()
{
	ShowWnd();

	SetChildInActive( ID_CHAR_CONTROL_WND );
	HideChildWnd( ID_CHAR_CONTROL_WND );
}

void NewShopHelpWnd::CreateCharacter( const CHARACTER &rkInfo, bool bEffect )
{
	
	CHARACTER& rkNewInfo = const_cast<CHARACTER&>( rkInfo );

	//INI상에서 여성 치장을 보여주고자 할때
	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByClass( rkInfo.m_class_type );
	if( pInfo )
	{
		if( g_MyInfo.GetCharIndexByClassType( rkInfo.m_class_type ) == 0 && pInfo->GetCharSexType() == ioSetItemInfo::CST_FEMALE )
			rkNewInfo.m_sex = ioSetItemInfo::CST_FEMALE;
	}

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( rkNewInfo, "ShopChar" );
	if( !pChar )
		return;

	if( m_aMyChar )
	{
		m_fPreYaw = m_aMyChar->GetYaw();
	}

	D3DXVECTOR3 vCharPos( 0.0f, 0.0f, m_fZPosition );
	pChar->SetPosition( vCharPos );
	pChar->SetYaw( m_fPreYaw );

	m_pUI3DRender->SetLookAt( vCharPos );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	if( m_aMyChar && bEffect )
		m_aMyChar->AttachEffect( "shop_delay_eff.txt", NULL );

	if( !m_szAwakeEffect.IsEmpty() )
		pChar->EndEffect( m_szAwakeEffect, true );

	m_szAwakeEffect = g_AwakeMgr.GetAwakeEffect( static_cast<AwakeType>( rkNewInfo.m_iAwakeType ) ).c_str();
	pChar->AttachEffect( m_szAwakeEffect, NULL );

	if( m_iClassType != rkNewInfo.m_class_type )
	{
		m_iClassType = rkNewInfo.m_class_type;
	}
}

void NewShopHelpWnd::SetCharAnimation( bool bShopMotion )
{
	if( !m_aMyChar )	return;

	if( bShopMotion )
	{
		if( m_aMyChar->SetWeaponShopMotion() )
			return;
	}

	ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
	if( !szUIAnimation.IsEmpty() )
	{
		m_aMyChar->SetLoopAni( szUIAnimation );
	}
}

void NewShopHelpWnd::SetCharAnimation( DWORD dwEtcItem, bool bMale )
{
	if( !m_aMyChar ) return;
	if( !COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) &&
		!COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) ) return;

	ioEtcItemMotion *pEtcItemMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( dwEtcItem ) );
	if( pEtcItemMotion )
	{
		if( bMale )
		{
			m_aMyChar->SetMotionItemAni( pEtcItemMotion->GetHumanManMotion() );
		}
		else
		{
			m_aMyChar->SetMotionItemAni( pEtcItemMotion->GetHumanWomanMotion() );
		}					
	}
}

void NewShopHelpWnd::SetCharLoopAnimation( const ioHashString &szAniName, float fFadeIn /*= 0.0f*/  )
{
	if( !m_aMyChar )	return;
	if( szAniName.IsEmpty() ) return;

	m_aMyChar->SetLoopAni( szAniName, fFadeIn );
}

void NewShopHelpWnd::SetCharActionAnimation( const ioHashString &szAniName, float fFadeIn /*= FLOAT100*/, float fFadeOut /*= FLOAT100 */ )
{
	if( !m_aMyChar )	return;
	if( szAniName.IsEmpty() ) return;

	m_aMyChar->SetActionAni( szAniName, fFadeIn, fFadeOut );
}

void NewShopHelpWnd::SetCharMotionItemAnimation( const ioHashString &szAniName )
{
	if( !m_aMyChar )	return;
	if( szAniName.IsEmpty() ) return;

	m_aMyChar->SetMotionItemAni( szAniName );
}

void NewShopHelpWnd::SetChangeSkeleton( DWORD dwEtcItem, bool bMale )
{
	if( !m_aMyChar ) return;
	if( !COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_SKELETON_BIG, ioEtcItem::EIT_ETC_SKELETON_SMALL + 1 ) ) return;

	ioHashString kEtcItemSkeleton;
	if( dwEtcItem > 0 )
	{
		ioEtcItemSkeleton *pEtcItemSkeleton = dynamic_cast< ioEtcItemSkeleton * >( g_EtcItemMgr.FindEtcItem( dwEtcItem ) );
		if( pEtcItemSkeleton )
		{
			if( bMale )
				kEtcItemSkeleton = pEtcItemSkeleton->GetHumanManSkl();
			else
				kEtcItemSkeleton = pEtcItemSkeleton->GetHumanWomanSkl();
		}
	}

	if( !kEtcItemSkeleton.IsEmpty() )
		m_aMyChar->ChangeSkeleton( kEtcItemSkeleton );
}

void NewShopHelpWnd::EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumCode )
{
	if( m_aMyChar )
	{
		m_aMyChar->EquipItem( dwItemCode, dwItemMaleCustom, dwItemFemaleCustom, nCostumCode );
	}
}

void NewShopHelpWnd::ReleaseItem( int iSlot )
{
	if( m_aMyChar )
	{
		m_aMyChar->ReleaseItem( iSlot );
	}
}

void NewShopHelpWnd::RotateY( float fYaw )
{
	m_fRotateYaw = fYaw;
}

void NewShopHelpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "3D_Back" )
	{
		SAFEDELETE(m_p3DBack);
		m_p3DBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void NewShopHelpWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	if( m_aMyChar )
	{
		m_aMyChar->RotateY( m_fRotateYaw );
		m_aMyChar->Update( fTimePerSec );
	}
	
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	RECT rcNameRect;
	rcNameRect.left = GetDerivedPosX();
	rcNameRect.top  = GetDerivedPosY();
	rcNameRect.right  = rcNameRect.left + GetWidth();
	rcNameRect.bottom = rcNameRect.top + GetHeight();

	if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
	{
		ShowChildWnd( ID_CHAR_CONTROL_WND );

		if( m_bUserInfoToolTip )
		{
			if( g_GUIMgr.GetPreOverWnd() == this || g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CHAR_CONTROL_WND ) )
				pMouse->SetToolTipID( g_MyInfo.GetPublicID(), m_CharInfo.m_class_type );
		}
	}
	else
	{
		NewShopHelpControlWnd *pControlWnd = dynamic_cast<NewShopHelpControlWnd*> (FindChildWnd( ID_CHAR_CONTROL_WND ) );
		if( pControlWnd )
			pControlWnd->HideWndAnimation();
	}
}

void NewShopHelpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_p3DBack )
	{
		int iNewXPos = iXPos - GetXPos() + (m_p3DBack->GetWidth()/2);
		int iNewYPos = iYPos - GetYPos() + (m_p3DBack->GetHeight()/2);

		m_p3DBack->SetScale( 2.0f );
		m_p3DBack->Render( iNewXPos, iNewYPos );
	}

	D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( iXPos - 320, iYPos - 60 );
	m_pUI3DRender->Render( &matUI, true );
}

void NewShopHelpWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CHAR_CONTROL_WND:
		if( cmd == IOBN_BTNDOWN)
		{
			if( param == NewShopHelpControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( m_fXMLRotateYaw );
			}
			else if( param == NewShopHelpControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( -m_fXMLRotateYaw );
			}
		}
		else if( cmd == IOBN_BTNUP )
		{
			if( param == NewShopHelpControlWnd::ID_DEFAULT_POS_BTN )
			{
				if( m_aMyChar )
					m_aMyChar->SetYaw( FLOAT10 );
				if( ioWnd::m_pParent )
					ioWnd::m_pParent->iwm_command( this, cmd, param );
				InitRotate();
			}
			else if( param == NewShopHelpControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
			else if( param == NewShopHelpControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
		}
		break;
	}
}

void NewShopHelpWnd::SetZPosition( float ZPos )
{
	m_fZPosition = ZPos;
}

void NewShopHelpWnd::SetChar( const CHARACTER &rkInfo, bool bEffect )
{
	if( rkInfo.m_class_type <= 0 ) return;

	int iPreClassType = m_CharInfo.m_class_type;
	int iPreSexType   = m_CharInfo.m_sex;

	m_CharInfo = rkInfo;
	// Set char
	CreateCharacter( m_CharInfo, bEffect );    

	if( iPreClassType != m_CharInfo.m_class_type || iPreSexType != m_CharInfo.m_sex )
		InitRotate();
}

void NewShopHelpWnd::SetEquip( bool bReleaseHelm, bool bReleaseAll, bool bShopMotion )
{
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		int iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
		EquipItem( iItemCode, 0, 0 );
	}

	if( g_GUIMgr.IsShow( MY_INVENTORY_WND ) || g_GUIMgr.IsShow( NEW_SHOP_WND ) || g_GUIMgr.IsShow( GRADE_ADJUSTMENT_WND ) )
	{
		int iArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
		for( int i = 0; i < MAX_INVENTORY; ++i )
		{
			int nCostumeCode = m_CharInfo.m_EquipCostume[i].m_CostumeCode;
			DWORD dwCostumeMaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Male_Custom;
			DWORD dwCostumeFemaleCustom = m_CharInfo.m_EquipCostume[i].m_Costume_Female_Custom;

			if( g_GUIMgr.IsShow( NEW_SHOP_WND ) && g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_RENTAL ) )
			{
				for( int i = 0; i < MAX_INVENTORY; i++ )
				{
					int iItemCode = 0;					
					if( m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
					{
						iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + ConvertPowerUpCharToItem( m_CharInfo.m_eCharPowerUpType ) + m_CharInfo.m_class_type;
						if( !g_PowerUpManager.IsPowerUpGradeItem( m_CharInfo.m_class_type, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
						{
							iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
						}
					}
					else
					{
						iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
					}

					if ( nCostumeCode <= 0 )
					{
						dwCostumeMaleCustom = 0;
						dwCostumeFemaleCustom = 0;
					}
					EquipItem( iItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, nCostumeCode );
				}
			}
			else if( COMPARE( iArray, 0, g_MyInfo.GetCharCount() ) )
			{
				const ITEM_DATA &rkItemData = g_MyInfo.GetCharItem( iArray, i );
				const ioItem *pItem = g_ItemMaker.GetItemConst( rkItemData.m_item_code, __FUNCTION__ );
				if( pItem )
				{
					if ( nCostumeCode <= 0 )
					{
						dwCostumeMaleCustom = rkItemData.m_item_male_custom;
						dwCostumeFemaleCustom = rkItemData.m_item_female_custom;
					}
					EquipItem( pItem->GetItemCode(), dwCostumeMaleCustom, dwCostumeFemaleCustom, nCostumeCode );
				}
			}
			else
			{
				for( int i = 0; i < MAX_INVENTORY; i++ )
				{
					int iItemCode = 0;
					if( m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
					{
						iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + ConvertPowerUpCharToItem( m_CharInfo.m_eCharPowerUpType ) + m_CharInfo.m_class_type;
						if( !g_PowerUpManager.IsPowerUpGradeItem( m_CharInfo.m_class_type, iItemCode ) || !g_ItemMaker.IsItem( iItemCode ) )
						{
							iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
						}
					}
					else
					{
						iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
					}

					if ( nCostumeCode <= 0 )
					{
						dwCostumeMaleCustom = 0;
						dwCostumeFemaleCustom = 0;
					}
					EquipItem( iItemCode, dwCostumeMaleCustom, dwCostumeFemaleCustom, nCostumeCode );
				}
			}
		}
	}

	SetCharAnimation( bShopMotion );

	if( bReleaseAll )
	{
		ReleaseItem( ioCreateChar::CCS_HELMET );
		ReleaseItem( ioCreateChar::CCS_ARMOR );
		ReleaseItem( ioCreateChar::CCS_CLOAK );
		ReleaseItem( ioCreateChar::CCS_WEAPON );
	}
	else if( bReleaseHelm )
	{
		ReleaseItem( ioCreateChar::CCS_HELMET );
	}

	m_bReleaseHelm    = bReleaseHelm;
	m_bReleaseAllItem = bReleaseAll;
	m_bShopMotion     = bShopMotion;
}

void NewShopHelpWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fXMLRotateYaw = xElement.GetFloatAttribute_e( "RotateYaw" );
}

void NewShopHelpWnd::InitRotate()
{
	RotateY( 0.0f );
	m_bLeftRotateOn  = false;
	m_bRightRotateOn = false;
	if( m_aMyChar )
		m_aMyChar->SetYaw( FLOAT10 );
}

void NewShopHelpWnd::SetCharScale( float fScale )
{
	if( m_aMyChar )
		m_aMyChar->SetScale( fScale );
}

void NewShopHelpWnd::iwm_show()
{
	HideChildWnd( ID_CHAR_CONTROL_WND );
}

void NewShopHelpWnd::iwm_hide()
{
	HideChildWnd( ID_CHAR_CONTROL_WND );

	if( m_aMyChar )
		m_aMyChar->ClearAllAnimations( true );
	SAFEDELETE( m_aMyChar );
}

ioEffect* NewShopHelpWnd::CharAttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner /*= NULL */ )
{
	if( m_aMyChar )
		return m_aMyChar->AttachEffect( rkEffect, pEntityEventOwner );
	return NULL;
}

void NewShopHelpWnd::CharEndEffect( const ioHashString &rkEffect, bool bGrace )
{
	if( m_aMyChar )
		m_aMyChar->EndEffect( rkEffect, bGrace );
}
//-------------------------------------------------------------------------
NewShopNeedLevelWnd::NewShopNeedLevelWnd( void )
{
	m_pIcon = NULL;
	m_kNeedLevelInfo.Clear();
	m_dwSelectPriceBtnID = 0;
	m_dwLastPriceBtnID   = 0;
}

NewShopNeedLevelWnd::~NewShopNeedLevelWnd( void )
{
	m_kNeedLevelInfo.Clear();
	SAFEDELETE( m_pIcon );
}

bool NewShopNeedLevelWnd::SetInfo( const NeedLevelInfo &rPriceInfo )
{
	m_kNeedLevelInfo = rPriceInfo;
	if( m_kNeedLevelInfo.m_sIcon.IsEmpty() )
		return false;

	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_kNeedLevelInfo.m_sIcon );

	for (int i = 0; i < MAX_RBTN ; i++)
		HideChildWnd( ID_PRICE1_RBTN + i );

	int iSize = m_kNeedLevelInfo.m_vPriceVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		DWORD dwID = ID_PRICE1_RBTN + i ;
		ShowChildWnd( dwID );	
		m_dwLastPriceBtnID = dwID;
	}
	
	m_dwSelectPriceBtnID = m_dwLastPriceBtnID;
	CheckRadioButton( ID_PRICE1_RBTN, m_dwLastPriceBtnID, m_dwSelectPriceBtnID );
	return true;
}

void NewShopNeedLevelWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		ICON_X_OFFSET = 150,
		ICON_Y_OFFSET = 91,

		TITLE_X_OFFSET = 150,
		TITLE_Y_OFFSET = 144,

		DESC_X_OFFSET = 150,
		DESC_Y_OFFSET = 168,
		DESC_Y_GAP    = 18,

		BIG_DESC_X_OFFSET = 150,
		BIG_DESC_Y_OFFSET = 215,

		INFO_LEFT_X_OFFSET = 55,
		INFO_LEFT_Y_OFFSET = 242,

		INFO_RIGHT_X_OFFSET = 263,
		INFO_RIGHT_Y_OFFSET = 242,

		INFO_Y_GAP  = 21,

		PESOGOLD_X_OFFSET = 150,
		PESOGOLD_Y_OFFSET = 358,
	};

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

	// title
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + TITLE_X_OFFSET, iYPos + TITLE_Y_OFFSET, FONT_SIZE_17, m_kNeedLevelInfo.m_sTitle.c_str() );

	// desc
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL);
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter.AddTextPiece( FONT_SIZE_13, m_kNeedLevelInfo.m_sNeedLevel.c_str() );

	kPrinter.SetTextStyle( TS_NORMAL);
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.PrintFullText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.PrintText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET + DESC_Y_GAP, FONT_SIZE_13, STR(2) );

	// warning
	g_FontMgr.SetTextColor( 141, 87, 4 );
	g_FontMgr.PrintText( iXPos + BIG_DESC_X_OFFSET, iYPos + BIG_DESC_Y_OFFSET, FONT_SIZE_17, STR(3) );

	// price
	int iSize  = m_kNeedLevelInfo.m_vNameVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		ioHashString &rsName = m_kNeedLevelInfo.m_vNameVec[i];
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET + INFO_Y_GAP*i, FONT_SIZE_13, rsName.c_str() );
	}

	iSize = m_kNeedLevelInfo.m_vPriceVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		ioHashString &rsPrice = m_kNeedLevelInfo.m_vPriceVec[i];
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextColor( 141, 87, 4 );
		g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET + INFO_Y_GAP*i, FONT_SIZE_13, rsPrice.c_str() );	
	}
	
	// peso / gold
	char szPeso[MAX_PATH]="";
	char szCash[MAX_PATH]="";

	Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szPeso, sizeof( szPeso ) );
	Help::ConvertNumToStrComma( g_MyInfo.GetTotalCash(), szCash, sizeof( szCash ) );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.AddTextPiece( FONT_SIZE_13, szPeso );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_13, " / " );
	kPrinter.SetTextColor( 141, 87, 4 );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.AddTextPiece( FONT_SIZE_13, szCash );
	kPrinter.PrintFullText( iXPos + PESOGOLD_X_OFFSET, iYPos + PESOGOLD_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();
}

void NewShopNeedLevelWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_PRICE1_RBTN:
	case ID_PRICE2_RBTN:
		if( cmd == IOBN_BTNDOWN )
		{
			m_dwSelectPriceBtnID = dwID;
			CheckRadioButton( ID_PRICE1_RBTN, m_dwLastPriceBtnID, m_dwSelectPriceBtnID );
		}
		break;
	case ID_BUY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioWnd *pNewShopWnd = g_GUIMgr.FindWnd( NEW_SHOP_WND );
			if( pNewShopWnd )
			{
				ioWnd *pNewShopBuyWnd = pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND );
				if( pNewShopBuyWnd )
				{
					pNewShopBuyWnd->iwm_command( this, IOBN_BTNDOWN, m_dwSelectPriceBtnID-ID_PRICE1_RBTN );
					pNewShopBuyWnd->iwm_command( this, IOBN_BTNUP, 0 );
				}
			}
			HideWnd();
		}
		break;
	case ID_X_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}
//-------------------------------------------------------------------------
NewShopNeedPesoWnd::NewShopNeedPesoWnd( void )
{
	m_pIcon = NULL;
	m_iMaxPage = 0;
	m_iCurPage = 0;
}

NewShopNeedPesoWnd::~NewShopNeedPesoWnd( void )
{
	SAFEDELETE( m_pIcon );
	m_vNewShopItemInfoVec.clear();
}

bool NewShopNeedPesoWnd::SetInfo( const NeedPesoInfo &rkNeedPesoInfo )
{
	m_kNeedPesoInfo = rkNeedPesoInfo;
	if( m_kNeedPesoInfo.m_sIcon.IsEmpty() )
		return false;

	// icon
	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_kNeedPesoInfo.m_sIcon );

	// item info vec
	m_vNewShopItemInfoVec.clear();
	NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pNewShopWnd )
		pNewShopWnd->GetBonusPesoItemInfoVec( m_vNewShopItemInfoVec );

	if( m_vNewShopItemInfoVec.empty() )
		return false;

	// page
	int iTotalCnt = m_vNewShopItemInfoVec.size();
	m_iMaxPage = iTotalCnt / PAGE_PER_BTN_NUM;

	if( iTotalCnt%PAGE_PER_BTN_NUM > 0 )
		m_iMaxPage += 1;

	UpdatePage( 0 );
	return true;
}

void NewShopNeedPesoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		ICON_X_OFFSET = 175,
		ICON_Y_OFFSET = 91,

		DESC_X_OFFSET = 175,
		DESC_Y_OFFSET = 144,
		DESC_Y_GAP    = 22,

		SMALL_DESC_X_OFFSET = 175,
		SMALL_DESC_Y_OFFSET = 197,

		PAGE_X_OFFSET       = 175,
		PAGE_Y_OFFSET       = 407,

		PESOGOLD_X_OFFSET = 175,
		PESOGOLD_Y_OFFSET = 490,
	};

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

	// desc
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_17, m_kNeedPesoInfo.m_sName.c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	kPrinter.AddTextPiece( FONT_SIZE_17, m_kNeedPesoInfo.m_sPrice.c_str() );
	kPrinter.PrintFullText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	kPrinter.AddTextPiece( FONT_SIZE_17, m_kNeedPesoInfo.m_sMinusPrice.c_str() );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(1) );
	kPrinter.PrintFullText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET + DESC_Y_GAP, TAT_CENTER );
	kPrinter.ClearList();

	// small desc
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + SMALL_DESC_X_OFFSET, iYPos + SMALL_DESC_Y_OFFSET , TAT_CENTER );
	kPrinter.ClearList();

	// page
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+PAGE_X_OFFSET, iYPos+PAGE_Y_OFFSET, FONT_SIZE_13, m_sPage.c_str() );

	// peso / gold
	char szPeso[MAX_PATH]="";
	char szCash[MAX_PATH]="";

	Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szPeso, sizeof( szPeso ) );
	Help::ConvertNumToStrComma( g_MyInfo.GetTotalCash(), szCash, sizeof( szCash ) );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.AddTextPiece( FONT_SIZE_13, szPeso );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_13, " / " );
	kPrinter.SetTextColor( 141, 87, 4 );		
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.AddTextPiece( FONT_SIZE_13, szCash );
	kPrinter.PrintFullText( iXPos + PESOGOLD_X_OFFSET, iYPos + PESOGOLD_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();
}

void NewShopNeedPesoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ITEM_1_BTN:
	case ID_ITEM_2_BTN:
	case ID_ITEM_3_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
				int iSize       = m_vNewShopItemInfoVec.size();
				int iCurArray   = iStartArray + dwID - ID_ITEM_1_BTN;
				if( COMPARE( iCurArray, 0, iSize ) )
				{
					NewShopItemInfo &rkInfo = m_vNewShopItemInfoVec[iCurArray];
					pNewShopWnd->SetTabAndItem( rkInfo );
					NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND ) );
					if( pNewShopBuyWnd )
						pNewShopBuyWnd->SetPriceRadioBtnByCash( rkInfo.GetCash(0) );
				}
			}
			HideWnd();
		}
		break;
	case ID_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iNewPage = m_iCurPage - 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iNewPage = m_iCurPage + 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_X_BTN:
	case ID_ESC_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void NewShopNeedPesoWnd::UpdatePage( int iPage )
{
	m_iCurPage = iPage;

	char szBuf[MAX_PATH] = "";
	wsprintf_e( szBuf, "%d/%d", m_iCurPage+1, m_iMaxPage );
	m_sPage = szBuf;

	SetChildActive( ID_LEFT_BTN );
	SetChildActive( ID_RIGHT_BTN );

	if( m_iCurPage == 0 )
		SetChildInActive( ID_LEFT_BTN );

	if( m_iCurPage == m_iMaxPage-1 )
		SetChildInActive( ID_RIGHT_BTN );

	UpdateItemBtn();
}

void NewShopNeedPesoWnd::UpdateItemBtn()
{
	int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
	int iSize       = m_vNewShopItemInfoVec.size();
	for( int i=0; i < MAX_ITEM_BTN; ++i )
	{
		NewShopItemInfoWnd *pItem = dynamic_cast<NewShopItemInfoWnd*> ( FindChildWnd( ID_ITEM_1_BTN + i ) );
		if( !pItem )
			continue;

		int iCurArray = iStartArray + i;
		if( !COMPARE( iCurArray, 0, iSize ) )
		{
			// 초기화
			NewShopItemInfo kInfo;
			pItem->SetInfo( kInfo );
			pItem->SetSelect( false );
			continue;
		}

		pItem->SetInfo( m_vNewShopItemInfoVec[iCurArray] );
		pItem->SetSelect( false );
	}
}

bool NewShopNeedPesoWnd::GetCurItemInfo( OUT NewShopItemInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( !COMPARE( dwItemInfoID, ID_ITEM_1_BTN, ID_ITEM_3_BTN+1) )
		return false;

	int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
	int iCurArray   = iStartArray + ( dwItemInfoID - ID_ITEM_1_BTN );
	int iSize       = m_vNewShopItemInfoVec.size();
	if( !COMPARE( iCurArray, 0, iSize ) )
		return false;

	rkInfo = m_vNewShopItemInfoVec[iCurArray];
	return true;
}
//-------------------------------------------------------------------------
NewShopBuyWnd::NewShopBuyWnd(void)
{
	m_pIcon        = NULL;
	m_pIconBack    = NULL;
	m_pIconBack2   = NULL;
	m_pNumText	   = NULL;
	m_pPlusText	   = NULL;
	m_pEventMark   = NULL;
	m_pBonusMark   = NULL;
	m_pTitleLeft   = NULL;
	m_pTitleCenter = NULL;
	m_pLock        = NULL;
	m_pStripGrayDark = NULL;
	m_pStripGrayLight= NULL;
	m_pCostumeMark		= NULL;

	m_CharInfo.m_class_type		 = 1;
	m_CharInfo.m_kindred		 = 1;
	m_CharInfo.m_sex			 = 1;
	m_CharInfo.m_beard			 = 1;
	m_CharInfo.m_face			 = 1;
	m_CharInfo.m_hair			 = 1;
	m_CharInfo.m_skin_color		 = 1;
	m_CharInfo.m_hair_color		 = 1;
	m_CharInfo.m_accessories	 = 1;
	m_CharInfo.m_underwear		 = 1;
	m_CharInfo.m_iAwakeType		 = AWAKE_NONE;
	m_CharInfo.m_iAwakeLimitTime = 0;

	m_dwSelectWndID      = 0;
	m_dwLastPriceBtnID   = 0;
	m_dwSelectPriceBtnID = 0;
	m_dwChangeMarkDelay  = 0;
	m_dwCurChangeMarkTime= 0;
	m_bChangeMark        = false;
	m_iBoughtBonusPeso   = 0;
	m_bEtcItem3DChar	 = false;

	m_bSubscriptionResult = false;

	m_bEquipChangeFemale = false;
	m_nEquipChangeSex = 0;
	m_nGradeType = 0;
}

NewShopBuyWnd::~NewShopBuyWnd(void)
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconBack2 );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pEventMark );
	SAFEDELETE( m_pBonusMark );
	SAFEDELETE( m_pTitleLeft );
	SAFEDELETE( m_pTitleCenter );
	SAFEDELETE( m_pLock );
	SAFEDELETE( m_pStripGrayDark );
	SAFEDELETE( m_pStripGrayLight );
	SAFEDELETE( m_pCostumeMark );
}

void NewShopBuyWnd::SetInfo( const NewShopItemInfo &rkInfo )
{
	m_dwSelectWndID = 0;
	m_kItemInfo     = rkInfo;
	m_bEtcItem3DChar= false;
	m_bSubscriptionResult = false;

	m_bSetPreview = false;
	m_bFlashDown = false;
	m_nGradeType = g_PresentMgr.GetGradeType( m_kItemInfo.m_iItemType, m_kItemInfo.m_iValue1 );

	// icon
	SAFEDELETE( m_pIcon );
	if( m_kItemInfo.m_iItemType != PRESENT_SOLDIER && m_kItemInfo.m_iItemType != PRESENT_DECORATION )
	{
		if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_iValue1 );
			if( pEtcItem && !pEtcItem->IsShop3DChar() )
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_kItemInfo.m_sIcon );
		}
		else
		{
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( m_kItemInfo.m_sIcon );
		}
	}

	// text
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Lv%d %s",g_MyInfo.GetClassLevel( m_kItemInfo.m_iValue1, true ),g_MyInfo.GetClassName( m_kItemInfo.m_iValue1 ) );
		m_sTitle = szName;
	}
	else
	{
		m_sTitle = m_kItemInfo.m_sName;
	}

	SetPriceInfo();

	// 상점가격디폴트
	bool bTextPeso = false;
	if( ( m_kItemInfo.m_bShowPeso && m_kItemInfo.GetPeso( 0 ) > 0 )       ||
		( m_kItemInfo.GetPeso( 0 ) > 0 && m_kItemInfo.GetCash( 0 ) <= 0 ) ||
		( m_kItemInfo.m_iNeedLevel == 0 && m_kItemInfo.GetPeso( 0 ) > 0 && m_kItemInfo.GetCash( 0 ) > 0 ) )
		bTextPeso = true;

	// 이벤트 상품이다
	bool bTextStamp = false;  
	if( m_kItemInfo.GetStamp( 0 ) > 0 )
		bTextStamp = true;

	// 경험치+200 예외
	if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && m_kItemInfo.m_iValue1 == ioEtcItem::EIT_ETC_EXP_BONUS && m_kItemInfo.GetPeso( 2 ) > 0 )
	{
		bTextPeso = true;
	}

	// 페소+100 예외
	if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && m_kItemInfo.m_iValue1 == ioEtcItem::EIT_ETC_PESO_BONUS && m_kItemInfo.GetPeso( 2 ) > 0 )
	{
		bTextPeso = true;
	}
	
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER && m_kItemInfo.m_iNeedLevel < g_MyInfo.GetGradeLevel() )
	{
		bTextPeso = true;
	}

	// 상점가격디폴트
	DWORD dwCashPriceBtnID = ID_PRICE1_RBTN;
	if( !bTextPeso && !bTextStamp )
	{
		bool bAscendingOrder = false;

		if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM ||
			m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX ||
			m_kItemInfo.m_iItemType == PRESENT_DECORATION||
			m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
			bAscendingOrder = true;

		if( bAscendingOrder )
		{
			for (int i = 0; i < MAX_PRICE; ++i )
			{
				if( m_kPriceInfoArray[i].m_sName.IsEmpty() )
					continue;
				if( !IsSelectPricePeso( ID_PRICE1_RBTN + i ) )
				{
					dwCashPriceBtnID = ID_PRICE1_RBTN + i;
					break;
				}
			}
		}
		else
		{
			if( rkInfo.m_iItemType != PRESENT_SOLDIER || rkInfo.m_iNeedLevel > g_MyInfo.GetGradeLevel() )
			{
				for (int i = MAX_PRICE - 1; i >= 0 ; i--)
				{
					if( m_kPriceInfoArray[i].m_sName.IsEmpty() )
						continue;

					if( !IsSelectPricePeso( ID_PRICE1_RBTN + i ) )
					{
						dwCashPriceBtnID = ID_PRICE1_RBTN + i;
						break;
					}
				}
			}
		}
	}

	SetPriceRadioBtn( dwCashPriceBtnID );

	//////////////////////////////////////////////////////////////////////////
	// 확성기 예외 처리 - 가격 디폴트 선택을 가장 비싼 금액으로 
	if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && m_kItemInfo.m_iValue1 == ioEtcItem::EIT_ETC_WHOLE_CHAT )
	{
		SetPriceRadioBtn( m_dwLastPriceBtnID );
	}
	//////////////////////////////////////////////////////////////////////////
	// 용병세일이벤트이면 가장 마지막 캐쉬 가격
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER && m_kItemInfo.m_iShopMarkType == SHOP_MARK_TYPE_EVENT )
	{
		SetPriceRadioBtn( m_dwLastPriceBtnID );
	}
	
	if( m_kItemInfo.m_iItemType != PRESENT_DECORATION )
	{
		for (int i = 0; i < MAX_DECO_BTN ; i++)
		{
			HideChildWnd( ID_DECO_PREVIEW_KINDRED_BTN + i );
			HideChildWnd( ID_DECO_EQUIP_KINDRED_BTN + i );
		}
	}
	else
	{
		for (int i = 0; i < MAX_DECO_BTN ; i++)
		{
			ShowChildWnd( ID_DECO_PREVIEW_KINDRED_BTN + i );
			ShowChildWnd( ID_DECO_EQUIP_KINDRED_BTN + i );
		}
	}

	SetChildExInActive( ID_BUY_CASH_BTN );
	SetChildExInActive( ID_BUY_PRESENT_BTN );

	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
	{
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
		HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_ITEM_MOVIE_BTN );
		ShowChildWnd( ID_RECOMMEND_DECO_BTN );

		ioShopItemExtraInfoManager::ShopItemExtraInfo* pShopExtraInfo = g_ShopItemExtraInfoMgr.FindExtraInfo( m_kItemInfo.m_iItemType, m_kItemInfo.m_iValue1, m_kItemInfo.m_iValue1 );
		if ( pShopExtraInfo )
		{
			if ( pShopExtraInfo->m_bSetMovie )
			{
				ShowChildWnd( ID_ITEM_MOVIE_BTN );

				m_bFlashDown = pShopExtraInfo->m_bSetMovieDown;

				ShopItemMovieWnd* pWnd = dynamic_cast<ShopItemMovieWnd*>(g_GUIMgr.FindWnd( SHOP_ITEM_MOVIE_WND ));
				if ( pWnd )
				{					
					for ( int i=0; i<(int)pShopExtraInfo->m_SkillList.size(); ++i )
						pWnd->SetSelectDesc( i, (DWORD)pShopExtraInfo->m_SkillList[i] );

					pWnd->SetSelectType( ShopItemMovieWnd::DT_ALL );
				}
			}
		}

		Help::GetDefaultCharInfo( m_CharInfo, m_kItemInfo.m_iValue1 );

		// 코스튬 관련 (상점에서 용병 선택)
		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
		{
			pCharWnd->SetChar( m_CharInfo );
			pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
			pCharWnd->SetEquip( false, false, false );
			pCharWnd->ShowWnd();
		}	
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
	{
		ShowChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
		ShowChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_RECOMMEND_DECO_BTN );
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
		HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_ITEM_MOVIE_BTN );

		SetChildActive( ID_DECO_CHANGE_SEXTYPE_BTN );

		// set charinfo
		bool bInitDecoBtns  = false;
		bool bSetCharInfo   = false;
		bool bSetCharInfoPreview = false;
		NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
		if( pShopWnd )
		{
			if( pShopWnd->IsSetDecoCharInfo() )
			{
				bSetCharInfo = true;
				bInitDecoBtns= true;
				pShopWnd->SetDecoCharInfo( false );
			}
		}

		// 여성성별선택후 남성치장선택
		if( !bSetCharInfo )
		{
			if( m_kItemInfo.GetValue3(0) != m_CharInfo.m_sex - 1 )
			{
				bSetCharInfo = true;
				bSetCharInfoPreview =  true;
				bInitDecoBtns = false;
			}
		}

		if( bSetCharInfo )
		{
			CHARACTER kCharInfo;
			int iCharArray = g_MyInfo.GetClassArray( pShopWnd->GetDecoClassType() );
			if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) && pShopWnd->GetDecoSexType() == (g_MyInfo.GetCharacter( iCharArray ).m_sex-1) )
				m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
			else
				Help::GetDefaultCharInfo( m_CharInfo, pShopWnd->GetDecoClassType(), pShopWnd->GetDecoSexType() );
		}

		// 여성성별선택후 남성치장선택
		if( bSetCharInfoPreview )
			SetCharInfoByPreview( m_CharInfo );

		// 여자성별을 선택하는 경우
		if( m_kItemInfo.m_iValue1 == UID_KINDRED && m_CharInfo.m_sex != 2 ) // 2: 여자
		{
			Help::GetDefaultCharInfo( m_CharInfo, pShopWnd->GetDecoClassType(), 1 ); // 1: 여자
		}

		SetDecoChangeSexTypeTitle();
		GetCharInfoByDeco( m_CharInfo, m_kItemInfo.m_iValue1, m_kItemInfo.m_iValue2 );

		// 치장 착용 해제
		if( pShopWnd && pShopWnd->IsReleaseDecoEquip() )
		{
			int iEquipCode = g_MyInfo.GetEquipItemCode((m_CharInfo.m_class_type * 100000) + ((m_CharInfo.m_sex - 1) * 1000) + m_kItemInfo.m_iValue1 );
			if( iEquipCode != -1 )
			{
				if( m_kItemInfo.m_iValue1 == UID_FACE )
					m_CharInfo.m_face = iEquipCode;
				else if( m_kItemInfo.m_iValue1 == UID_HAIR )
					m_CharInfo.m_hair = iEquipCode;
				else if( m_kItemInfo.m_iValue1 == UID_HAIR_COLOR )
					m_CharInfo.m_hair_color = iEquipCode;
				else if( m_kItemInfo.m_iValue1 == UID_KINDRED )
					m_CharInfo.m_kindred = iEquipCode;
				else if( m_kItemInfo.m_iValue1 == UID_SKIN_COLOR )
					m_CharInfo.m_skin_color = iEquipCode;
				else if( m_kItemInfo.m_iValue1 == UID_UNDERWEAR )
					m_CharInfo.m_underwear = iEquipCode;
			}
		}

		// 코스튬 관련 (상점 치장)
		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
		{
			pCharWnd->SetChar( m_CharInfo );
			pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
			if( m_kItemInfo.m_iValue1 == UID_UNDERWEAR )
				pCharWnd->SetEquip( true, true, false );
			else
				pCharWnd->SetEquip( true, false, false );
			pCharWnd->ShowWnd();
			if( bInitDecoBtns )
				pCharWnd->InitRotate();
		}

		if( !EnableDecoChangeByEquipItem( m_CharInfo ) )
		{
			SetChildInActive( ID_DECO_CHANGE_SEXTYPE_BTN );
		}

		SetDecoPreviewBtn( m_kItemInfo, bInitDecoBtns );

		if( bInitDecoBtns )
		{
			if( pShopWnd )
				pShopWnd->ReleaseDecoEquipArray();
		}

		// 치장 착용 해제
		if( pShopWnd && pShopWnd->IsReleaseDecoEquip() )
		{
			pShopWnd->SetReleaseDecoEquip( false );

			ReleaseDecoPreviewBtn( m_kItemInfo.m_iValue1 );

			SetChildExActive( ID_BUY_CASH_BTN );
			SetChildExActive( ID_BUY_PRESENT_BTN );

			HideChildWnd( ID_PRICE1_RBTN );          
			HideChildWnd( ID_PRICE2_RBTN );          
			HideChildWnd( ID_PRICE3_RBTN );  
			HideChildWnd( ID_PRICE4_RBTN ); 
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		RandomMachineInfo *pItemInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( m_kItemInfo.m_iValue1 );
		if ( !pItemInfo )
			return;

		HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_RECOMMEND_DECO_BTN );
		HideChildWnd( ID_ITEM_MOVIE_BTN );

		DWORD dwItemCode = pItemInfo->GetItemCode( 0 );
		ioShopItemExtraInfoManager::ShopItemExtraInfo* pShopExtraInfo = g_ShopItemExtraInfoMgr.FindExtraInfo( m_kItemInfo.m_iItemType, m_kItemInfo.m_iValue1, dwItemCode );
		if ( pShopExtraInfo && dwItemCode != -1 )
		{
			m_bSetPreview = pShopExtraInfo->m_bSetPreview;

			if ( pShopExtraInfo->m_bSetChangeSex )
			{
				ShowChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
				ShowChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );

				HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
			}
			else
			{
				ShowChildWnd( ID_MY_SOLDIER_LIST_BTN2 );	

				HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
				HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
			}

			if ( pShopExtraInfo->m_bSetMovie )
			{
				ShowChildWnd( ID_ITEM_MOVIE_BTN );

				m_bFlashDown = pShopExtraInfo->m_bSetMovieDown;

				ShopItemMovieWnd* pWnd = dynamic_cast<ShopItemMovieWnd*>(g_GUIMgr.FindWnd( SHOP_ITEM_MOVIE_WND ));
				if ( pWnd )
				{
					pWnd->SetSelectDesc( 0, (DWORD)pShopExtraInfo->m_dwItemCode );
					pWnd->SetSelectType( ShopItemMovieWnd::DT_FIRST );
				}
			}

			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
			if( pShopWnd )
			{
				int iCharArray = g_MyInfo.GetClassArray( pShopWnd->GetDecoClassType() );
				m_CharInfo = g_MyInfo.GetCharacter( iCharArray );

				if ( m_CharInfo.m_sex == 1 )
				{
					m_bEquipChangeFemale = false;
					m_nEquipChangeSex = 0;
				}
				else if ( m_CharInfo.m_sex == 2 )
				{
					m_bEquipChangeFemale = true;
					m_nEquipChangeSex = 1;
				}

				SetEquipChangeSexTypeTitle( PRESENT_EXTRAITEM_BOX );
				
				// 코스튬 관련 (상점 장비 구매)
				NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
				if( pCharWnd )
				{
					pCharWnd->SetChar( m_CharInfo );
					pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
					pCharWnd->SetEquip( false, false, false );
					pCharWnd->EquipItem( dwItemCode, 0, 0 );
					pCharWnd->ShowWnd();
					pCharWnd->InitRotate();
				}
			}	
		}
		
		if ( !m_bSetPreview )
		{
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
			HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
			HideChildWnd( ID_SOLDIER_WND );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_iValue1 );
		if( pEtcItem && pEtcItem->IsShop3DChar() )
		{
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
			ShowChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
			HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
			HideChildWnd( ID_RECOMMEND_DECO_BTN );
			HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
			HideChildWnd( ID_ITEM_MOVIE_BTN );

			// set charinfo
			bool bInitDecoBtns  = false;
			bool bSetCharInfo   = false;
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
			if( pShopWnd )
			{
				if( pShopWnd->IsSetDecoCharInfo() )
				{
					bSetCharInfo = true;
					bInitDecoBtns= true;
					pShopWnd->SetDecoCharInfo( false );
				}
			}

			if( bSetCharInfo )
			{
				CHARACTER kCharInfo;
				int iCharArray = g_MyInfo.GetClassArray( pShopWnd->GetDecoClassType() );
				if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) && pShopWnd->GetDecoSexType() == (g_MyInfo.GetCharacter( iCharArray ).m_sex-1) )
					m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
				else
					Help::GetDefaultCharInfo( m_CharInfo, pShopWnd->GetDecoClassType(), pShopWnd->GetDecoSexType() );
			}

			SetDecoChangeSexTypeTitle();

			NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
			if( pCharWnd )
			{
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
				if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) || 
					COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
				{
					pCharWnd->SetEquip( true, false, false );
					pCharWnd->ShowWnd();
					pCharWnd->SetCharAnimation( m_kItemInfo.m_iValue1, ( m_CharInfo.m_sex == 1 ? true : false ) );
				}
				else
				{
					pCharWnd->SetEquip( false, false, false );
					pCharWnd->ShowWnd();
					pCharWnd->SetChangeSkeleton( m_kItemInfo.m_iValue1, ( m_CharInfo.m_sex == 1 ? true : false ) );
				}
				if( bInitDecoBtns )
					pCharWnd->InitRotate();
			}	

			if( bInitDecoBtns )
			{
				if( pShopWnd )
					pShopWnd->ReleaseDecoEquipArray();
			}

			// 치장 착용 해제
			if( pShopWnd && pShopWnd->IsReleaseDecoEquip() )
			{
				pShopWnd->SetReleaseDecoEquip( false );

				SetChildExActive( ID_BUY_CASH_BTN );
				SetChildExActive( ID_BUY_PRESENT_BTN );

				HideChildWnd( ID_PRICE1_RBTN );          
				HideChildWnd( ID_PRICE2_RBTN );          
				HideChildWnd( ID_PRICE3_RBTN );  
				HideChildWnd( ID_PRICE4_RBTN ); 
			}
			m_bEtcItem3DChar = true;
		}
		else
		{
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
			HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
			HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
			HideChildWnd( ID_ITEM_MOVIE_BTN );
			HideChildWnd( ID_SOLDIER_WND );
			HideChildWnd( ID_RECOMMEND_DECO_BTN );
		}
	}
	// UI코스튬 관련 (상점 정보 - 왼쪽)
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		CostumeShopInfo *pItemInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( m_kItemInfo.m_iValue1 );
		if ( !pItemInfo )
			return;

		CostumeShopSubInfo sCostumeShopSubInfo;
		if ( !pItemInfo->GetCostumeSubInfo( 0, sCostumeShopSubInfo ) )
			return;

		int nSubType = sCostumeShopSubInfo.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE;
		if ( !COMPARE( nSubType, 0, MAX_INVENTORY ) )
			return;
		
		CostumeInfo sCostumeInfo;
		if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeShopSubInfo.m_nCostumeCode, sCostumeInfo ) )
			return;

		HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_RECOMMEND_DECO_BTN );
		HideChildWnd( ID_ITEM_MOVIE_BTN );

		ShowChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
		ShowChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );

		m_bSetPreview = pItemInfo->IsPreview();
		
		NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
		if( pShopWnd )
		{
			int iCharArray = g_MyInfo.GetClassArray( pShopWnd->GetDecoClassType() );
			m_CharInfo = g_MyInfo.GetCharacter( iCharArray );

			if ( m_CharInfo.m_sex == 1 )
			{
				m_bEquipChangeFemale = false;
				m_nEquipChangeSex = 0;
			}
			else if ( m_CharInfo.m_sex == 2 )
			{
				m_bEquipChangeFemale = true;
				m_nEquipChangeSex = 1;
			}

			SetEquipChangeSexTypeTitle( PRESENT_COSTUME_BOX );

			m_CharInfo.m_EquipCostume[nSubType].m_CostumeCode = sCostumeShopSubInfo.m_nCostumeCode;
			
			// 코스튬 관련 (상점 코스튬 구매)
			NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
			if( pCharWnd )
			{
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
				pCharWnd->SetEquip( false, false, false );
				pCharWnd->ShowWnd();
				pCharWnd->InitRotate();
			}
		}	

		if ( !m_bSetPreview )
		{
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
			HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
			HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
			HideChildWnd( ID_SOLDIER_WND );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );
		HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
		HideChildWnd( ID_ITEM_MOVIE_BTN );
		HideChildWnd( ID_SOLDIER_WND );
		HideChildWnd( ID_RECOMMEND_DECO_BTN );
	}
	ShowChildWnd( ID_FLASH_WND );
}

void NewShopBuyWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	ManualMgr *pManual = NULL;
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )	
		pManual = ioItemManualMgr::GetSingletonPtr();
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
		pManual = ioDecoManualMgr::GetSingletonPtr();
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM || m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
		pManual = ioEtcManualMgr::GetSingletonPtr();
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
		pManual = ioEventGoodsManualMgr::GetSingletonPtr();
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
		pManual = ioCostumeShopManualMgr::GetSingletonPtr();

	if( !pManual )
		return;

	// Find Manual Idx // 고급장비는 Manual 1개의 다수의 페소가 설정되니 Set Default 0으로 됨 // stamp는 캐쉬,페소가 없으니 무조건 defualt
	bool bPeso  = false;
	bool bStamp = false;
	int  iPrice = 0;
	int iPriceInfoArray = m_dwSelectPriceBtnID - ID_PRICE1_RBTN;
	if( COMPARE( iPriceInfoArray, 0, MAX_PRICE ) )
	{
		bStamp = m_kPriceInfoArray[iPriceInfoArray].m_bStamp;
		if( !bStamp )
			bPeso  = m_kPriceInfoArray[iPriceInfoArray].m_bPeso;
		iPrice = m_kPriceInfoArray[iPriceInfoArray].m_iPrice;
	}

	int iManualIndex = -1;
	int iMax = m_kItemInfo.m_vManualIndex.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( bStamp )
		{
			if( iPrice == m_kItemInfo.GetStamp( i ) )
			{
				iManualIndex = m_kItemInfo.GetManualIndex( i );
				break;
			}
		}
		else if( bPeso )
		{
			if( iPrice == m_kItemInfo.GetPeso( i ) )
			{
				iManualIndex = m_kItemInfo.GetManualIndex( i );
				break;
			}
		}
		else
		{
			if( iPrice == m_kItemInfo.GetCash( i ) )
			{
				iManualIndex = m_kItemInfo.GetManualIndex( i );
				break;
			}
		}
	}

	// Set Default 0
	if( iManualIndex == -1 )
		iManualIndex = m_kItemInfo.GetManualIndex( 0 );

	ioComplexStringPrinter kPrinter;
	int iMaxLine = pManual->GetMaxLine( iManualIndex );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		pManual->SetPrinter( iManualIndex, iLine, fScale, kPrinter);
		a_iYPos += pManual->GetSpaceLine( iManualIndex, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}	
}

void NewShopBuyWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		TITLE_X_OFFSET = 129,
		TITLE_Y_OFFSET = 152,

		PRICE_X_OFFSET = 129,
		PRICE_Y_OFFSET = 175,

		DESC_X_OFFSET = 27,
		DESC_Y_OFFSET = 206,

		INFO_LEFT_X_OFFSET = 49,
		INFO_LEFT_Y_OFFSET = 353,

		INFO_RIGHT_X_OFFSET = 223,
		INFO_RIGHT_Y_OFFSET = 352,

		INFO_MARK_X_GAP     =  2,
		INFO_MARK_Y_GAP     = -1,

		INFO_LOCK_X_GAP     =  3,
		INFO_LOCK_Y_GAP     =  2,

		INFO_GAP = 21,

		ICON_X_OFFSET = 129, 
		ICON_Y_OFFSET = 105,

		TITLE_LOWER_X_OFFSET = 128,
		TITLE_LOWER_Y_OFFSET = 322,

		DECO_X_OFFSET        = 35,
		DECO_Y_OFFSET        = 87,
		DECO_PREVIEW_X_OFFSET= 222,
		DECO_PREVIEW_Y_OFFSET= 87,

		PRICE_BACK_X_OFFSET  = 27,
		PRICE_BACK_Y_OFFSET  = 349,
		PRICE_Y_GAP          = 21,

		BOUGHT_TEXT_X_OFFSET = 128,
		BOUGHT_TEXT_Y_OFFSET = 379,
		BOUGHT_TEXT_Y_GAP    = 23,
	};

	bool bNoneSelect = false;
	ioWnd *pCashWnd = FindChildWnd( ID_BUY_CASH_BTN );
	if( pCashWnd && pCashWnd->GetWndStyle() & IWS_EXACTIVE )
		bNoneSelect = true;

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	ioWnd *pWnd = FindChildWnd( ID_BUY_ADD_BTN );

	// title and desc
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER ||
		m_kItemInfo.m_iItemType == PRESENT_DECORATION ||
		m_bEtcItem3DChar )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		if( bNoneSelect )
			g_FontMgr.PrintText( iXPos + TITLE_LOWER_X_OFFSET, iYPos + TITLE_LOWER_Y_OFFSET, FONT_SIZE_17, STR(3) );
		else 
			g_FontMgr.PrintText( iXPos + TITLE_LOWER_X_OFFSET, iYPos + TITLE_LOWER_Y_OFFSET, FONT_SIZE_17, m_sTitle.c_str() );
	}
	else if ( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX && m_bSetPreview )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		if( bNoneSelect )
			g_FontMgr.PrintText( iXPos + TITLE_LOWER_X_OFFSET, iYPos + TITLE_LOWER_Y_OFFSET, FONT_SIZE_17, STR(3) );
		else 
			g_FontMgr.PrintText( iXPos + TITLE_LOWER_X_OFFSET, iYPos + TITLE_LOWER_Y_OFFSET, FONT_SIZE_17, m_sTitle.c_str() );
	}
	else if ( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX && m_bSetPreview )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		if( bNoneSelect )
			g_FontMgr.PrintText( iXPos + TITLE_LOWER_X_OFFSET, iYPos + TITLE_LOWER_Y_OFFSET, FONT_SIZE_17, STR(3) );
		else 
			g_FontMgr.PrintText( iXPos + TITLE_LOWER_X_OFFSET, iYPos + TITLE_LOWER_Y_OFFSET, FONT_SIZE_17, m_sTitle.c_str() );
	}
	else
	{
		if( pWnd && pWnd->IsShow() )
		{
			if( m_pIconBack2 )
				m_pIconBack2->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_MULTIPLY );
		}
		else
		{
			if( m_pIconBack )
				m_pIconBack->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET, UI_RENDER_MULTIPLY );
		}

		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

		if( m_pIcon )
			m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );
		OnRenderNumber( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

		// UI코스튬 관련 (코스튬 마크)
		if ( m_kItemInfo.m_iItemType == PRESENT_COSTUME || m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX || 
			( m_kItemInfo.m_iItemType == PRESENT_DEFAULT && (m_kItemInfo.m_iValue0 == PRESENT_COSTUME || m_kItemInfo.m_iValue0 == PRESENT_COSTUME_BOX) ) )
		{
			if ( m_pCostumeMark )
				m_pCostumeMark->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );
		}

		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + TITLE_X_OFFSET, iYPos + TITLE_Y_OFFSET, FONT_SIZE_17, m_sTitle.c_str() );

		int iArray = m_dwSelectPriceBtnID - ID_PRICE1_RBTN;
		if( COMPARE( iArray, 0, MAX_PRICE ) )
		{
			g_FontMgr.SetTextColor(  m_kPriceInfoArray[iArray].m_dwPriceColor );
			g_FontMgr.PrintText( iXPos + PRICE_X_OFFSET, iYPos + PRICE_Y_OFFSET, FONT_SIZE_17, m_kPriceInfoArray[iArray].m_sPrice.c_str() );
		}

		PrintManual( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET, FONT_SIZE_12 );
	}

	// bought
	if( pWnd && pWnd->IsShow() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		int iYGap = BOUGHT_TEXT_Y_GAP;
		if( m_iBoughtBonusPeso > 0  )
		{
			char szPeso[MAX_PATH]="";
			Help::ConvertNumToStrComma( m_iBoughtBonusPeso, szPeso, sizeof(szPeso) );
			char szText[MAX_PATH]="";
			SafeSprintf( szText, sizeof( szText ), STR(1), szPeso );

			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintText( iXPos + BOUGHT_TEXT_X_OFFSET, iYPos + BOUGHT_TEXT_Y_OFFSET+BOUGHT_TEXT_Y_GAP, FONT_SIZE_17, szText );
			iYGap = 0;
		}

		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + BOUGHT_TEXT_X_OFFSET, iYPos + BOUGHT_TEXT_Y_OFFSET + iYGap, FONT_SIZE_17, STR(4) );
		
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		if( m_bSubscriptionResult )
			g_FontMgr.PrintText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 22, FONT_SIZE_12, "[보관함]-[임시보관함]에서 받아주세요" );
		else
			g_FontMgr.PrintText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 22, FONT_SIZE_12, STR(5) );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( pWnd->GetDerivedPosX() + 102, pWnd->GetDerivedPosY() - 15, FONT_SIZE_12, STR(6) );
		return; // 구매된창이면 price 생략
	}

	// price
	if( m_pStripGrayDark )
		m_pStripGrayDark->Render( iXPos + PRICE_BACK_X_OFFSET, iYPos + PRICE_BACK_Y_OFFSET, UI_RENDER_MULTIPLY );
	if( m_pStripGrayLight )
		m_pStripGrayLight->Render( iXPos + PRICE_BACK_X_OFFSET, iYPos + PRICE_BACK_Y_OFFSET+PRICE_Y_GAP, UI_RENDER_MULTIPLY );
	if( m_pStripGrayDark )
		m_pStripGrayDark->Render( iXPos + PRICE_BACK_X_OFFSET, iYPos + PRICE_BACK_Y_OFFSET+(PRICE_Y_GAP*2), UI_RENDER_MULTIPLY );
	if( m_pStripGrayLight )
		m_pStripGrayLight->Render( iXPos + PRICE_BACK_X_OFFSET, iYPos + PRICE_BACK_Y_OFFSET+(PRICE_Y_GAP*3), UI_RENDER_MULTIPLY );

	if( bNoneSelect )
		return;

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	for (int i = 0; i < MAX_PRICE  ; i++)
	{
		if( m_kPriceInfoArray[i].m_sName.IsEmpty() )
			continue;

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET + INFO_GAP*i, FONT_SIZE_13, m_kPriceInfoArray[i].m_sName.c_str() );

		if( m_kPriceInfoArray[i].m_bPeso )
		{
			if( m_kItemInfo.m_iNeedLevel != 0 )
			{
				float fNameSize = g_FontMgr.GetTextWidth( m_kPriceInfoArray[i].m_sName.c_str(), TS_NORMAL,  FONT_SIZE_13 );
				int iLockX = iXPos + INFO_LEFT_X_OFFSET + INFO_LOCK_X_GAP + fNameSize;
				int iLockY = iYPos + INFO_LEFT_Y_OFFSET + INFO_GAP*i + INFO_LOCK_Y_GAP;	
				if( m_pLock )
					m_pLock->Render( iLockX, iLockY );
			}
		}
		
		if( m_kPriceInfoArray[i].m_bBonus || m_kPriceInfoArray[i].m_bEvent )
		{
			float fNameSize = g_FontMgr.GetTextWidth( m_kPriceInfoArray[i].m_sName.c_str(), TS_NORMAL,  FONT_SIZE_13 );
			int iMarkX = iXPos + INFO_LEFT_X_OFFSET + INFO_MARK_X_GAP + fNameSize;
			int iMarkY = iYPos + INFO_LEFT_Y_OFFSET + INFO_GAP*i + INFO_MARK_Y_GAP;
			if( m_kPriceInfoArray[i].m_bBonus && !m_kPriceInfoArray[i].m_bEvent )
			{
				if( m_pBonusMark )
					m_pBonusMark->Render( iMarkX,  iMarkY );
			}
			else if( !m_kPriceInfoArray[i].m_bBonus && m_kPriceInfoArray[i].m_bEvent )
			{
				if( m_pEventMark )
					m_pEventMark->Render( iMarkX,  iMarkY );
			}
			else if( m_kPriceInfoArray[i].m_bBonus && m_kPriceInfoArray[i].m_bEvent )
			{
				if( m_bChangeMark )
				{
					if( m_pEventMark )
						m_pEventMark->Render( iMarkX,  iMarkY );
				}
				else
				{
					if( m_pBonusMark )
						m_pBonusMark->Render( iMarkX,  iMarkY );
				}
			}
		}

		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextColor( m_kPriceInfoArray[i].m_dwPriceColor );
		g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET + INFO_GAP*i, FONT_SIZE_13, m_kPriceInfoArray[i].m_sPrice.c_str() );	
	}
}

void NewShopBuyWnd::OnRenderNumber( int iXPos, int iYPos )
{
	if( m_kItemInfo.m_iItemType != PRESENT_DEFAULT ) return;
	if( m_kItemInfo.m_iValue0 != PRESENT_EXTRAITEM ) return;

	int iNumber = ( m_kItemInfo.m_iValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
	if( iNumber <= 0 ) return;
	if( !m_pNumText || !m_pPlusText ) return;

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	int iNewXPos = iXPos;
	int iNewYPos = iYPos;
	int iCurPos = 0;
	int iDefaultPos = iNewXPos;

	int iCurYPos = iNewYPos + REINFORCE_Y;

	int iTotalSize = 0;
	if( iNumber < 10 )
		iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
	else if( iNumber < 100 )
		iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
	else
		iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

	iCurPos = iDefaultPos - (iTotalSize / 2);

	m_pPlusText->Render( iCurPos, iCurYPos );

	iCurPos += (PLUS_SIZE-NUM_GAP);

	m_pNumText->RenderNum( iCurPos, iCurYPos, iNumber, -NUM_GAP );
}

void NewShopBuyWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconBack2" )
	{
		SAFEDELETE( m_pIconBack2 );
		m_pIconBack2 = pImage;
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
	else if( szType == "EventMark" )
	{
		SAFEDELETE( m_pEventMark );
		m_pEventMark = pImage;
	}
	else if( szType == "BonusMark" )
	{
		SAFEDELETE( m_pBonusMark );
		m_pBonusMark = pImage;
	}
	else if( szType == "TitleLeft" )
	{
		SAFEDELETE( m_pTitleLeft );
		m_pTitleLeft = pImage;
	}
	else if( szType == "TitleCenter" )
	{
		enum { WIDTH_OFFSET = 13, }; // center 시작 위치 만큼 제거
		SAFEDELETE( m_pTitleCenter );
		m_pTitleCenter = pImage;
		if( m_pTitleCenter )
		{
			m_pTitleCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pTitleCenter->SetSize( GetWidth() - WIDTH_OFFSET  , m_pTitleCenter->GetHeight() );
		}
	}
	else if( szType == "Lock" )
	{
		SAFEDELETE( m_pLock );
		m_pLock = pImage;
	}
	else if( szType == "StripGrayDark" )
	{
		enum { WIDTH = 204, HEIGHT = 21, };
		SAFEDELETE( m_pStripGrayDark );
		m_pStripGrayDark = pImage;
		if( m_pStripGrayDark )
		{
			m_pStripGrayDark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pStripGrayDark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pStripGrayDark->SetSize( WIDTH, HEIGHT );
		}
	}
	else if( szType == "StripGrayLight" )
	{
		enum { WIDTH = 204, HEIGHT = 21, };
		SAFEDELETE( m_pStripGrayLight );
		m_pStripGrayLight = pImage;
		if( m_pStripGrayLight )
		{
			m_pStripGrayLight->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pStripGrayLight->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pStripGrayLight->SetSize( WIDTH, HEIGHT );
		}
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void NewShopBuyWnd::iwm_create()
{
	UppderAlphaWnd *pFlashWnd = dynamic_cast<UppderAlphaWnd*> ( FindChildWnd( ID_FLASH_WND ) );
	if( pFlashWnd )
		pFlashWnd->SetSizeFromParent( false );

	NewShopHelpWnd *pHelpWnd = dynamic_cast<NewShopHelpWnd*> ( FindChildWnd( ID_SOLDIER_WND ) );
	if( pHelpWnd )
		pHelpWnd->SetUserInfoToolTip( false );
}

void NewShopBuyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY_PESO_BTN:
	case ID_BUY_STAMP_BTN:
	case ID_BUY_CASH_BTN:
	case ID_BUY_PRESENT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
				g_MyInfo.IsExpertEntryTemporary() )
			{
				g_App.OpenFormalityEntryPage();
				return;
			}
			if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
			{
				if( m_kItemInfo.m_iValue1 == ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON ||
					COMPARE( m_kItemInfo.m_iValue1, ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON02, ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON51+1 ) )
				{
					if ( dwID != ID_BUY_PRESENT_BTN )
					{
						SelectExtraGashaponWnd *pWnd = dynamic_cast<SelectExtraGashaponWnd*>( g_GUIMgr.FindWnd( SELECT_EXTRA_GASHAPON_WND ) );
						if( pWnd )
						{
							if( pWnd->IsShow() )
							{
								pWnd->GoToTop();
							}
							else
							{
								pWnd->ShowSelectExtraGashaponWnd( m_kItemInfo.m_iValue1, SelectExtraGashaponWnd::UT_BUY_BTN );
							}
						}
					}
					else
						OnBuyBtnUp( dwID );
				}
				else if( COMPARE( m_kItemInfo.m_iValue1, ioEtcItem::EIT_ETC_SELECT_GASHAPON001, ioEtcItem::EIT_ETC_SELECT_GASHAPON300+1 ) )
				{
					if ( dwID != ID_BUY_PRESENT_BTN )
					{
						SelectGashaponWnd *pWnd = dynamic_cast<SelectGashaponWnd*>( g_GUIMgr.FindWnd( SELECT_GASHAPON_WND ) );
						if( pWnd )
						{
							if( pWnd->IsShow() && pWnd->GetSelectGashaponCode() == m_kItemInfo.m_iValue1 )
							{
								pWnd->GoToTop();
							}
							else
							{							
								pWnd->ShowSelectGashaponWnd( m_kItemInfo.m_iValue1, SelectGashaponWnd::UT_BUY_BTN );
							}
						}
					}
					else
						OnBuyBtnUp( dwID );
				}
				else
				{
					OnBuyBtnUp( dwID );
				}
			}
			else
			{
				OnBuyBtnUp( dwID );
			}
						
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case ID_BUY_GO_DECO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
			if( pShopWnd )
				pShopWnd->UpdateTab( NewShopWnd::ID_DECO_TAB_BTN, true );
		}
		break;
	case ID_BUY_GOODS_LIST_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
			{
				ioExtraItemListWnd *pListWnd = dynamic_cast<ioExtraItemListWnd*>(g_GUIMgr.FindWnd(EXTRAITEM_LIST_WND));
				if( pListWnd )
				{
					int iMachineCode  = m_kItemInfo.m_iValue1;
					pListWnd->ShowItemList( iMachineCode );
				}
			}
			else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
			{
				ioDecoItemListWnd *pListWnd = dynamic_cast<ioDecoItemListWnd*>(g_GUIMgr.FindWnd(DECOITEM_LIST_WND));
				
				int iEtcItemType = m_kItemInfo.m_iValue1;
				if( iEtcItemType == ioEtcItem::EIT_ETC_RANDOM_DECO_M )
				{
					if( pListWnd )
						pListWnd->ShowItemList( false );
				}
				else if( iEtcItemType == ioEtcItem::EIT_ETC_RANDOM_DECO_W )
				{
					if( pListWnd )
						pListWnd->ShowItemList( true );
				}
				else if( iEtcItemType == ioEtcItem::EIT_ETC_GASHAPON14 )
					g_GUIMgr.ShowWnd( GASHAPON_PRESENT_LIST_WND );
			}
			else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
			{
				ioCostumeListWnd *pListWnd = dynamic_cast<ioCostumeListWnd*>(g_GUIMgr.FindWnd(COSTUME_LIST_WND));
				if( pListWnd )
				{
					int nMachineCode  = m_kItemInfo.m_iValue1;
					pListWnd->ShowItemList( nMachineCode );
				}
			}
		}
		break;
	case ID_MY_SOLDIER_LIST_BTN1:
	case ID_MY_SOLDIER_LIST_BTN2:
		if( cmd == IOBN_BTNUP )
		{
			UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*> ( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
			if( pUISoldierWnd )
			{
				pUISoldierWnd->ShowUISoldierSelectWnd( this );
				int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
				if( iCharArray != -1 )
					pUISoldierWnd->SetSelectCharArray( iCharArray );
			}
		}
		break;
	case ID_ITEM_MOVIE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetMovieBtnFunc();
		}
		break;
	case ID_EQUIP_CHANGE_SEXTYPE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetEquipChangeSexTypeBtnUp();
		}
		break;
	case ID_DECO_CHANGE_SEXTYPE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetDecoChangeSexTypeBtnUp();
		}
		break;
	case ID_RECOMMEND_DECO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetRecommendDecoBtnUp();
		}
		break;
	case ID_DECO_PREVIEW_KINDRED_BTN:     
	case ID_DECO_PREVIEW_HAIR_BTN:        
	case ID_DECO_PREVIEW_HAIRCOLOR_BTN:   
	case ID_DECO_PREVIEW_FACE_BTN:        
	case ID_DECO_PREVIEW_SKINCOLOR_BTN:   
	case ID_DECO_PREVIEW_UNDERWEAR_BTN:   
		if( cmd == IOBN_BTNUP )
		{
			SetDecoPreviewBtnUp( dwID );
		}
		break;
	case ID_BUY_ADD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NewShopItemInfo kInfo = m_kItemInfo;
			SetInfo( kInfo );
		}
		break;
	case ID_SHOW_BOUGHT_ITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( !pInvenWnd )
				return;

			if( m_bSubscriptionResult )
			{
				pInvenWnd->ShowSubscriptionTabDirect();
			}
			else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && COMPARE( m_kItemInfo.m_iValue1, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_1000 + 1 ) )
			{
				ioHousingBlockInvenWnd* pWnd = dynamic_cast<ioHousingBlockInvenWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_INVEN_WND ) );
				if( pWnd )
				{
					pWnd->SetBlockModeType( BMT_GUILD );
					pWnd->ShowWnd();
				}
			}
			else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && COMPARE( m_kItemInfo.m_iValue1, ioEtcItem::EIT_ETC_HOME_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_HOME_HOUSING_BLOCK_1000 + 1 ) )
			{
				ioHousingBlockInvenWnd* pWnd = dynamic_cast<ioHousingBlockInvenWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_INVEN_WND ) );
				if( pWnd )
				{
					pWnd->SetBlockModeType( BMT_PERSONAL );
					pWnd->ShowWnd();
				}
			}
			else
			{
				if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
					pInvenWnd->ShowSoldierTabDirect( m_kItemInfo.m_iValue1 );
				else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
					pInvenWnd->ShowDecoTabDirect( m_CharInfo.m_class_type );
				else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
					pInvenWnd->ShowEtcTabDirect( m_kItemInfo.m_iValue1 );
				else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
					pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
				else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
					pInvenWnd->ShowPresentTabDirect();
				else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
					pInvenWnd->ShowCostumeTabDirect( g_MyInfo.GetClassType() );
			}
		}
		break;
	case ID_PRICE1_RBTN:
	case ID_PRICE2_RBTN:
	case ID_PRICE3_RBTN:
	case ID_PRICE4_RBTN:
		if( cmd == IOBN_BTNDOWN )
		{
			SetPriceRadioBtn( dwID );
		}
		break;
	case NEW_SHOP_NEED_LEVEL_WND:
		if( cmd == IOBN_BTNUP)
		{
			OnBuyBtnUp( ID_BUY_CASH_BTN );
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			DWORD dwRID    = 0;
			int iGoldArray = 0;
			for (int i = 0; i < MAX_PRICE ; i++)
			{
				if( m_kPriceInfoArray[i].m_sName.IsEmpty() )
					continue;
				DWORD dwPriceID = ID_PRICE1_RBTN + i;
				if( !IsSelectPricePeso( dwPriceID ) && !IsSelectPriceStamp( dwPriceID ) )
				{
					if( iGoldArray == param )
					{
						dwRID = dwPriceID;
						break;
					}
					iGoldArray++;
				}
			}

			SetPriceRadioBtn( dwRID );
		}
		break;
	case UI_SOLDIERSELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			UISoldierSelectWndBtnUp( param );
		}
		break;
	}
}

void NewShopBuyWnd::SendBuyProcess()
{
	if( SendBuy() )
		SetBoughtBonusPeso(); // 서버에서 구매실패할 수 있지만 린턴시 설정하면 여러군데 추가해야 하므로 보내는 것이 성공하면 셋팅함.
}

void NewShopBuyWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_sBuySound          = xElement.GetStringAttribute_e( "BuySound" );
	m_dwChangeMarkDelay  = xElement.GetIntAttribute_e( "ChangeMarkDelay" );
	m_dwCurChangeMarkTime= FRAMEGETTIME();
}

void NewShopBuyWnd::GetCharInfoByDeco( OUT CHARACTER &rkCharInfo, IN int iType, IN int iCode )
{
	switch( iType )
	{
	case UID_FACE:
		rkCharInfo.m_face = iCode;
		break;
	case UID_HAIR:
		rkCharInfo.m_hair = iCode;
		break;
	case UID_SKIN_COLOR:
		rkCharInfo.m_skin_color = iCode;
		break;
	case UID_HAIR_COLOR:
		rkCharInfo.m_hair_color = iCode;
		break;
	case UID_UNDERWEAR:
		rkCharInfo.m_underwear = iCode;
		break;
	case UID_KINDRED:
		{
			switch( iCode )
			{
			case RDT_HUMAN_MAN:
				rkCharInfo.m_kindred = 1;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_HUMAN_WOMAN:
				rkCharInfo.m_kindred = 1;
				rkCharInfo.m_sex = 2;
				break;
			case RDT_ELF_MAN:
				rkCharInfo.m_kindred = 2;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_ELF_WOMAN:
				rkCharInfo.m_kindred = 2;
				rkCharInfo.m_sex = 2;
				break;
			case RDT_DWARF_MAN:
				rkCharInfo.m_kindred = 3;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_DWARF_WOMAN:
				rkCharInfo.m_kindred = 3;
				rkCharInfo.m_sex = 2;
				break;
			}
		}
		break;	
	default:
		break;
	}
}

void NewShopBuyWnd::OnBuyBtnUp( DWORD dwID )
{
	bool bCheck = false;
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
		bCheck = CheckSoldierBuy( dwID );
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
		bCheck = CheckExtraBuy( dwID );
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
		bCheck = CheckDecoBuy( dwID );
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
		bCheck = CheckEtcBuy( dwID );
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
		bCheck = CheckEventGoodsBuy( dwID );
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
		bCheck = CheckCostumeBuy( dwID );
	else
		LOG.PrintTimeAndLog( 0, "%s Type is wrong. (%d)", __FUNCTION__, m_kItemInfo.m_iItemType );

	if( bCheck )
		ShowBuyMsgBox( dwID );
}

bool NewShopBuyWnd::CheckSoldierBuy( DWORD dwID )
{
	if( dwID == ID_BUY_PESO_BTN )
	{
		// level
		if( m_kItemInfo.m_iNeedLevel > g_MyInfo.GetGradeLevel() )
		{
			ShowNeedLevelMsg();
			return false;
		}		
	}

	int iCheckPrice = CheckPrice(dwID);
	if( iCheckPrice == CHECK_PRICE_PRESENT_OK )
		return true;
	else if( iCheckPrice == CHECK_PRICE_ERROR )
		return false;

	int iClassType = m_kItemInfo.m_iValue1;

	if( g_MyInfo.IsCharMortmain( iClassType ) && g_MyInfo.IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) == false )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return false;
	}
	// 가지고 있는 캐릭터
	if( g_MyInfo.GetClassArrayExceptExercise( iClassType ) != -1 )
	{
		//페소 구매 충전 불가 체크
		int iPeriodCharPseoChargeLimit = g_ClassPrice.GetPeriodCharPesoChargeLimit();
		if( dwID == ID_BUY_PESO_BTN && iPeriodCharPseoChargeLimit > 0 )
		{
			int iPriceArray			= GetSelectPriceArray( m_dwSelectPriceBtnID );
			int iCurrCharPesoTime	= g_MyInfo.GetCharTotalSecond( g_MyInfo.GetClassArray( iClassType ) ) + g_ClassPrice.GetLimitDatePeso( iClassType, iPriceArray );
			if( iCurrCharPesoTime > iPeriodCharPseoChargeLimit )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), ( iPeriodCharPseoChargeLimit / 60 ) / 60 );
				return false;
			}
		}

		//골드 구매 충전 불가 체크		
		int iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, GetSelectPriceArray( m_dwSelectPriceBtnID ) );
		
		// 영구는 iLimitDate가 0으로, 기간제는 1이상
		if( 0 < iLimitDate )
		{
			int iPeriodCharGoldChargeLimit = g_ClassPrice.GetPeriodCharGoldChargeLimit();
			int iCurrCharGoldTime =  g_MyInfo.GetCharTotalSecond( g_MyInfo.GetClassArray( iClassType ) ) + (int)g_ClassPrice.GetDefaultLimit();
			if( iCurrCharGoldTime >= g_ClassPrice.GetPeriodCharGoldChargeLimit() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드충전으로 %d시간을 초과할 수 없습니다.", ( iPeriodCharGoldChargeLimit / 60 ) / 60 );
				return false;
			}
		}
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) == false )
	{
		int iCurMaxSlot = g_MyInfo.GetCurMaxCharSlot();
		if( g_MyInfo.GetCharCount() >= iCurMaxSlot && g_MyInfo.GetClassArrayExceptExercise( iClassType ) == -1 ) 
		{
			NewShopItemPurchaseLeedWnd *pPurchaseLeedWnd = dynamic_cast<NewShopItemPurchaseLeedWnd*> ( g_GUIMgr.FindWnd( ITEM_PURCHASE_LEED_WND ) );
			if ( pPurchaseLeedWnd )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND );
				if ( pItem )
				{
					ioHashString sIcon = pItem->GetIconName();
					ioHashString sSubIcon = pItem->GetSubIconName();
					ioHashString sTitle= pItem->GetName();
					ioHashStringVec sDescVec;
					sDescVec.push_back( "[용병슬롯확장]이 필요합니다." );
					sDescVec.push_back( "용병 슬롯을 확장하기 위해 구입해주세요." );
					sDescVec.push_back( "상점 - 특별에서 구입하실 수 있습니다." );
					if( pPurchaseLeedWnd->SetInfo( ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND, sIcon, sSubIcon, sTitle, sDescVec ) )
						pPurchaseLeedWnd->ShowWnd();
				}
			}
			return false;
		}
	}
	return true;
}

bool NewShopBuyWnd::CheckExtraBuy( DWORD dwID )
{
	int iCheckPrice = CheckPrice(dwID);
	if( iCheckPrice == CHECK_PRICE_PRESENT_OK )
		return true;
	else if( iCheckPrice == CHECK_PRICE_ERROR ) 
		return false;

	if( dwID == ID_BUY_PESO_BTN )
	{
		if( m_kItemInfo.m_iNeedLevel > g_MyInfo.GetGradeLevel() )
		{
			char szHelp[MAX_PATH]="";
			g_LevelMgr.GetGradeName( m_kItemInfo.m_iNeedLevel, szHelp, sizeof( szHelp ), false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1),  szHelp );
			return false;
		}
	}

	if( g_MyInfo.IsExtraItemFull( true ) )
		return false;

	int iMachineCode = m_kItemInfo.m_iValue1;
	RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iMachineCode );
	if( pInfo )
	{
		if( pInfo->IsPackage() )
		{
			if( g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool NewShopBuyWnd::CheckDecoBuy( DWORD dwID )
{
	if( dwID == ID_BUY_PESO_BTN )
	{
		bool bCanBuy = false;
		if( g_MyInfo.GetUserEventType() == USER_TYPE_LIMIT_IGNORE )
			bCanBuy = true;
		else
		{
			int iClassLevel  = g_MyInfo.GetClassLevel( m_CharInfo.m_class_type, true );
			if( iClassLevel >= m_kItemInfo.m_iNeedLevel )
				bCanBuy = true;
			else
				bCanBuy = false;
		}

		if( !bCanBuy )
		{
			ShowNeedLevelMsg();
			return false;
		}
	}

	int iCheckPrice = CheckPrice(dwID);
	if( iCheckPrice == CHECK_PRICE_PRESENT_OK )
		return true;
	else if( iCheckPrice == CHECK_PRICE_ERROR ) 
		return false;

	if( g_MyInfo.IsCharRentalToClassType( m_CharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_CharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
		return false;
	}	

	int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
	if( iCharArray == - 1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	if( g_MyInfo.IsDecorationFull( true ) )
		return false;

	const CHARACTER &kCurCharInfo = g_MyInfo.GetCharacter( iCharArray );
	int iDecoType = m_kItemInfo.m_iValue1;
	int iDecoCode = m_kItemInfo.m_iValue2;
	if( m_CharInfo.m_sex == 2 && iDecoType != UID_KINDRED ) // 2: 여자 , 여자 종족은 다른 치장과 다르게 여자종족이 없어도 구매 가능
	{
		int iInvenType = 0;
		int iInvenCode = 0;
		GetInvenDecoInfo( iInvenType, iInvenCode, m_CharInfo.m_class_type, m_CharInfo.m_sex-1, UID_KINDRED, RDT_HUMAN_WOMAN );
		if( !g_MyInfo.IsSlotItem( iInvenType, iInvenCode ) ) 
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return false;
		}
	}

	if( m_CharInfo.m_sex == 2 && kCurCharInfo.m_sex == 1 )  // 성별 1(남자)2(여자) : 여자착용해 보기 상태에서 여자 아이템 구매 시도
	{
		if( iDecoType != UID_KINDRED )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			return false;
		}
	}

	if( m_CharInfo.m_sex == 1 && kCurCharInfo.m_sex == 2 )  // 성별 1(남자)2(여자) : 남자착용해 보기 상태에서 남자 아이템 구매 시도
	{
		if( iDecoType != UID_KINDRED )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			return false;
		}
	}

	if( g_MyInfo.IsCharExercise( iCharArray ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return false;
	}

	if( !g_MyInfo.IsCharActive( iCharArray ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return false;
	}
	
	int iInvenType = 0;
	int iInvenCode = 0;
	GetInvenDecoInfo( iInvenType, iInvenCode, kCurCharInfo.m_class_type, kCurCharInfo.m_sex-1, iDecoType, iDecoCode );
	if( g_MyInfo.IsSlotItem( iInvenType, iInvenCode ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return false;
	}

	if( iDecoType != UID_KINDRED )
	{
		if( m_kItemInfo.GetValue3(0) != (m_CharInfo.m_sex-1) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
			return false;
		}
	}

	return true;
}

bool NewShopBuyWnd::CheckEtcBuy( DWORD dwID )
{
	int iCheckPrice = CheckPrice(dwID);
	if( iCheckPrice == CHECK_PRICE_PRESENT_OK )
		return true;
	else if( iCheckPrice == CHECK_PRICE_ERROR ) 
		return false;

	int iPriceArray = GetSelectPriceArray( m_dwSelectPriceBtnID );
	int iType  = m_kItemInfo.m_iValue1;
	int iArray = m_kItemInfo.GetValue3( iPriceArray );

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem is NULL.", __FUNCTION__ );
		return false;
	}

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( !pEtcItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		LOG.PrintTimeAndLog( 0, "%s pEtcItem is NULL. :%d", __FUNCTION__, iType );
		return false;
	}

	if( pEtcItem->IsDecorationMaxCheck() )
	{
		// 치장 제한 체크
		if( g_MyInfo.IsDecorationFull( true ) )
			return false;
	}
	
	if( pEtcItem->IsExtraItemMaxCheck() )
	{
		// 장비 제한 체크
		if( g_MyInfo.IsExtraItemFull( true ) )
			return false;
	}

	if( pEtcItem->IsMedalItemMaxCheck() )
	{
		if( g_MyInfo.IsMedalItemFull( true ) )
			return false;
	}
	
	int iServerValue = pEtcItem->GetValue( iArray );
	bool bCanMortmain = pEtcItem->IsCanMortMain( iArray );
	if( iServerValue <= 0 )
	{
		if( !bCanMortmain || iServerValue < 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			LOG.PrintTimeAndLog( 0, "%s Value is wrong. :%d,%d", __FUNCTION__, iType, iArray );
			return false;
		}
	}

	if( ( g_MyInfo.GetActiveCharCount() - g_MyInfo.GetActiveExerciseCharCount() ) <= 0 )
	{

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	bool bExist = pUserEtcItem->GetEtcItem( iType, kItemSlot );

	// check max
	ioEtcItem::UseType eUseType = (ioEtcItem::UseType) ( iType/ioEtcItem::USE_TYPE_CUT_VALUE );
	if( pEtcItem->GetMaxUse() != 0 ) // 특별한  max 조건
	{
		if( !pEtcItem->IsBuyCondition( kItemSlot.GetUse() ) )
		{
			if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), pEtcItem->GetMaxUse() );
			else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8), pEtcItem->GetMaxUse() );
			else if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
				     COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			else if( ( pEtcItem->GetType() / ioEtcItemManager::USE_TYPE_POS ) == ioEtcItem::UT_TIME ) 
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), pEtcItem->GetMaxUse()/3600 );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7), pEtcItem->GetMaxUse() );

			return false;
		}
	}
	else if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1,		ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) ||
			 COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101,	ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) ||
			 COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301,	ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) ||
		     COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_ADD_CASH001, ioEtcItem::EIT_ETC_ADD_CASH100 + 1 ) )
	{
		if( pEtcItem->GetType() == kItemSlot.m_iType )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			return false;
		}
	}
	else if( pEtcItem->GetType() ==  ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND ) // 용병슬롯 over 체크
	{
		int iNewCnt = g_MyInfo.GetCurMaxCharSlot() + iServerValue;
		if( iNewCnt > g_MyInfo.GetLimiteMaxCharSlot() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8), g_MyInfo.GetLimiteMaxCharSlot() );
			return false;
		}
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FISHING_BAIT ) 
	{
		int iNewCnt = 0;
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FISHING_BAIT, kSlot ) )
			iNewCnt = kSlot.GetUse();

		iNewCnt += iServerValue;
		if( iNewCnt > 100000 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
			return false;
		}
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FISHING_MOON_BAIT ) 
	{
		int iNewCnt = 0;
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FISHING_MOON_BAIT, kSlot ) )
			iNewCnt = kSlot.GetUse();

		iNewCnt += iServerValue;
		if( iNewCnt > 100000 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
			return false;
		}
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_CREATE_GUILD_HQ )
	{
		if( pEtcItem->GetType() == kItemSlot.m_iType )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			return false;
		}

		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드에 가입된 상태에서만#구입이 가능한 아이템 입니다." );
			return false;
		}

		if( pGuildData->GetGuildMasterName() != g_MyInfo.GetPublicID() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드장만 구입 할 수 있는 아이템 입니다." );
			return false;
		}
	}
	else if( eUseType == ioEtcItem::UT_COUNT )
	{
		if(  (kItemSlot.GetUse() + iServerValue) > ioEtcItem::MAX_COUNT )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11), ioEtcItem::MAX_COUNT );
			return false;
		}
	}
	else if( eUseType == ioEtcItem::UT_TIME )
	{
		if( (kItemSlot.GetUse() + iServerValue > ioEtcItem::MAX_TIME ) )
		{
			enum { ONE_DAY_SEC = 86400, };
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12), ioEtcItem::MAX_TIME/ONE_DAY_SEC );
			return false;
		}
	}
	else if( eUseType == ioEtcItem::UT_ETERNITY ||
		eUseType == ioEtcItem::UT_ONCE )
	{
		if( kItemSlot.GetUse() != 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			return false;
		}
	}
	else if( eUseType == ioEtcItem::UT_DATE )
	{
		if( bExist && pEtcItem->GetType() == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
			return false;
		}
		if( bExist && g_EtcItemMgr.IsBlockEtcItem( iType ) && kItemSlot.m_iValue1 == 0 && kItemSlot.m_iValue2 == 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(22) );
			return false;
		}
		else
		{
			time_t kLimitTime = DateHelp::ConvertSecondTime( kItemSlot.GetYear(), kItemSlot.GetMonth(), kItemSlot.GetDay(), kItemSlot.GetHour(), kItemSlot.GetMinute(), 0 );
			time_t kCurServerTime = g_MyInfo.GetServerDate();
			kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

			enum { DAY_SEC = 86400, };
			int iGapSec = kLimitTime - kCurServerTime + (iServerValue*DAY_SEC);
			if( ( iGapSec/DAY_SEC ) >= ioEtcItem::MAX_DAY )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14), ioEtcItem::MAX_DAY );
				return false;
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(15) );
		LOG.PrintTimeAndLog( 0, "%s UseType is wrong. :%d", __FUNCTION__, (int) eUseType );
		return false;
	}

	switch( iType )
	{
	case ioEtcItem::EIT_ETC_GUILD_CREATE:
		{
			// 길드 생성 예외 처리
			if( ( !g_App.IsMeDeveloper() && !g_App.IsMeGameMaster() ) && 
				g_MyInfo.GetGradeLevel() < g_GuildInfo.GetGuildCreateLimitLevel() && dwID == ID_BUY_PESO_BTN )
			{
				char szName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_GuildInfo.GetGuildCreateLimitLevel(), szName, sizeof( szName ), false );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(16), szName );
				return false;
			}

			if( g_GuildInfo.IsGuild() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(17) );
				return false;
			}
		}
		break;
	case ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE:
		{
			// 길드 마크 변경 
			if( !g_App.IsGuildMarkChangeOK() )
				return false;
		}
		break;
	case ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE:
		{
			if( !g_GuildInfo.IsGuild() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(18) );
				return false;
			}
			else if( !g_GuildInfo.IsGuildMaster() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(19) );
				return false;
			}
		}
		break;
	case ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE:
		{
			if( g_MyInfo.GetGradeLevel() < g_TradeInfoMgr.GetTradeStateChangeLevel() )
			{
				char szName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_TradeInfoMgr.GetTradeStateChangeLevel(), szName, sizeof( szName ), false );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(20), szName );
				return false;
			}

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem && !pExtraItem->HasTradeWaitItem() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(21) );
				return false;
			}
		}
	}

	return true;
}

bool NewShopBuyWnd::CheckEventGoodsBuy( DWORD dwID )
{
	int iCheckPrice = CheckPrice(dwID);
	if( iCheckPrice == CHECK_PRICE_ERROR ) 
		return false;

	if( m_kItemInfo.m_iItemType != PRESENT_DEFAULT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}
	
	if( m_kItemInfo.GetValue3(1) != EVENT_GOODS_INFINITY_COUNT && m_kItemInfo.GetValue3(1) <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	return true;
}

// UI코스튬 관련 (코스튬 구입가능 확인)
bool NewShopBuyWnd::CheckCostumeBuy( DWORD dwID )
{	
	int iCheckPrice = CheckPrice(dwID);
	if( iCheckPrice == CHECK_PRICE_PRESENT_OK )
		return true;
	else if( iCheckPrice == CHECK_PRICE_ERROR ) 
		return false;

	if( dwID == ID_BUY_PESO_BTN )
	{
		if( m_kItemInfo.m_iNeedLevel > g_MyInfo.GetGradeLevel() )
		{
			char szHelp[MAX_PATH]="";
			g_LevelMgr.GetGradeName( m_kItemInfo.m_iNeedLevel, szHelp, sizeof( szHelp ), false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 계급이 되어야 구매가 가능합니다.",  szHelp );
			return false;
		}
	}

	if( g_MyInfo.IsCostumeFull( true ) )
		return false;

	int nMachineCode = m_kItemInfo.m_iValue1;
	CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( nMachineCode );
	if( pInfo )
	{
		if( pInfo->IsPackage() )
		{
			if( g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "선물함이 꽉 찼습니다.#선물함을 비워주시고 사용해 주세요." );
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void NewShopBuyWnd::ShowBuyMsgBox( DWORD dwID )
{
	m_dwSelectWndID = dwID;

	int iPriceArray = GetSelectPriceArray( m_dwSelectPriceBtnID );
	char szName[MAX_PATH]="";
	if( m_dwSelectWndID != ID_BUY_PRESENT_BTN )
	{
		bool bPeso = false;
		if( m_dwSelectWndID == ID_BUY_PESO_BTN )
			bPeso = true;	
		
		char szUseName[MAX_PATH]="";
		GetGoodsUseName( szUseName, sizeof( szUseName ), bPeso, iPriceArray );
		StringCbPrintf( szName, sizeof( szName ), "%s %s", m_kItemInfo.m_sName.c_str(), szUseName );
	}

	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
	{
		int iClassType = m_kItemInfo.m_iValue1;

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			int iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray ); // 영구는 iLimitDate가 0으로 표기된다.
			ShowSendPresentIdWnd( PRESENT_SOLDIER, iClassType, iLimitDate );
		}
		else
		{
			if( m_dwSelectWndID == ID_BUY_CASH_BTN )
				ShowBuyConfirmMsg( m_dwSelectWndID, szName, m_kItemInfo.m_iSubscriptionType );
			else
				ShowBuyConfirmMsg( m_dwSelectWndID, szName, SUBSCRIPTION_NONE );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		int iMachineCode = m_kItemInfo.m_iValue1;
		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			ShowSendPresentIdWnd( PRESENT_EXTRAITEM_BOX, iMachineCode, 0 );// 사용하지 않는 값으로 0으로 셋팅함.
		}
		else
			ShowBuyConfirmMsg( m_dwSelectWndID, szName, m_kItemInfo.m_iSubscriptionType );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
	{
		int iDecoType = m_kItemInfo.m_iValue1;
		int iDecoCode = m_kItemInfo.m_iValue2;

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			int iInvenType = 0;
			int iInvenCode = 0;
			GetInvenDecoInfo( iInvenType, iInvenCode, m_CharInfo.m_class_type, m_CharInfo.m_sex-1, iDecoType, iDecoCode );
			ShowSendPresentIdWnd( PRESENT_DECORATION, iInvenType, iInvenCode );
		}
		else
		{
			int iSubscriptionType = SUBSCRIPTION_NONE;
			if( m_dwSelectWndID == ID_BUY_CASH_BTN )
				iSubscriptionType = m_kItemInfo.GetSubscriptionType( iPriceArray );
			
			ShowBuyConfirmMsg( m_dwSelectWndID, szName, iSubscriptionType );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		int iType  = m_kItemInfo.m_iValue1;
		int iArray = m_kItemInfo.GetValue3( iPriceArray );

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
			if( !pEtcItem )
			{
				LOG.PrintTimeAndLog( 0, "%s : pEtcItem is NULL. :%d", __FUNCTION__, iType );
				return;
			}

			ShowSendPresentIdWnd( PRESENT_ETC_ITEM, iType, pEtcItem->GetValue( iArray ) );
		}
		else
		{
			int iSubscriptionType = SUBSCRIPTION_NONE;
			if( m_dwSelectWndID == ID_BUY_CASH_BTN )
				iSubscriptionType = m_kItemInfo.GetSubscriptionType( iPriceArray );

			ShowBuyConfirmMsg( m_dwSelectWndID, szName, iSubscriptionType );
		}
	}
	// UI코스튬 관련 (구입 성공 메세지)
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		int iMachineCode = m_kItemInfo.m_iValue1;
		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			ShowSendPresentIdWnd( PRESENT_COSTUME_BOX, iMachineCode, 0 );// 사용하지 않는 값으로 0으로 셋팅함.
		}
		else
			ShowBuyConfirmMsg( m_dwSelectWndID, szName, m_kItemInfo.m_iSubscriptionType );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		ShowBuyConfirmMsg( m_dwSelectWndID, szName, m_kItemInfo.m_iSubscriptionType );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s type is wrong. (%d)", __FUNCTION__, m_kItemInfo.m_iItemType );
	}
}

int NewShopBuyWnd::GetSubscriptionType()
{
	int iPriceArray = GetSelectPriceArray( m_dwSelectPriceBtnID );

	int iSubscriptionType = SUBSCRIPTION_NONE;
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
	{
		if( m_dwSelectWndID == ID_BUY_CASH_BTN )
			iSubscriptionType = m_kItemInfo.m_iSubscriptionType;
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		iSubscriptionType = m_kItemInfo.m_iSubscriptionType;
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
	{
		if( m_dwSelectWndID == ID_BUY_CASH_BTN )
			iSubscriptionType = m_kItemInfo.GetSubscriptionType( iPriceArray );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		if( m_dwSelectWndID == ID_BUY_CASH_BTN )
			iSubscriptionType = m_kItemInfo.GetSubscriptionType( iPriceArray );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		iSubscriptionType = m_kItemInfo.m_iSubscriptionType;
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		iSubscriptionType = m_kItemInfo.m_iSubscriptionType;
	}

	return iSubscriptionType;
}

bool NewShopBuyWnd::SendBuy()
{
	if( g_App.IsMouseBusy() )
		return false;

	g_App.SetMouseBusy( true );

	bool bCash = false;
	if( m_dwSelectWndID == ID_BUY_CASH_BTN )
		bCash = true;

	int iPriceArray = GetSelectPriceArray( m_dwSelectPriceBtnID );

	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
	{
		int iClassType = m_kItemInfo.m_iValue1;

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			int iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray ); // 영구는 iLimitDate가 0으로 표기된다.
			if( !SendPresentBuy( PRESENT_SOLDIER, iClassType, iLimitDate ) )
				return false;
		}
		else if( GetSubscriptionType() == SUBSCRIPTION_TYPE1 )
		{
			int iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray ); // 영구는 iLimitDate가 0으로 표기된다.
			if( !SendSubscriptionBuy( PRESENT_SOLDIER, iClassType, iLimitDate ) )
				return false;
		}
		else if( g_MyInfo.GetClassArrayExceptExercise( iClassType ) != -1 )
		{
			int iLimitDate = 0;
			if( bCash )
				iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray ); // 영구는 iLimitDate가 0으로 표기된다.
			else
				iLimitDate = g_ClassPrice.GetLimitDatePeso( iClassType, iPriceArray ); // 영구는 iLimitDate가 0으로 표기된다.

			if( bCash && iLimitDate == 0 )  // 영구
			{
				SP2Packet kPeriodPacket( CTPK_CHAR_CHANGE_PERIOD );
				kPeriodPacket << g_MyInfo.GetCharIndex( g_MyInfo.GetClassArray( iClassType ) );
				kPeriodPacket << bCash;
				TCPNetwork::SendToServer( kPeriodPacket );
			}
			else if( iLimitDate != 0 )
			{
				// 보유중인 용병은 기간 연장
				SP2Packet kPacket( CTPK_CHAR_EXTEND );
				kPacket << CHAR_EXTEND_NORMAL;
				kPacket << g_MyInfo.GetCharIndex( g_MyInfo.GetClassArray( iClassType ) );
				if( bCash )
					kPacket << g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray );
				else
					kPacket << g_ClassPrice.GetLimitDatePeso( iClassType, iPriceArray );
				kPacket << bCash;
				TCPNetwork::SendToServer( kPacket );
			}
			else 
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return false;
			}
		}
		else
		{
			CharPeriodType ePeriodType = CPT_TIME;
			int iLimitDate = 0;
			if( bCash )
				iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray ); 
			else
				iLimitDate = g_ClassPrice.GetLimitDatePeso( iClassType, iPriceArray ); 

			if( bCash && iLimitDate == 0 ) // 영구
				ePeriodType = CPT_MORTMAIN;

			int iPrice = 0;
			if( bCash )
				iPrice = m_kItemInfo.GetCash( iPriceArray );
			else
				iPrice = m_kItemInfo.GetPeso( iPriceArray );

			SP2Packet kPacket( CTPK_CHAR_CREATE );
			kPacket << CHAR_CREATE_NORMAL;
			kPacket << iClassType;
			kPacket << 1; // 휴먼
			kPacket << 1; // 남자
			kPacket << iLimitDate;
			kPacket << iPrice;
			kPacket << bCash;
			kPacket << (int) ePeriodType;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		int iMachineCode = m_kItemInfo.m_iValue1;

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			int iPresentValue2 = 1; // 1이면 캐쉬 구매 상태
			if( !SendPresentBuy( PRESENT_EXTRAITEM_BOX, iMachineCode, iPresentValue2 ) ) 
				return false;
		}
		else if( GetSubscriptionType() == SUBSCRIPTION_TYPE1 )
		{
			int iPresentValue2 = 1; // 1이면 캐쉬 구매 상태
			if( !SendSubscriptionBuy( PRESENT_EXTRAITEM_BOX, iMachineCode, iPresentValue2 ) )
				return false;
		}
		else 
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iMachineCode );
			if( !pInfo )
			{
				LOG.PrintTimeAndLog(0, "ShopExtraBuyTab::SendBuyPacket MachineInfo == NULL, %d", iMachineCode );
				return false;
			}

			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
			if( pNewShopWnd )
			{
				NewShopBoughtWnd *pBoughtWnd = dynamic_cast<NewShopBoughtWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BOUGHT_WND ));
				if( pBoughtWnd )
					pBoughtWnd->ShowBoughtWnd( PRESENT_EXTRAITEM_BOX, iMachineCode, 0 );
				HideWnd();
			}

			SP2Packet kPacket( CTPK_EXTRAITEM_BUY );
			kPacket << iMachineCode;
			kPacket << bCash; 
			kPacket << iPriceArray;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
	{
		int iDecoType    = m_kItemInfo.m_iValue1;
		int iDecoCode    = m_kItemInfo.m_iValue2;
		int iDecoSexType = m_kItemInfo.GetValue3(0);

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			int iInvenType = 0;
			int iInvenCode = 0;
			GetInvenDecoInfo( iInvenType, iInvenCode, m_CharInfo.m_class_type, iDecoSexType, iDecoType, iDecoCode );
			if( !SendPresentBuy( PRESENT_DECORATION, iInvenType, iInvenCode) )
				return false;
		}
		else if( GetSubscriptionType() == SUBSCRIPTION_TYPE1 )
		{
			int iInvenType = 0;
			int iInvenCode = 0;
			GetInvenDecoInfo( iInvenType, iInvenCode, m_CharInfo.m_class_type, iDecoSexType, iDecoType, iDecoCode );
			if( !SendSubscriptionBuy( PRESENT_DECORATION, iInvenType, iInvenCode) )
				return false;
		}
		else 
		{
			int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
			if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
			{
				LOG.PrintTimeAndLog(0, "%s : Array is wrong. ", __FUNCTION__ );
				return false;
			}

			int iInvenType = 0;
			int iInvenCode = 0;
			GetInvenDecoInfo( iInvenType, iInvenCode, m_CharInfo.m_class_type, iDecoSexType, iDecoType, iDecoCode );

			SP2Packet kPacket( CTPK_CHAR_DECORATION_BUY );
			kPacket << iCharArray;
			kPacket << iInvenType << iInvenCode << bCash;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		int iType  = m_kItemInfo.m_iValue1;
		int iArray = m_kItemInfo.GetValue3( iPriceArray );

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			if( !SendPresentBuy( PRESENT_ETC_ITEM, iType, iArray) )
				return false;
		}
		else if( GetSubscriptionType() == SUBSCRIPTION_TYPE1 )
		{
			if( !SendSubscriptionBuy( PRESENT_ETC_ITEM, iType, iArray) )
				return false;
		}
		else 
		{
			SP2Packet kPacket( CTPK_ETCITEM_BUY );
			kPacket << iType << iArray << bCash; 
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		if( m_kItemInfo.GetValue3( 0 ) > 0 )
		{
			SP2Packet kPacket( CTPK_EVENT_SHOP_GOODS_BUY );
			kPacket << m_kItemInfo.GetValue3( 0 );
			TCPNetwork::SendToServer( kPacket );
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
	}
	// UI코스튬 관련 (구매 패킷 전송)
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		int iMachineCode = m_kItemInfo.m_iValue1;

		if( m_dwSelectWndID == ID_BUY_PRESENT_BTN )
		{
			int iPresentValue2 = 1; // 1이면 캐쉬 구매 상태
			if( !SendPresentBuy( PRESENT_COSTUME_BOX, iMachineCode, iPresentValue2 ) ) 
				return false;
		}
		else if( GetSubscriptionType() == SUBSCRIPTION_TYPE1 )
		{
			int iPresentValue2 = 1; // 1이면 캐쉬 구매 상태
			if( !SendSubscriptionBuy( PRESENT_COSTUME_BOX, iMachineCode, iPresentValue2 ) )
				return false;
		}
		else 
		{
			CostumeShopInfo *pItemInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( m_kItemInfo.m_iValue1 );
			if ( !pItemInfo )
				return false;

			CostumeShopSubInfo sCostumeShopSubInfo;
			if ( !pItemInfo->GetCostumeSubInfo( 0, sCostumeShopSubInfo ) )
				return false;
			
			CostumeInfo sCostumeInfo;
			if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeShopSubInfo.m_nCostumeCode, sCostumeInfo ) )
			{
				LOG.PrintTimeAndLog(0, "ShopCostumeBuyTab::SendBuyPacket MachineInfo == NULL, %d", iMachineCode );
				return false;
			}

			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
			if( pNewShopWnd )
			{
				NewShopBoughtWnd *pBoughtWnd = dynamic_cast<NewShopBoughtWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BOUGHT_WND ));
				if( pBoughtWnd )
					pBoughtWnd->ShowBoughtWnd( PRESENT_COSTUME_BOX, iMachineCode, m_kItemInfo.m_iValue2 );
				HideWnd();
			}

			SP2Packet kPacket( CTPK_COSTUME_BUY );
			kPacket << iMachineCode;
			kPacket << bCash;
			kPacket << (BYTE)iPriceArray;
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Type is wrong.(%d)", __FUNCTION__, m_kItemInfo.m_iItemType );
		return false;
	}
	return true;
}

bool NewShopBuyWnd::SendPresentBuy( short isPresentType, int iPresentValue1, int iPresentValue2 )
{
	SendPresentConfirmWnd *pSendWnd = dynamic_cast< SendPresentConfirmWnd* > (g_GUIMgr.FindWnd( SEND_PRESENT_CONFIRM_WND ) );
	if( !pSendWnd )
	{
		LOG.PrintTimeAndLog(0, "%s : pSendWnd == NULL ", __FUNCTION__ );
		return false;
	}

	SP2Packet kPacket( CTPK_PRESENT_BUY );
	kPacket << isPresentType;
	kPacket << iPresentValue1;
	kPacket << iPresentValue2;
	kPacket << pSendWnd->GetRecvPublicID();
	TCPNetwork::SendToServer( kPacket );

#if defined( USE_GA )
	int iCashPrice	= 0;
	int iItemCode	= 0;

	g_HttpMng.SetGiftIndex( 0 );
	g_HttpMng.SetGiftCash( 0 );

	switch( isPresentType )
	{
	case PRESENT_SOLDIER:
		{
			if( iPresentValue2 == 0 )
				iCashPrice = g_ClassPrice.GetMortmainCharCash( iPresentValue1 );
			else
				iCashPrice = g_ClassPrice.GetClassBuyCash( iPresentValue1, iPresentValue2 );

			if( iCashPrice > 0 )
			{
				g_HttpMng.SetGiftIndex( iPresentValue1 );
				g_HttpMng.SetGiftCash( iCashPrice );
			}
		}
		break;
		
	case PRESENT_EXTRAITEM_BOX:
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
			if( pInfo )
			{
				iCashPrice	= pInfo->GetNeedCash();
				iItemCode	= pInfo->GetItemCode( 0 );
				if( iCashPrice > 0 )
				{
					g_HttpMng.SetGiftIndex( iItemCode );
					g_HttpMng.SetGiftCash( iCashPrice );
				}
			}
		}
		break;

	case PRESENT_DECORATION:
		{
			char chType[32]		= {0,};
			char chParse[32]	= {0,};
			char chCode[32]		= {0,};
			int iSendCode		= 0;

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chType, "%d", iPresentValue1 );
			else
				SafeSprintf( chType, sizeof(chType), "%1", iPresentValue1 );

			int iSize = strlen( chType ) - 4;
			strncpy( chParse, chType + iSize, 4 );

			// 코드 조합
			// "7 (치장)" + "000X(남자) or 100X(여자), (X는 치장 종류, ex> 머리모양, 피부색.. )," + "치장 종류에 따른 index (ex>머리모양 1,2,3..)"
			// ex> 710031 -> 7 치장 + 1003 여자, 피부색 + 1 피부색 중 첫번쨰 아이템.
			// 1 표정, 2 머리모양, 3 피부색, 4 머리색, 7 속옷
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chCode, "%d%s%d", 7, chParse, iPresentValue2 );
			else
				SafeSprintf( chCode, sizeof(chCode), "%1%2%3", 7, chParse, iPresentValue2 );

			iSendCode = ioStringConverter::ParseInt( chCode );
			
			iCashPrice = g_DecorationPrice.GetDecoCashByType( iPresentValue1, iPresentValue2 ); 
			if( iCashPrice > 0 )
			{
				g_HttpMng.SetGiftIndex( iSendCode );
				g_HttpMng.SetGiftCash( iCashPrice );
			}
		}
		break;

	case PRESENT_ETC_ITEM:
		{
			iCashPrice = g_App.GetGAEtcCash2( iPresentValue1, iPresentValue2 );
			if( iCashPrice > 0 )
			{
				g_HttpMng.SetGiftIndex( iPresentValue1 );
				g_HttpMng.SetGiftCash( iCashPrice );
			}
		}
		break;

	default:
		break;
	}
#endif

	return true;
}

bool NewShopBuyWnd::SendSubscriptionBuy( short isPresentType, int iPresentValue1, int iPresentValue2 )
{
	SP2Packet kPacket( CTPK_SUBSCRIPTION_BUY );
	kPacket << isPresentType;
	kPacket << iPresentValue1;
	kPacket << iPresentValue2;
	TCPNetwork::SendToServer( kPacket );

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		g_HttpMng.SetScripHeroIndex( iPresentValue1 );
		g_HttpMng.SetScripHeroLimitDate( iPresentValue2 );
	}
#endif

	return true;
}

void NewShopBuyWnd::ShowSendPresentIdWnd( short isPresentType, int iPresentValue1, int iPresentValue2 )
{
	SendPresentIdWnd *pSendWnd = dynamic_cast< SendPresentIdWnd* > (g_GUIMgr.FindWnd( SEND_PRESENT_ID_WND ) );
	if( !pSendWnd )
	{
		LOG.PrintTimeAndLog(0, "%s - pSendWnd == NULL." , __FUNCTION__ );
		return;
	}
	pSendWnd->ShowSendPresentIdWnd( this, isPresentType, iPresentValue1, iPresentValue2 );
}

void NewShopBuyWnd::ShowBuyConfirmMsg( DWORD dwID, const char *szName, int iType )
{
	if( szName == NULL )
		return;

	char szPrice[MAX_PATH]="";
	if( dwID == ID_BUY_PESO_BTN )
	{
		Help::ConvertNumToStrComma( m_kItemInfo.GetPeso( GetSelectPriceArray( m_dwSelectPriceBtnID ) ), szPrice, sizeof( szPrice ) );
		StringCbCat( szPrice, sizeof( szPrice ), STR(1));
	}
	else if( dwID == ID_BUY_STAMP_BTN )
	{
		char szComma[MAX_PATH] = "";
		Help::ConvertNumToStrComma( m_kItemInfo.GetStamp( GetSelectPriceArray( m_dwSelectPriceBtnID ) ), szComma, sizeof( szComma ) );
		
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_dwStampEtcItem );
		if( pEtcItem )
			SafeSprintf( szPrice, sizeof(szPrice), STR(10), pEtcItem->GetName().c_str(), szComma );
		else
			SafeSprintf( szPrice, sizeof(szPrice), STR(8), szComma );
	}
	else
	{
		Help::ConvertNumToStrComma( m_kItemInfo.GetCash( GetSelectPriceArray( m_dwSelectPriceBtnID ) ), szPrice, sizeof( szPrice ) );
		StringCbCat( szPrice, sizeof( szPrice ), STR(2));
	}

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(3), szName );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4), szPrice );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(5) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "                 " );

	int iCash = m_kItemInfo.GetCash( GetSelectPriceArray( m_dwSelectPriceBtnID ) );
	int iRealCash = g_MyInfo.GetTotalCash() - g_MyInfo.GetBonusCash();
	
	//캐쉬로 사는 용병만
	if ( dwID == ID_BUY_CASH_BTN && m_kItemInfo.m_iItemType == PRESENT_SOLDIER && iType == SUBSCRIPTION_TYPE1 && iRealCash < iCash )
	{
		//사용될 예상 보너스 캐쉬
		char szText[MAX_PATH]="";
		int iUsingBounsCash = iCash - iRealCash;

		kPrinter[4].SetTextStyle( TS_NORMAL );
		kPrinter[4].SetBkColor( 0, 0, 0 );	
		kPrinter[4].SetTextColor( TCT_DEFAULT_BLUE );
		SafeSprintf( szText, sizeof(szText), "보너스 골드 : %d 골드 사용", iUsingBounsCash );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, szText );

		kPrinter[5].SetTextStyle( TS_NORMAL );
		kPrinter[5].SetBkColor( 0, 0, 0 );	
		kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[5].AddTextPiece( FONT_SIZE_13, "보너스 골드 사용시 청약철회에서 제외" );

		kPrinter[6].SetTextStyle( TS_NORMAL );
		kPrinter[6].SetBkColor( 0, 0, 0 );	
		kPrinter[6].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[6].AddTextPiece( FONT_SIZE_13, STR(11) );
	}
	else if( iType == SUBSCRIPTION_TYPE1 )
	{
		kPrinter[4].SetTextStyle( TS_NORMAL );
		kPrinter[4].SetBkColor( 0, 0, 0 );	
		kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, STR(13) );

		kPrinter[5].SetTextStyle( TS_NORMAL );
		kPrinter[5].SetBkColor( 0, 0, 0 );	
		kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[5].AddTextPiece( FONT_SIZE_13, STR(14) );

		kPrinter[6].SetTextStyle( TS_NORMAL );
		kPrinter[6].SetBkColor( 0, 0, 0 );	
		kPrinter[6].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[6].AddTextPiece( FONT_SIZE_13, STR(11) );
	}
	else
	{
		kPrinter[4].SetTextStyle( TS_NORMAL );
		kPrinter[4].SetBkColor( 0, 0, 0 );	
		kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, STR(9) );

		kPrinter[5].SetTextStyle( TS_NORMAL );
		kPrinter[5].SetBkColor( 0, 0, 0 );	
		kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[5].AddTextPiece( FONT_SIZE_13, STR(11) );
	}

	NewShopBuyConfirmWnd *pBuyMsgWnd = dynamic_cast< NewShopBuyConfirmWnd* >( g_GUIMgr.FindWnd( NEWSHOP_BUY_CONFIRM_WND ) ); 
	if( pBuyMsgWnd )
		pBuyMsgWnd->SetPrintMessage( this, kPrinter, iType );
	else
	{
		LOG.PrintTimeAndLog(0, "%s - pBuyMsgWnd == NULL." , __FUNCTION__ );
	}
}

void NewShopBuyWnd::ShowNeedLevelMsg()
{
	char szHelp[MAX_PATH]="";
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
		g_LevelMgr.GetGradeName( m_kItemInfo.m_iNeedLevel, szHelp, sizeof( szHelp ), false );
	else 
		StringCbPrintf( szHelp, sizeof( szHelp ), "LV%d", m_kItemInfo.m_iNeedLevel );

	NewShopNeedLevelWnd::NeedLevelInfo kInfo;
	kInfo.m_sTitle = m_kItemInfo.m_sName;
	kInfo.m_sIcon  = m_kItemInfo.m_sIcon;
	kInfo.m_sNeedLevel = szHelp;

	for (int i = 0; i < MAX_PRICE ; i++)
	{
		if( m_kPriceInfoArray[i].m_sName.IsEmpty() )
			continue;
		if( m_kPriceInfoArray[i].m_bPeso )
			continue;
		kInfo.m_vNameVec.push_back( m_kPriceInfoArray[i].m_sName );
		kInfo.m_vPriceVec.push_back( m_kPriceInfoArray[i].m_sPrice );
	}

	NewShopNeedLevelWnd *pWnd = dynamic_cast<NewShopNeedLevelWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_NEED_LEVEL_WND ) );
	if( pWnd )
	{
		if( pWnd->SetInfo( kInfo ) )
			pWnd->ShowWnd();
	}
}

int NewShopBuyWnd::CheckPrice( DWORD dwID )
{
	if( dwID == ID_BUY_PESO_BTN )
	{
		int iPeso = m_kItemInfo.GetPeso( GetSelectPriceArray( m_dwSelectPriceBtnID ) );
		if( iPeso <= 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return CHECK_PRICE_ERROR;
		}

		if( g_MyInfo.GetMoney() < iPeso )
		{
			ShowNeedPesoMsg();
			return CHECK_PRICE_ERROR;
		}
	}
	else if( dwID == ID_BUY_STAMP_BTN )
	{
		int iStamp = m_kItemInfo.GetStamp( GetSelectPriceArray( m_dwSelectPriceBtnID ) );
		if( iStamp <= 0 )
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_dwStampEtcItem );
			if( pEtcItem )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8), pEtcItem->GetName().c_str() );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			return CHECK_PRICE_ERROR;
		}

		if( g_MyInfo.GetEtcCoinMoney( m_kItemInfo.m_dwStampEtcItem ) < iStamp )
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_dwStampEtcItem );
			if( pEtcItem )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9), pEtcItem->GetName().c_str() );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			return CHECK_PRICE_ERROR;
		}
	}
	else if( dwID == ID_BUY_CASH_BTN )
	{
		if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
			g_MyInfo.IsExpertEntryTemporary() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return CHECK_PRICE_ERROR;;
		}

		int iCash = m_kItemInfo.GetCash( GetSelectPriceArray( m_dwSelectPriceBtnID ) );
		if( iCash <= 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			return CHECK_PRICE_ERROR;;
		}

		if( g_MyInfo.GetTotalCash() < iCash )
		{
			if( ioChannelingNodeParent::IsBillingTestID( g_MyInfo.GetPublicID() ) )
			{
				LOG.PrintTimeAndLog( 0, "IsBillingTestID is true : %s", g_MyInfo.GetPublicID().c_str() );
			}
			else
			{
				g_GUIMgr.ShowWnd( BILLING_WANT_WND );
				return CHECK_PRICE_ERROR;;
			}
		}
	}
	else if( dwID == ID_BUY_PRESENT_BTN )
	{
		if( g_MyInfo.GetEntryType() == ET_TERMINATION || g_MyInfo.GetEntryType() == ET_TEMPORARY ||
			g_MyInfo.IsExpertEntryTemporary() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			return CHECK_PRICE_ERROR;;
		}

		int iCash = m_kItemInfo.GetCash( GetSelectPriceArray( m_dwSelectPriceBtnID ) );
		if( iCash <= 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			return CHECK_PRICE_ERROR;;
		}

		if( g_MyInfo.GetPurchasedCash() < iCash )
		{
			if( ioChannelingNodeParent::IsBillingTestID( g_MyInfo.GetPublicID() ) )
			{
				LOG.PrintTimeAndLog( 0, "IsBillingTestID is true : %s", g_MyInfo.GetPublicID().c_str() );
			}
			else
			{
				BillingWantWnd *pWnd = dynamic_cast< BillingWantWnd* > ( g_GUIMgr.FindWnd( BILLING_WANT_WND ) );
				if( pWnd )
					pWnd->ShowPresent();
				return CHECK_PRICE_ERROR;;
			}
		}
		else
			return CHECK_PRICE_PRESENT_OK;
	}

	return CHECK_PRICE_OK;
}

void NewShopBuyWnd::GetInvenDecoInfo( OUT int &riInvenDecoType, OUT int &rInvenDecoCode, IN int iClassType, IN int iDecoSexType, IN int iDecoType, IN int iDecoCode )
{
	//고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )
	riInvenDecoType = ( iClassType * 100000) + ( iDecoSexType * 1000);

	// 종족은 무조건 남성만 존재하므로 iDecoSexType 0 남자 , 1 여자 ( g_MyInfo.IsSlotItem()에서도 여자 종족 성별은 구분하지 않음 )
	if( iDecoSexType == 1 && iDecoType == UID_KINDRED ) 
		riInvenDecoType -= 1000; 

	rInvenDecoCode = 0;
	switch( iDecoType )
	{
	case UID_FACE:
	case UID_HAIR:
	case UID_SKIN_COLOR:
	case UID_HAIR_COLOR:
	case UID_UNDERWEAR:
	case UID_KINDRED:
		{
			riInvenDecoType += iDecoType;
			rInvenDecoCode  =  iDecoCode;
		}
		break;
	case UID_CLASS:
		{
			int iEquipType = iDecoCode / 1000;
			int iEquipCode = iDecoCode % 1000;
			riInvenDecoType += iEquipType;
			rInvenDecoCode  =  iEquipCode;
		}
		break;	
	}
}

void NewShopBuyWnd::ReSettingBtn( DWORD dwID, int iWidth, int iPosX, int iTitleX )
{
	ioWnd *pBtn = FindChildWnd( dwID );
	if( !pBtn )
		return;

	pBtn->SetSize( iWidth, pBtn->GetHeight() );
	pBtn->SetWndPos( iPosX, pBtn->GetYPos() );

	ioUITitle *pTitle = pBtn->GetTitle();
	if( !pTitle )
		return;

	pTitle->SetOffset( iTitleX , pTitle->GetTextYOffSet() );
}

void NewShopBuyWnd::SetPriceInfo()
{
	int i = 0;
	int iPriceInfoArray = 0;
	for (i = 0; i < MAX_PRICE ; i++)
	{
		m_kPriceInfoArray[i].Clear();
	}

	int iPesoSize = m_kItemInfo.m_vPeso.size();
	for (i = 0; i < iPesoSize ; i++)
	{
		if( m_kItemInfo.GetPeso( i ) == 0 )
			continue;

		char szPrice[MAX_PATH]="";
		Help::ConvertNumToStrComma( m_kItemInfo.GetPeso( i ), szPrice, sizeof( szPrice ) );
		StringCbCat( szPrice, sizeof( szPrice ), STR(1) );

		char szName[MAX_PATH]="";
		GetGoodsUseName( szName, sizeof( szName ), true, i );

		if( COMPARE( iPriceInfoArray, 0, MAX_PRICE ) )
		{
			m_kPriceInfoArray[iPriceInfoArray].m_bPeso        = true;
			m_kPriceInfoArray[iPriceInfoArray].m_iPriceArray  = i;
			m_kPriceInfoArray[iPriceInfoArray].m_sName        = szName;
			m_kPriceInfoArray[iPriceInfoArray].m_sPrice       = szPrice;
			m_kPriceInfoArray[iPriceInfoArray].m_iPrice       = m_kItemInfo.GetPeso( i );
			m_kPriceInfoArray[iPriceInfoArray].m_dwPriceColor = 0xFF1646B2;

			// 경험치+200 예외
			if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && m_kItemInfo.m_iValue1 == ioEtcItem::EIT_ETC_EXP_BONUS && i == 2 && m_kItemInfo.GetPeso( 2 ) > 0 )
			{
				m_kPriceInfoArray[iPriceInfoArray].m_bEvent   = true;
			}

			// 페소+100 예외
			if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM && m_kItemInfo.m_iValue1 == ioEtcItem::EIT_ETC_PESO_BONUS && i == 2 && m_kItemInfo.GetPeso( 2 ) > 0 )
			{
				m_kPriceInfoArray[iPriceInfoArray].m_bEvent   = true;
			}

			iPriceInfoArray++;
		}
	}

	int iCashSize = m_kItemInfo.m_vCash.size();
	for (i = 0; i < iCashSize ; i++)
	{
		if( m_kItemInfo.GetCash( i ) == 0 )
			continue;

		char szPrice[MAX_PATH]="";
		Help::ConvertNumToStrComma( m_kItemInfo.GetCash( i ), szPrice, sizeof( szPrice ) );
		StringCbCat( szPrice, sizeof( szPrice ), STR(2) );

		char szName[MAX_PATH]="";
		GetGoodsUseName( szName, sizeof( szName ), false, i );

		if( COMPARE( iPriceInfoArray, 0, MAX_PRICE ) )
		{
			m_kPriceInfoArray[iPriceInfoArray].m_bPeso        = false;
			m_kPriceInfoArray[iPriceInfoArray].m_iPriceArray  = i;
			m_kPriceInfoArray[iPriceInfoArray].m_sName       = szName;
			m_kPriceInfoArray[iPriceInfoArray].m_sPrice      = szPrice;
			m_kPriceInfoArray[iPriceInfoArray].m_iPrice      = m_kItemInfo.GetCash( i );
			m_kPriceInfoArray[iPriceInfoArray].m_dwPriceColor = 0xFF8d5704;
			if( m_kItemInfo.GetBonusPeso( i ) != 0 )
				m_kPriceInfoArray[iPriceInfoArray].m_bBonus   = true;
			if( m_kItemInfo.m_iShopMarkType == SHOP_MARK_TYPE_EVENT && ( i == ( iCashSize-1 ) ) ) // 가장 비싼 캐쉬 상품에만 event 표시 , ini에서 읽도록 처리해야 하지만 애매한 부분이 있어 이렇게 처리
				m_kPriceInfoArray[iPriceInfoArray].m_bEvent   = true;
			iPriceInfoArray++;
		}
	}

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_dwStampEtcItem );

	int iStampSize = m_kItemInfo.m_vStamp.size();
	for(i = 0;i < iStampSize;i++)
	{
		if( m_kItemInfo.GetStamp( i ) == 0 ) continue;
		
		char szComma[MAX_PATH] = "";
		Help::ConvertNumToStrComma( m_kItemInfo.GetStamp( i ), szComma, sizeof( szComma ) );

		char szPrice[MAX_PATH]="";
		if( pEtcItem )
			SafeSprintf( szPrice, sizeof(szPrice), STR(4), pEtcItem->GetName().c_str(), szComma );
		else
			SafeSprintf( szPrice, sizeof(szPrice), STR(3), szComma );

		char szName[MAX_PATH]="";
		GetGoodsUseName( szName, sizeof( szName ), false, i );

		if( COMPARE( iPriceInfoArray, 0, MAX_PRICE ) )
		{
			m_kPriceInfoArray[iPriceInfoArray].m_bPeso        = false;
			m_kPriceInfoArray[iPriceInfoArray].m_bStamp       = true;
			m_kPriceInfoArray[iPriceInfoArray].m_iPriceArray  = i;
			m_kPriceInfoArray[iPriceInfoArray].m_sName        = szName;
			m_kPriceInfoArray[iPriceInfoArray].m_sPrice       = szPrice;
			m_kPriceInfoArray[iPriceInfoArray].m_iPrice       = m_kItemInfo.GetStamp( i );
			m_kPriceInfoArray[iPriceInfoArray].m_dwPriceColor = 0xFF1646B2;
			iPriceInfoArray++;
		}
	}

	for (i = 0; i < MAX_PRICE ; i++)
	{
		if( i < iPriceInfoArray )
		{
			ShowChildWnd( ID_PRICE1_RBTN + i );
			m_dwLastPriceBtnID = ID_PRICE1_RBTN + i;
		}
		else
			HideChildWnd( ID_PRICE1_RBTN + i );
	}
}

void NewShopBuyWnd::GetGoodsUseName( OUT char *szName, IN int iNameSize, IN bool bPeso, IN int iPriceArray )
{
	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER  )
	{
		int iClassType = m_kItemInfo.m_iValue1;
		int iLimitDate = 0;
		if( bPeso )
			iLimitDate = g_ClassPrice.GetLimitDatePeso( iClassType, iPriceArray ); 
		else
			iLimitDate = g_ClassPrice.GetLimitDateCash( iClassType, iPriceArray ); 

		if( iLimitDate != 0 ) // 시간제
			SafeSprintf( szName, iNameSize, STR(1), iLimitDate/3600 );	
		else
			SafeSprintf( szName, iNameSize, STR(2) );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
	{
		SafeSprintf( szName , iNameSize, STR(3) );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		int iEtcType  = m_kItemInfo.m_iValue1;
		int iEtcArray = m_kItemInfo.GetValue3( iPriceArray ); 
		int iUseType = iEtcType/ioEtcItem::USE_TYPE_CUT_VALUE;

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iEtcType );
		if( !pEtcItem )
			return;
		int iValue = pEtcItem->GetValue( iEtcArray );

		if( iEtcType == ioEtcItem::EIT_ETC_FRIEND_SLOT_EXTEND )
		{
			SafeSprintf( szName , iNameSize, STR(4), iValue );
		}
		else if( iEtcType == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND ||
				 iEtcType == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND ||
				 iEtcType == ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND )
		{
			SafeSprintf( szName , iNameSize, STR(5), iValue );
		}
		else if( iUseType == ioEtcItem::UT_ETERNITY )
		{
			SafeSprintf( szName , iNameSize, STR(6) );
		}
		else if( iUseType == ioEtcItem::UT_TIME )
		{
			if( iValue > 3600 )
				SafeSprintf( szName , iNameSize, STR(7), iValue / 3600 );
			else
				SafeSprintf( szName , iNameSize, STR(8), iValue / 60 );
		}
		else if( iUseType == ioEtcItem::UT_COUNT || 
			     iUseType == ioEtcItem::UT_ONCE )
		{
			SafeSprintf( szName , iNameSize, STR(9), iValue );
		}
		else if( iUseType == ioEtcItem::UT_DATE )
		{
			if( g_EtcItemMgr.IsBlockEtcItem( iEtcType ) && iValue == 0 )
				SafeSprintf( szName , iNameSize, STR(13) );
			else
				SafeSprintf( szName , iNameSize, STR(10), iValue );
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX || m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		if( m_kItemInfo.GetValue3( iPriceArray ) == -1 )
			SafeSprintf( szName , iNameSize, STR(11) );
		else
		{
			bool bPeriod = false;
			int iDay = 0;
			if( bPeso )
			{
				bPeriod = true;
				iDay = m_kItemInfo.GetValue3( iPriceArray )/24;
			}
			else if( !bPeso && m_kItemInfo.GetValue3( 0 ) > 0 ) // 멀티페소사용중인 캐쉬
			{
				bPeriod = true;
				iDay = m_kItemInfo.GetValue3( m_kItemInfo.m_vValue3.size()-1 )/24; // 마지막이 캐쉬 날짜
				if( iDay == 0 )
					bPeriod = false;
			}

			if( bPeriod )
				SafeSprintf( szName , iNameSize, STR(12), iDay );
			else
			{
				SafeSprintf( szName , iNameSize, STR(14) );
			}
		}
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		SafeSprintf( szName, iNameSize, g_PresentMgr.GetPresentValue2Text( m_kItemInfo.m_iValue0, m_kItemInfo.m_iValue1, m_kItemInfo.m_iValue2 ).c_str() );
	}
}

void NewShopBuyWnd::SetPriceRadioBtn( DWORD dwBtnID )
{
	bool bRight = false;
	for (int i = 0; i < MAX_PRICE ; i++)
	{
		if( dwBtnID == ID_PRICE1_RBTN + i )
		{
			bRight = true;
			break;
		}
	}

	if( !bRight )
		return;

	m_dwSelectPriceBtnID = dwBtnID;
	CheckRadioButton( ID_PRICE1_RBTN, m_dwLastPriceBtnID, m_dwSelectPriceBtnID );
	bool bPeso = IsSelectPricePeso( m_dwSelectPriceBtnID );
	bool bStamp= IsSelectPriceStamp( m_dwSelectPriceBtnID );
	SetBuyBtns( bPeso, bStamp );
}


void NewShopBuyWnd::SetPriceRadioBtnByCash( int iCash )
{
	int iMax = m_kItemInfo.m_vCash.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( iCash == m_kItemInfo.GetCash( i ) )
		{
			for (int j = 0; j < MAX_PRICE ; j++)
			{
				if( i == m_kPriceInfoArray[j].m_iPriceArray	)
				{
					SetPriceRadioBtn( ID_PRICE1_RBTN + j );
					break;
				}
			}
			break;
		}
	}
}

void NewShopBuyWnd::SetBuyBtns( bool bPeso, bool bStamp )
{
	// button
	if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		if( bStamp )
		{
			ShowChildWnd( ID_BUY_STAMP_BTN );
			HideChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_GO_DECO_BTN );
		}
		else
		{
			HideChildWnd( ID_BUY_STAMP_BTN );
			HideChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_GO_DECO_BTN );
		}

		HideChildWnd( ID_BUY_GOODS_LIST_BTN );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
	{
		if( bPeso )
		{
			ShowChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_GO_DECO_BTN ); // 현재는 사용하지 않아 hide
			HideChildWnd( ID_BUY_STAMP_BTN );
		}
		else
		{
			HideChildWnd( ID_BUY_PESO_BTN );
			ShowChildWnd( ID_BUY_CASH_BTN );
			ShowChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_GO_DECO_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );
		}

		HideChildWnd( ID_BUY_GOODS_LIST_BTN );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_DECORATION )
	{
		if( bPeso )
		{
			ShowChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );
		}
		else
		{
			HideChildWnd( ID_BUY_PESO_BTN );
			ShowChildWnd( ID_BUY_CASH_BTN );
			ShowChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );
		}

		HideChildWnd( ID_BUY_GO_DECO_BTN );
		HideChildWnd( ID_BUY_GOODS_LIST_BTN );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		if( bPeso )
		{
			ShowChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );
			int iEtcItemType = m_kItemInfo.m_iValue1;
			if( iEtcItemType == ioEtcItem::EIT_ETC_RANDOM_DECO_M ||
				iEtcItemType == ioEtcItem::EIT_ETC_RANDOM_DECO_W )
			{
				ShowChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
			else
			{
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
		}
		else
		{
			HideChildWnd( ID_BUY_PESO_BTN );
			ShowChildWnd( ID_BUY_CASH_BTN );
			ShowChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );

			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_iValue1 );
			if ( pEtcItem )
			{
				if ( pEtcItem->IsOnlyBuyBtn() )
					HideChildWnd( ID_BUY_PRESENT_BTN );
				else if ( pEtcItem->IsOnlyGiftBtn() )
					HideChildWnd( ID_BUY_CASH_BTN );
			}
			
			int iEtcItemType = m_kItemInfo.m_iValue1;
			if( iEtcItemType == ioEtcItem::EIT_ETC_GASHAPON14 )
			{
				ShowChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
		}

		HideChildWnd( ID_BUY_GO_DECO_BTN );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		enum { EXTRAITEM_GROUP_PACKAGE = 20000, };
		if( bPeso )
		{
			ShowChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );

			if( m_kItemInfo.m_iValue2 == EXTRAITEM_GROUP_PACKAGE )
			{
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
			else if( m_kItemInfo.GetValue3( 0 ) == -1 )
			{
				ShowChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
			else // multi peso가 있으면 용병별 고급장비로 목록1개이므로 hide
			{
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
		}
		else
		{
			HideChildWnd( ID_BUY_PESO_BTN );
			ShowChildWnd( ID_BUY_CASH_BTN );
			ShowChildWnd( ID_BUY_PRESENT_BTN );
			HideChildWnd( ID_BUY_STAMP_BTN );


			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

			if( m_kItemInfo.m_iValue2 == EXTRAITEM_GROUP_PACKAGE )
			{
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
			else if( pLocal && pLocal->IsShowExtraGoodsList())
			{
				ShowChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
			else if( m_kItemInfo.GetValue3( 0 ) > 0 && m_kItemInfo.GetCash( 0 ) > 0  ) // 멀티 페소가 있는 캐쉬
			{
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			}
			else 
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
		}

		HideChildWnd( ID_BUY_GO_DECO_BTN );
	}
	else if( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		enum { COSTUME_GROUP_PACKAGE = 20000, };
		if( bPeso )
		{
			ShowChildWnd( ID_BUY_PESO_BTN );
			HideChildWnd( ID_BUY_CASH_BTN );
			HideChildWnd( ID_BUY_PRESENT_BTN );

			if( m_kItemInfo.m_iValue2 == COSTUME_GROUP_PACKAGE )
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			else if( m_kItemInfo.GetValue3( 0 ) == -1 )
				ShowChildWnd( ID_BUY_GOODS_LIST_BTN );
			else
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
		}
		else
		{
			HideChildWnd( ID_BUY_PESO_BTN );
			ShowChildWnd( ID_BUY_CASH_BTN );
			ShowChildWnd( ID_BUY_PRESENT_BTN );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

			if( m_kItemInfo.m_iValue2 == COSTUME_GROUP_PACKAGE )
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			else if( pLocal && pLocal->IsShowExtraGoodsList())
				ShowChildWnd( ID_BUY_GOODS_LIST_BTN );
			else if( m_kItemInfo.GetValue3( 0 ) > 0 && m_kItemInfo.GetCash( 0 ) > 0  )
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
			else 
				HideChildWnd( ID_BUY_GOODS_LIST_BTN );
		}

		HideChildWnd( ID_BUY_STAMP_BTN );
		HideChildWnd( ID_BUY_GO_DECO_BTN );
	}

	HideChildWnd( ID_BUY_ADD_BTN );
	HideChildWnd( ID_SHOW_BOUGHT_ITEM_BTN );
	HideChildWnd( ID_PRESENT_EVENT_BTN );

	if( !bPeso && !bStamp )
	{
		if( g_EventMgr.IsPresentBonus(EVT_PRESENT_BUY) )
		{
			if ( g_ShopManager.IsUnablePresentBonus() && !g_ShopManager.GetUnablePresentBonus( m_kItemInfo.m_iItemType, m_kItemInfo.m_iValue1 ) )
				ShowChildWnd( ID_PRESENT_EVENT_BTN );
		}
		
		if( g_EventMgr.IsPresentBonus(EVT_PRESENT_BUY_2) )
		{
			if ( g_ShopManager.IsEnablePresentBonus() && g_ShopManager.GetEnablePresentBonus( m_kItemInfo.m_iItemType, m_kItemInfo.m_iValue1 ) )
				ShowChildWnd( ID_PRESENT_EVENT_BTN );
		}
	}

	// 버튼 크기, 위치 조정
	enum { MAX_CHECK_BTN = 6, };
	int iBtnID[MAX_CHECK_BTN]={ ID_BUY_PESO_BTN, ID_BUY_CASH_BTN, ID_BUY_PRESENT_BTN, ID_BUY_GO_DECO_BTN, ID_BUY_GOODS_LIST_BTN, ID_BUY_STAMP_BTN };
	int iShowWndCnt = 0;
	for (int i = 0; i < MAX_CHECK_BTN ; i++)
	{
		ioWnd *pWnd = FindChildWnd( iBtnID[i] );
		if( !pWnd )
			continue;
		if( !pWnd->IsShow() )
			continue;
		iShowWndCnt++;
	}

	enum
	{ 
		ONE_BTN_WIDTH = 204, 
		TWO_BTN_WIDTH = 102, 
		THREE_BTN_WIDTH = 67,

		ONE_BTN_POS_X = 27,
		TWO_BTN_POS_X = 131,
		THREE_BTN_2_POS_X = 96,
		THREE_BTN_3_POS_X = 165,

		ONE_TITLE_X = 102, 
		TWO_TITLE_X = 51, 
		THREE_TITLE_X = 33,
	};

	vBtnInfo vBtnInfoVec;
	vBtnInfoVec.reserve(3);

	// 버튼 1개
	BtnInfo kBtnInfo;
	kBtnInfo.m_iBtnWidth[0]  = ONE_BTN_WIDTH;
	kBtnInfo.m_iBtnPosX[0]   = ONE_BTN_POS_X;
	kBtnInfo.m_iBtnTitleX[0] = ONE_TITLE_X;
	vBtnInfoVec.push_back( kBtnInfo );

	// 버튼 2개
	kBtnInfo.m_iBtnWidth[0] = TWO_BTN_WIDTH;
	kBtnInfo.m_iBtnPosX[0]  = ONE_BTN_POS_X;
	kBtnInfo.m_iBtnTitleX[0]= TWO_TITLE_X;
	kBtnInfo.m_iBtnWidth[1] = TWO_BTN_WIDTH;
	kBtnInfo.m_iBtnPosX[1]  = TWO_BTN_POS_X;
	kBtnInfo.m_iBtnTitleX[1]= TWO_TITLE_X;
	vBtnInfoVec.push_back( kBtnInfo );

	// 버튼 3개
	kBtnInfo.m_iBtnWidth[0] = THREE_BTN_WIDTH;
	kBtnInfo.m_iBtnPosX[0]  = ONE_BTN_POS_X;
	kBtnInfo.m_iBtnTitleX[0]= THREE_TITLE_X;
	kBtnInfo.m_iBtnWidth[1] = THREE_BTN_WIDTH;
	kBtnInfo.m_iBtnPosX[1]  = THREE_BTN_2_POS_X;
	kBtnInfo.m_iBtnTitleX[1]= THREE_TITLE_X;
	kBtnInfo.m_iBtnWidth[2] = THREE_BTN_WIDTH;
	kBtnInfo.m_iBtnPosX[2]  = THREE_BTN_3_POS_X;
	kBtnInfo.m_iBtnTitleX[2]= THREE_TITLE_X;
	vBtnInfoVec.push_back( kBtnInfo );

	int iVecSize = vBtnInfoVec.size();
	int iCurShowWndCnt = 0;
	for (int i = 0; i < MAX_CHECK_BTN ; i++)
	{
		ioWnd *pWnd = FindChildWnd( iBtnID[i] );
		if( !pWnd )
			continue;
		if( !pWnd->IsShow() )
			continue;
		if( iCurShowWndCnt >= iShowWndCnt )
			break;
		if( !COMPARE( iShowWndCnt-1, 0, iVecSize ) )
			break;
		if( !COMPARE( iCurShowWndCnt, 0, MAX_LOW_BTN_DATA ) )
			break;
		
		BtnInfo &kBtnInfo = vBtnInfoVec[iShowWndCnt-1];

		ReSettingBtn( pWnd->GetID(), kBtnInfo.m_iBtnWidth[iCurShowWndCnt], kBtnInfo.m_iBtnPosX[iCurShowWndCnt], kBtnInfo.m_iBtnTitleX[iCurShowWndCnt] );

		iCurShowWndCnt++;
	}
}

bool NewShopBuyWnd::IsSelectPricePeso( DWORD dwSelectPriceBtnID )
{
	int iArray = dwSelectPriceBtnID - ID_PRICE1_RBTN;
	if( !COMPARE( iArray, 0, MAX_PRICE ) )
		return false;

	return m_kPriceInfoArray[iArray].m_bPeso;
}

bool NewShopBuyWnd::IsSelectPriceStamp( DWORD dwSelectPriceBtnID )
{
	int iArray = dwSelectPriceBtnID - ID_PRICE1_RBTN;
	if( !COMPARE( iArray, 0, MAX_PRICE ) )
		return false;

	return m_kPriceInfoArray[iArray].m_bStamp;
}

int NewShopBuyWnd::GetSelectPriceArray( DWORD dwSelectPriceBtnID )
{
	int iArray = dwSelectPriceBtnID - ID_PRICE1_RBTN;
	if( !COMPARE( iArray, 0, MAX_PRICE ) )
		return 0;

	return m_kPriceInfoArray[iArray].m_iPriceArray;
}

void NewShopBuyWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( FRAMEGETTIME() - m_dwCurChangeMarkTime > m_dwChangeMarkDelay )
	{
		m_dwCurChangeMarkTime = FRAMEGETTIME();
		m_bChangeMark = !m_bChangeMark;
	}
}

void NewShopBuyWnd::InitCharRotate()
{
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->InitRotate();
	}
}

void NewShopBuyWnd::ShowOnlyBoughtBtns( bool bSubscription )
{
	ShowChildWnd( ID_BUY_ADD_BTN );
	ShowChildWnd( ID_SHOW_BOUGHT_ITEM_BTN );
	ShowChildWnd( ID_FLASH_WND );

	if( m_kItemInfo.m_iItemType == PRESENT_DEFAULT )
	{
		SetChildInActive( ID_BUY_ADD_BTN );		
	}
	else
	{
		SetChildActive( ID_BUY_ADD_BTN );
	}

	m_bSubscriptionResult = bSubscription;

	HideChildWnd( ID_BUY_PESO_BTN );
	HideChildWnd( ID_BUY_CASH_BTN );         
	HideChildWnd( ID_BUY_PRESENT_BTN );      
	HideChildWnd( ID_BUY_GO_DECO_BTN );      
	HideChildWnd( ID_MY_SOLDIER_LIST_BTN1 );  
	HideChildWnd( ID_MY_SOLDIER_LIST_BTN2 );  
	HideChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
	HideChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
	HideChildWnd( ID_ITEM_MOVIE_BTN );
	HideChildWnd( ID_BUY_GOODS_LIST_BTN );   
	HideChildWnd( ID_RECOMMEND_DECO_BTN ); 
	HideChildWnd( ID_BUY_STAMP_BTN );
		
	HideChildWnd( ID_PRICE1_RBTN );          
	HideChildWnd( ID_PRICE2_RBTN );          
	HideChildWnd( ID_PRICE3_RBTN );  
	HideChildWnd( ID_PRICE4_RBTN );  

	for (int i = 0; i < MAX_DECO_BTN ; i++)
	{
		HideChildWnd( ID_DECO_PREVIEW_KINDRED_BTN + i );
		HideChildWnd( ID_DECO_EQUIP_KINDRED_BTN + i );
	}

	if( g_ExSoundMgr.IsSound( ExSound::EST_SHOP_BUY_SUCCESS ) )
	{
		g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );
	}
	else
	{
		if( !m_sBuySound.IsEmpty() )
			g_SoundMgr.PlaySound( m_sBuySound );
	}
	

	HideChildWnd( ID_PRESENT_EVENT_BTN );  
}

void NewShopBuyWnd::SetBoughtBonusPeso()
{
	int iPriceArray = GetSelectPriceArray( m_dwSelectPriceBtnID );
	int iArray      = m_dwSelectPriceBtnID - ID_PRICE1_RBTN;
	if( COMPARE( iArray, 0, MAX_PRICE ) && m_kPriceInfoArray[iArray].m_bBonus )
		m_iBoughtBonusPeso = m_kItemInfo.GetBonusPeso( iPriceArray );
	else
		m_iBoughtBonusPeso = 0;
}

void NewShopBuyWnd::ShowNeedPesoMsg()
{
	NewShopNeedPesoWnd *pNeedPesoWnd = dynamic_cast<NewShopNeedPesoWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_NEED_PESO_WND ) );
	if( !pNeedPesoWnd )
		return;

	NewShopNeedPesoWnd::NeedPesoInfo kInfo;
	kInfo.m_sName = m_kItemInfo.m_sName;
	kInfo.m_sIcon = m_kItemInfo.m_sIcon;

	int iPeso = m_kItemInfo.GetPeso( GetSelectPriceArray( m_dwSelectPriceBtnID ) );
	char szPrice[MAX_PATH]="";
	Help::ConvertNumToStrComma( iPeso, szPrice, sizeof( szPrice ) );
	StringCbCat( szPrice, sizeof( szPrice ), STR(1) );
	kInfo.m_sPrice = szPrice;

	ZeroMemory( szPrice, sizeof( szPrice ) );
	char szTemp[MAX_PATH]="";
	int iNeedPeso = iPeso - g_MyInfo.GetMoney();
	Help::ConvertNumToStrComma( iNeedPeso, szTemp, sizeof( szTemp ) );
	SafeSprintf( szPrice, sizeof( szPrice ), STR(2), szTemp );
	kInfo.m_sMinusPrice = szPrice;
	if( pNeedPesoWnd->SetInfo( kInfo ) )
		pNeedPesoWnd->ShowWnd();
}

bool NewShopBuyWnd::QuestGuide( int iBuyValue1, int iBuyValue2, int iBuyValue3, int iBuyValue4, int iBuyValue5, float &rfXPos, float &rfYPos )
{
	if( m_kItemInfo.m_iItemType != iBuyValue1 ) return false;
	if( m_kItemInfo.m_iValue1 != iBuyValue2 ) return false;
	if( m_kItemInfo.m_iValue2 != iBuyValue3 ) return false;

	if( iBuyValue4 == -1 || iBuyValue5 == -1 )
	{
		// 가격이 정해져있지 않으면..
		ioWnd *pBuyPesoBtn = FindChildWnd( ID_BUY_PESO_BTN );
		if( pBuyPesoBtn && pBuyPesoBtn->IsShow() )
		{
			rfXPos = pBuyPesoBtn->GetDerivedPosX() + (pBuyPesoBtn->GetWidth() / 2);
			rfYPos = pBuyPesoBtn->GetDerivedPosY();
			return true;
		}

		ioWnd *pBuyCashBtn = FindChildWnd( ID_BUY_CASH_BTN );
		if( pBuyCashBtn && pBuyCashBtn->IsShow() )
		{
			rfXPos = pBuyCashBtn->GetDerivedPosX() + (pBuyCashBtn->GetWidth() / 2);
			rfYPos = pBuyCashBtn->GetDerivedPosY();
			return true;
		}
	}
	else if( m_dwSelectPriceBtnID != iBuyValue4 )
	{
		ioWnd *pPriceBtn = FindChildWnd( iBuyValue4 );
		if( pPriceBtn )
		{
			rfXPos = pPriceBtn->GetDerivedPosX() + (pPriceBtn->GetWidth() / 2);
			rfYPos = pPriceBtn->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		ioWnd *pBuyBtn = FindChildWnd( iBuyValue5 );
		if( pBuyBtn )
		{
			rfXPos = pBuyBtn->GetDerivedPosX() + (pBuyBtn->GetWidth() / 2);
			rfYPos = pBuyBtn->GetDerivedPosY();
			return true;
		}
	}
	return false;
}

void NewShopBuyWnd::GetMyClassTypeVec( OUT IntVec &rvClassTypeVec )
{
	rvClassTypeVec.clear();

	int iMaxClass =  g_ClassPrice.MaxClassPrice();
	for (int i = 0; i <  iMaxClass; i++)
	{
		int iCharArray = g_MyInfo.GetCharSlotIndexToArray( i );
		if( iCharArray == -1 ) continue;
		if( g_MyInfo.IsCharExercise( iCharArray ) ) continue;       

		int iSetIdx = g_MyInfo.GetClassType( iCharArray ) - 1;
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo ) continue;

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
		rvClassTypeVec.push_back( iClassType );
	}
}

void NewShopBuyWnd::SetDecoEquipBtns()
{
	int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		for (int i = 0; i < MAX_DECO_BTN ; i++)
		{
			DWORD dwID = ID_DECO_EQUIP_KINDRED_BTN + i;
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
			if( !pBtn )
				continue;
			pBtn->SetIcon( NULL, NEW_SHOP_BUY_ICON_BTN_SCALE , NEW_SHOP_BUY_ICON_BTN_SCALE );
			pBtn->SetSelect( false );
		}
		return;
	}

	CHARACTER kCharInfo;
	kCharInfo.m_class_type = m_CharInfo.m_class_type;
	g_MyInfo.GetEquipItemCode( kCharInfo );
	
	int iDecoTypeArray[MAX_DECO_BTN]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	for (int i = 0; i < MAX_DECO_BTN ; i++)
	{
		DWORD dwID = ID_DECO_EQUIP_KINDRED_BTN + i;
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
		if( !pBtn )
			continue;

		pBtn->ShowWnd();

		if( dwID == ID_DECO_EQUIP_KINDRED_BTN )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName(  g_MyInfo.GetMySoldierIconName( kCharInfo.m_class_type )  ), NEW_SHOP_BUY_ICON_BTN_SCALE , NEW_SHOP_BUY_ICON_BTN_SCALE ); 
			pBtn->SetSubIcon( g_UIImageSetMgr.CreateImageByFullName(  g_MyInfo.GetSoldierSubIconName( kCharInfo.m_class_type )  ) ); 
		}
		else
		{
			int iDecoType = iDecoTypeArray[i];
			int iDecoCode = 0;
			
			if( dwID == ID_DECO_EQUIP_HAIR_BTN )
				iDecoCode = kCharInfo.m_hair;
			else if( dwID == ID_DECO_EQUIP_HAIRCOLOR_BTN )
				iDecoCode = kCharInfo.m_hair_color;
			else if( dwID == ID_DECO_EQUIP_FACE_BTN )
				iDecoCode = kCharInfo.m_face;
			else if( dwID == ID_DECO_EQUIP_SKINCOLOR_BTN )
				iDecoCode = kCharInfo.m_skin_color;
			else if( dwID == ID_DECO_EQUIP_UNDERWEAR_BTN )
				iDecoCode = kCharInfo.m_underwear;

			const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( kCharInfo.m_sex - 1, iDecoType, iDecoCode );
			if( pData )
			{
				ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
				pBtn->SetIcon( pImg, NEW_SHOP_BUY_ICON_BTN_SCALE , NEW_SHOP_BUY_ICON_BTN_SCALE );
			}
		}
	}
}

void NewShopBuyWnd::SetDecoPreviewBtn( const NewShopItemInfo &rkNewShopItemInfo, bool bInit )
{
	if( bInit )
	{
		for (int i = 0; i < MAX_DECO_BTN ; i++)
		{
			DWORD dwID = ID_DECO_PREVIEW_KINDRED_BTN + i;
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
			if( !pBtn )
				continue;
			pBtn->SetIcon( NULL, NEW_SHOP_BUY_ICON_BTN_SCALE , NEW_SHOP_BUY_ICON_BTN_SCALE );
			pBtn->SetSelect( false );
			pBtn->SetInActive();
		}
	}

	int iDecoType = rkNewShopItemInfo.m_iValue1;

	int iDecoTypeArray[MAX_DECO_BTN]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	for (int i = 0; i < MAX_DECO_BTN ; i++)
	{
		DWORD dwID = ID_DECO_PREVIEW_KINDRED_BTN + i;
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
		if( !pBtn )
			continue;
		if( iDecoType != iDecoTypeArray[i] )
		{
			pBtn->SetSelect( false );
			continue;
		}

		pBtn->SetActive();
		pBtn->ShowWnd();
		pBtn->SetSelect( true );
		pBtn->SetNewShopItemInfo( rkNewShopItemInfo );

		int iDecoType = iDecoTypeArray[i];
		int iDecoCode = 0;

		if( dwID == ID_DECO_PREVIEW_KINDRED_BTN )
			iDecoCode = 1;
		else if( dwID == ID_DECO_PREVIEW_HAIR_BTN )
			iDecoCode = m_CharInfo.m_hair;
		else if( dwID == ID_DECO_PREVIEW_HAIRCOLOR_BTN )
			iDecoCode = m_CharInfo.m_hair_color;
		else if( dwID == ID_DECO_PREVIEW_FACE_BTN )
			iDecoCode = m_CharInfo.m_face;
		else if( dwID == ID_DECO_PREVIEW_SKINCOLOR_BTN )
			iDecoCode = m_CharInfo.m_skin_color;
		else if( dwID == ID_DECO_PREVIEW_UNDERWEAR_BTN )
			iDecoCode = m_CharInfo.m_underwear;

		const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( m_CharInfo.m_sex - 1, iDecoType, iDecoCode );
		if( pData )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
			pBtn->SetIcon( pImg, NEW_SHOP_BUY_ICON_BTN_SCALE , NEW_SHOP_BUY_ICON_BTN_SCALE );
		}
	}
}

void NewShopBuyWnd::ReleaseDecoPreviewBtn( int iDecoType )
{
	int iDecoTypeArray[MAX_DECO_BTN]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	for (int i = 0; i < MAX_DECO_BTN ; i++)
	{
		DWORD dwID = ID_DECO_PREVIEW_KINDRED_BTN + i;
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
		if( !pBtn )
			continue;
		if( iDecoType != iDecoTypeArray[i] )
			continue;
		pBtn->SetIcon( NULL, NEW_SHOP_BUY_ICON_BTN_SCALE , NEW_SHOP_BUY_ICON_BTN_SCALE );
		pBtn->SetSelect( false );
		pBtn->SetInActive();
	}
}

void NewShopBuyWnd::SetDecoPreviewBtnUp( DWORD dwID )
{
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
	if( !pBtn )
		return;

	NewShopItemInfo kInfo;

	pBtn->GetNewShopItemInfo( kInfo );
	if( kInfo.m_sName.IsEmpty() )
		return;

	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
	if( !pShopWnd )
		return;

	int iSmallTabID = NewShopWnd::ID_SMALL_TAB_1_BTN;
	if( kInfo.m_iValue1 == UID_KINDRED )
		iSmallTabID = NewShopWnd::ID_SMALL_TAB_2_BTN;
	else if( kInfo.m_iValue1 == UID_HAIR )
		iSmallTabID = NewShopWnd::ID_SMALL_TAB_3_BTN;
	else if( kInfo.m_iValue1 == UID_HAIR_COLOR )
		iSmallTabID = NewShopWnd::ID_SMALL_TAB_4_BTN;
	else if( kInfo.m_iValue1 == UID_FACE )
		iSmallTabID = NewShopWnd::ID_SMALL_TAB_5_BTN;
	else if( kInfo.m_iValue1 == UID_SKIN_COLOR )
		iSmallTabID = NewShopWnd::ID_SMALL_TAB_6_BTN;
	else if( kInfo.m_iValue1 == UID_UNDERWEAR )
		iSmallTabID = NewShopWnd::ID_SMALL_TAB_7_BTN;
	
	pShopWnd->SetDirectSmallTab( NewShopWnd::ID_DECO_TAB_BTN, iSmallTabID, kInfo.m_iValue1, kInfo.m_iValue2, false );
}

void NewShopBuyWnd::UISoldierSelectWndBtnUp( int iSoldierArray )
{
	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
	if( !pShopWnd )
		return;
	
	int iClassType = g_MyInfo.GetClassType( iSoldierArray );
	if( iClassType == -1 )
		return;

	if( m_kItemInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		// 모션 아이템은 치장 탭에서 판매하므로 예외처리
		if( COMPARE( m_kItemInfo.m_iValue1, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
			COMPARE( m_kItemInfo.m_iValue1, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
		{
			pShopWnd->SetDecoClassSex( iClassType, 0, true ); // 0 : 남자
			pShopWnd->UpdateTab( pShopWnd->GetCurrTab(), true );
			pShopWnd->UpdateTabBtnUpAfter( pShopWnd->GetCurrTab() );
		}
		else
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_kItemInfo.m_iValue1 );
			if( pEtcItem && pEtcItem->IsShop3DChar() )
			{
				pShopWnd->SetDecoClassSex( iClassType, 0, true ); // 0 : 남자
				SetInfo( m_kItemInfo );    // Reload
			}
		}
	}
	else if ( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX || m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		int iCurrPage		= pShopWnd->GetCurrPage();
		int iCurrInfo		= pShopWnd->GetCurrInfoBtn();
		int nCurrSmallTab	= pShopWnd->GetCurrSmallTab();

		pShopWnd->SetDecoClassSex( iClassType, 0, true ); // 0 : 남자
		pShopWnd->UpdateTab( pShopWnd->GetCurrTab(), true, nCurrSmallTab );
		pShopWnd->UpdateTabBtnUpAfter( pShopWnd->GetCurrTab(), iCurrPage, iCurrInfo );
	}
	else
	{
		int iCurrPage		= pShopWnd->GetCurrPage();
		int iCurrInfo		= pShopWnd->GetCurrInfoBtn();
		int iSexType		= pShopWnd->GetDecoSexType();
		int nCurrSmallTab	= pShopWnd->GetCurrSmallTab();

		if( pShopWnd->GetCurrTab() == NewShopWnd::ID_RECOMMEND_TAB_BTN )
		{
			pShopWnd->SetDecoClassSexByRecommendedOrder( iSexType, iClassType );
			pShopWnd->UpdateTab( pShopWnd->GetCurrTab(), true );
			pShopWnd->UpdateTabBtnUpAfter( pShopWnd->GetCurrTab(), iCurrPage, iCurrInfo );
		}
		else
		{
			pShopWnd->SetDecoClassSex( iClassType, 0, true ); // 0 : 남자
			pShopWnd->UpdateTab( pShopWnd->GetCurrTab(), true, nCurrSmallTab );
			pShopWnd->UpdateTabBtnUpAfter( pShopWnd->GetCurrTab(), iCurrPage, iCurrInfo );
		}
	}
}

void NewShopBuyWnd::SetDecoChangeSexTypeBtnUp()
{
	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
	if( !pShopWnd )
		return;

	int iCurSexType = m_kItemInfo.GetValue3(0);
	int iSetSexType = 1; // 여자
	if( iCurSexType == 1 )
		iSetSexType = 0; // 남자

	int iCurrPage		= pShopWnd->GetCurrPage();
	int iCurrInfo		= pShopWnd->GetCurrInfoBtn();
	int iClassType		= pShopWnd->GetDecoClassType();
	int nCurrSmallTab	= pShopWnd->GetCurrSmallTab();

	if( pShopWnd->GetCurrTab() == NewShopWnd::ID_RECOMMEND_TAB_BTN )
	{
		pShopWnd->SetDecoClassSexByRecommendedOrder( iSetSexType, iClassType );
		pShopWnd->UpdateTab( pShopWnd->GetCurrTab(), true, nCurrSmallTab );
		pShopWnd->UpdateTabBtnUpAfter( pShopWnd->GetCurrTab(), iCurrPage, iCurrInfo );
	}
	else
	{
		pShopWnd->SetDecoClassSex( m_CharInfo.m_class_type, iSetSexType, false );
		pShopWnd->UpdateTab( pShopWnd->GetCurrTab(), true, nCurrSmallTab );	
		pShopWnd->UpdateTabBtnUpAfter( pShopWnd->GetCurrTab(), iCurrPage, iCurrInfo );
	}
}

void NewShopBuyWnd::SetRecommendDecoBtnUp()
{
	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
	if( !pShopWnd )
		return;

	pShopWnd->SetDecoClassSex( m_CharInfo.m_class_type, 1, false ); // 1 : 여자
	
	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( m_CharInfo.m_class_type - 1 );
	if( !pInfo ) 
		return;

	int iDecoTypeArray[MAX_DECO_BTN]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	for (int i = MAX_DECO_BTN - 1; i >= 0 ; i--)
	{
		int iDefaultCode = pInfo->GetPackageDefaultDecoCode( iDecoTypeArray[i], RDT_HUMAN_WOMAN );
		if( iDecoTypeArray[i] == UID_HAIR       ||
			iDecoTypeArray[i] == UID_HAIR_COLOR || 
			iDecoTypeArray[i] == UID_FACE       || 
			iDecoTypeArray[i] == UID_SKIN_COLOR ||
			iDecoTypeArray[i] == UID_UNDERWEAR )
			pShopWnd->SetDirectTab( NewShopWnd::ID_DECO_TAB_BTN, iDecoTypeArray[i], iDefaultCode, false );
	}

	//여성 성별 선택
	pShopWnd->SetDirectTab( NewShopWnd::ID_DECO_TAB_BTN, 5, 1, false );
}

void NewShopBuyWnd::SetDecoChangeSexTypeTitle()
{
	int iCurSexType = m_kItemInfo.GetValue3(0);
	ioWnd *pBtn = FindChildWnd( ID_DECO_CHANGE_SEXTYPE_BTN );
	if( !pBtn )	
		return;
	
	if( iCurSexType == 0 )
		pBtn->SetTitleText( STR(1) );
	else
		pBtn->SetTitleText( STR(2) );
}

void NewShopBuyWnd::SetCharInfoByPreview( OUT CHARACTER &rkCharInfo )
{
	int iDecoTypeArray[MAX_DECO_BTN]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	for (int i = 0; i < MAX_DECO_BTN ; i++)
	{
		DWORD dwID = ID_DECO_PREVIEW_KINDRED_BTN + i;
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( dwID ) );
		if( !pBtn )
			continue;
		if( !pBtn->IsShow() )
			continue;
		if( pBtn->IsDisabled() )
			continue;

		NewShopItemInfo kNewShopItemInfo;
		pBtn->GetNewShopItemInfo( kNewShopItemInfo );
		if( kNewShopItemInfo.GetValue3( 0 ) != ( rkCharInfo.m_sex - 1 ) )
			continue;

		if( dwID == ID_DECO_PREVIEW_HAIR_BTN )
			m_CharInfo.m_hair       = kNewShopItemInfo.m_iValue2;
		else if( dwID == ID_DECO_PREVIEW_HAIRCOLOR_BTN )
			m_CharInfo.m_hair_color = kNewShopItemInfo.m_iValue2;
		else if( dwID == ID_DECO_PREVIEW_FACE_BTN )
			m_CharInfo.m_face       = kNewShopItemInfo.m_iValue2;
		else if( dwID == ID_DECO_PREVIEW_SKINCOLOR_BTN )
			m_CharInfo.m_skin_color = kNewShopItemInfo.m_iValue2;
		else if( dwID == ID_DECO_PREVIEW_UNDERWEAR_BTN )
			m_CharInfo.m_underwear  = kNewShopItemInfo.m_iValue2;
	}
}

void NewShopBuyWnd::SetChildExActive( DWORD dwID )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
		pChild->AddWndStyle( IWS_EXACTIVE );
}

void NewShopBuyWnd::SetChildExInActive( DWORD dwID )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
		pChild->RemoveWndStyle( IWS_EXACTIVE );
}

bool NewShopBuyWnd::EnableDecoChangeByEquipItem( CHARACTER &rkCharInfo )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return true;

	for( int i=0; i < 4; ++i )
	{
		EXTRAITEMSLOT kSlot;
		int iExtraItemIndex = rkCharInfo.m_extra_item[i];

		if( pExtraItem->GetExtraItem( iExtraItemIndex, kSlot ) )
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
			if( pItem && pItem->GetCharOnlySexType() != ioSetItemInfo::CST_ALL )
				return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
NewShopBuyConfirmWnd::NewShopBuyConfirmWnd()
{
	m_pReturnWnd = NULL;

	m_iSubscriptionType = SUBSCRIPTION_NONE;
}

NewShopBuyConfirmWnd::~NewShopBuyConfirmWnd()
{
	ClearList();
	m_iSubscriptionType = SUBSCRIPTION_NONE;
}

void NewShopBuyConfirmWnd::ClearList()
{
	for( int i=0; i<MAX_MSG_LIST_SIZE; i++ )
	{
		m_PrinterList[i].ClearList();
	}
}

void NewShopBuyConfirmWnd::SetPrintMessage( ioWnd *pReturnWnd, ioComplexStringPrinter *MsgPrinter, int iType )
{
	m_pReturnWnd = pReturnWnd;
	m_iSubscriptionType = iType;

	ClearList();

	int i = 0;
	for(i = 0;i < MAX_MSG_LIST_SIZE;i++)
	{
		if( MsgPrinter[i].IsEmpty() )
			break;

		MsgPrinter[i].CopyFullText( m_PrinterList[i] );
	}

	int iMinWidth  = GetWidth();
	int iMinHeight = 0;
	int iMaxLine   = 0;
	for(i = 0;i < MAX_MSG_LIST_SIZE;i++)
	{
		if( m_PrinterList[i].IsEmpty() )
			break;

		int iCurLineWidth = m_PrinterList[i].GetFullWidth() + 34;
		if( iMinWidth < iCurLineWidth )
			iMinWidth = iCurLineWidth;
		iMaxLine++;
	}
	iMinHeight += ( 20 * iMaxLine );


	SetSize( iMinWidth, iMinHeight + NORMAL_HEIGHT );
	ioWnd *pClauseBtn = FindChildWnd( ID_CLAUSE_VIEW_BTN );
	if( pClauseBtn )
	{
		pClauseBtn->SetWndPos( pClauseBtn->GetXPos(), GetHeight() - 96 );
		pClauseBtn->ShowWnd();
	}

	SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );

	// 하단바
	if( m_pMultiplyFrm )
	{
		m_pMultiplyFrm->SetSize( iMinWidth, 65 );
		m_pMultiplyFrm->SetPosition( 0, GetHeight() - m_pMultiplyFrm->GetHeight() );
	}

	// 하단 버튼
	ioWnd *pOkBtn = FindChildWnd( ID_OK_BTN );
	if( pOkBtn )
	{
		pOkBtn->SetWndPos( pOkBtn->GetXPos(), GetHeight() - 48 );
	}

	ioWnd *pCancelBtn = FindChildWnd( ID_CANCEL_BTN );
	if( pCancelBtn )
	{
		pCancelBtn->SetWndPos( pCancelBtn->GetXPos(), GetHeight() - 48 );
	}

	ShowWnd();
}

bool NewShopBuyConfirmWnd::iwm_spacebar()
{
	ioWnd *pOkBtn = FindChildWnd( ID_OK_BTN );
	if( !pOkBtn ) return false;
	if( !pOkBtn->IsShow() ) return false;

	iwm_command( pOkBtn, IOBN_BTNUP, 0 );
	return true;
}

void NewShopBuyConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_OK_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendBuyFromReturnWnd();
			HideWnd();
		}
		break;
	case ID_CLAUSE_VIEW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( CLAUSE_VIEW_WND );
		}
		break;
	}
}

void NewShopBuyConfirmWnd::SendBuyFromReturnWnd()
{
	if( !m_pReturnWnd ) return;

	if( m_pReturnWnd->GetID() == NewShopWnd::ID_BUY_WND )
	{
		NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( m_pReturnWnd );
		if( pNewShopBuyWnd )
			pNewShopBuyWnd->SendBuyProcess();
	}
	else if( m_pReturnWnd->GetID() == SELECT_EXTRA_GASHAPON_WND )
	{
		SelectExtraGashaponWnd *pSelectExtraGashaponWnd = dynamic_cast<SelectExtraGashaponWnd*> ( m_pReturnWnd );
		if( pSelectExtraGashaponWnd )
			pSelectExtraGashaponWnd->SendBuy();
	}
	else if( m_pReturnWnd->GetID() == SELECT_GASHAPON_WND )
	{
		SelectGashaponWnd* pWnd = dynamic_cast<SelectGashaponWnd*> ( m_pReturnWnd );
		if( pWnd )
			pWnd->SendBuy();
	}
}

void NewShopBuyConfirmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + GetWidth() / 2;
	int iYPos = GetDerivedPosY() + 53;

	for( int i=0 ; i<MAX_MSG_LIST_SIZE ; i++ )
	{
		if( m_PrinterList[i].IsEmpty() )
			break;

		m_PrinterList[i].PrintFullText( iXPos, iYPos + 20 * i, TAT_CENTER );
	}
}

void NewShopBuyWnd::SetEquipChangeSexTypeTitle( int nType )
{	
	ioWnd *pBtn = FindChildWnd( ID_EQUIP_CHANGE_SEXTYPE_BTN );
	if( !pBtn )	
		return;

	if ( nType == PRESENT_EXTRAITEM_BOX )
	{
		if( !m_bEquipChangeFemale )
			pBtn->SetTitleText( STR(1) );
		else
			pBtn->SetTitleText( STR(2) );
	}
	else if ( nType == PRESENT_COSTUME_BOX )
	{
		if( !m_bEquipChangeFemale )
			pBtn->SetTitleText( "여성코스튬보기" );
		else
			pBtn->SetTitleText( "남성코스튬보기" );
	}
}

void NewShopBuyWnd::SetEquipChangeSexTypeBtnUp()
{
	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*> ( GetParent() );
	if( !pShopWnd )
		return;

	m_bEquipChangeFemale = !m_bEquipChangeFemale;

	if ( m_bEquipChangeFemale )
		m_nEquipChangeSex = 1;
	else
		m_nEquipChangeSex = 0;

	SetEquipChangeSexTypeTitle( m_kItemInfo.m_iItemType );

	int iCharArray = g_MyInfo.GetClassArray( pShopWnd->GetDecoClassType() );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) && m_nEquipChangeSex == (g_MyInfo.GetCharacter( iCharArray ).m_sex-1) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( m_CharInfo, pShopWnd->GetDecoClassType(), m_nEquipChangeSex );

	if ( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		RandomMachineInfo *pItemInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( m_kItemInfo.m_iValue1 );
		if ( pItemInfo )
		{
			DWORD dwItemCode = pItemInfo->GetItemCode( 0 );
			if ( dwItemCode != -1 )
			{
				NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
				if( pCharWnd )
				{
					pCharWnd->SetChar( m_CharInfo );
					pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
					pCharWnd->SetEquip( false, false, false );
					pCharWnd->EquipItem( dwItemCode, 0, 0 );
					pCharWnd->ShowWnd();
					pCharWnd->InitRotate();
				}
			}
		}
	}
	// UI코스튬 관련 (상점 미리보기 성별 설정)
	else if ( m_kItemInfo.m_iItemType == PRESENT_COSTUME_BOX )
	{
		CostumeShopInfo *pItemInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( m_kItemInfo.m_iValue1 );
		if ( !pItemInfo )
			return;

		CostumeShopSubInfo sCostumeShopSubInfo;
		if ( !pItemInfo->GetCostumeSubInfo( 0, sCostumeShopSubInfo ) )
			return;

		int nSubType = sCostumeShopSubInfo.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE;
		if ( !COMPARE( nSubType, 0, MAX_INVENTORY ) )
			return;

		CostumeInfo sCostumeInfo;
		if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeShopSubInfo.m_nCostumeCode, sCostumeInfo ) )
			return;

		for ( int i=0; i<MAX_INVENTORY; i++ )
			m_CharInfo.m_EquipCostume[i] = g_MyInfo.GetCharacter( iCharArray ).m_EquipCostume[i];

		m_CharInfo.m_EquipCostume[nSubType].Init();
		m_CharInfo.m_EquipCostume[nSubType].m_CostumeCode = sCostumeShopSubInfo.m_nCostumeCode;
		
		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
		{
			pCharWnd->SetChar( m_CharInfo );
			pCharWnd->SetCharScale( SHOP_HELP_CHAR_SCALE );
			pCharWnd->SetEquip( false, false, false );
			pCharWnd->ShowWnd();
			pCharWnd->InitRotate();
		}
	}
}

void NewShopBuyWnd::SetMovieBtnFunc()
{
	int nItemType, nItemValue, nItemCode;

	nItemType = m_kItemInfo.m_iItemType;
	nItemValue = m_kItemInfo.m_iValue1;

	if( m_kItemInfo.m_iItemType == PRESENT_SOLDIER )
		nItemCode = nItemValue;
	else if( m_kItemInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
	{
		RandomMachineInfo *pItemInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( m_kItemInfo.m_iValue1 );
		if ( !pItemInfo )
			return;
		
		nItemCode = pItemInfo->GetItemCode( 0 );		
	}

	ShopItemMovieWnd* pWnd = dynamic_cast<ShopItemMovieWnd*>(g_GUIMgr.FindWnd( SHOP_ITEM_MOVIE_WND ));
	if ( pWnd )
		pWnd->ShowWnd();

	ioShopItemExtraInfoManager::ShopItemExtraInfo* pShopExtraInfo = g_ShopItemExtraInfoMgr.FindExtraInfo( nItemType, nItemValue, nItemCode );
	if ( pShopExtraInfo )
	{
		ioShopItemExtraInfoManager::ShopItemMovieInfoList vShopItemMovieInfoList = pShopExtraInfo->m_ShopItemMovieInfoList;
		for ( int i=0; i< (int)vShopItemMovieInfoList.size(); ++i )
		{
			ioShopItemExtraInfoManager::ShopItemMovieInfo sShopItemMovieInfo = vShopItemMovieInfoList[i];
			SetMovieFile( sShopItemMovieInfo.m_szMovieDir, sShopItemMovieInfo.m_szMovieFile, sShopItemMovieInfo.m_szMovieUrl, sShopItemMovieInfo.m_bAfterProcess );
		}
	}
}

void NewShopBuyWnd::SetMovieFile( const ioHashString& szFlashDir, const ioHashString& szFlashFile, const ioHashString& szUrl, bool bAfterProcess )
{
	char szBuf[MAX_PATH];
	sprintf( szBuf, "%s/%s%s", g_FlashDownLoadMgr.GetDefaultDir().c_str(), szFlashDir.c_str(), szFlashFile.c_str() );

	ShopItemMovieWnd* pWnd = dynamic_cast<ShopItemMovieWnd*>(g_GUIMgr.FindWnd( SHOP_ITEM_MOVIE_WND ));
	if ( pWnd )
	{
		if ( m_bFlashDown )
		{
			if ( g_FlashDownLoadMgr.IsExistenceFile( szFlashDir, szFlashFile, szUrl, ioFlashDownLoadMgr::FDEP_ShopItemMovieWnd, bAfterProcess ) )
			{
				pWnd->SetResetFlash();
				pWnd->SetLoadFlash( szBuf );
			}
		}
		else
		{
			if ( bAfterProcess )
			{
				pWnd->SetResetFlash();
				pWnd->SetLoadFlash( szBuf );
			}
		}
	}
}