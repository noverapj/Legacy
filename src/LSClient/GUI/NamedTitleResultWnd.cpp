#include "StdAfx.h"

#include "NamedTitleResultWnd.h"

#include "NamedTitleSelectWnd.h"

NamedTitleResultWnd::NamedTitleResultWnd(void)
{
	m_pIcon = NULL;
	m_pBackEffect = NULL;
	
	for ( int i=0; i<MAXLINE; i++ )
		m_Desc[i].ClearList();
}

NamedTitleResultWnd::~NamedTitleResultWnd(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pBackEffect);
}

void NamedTitleResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			NamedTitleSelectWnd *pWnd = dynamic_cast<NamedTitleSelectWnd*>( g_GUIMgr.FindWnd( NAMED_TITLE_SELECT_WND ) );
			if( pWnd )
				pWnd->ShowSelectWnd();
		}
		break;
	}
}

void NamedTitleResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	// Back
	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	
	// Icon
	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	
	// Desc
	if( !m_Desc[0].IsEmpty() )
		m_Desc[0].PrintFullTextWidthCut( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER, 180 );

	if( !m_Desc[1].IsEmpty() )
		m_Desc[1].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y, TAT_LEFT );

	if( !m_Desc[2].IsEmpty() )
		m_Desc[2].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+18, TAT_LEFT );

	if( !m_Desc[3].IsEmpty() )
		m_Desc[3].PrintFullText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, TAT_CENTER );
}

////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleResultWnd::ShowResult( DWORD dwCode, int nLevel, BYTE byPremium )
{
	SAFEDELETE(m_pIcon);

	for ( int i=0; i<MAXLINE; i++ )
		m_Desc[i].ClearList();

	NamedTitleInfo sNamedTitleInfo;
	if ( !g_NamedTitleInfoMgr.GetNamedTitleInfo( dwCode, sNamedTitleInfo ) )
		return;
		
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( g_NamedTitleInfoMgr.GetNamedTitleIcon() );

	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( 0, 0, 0 );	
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_17, "칭호:" );

	m_Desc[0].SetTextColor( g_NamedTitleInfoMgr.GetNamedTitleTextColor( dwCode, nLevel, byPremium, false ) );
	m_Desc[0].AddTextPiece( FONT_SIZE_17, "%s", sNamedTitleInfo.m_szName.c_str() );

	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( 0, 0, 0 );	
	m_Desc[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[1].AddTextPiece( FONT_SIZE_12, "★ 해당 상품은 " );

	m_Desc[1].SetTextColor( TCT_DEFAULT_RED );
	m_Desc[1].AddTextPiece( FONT_SIZE_12, "내정보창" );

	m_Desc[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[1].AddTextPiece( FONT_SIZE_12, "에 칭호 버튼을" );

	m_Desc[2].SetTextStyle( TS_NORMAL );
	m_Desc[2].SetBkColor( 0, 0, 0 );	
	m_Desc[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[2].AddTextPiece( FONT_SIZE_12, "클릭하시면 확인하실 수 있습니다." );

	m_Desc[3].SetTextStyle( TS_NORMAL );
	m_Desc[3].SetBkColor( 0, 0, 0 );	
	m_Desc[3].SetTextColor( TCT_DEFAULT_GREEN );
	m_Desc[3].AddTextPiece( FONT_SIZE_17, "적용되었습니다." );

	ShowWnd();
}

////////////////////////////////////////////////////////////////////////////////////////////////

NamedTitlePremiumResultWnd::NamedTitlePremiumResultWnd(void)
{
	m_pIcon = NULL;
	m_pBackEffect = NULL;
	
	for ( int i=0; i<MAXLINE; i++ )
		m_Desc[i].ClearList();
}

NamedTitlePremiumResultWnd::~NamedTitlePremiumResultWnd(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pBackEffect);
}

void NamedTitlePremiumResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			NamedTitleSelectWnd *pWnd = dynamic_cast<NamedTitleSelectWnd*>( g_GUIMgr.FindWnd( NAMED_TITLE_SELECT_WND ) );
			if( pWnd )
				pWnd->ShowSelectWnd();
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_NAMEDTITLE_PREMIUM );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, ioEtcItem::EIT_ETC_NAMEDTITLE_PREMIUM );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( ioEtcItem::EIT_ETC_NAMEDTITLE_PREMIUM, pUserEtcItem );
			}
		}
		break;
	}
}

void NamedTitlePremiumResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitlePremiumResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,
	};

	// Back
	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	// Icon
	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// Desc
	if( !m_Desc[0].IsEmpty() )
		m_Desc[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );

	if( !m_Desc[1].IsEmpty() )
		m_Desc[1].PrintFullTextWidthCut( iXPos+DESC_X, iYPos+DESC_Y, TAT_LEFT, 180 );
	
	if( !m_Desc[2].IsEmpty() )
		m_Desc[2].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+18, TAT_LEFT );
}

////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitlePremiumResultWnd::ReBatchButton()
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};


	HideChildWnd( ID_USE );
	HideChildWnd( ID_GO );

	// 예외처리
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );

			pButton->ShowWnd();
		}
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_NAMEDTITLE_PREMIUM, kSlot ) )
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );

			pButton->ShowWnd();
		}
	}
	else
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
		if( pButton )
		{
			pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );

			pButton->ShowWnd();
		}

		pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );

			pButton->ShowWnd();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitlePremiumResultWnd::ShowResult( DWORD dwCode, int nLevel, BYTE byPremium )
{
	SAFEDELETE(m_pIcon);

	for ( int i=0; i<MAXLINE; i++ )
		m_Desc[i].ClearList();
		
	NamedTitleInfo sNamedTitleInfo;
	if ( !g_NamedTitleInfoMgr.GetNamedTitleInfo( dwCode, sNamedTitleInfo ) )
		return;

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( g_NamedTitleInfoMgr.GetNamedTitlePremiumIcon() );

	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( 0, 0, 0 );	
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_17, "프리미엄 칭호 변경" );

	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( 0, 0, 0 );	
	m_Desc[1].SetTextColor( g_NamedTitleInfoMgr.GetNamedTitleTextColor( dwCode, nLevel, byPremium, false ) );
	m_Desc[1].AddTextPiece( FONT_SIZE_12, "%s", sNamedTitleInfo.m_szName.c_str() );

	m_Desc[2].SetTextStyle( TS_NORMAL );
	m_Desc[2].SetBkColor( 0, 0, 0 );	
	m_Desc[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[2].AddTextPiece( FONT_SIZE_12, "칭호가 프리미엄으로 변경되었습니다." );

	ReBatchButton();
	ShowWnd();
}