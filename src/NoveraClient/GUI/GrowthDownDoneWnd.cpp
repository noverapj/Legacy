#include "StdAfx.h"
#include "GrowthDownDoneWnd.h"

//////////////////////////////////////////////////////////////////////////////////////////
GrowthDownDoneWnd::GrowthDownDoneWnd(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
	m_pPointImg = NULL;

	m_iClassType = 0;
	m_iReturnPoint = 0;

	m_Desc[0].ClearList();
	m_Desc[1].ClearList();
}

GrowthDownDoneWnd::~GrowthDownDoneWnd(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pBackEffect);
	SAFEDELETE(m_pPointImg);
}

void GrowthDownDoneWnd::ShowGrowthDownDone( int iClassType, int iReturnPoint )
{
	m_iClassType = iClassType;
	m_iReturnPoint = iReturnPoint;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetMySoldierIconName( m_iClassType ) );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( m_iClassType ) );

	ReBatchButton();

	m_Desc[0].ClearList();
	m_Desc[1].ClearList();

	ioHashString szName = g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) );

	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( 0, 0, 0 );	
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_12, STR(1), szName.c_str() );
	m_Desc[0].SetTextColor( TCT_DEFAULT_RED );
	m_Desc[0].AddTextPiece( FONT_SIZE_12, STR(2), m_iReturnPoint );

	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( 0, 0, 0 );
	m_Desc[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[1].AddTextPiece( FONT_SIZE_12, STR(3) );
	
	ShowWnd();
}

void GrowthDownDoneWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "P_Image" )
	{
		SAFEDELETE( m_pPointImg );
		m_pPointImg = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void GrowthDownDoneWnd::ReBatchButton()
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
	
	HideChildWnd(ID_USE);
	HideChildWnd(ID_GO);

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
			{
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_GROWTH_DOWN, kSlot) )
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
			}

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
			{
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}

		pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}
}

void GrowthDownDoneWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_GROWTH_DOWN );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, ioEtcItem::EIT_ETC_GROWTH_DOWN );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( ioEtcItem::EIT_ETC_GROWTH_DOWN, pUserEtcItem );
			}
		}
		break;
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowSoldierTabDirect( m_iClassType );
			}
		}
		break;
	}
}

void GrowthDownDoneWnd::OnRender()
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
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, FONT_SIZE_17, STR(1) );


	// Desc
	if( !m_Desc[0].IsEmpty() )
		m_Desc[0].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y, TAT_LEFT );

	if( m_pPointImg )
	{
		float fSize = m_Desc[0].GetFullWidth();

		m_pPointImg->Render( iXPos+DESC_X+fSize, iYPos+DESC_Y );
	}

	if( !m_Desc[1].IsEmpty() )
		m_Desc[1].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+18, TAT_LEFT );
}
//////////////////////////////////////////////////////////////////////////////////////////
GrowthAllDownDoneWnd::GrowthAllDownDoneWnd(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;

	m_iClassType = 0;
	m_iReturnPoint = 0;

	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_Desc[i].ClearList();	
	}
}

GrowthAllDownDoneWnd::~GrowthAllDownDoneWnd(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pBackEffect);
}

void GrowthAllDownDoneWnd::ShowGrowthAllDownDone( int iClassType, int iReturnPoint, __int64 iReturnPeso )
{
	m_iClassType   = iClassType;
	m_iReturnPoint = iReturnPoint;
	m_iReturnPeso  = iReturnPeso;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetMySoldierIconName( m_iClassType ) );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( m_iClassType ) );

	ReBatchButton();

	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_Desc[i].ClearList();	
	}

	ioHashString szName = g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) );

	char szPoint[MAX_PATH]="";
	Help::ConvertNumToStrComma( m_iReturnPoint, szPoint, sizeof( szPoint ) );

	char szPeso[MAX_PATH]="";
	int iPeso = (int)m_iReturnPeso;
	if( iPeso < 0 )
		iPeso = 0;
	Help::ConvertNumToStrComma( iPeso, szPeso, sizeof( szPeso ) );

	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( 0, 0, 0 );	
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_17, szName.c_str() );

	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( 0, 0, 0 );	
	m_Desc[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[1].AddTextPiece( FONT_SIZE_17, "육성 복구" );

	m_Desc[2].SetTextStyle( TS_NORMAL );
	m_Desc[2].SetBkColor( 0, 0, 0 );	
	m_Desc[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[2].AddTextPiece( FONT_SIZE_12, "★ 육성포인트와 페소가 복구되었습니다." );

	m_Desc[3].SetTextStyle( TS_NORMAL );
	m_Desc[3].SetBkColor( 0, 0, 0 );	
	m_Desc[3].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[3].AddTextPiece( FONT_SIZE_12, "★ 육성포인트 " );
	m_Desc[3].SetTextColor( TCT_DEFAULT_RED );
	m_Desc[3].AddTextPiece( FONT_SIZE_12, "%s 복구", szPoint );

	m_Desc[4].SetTextStyle( TS_NORMAL );
	m_Desc[4].SetBkColor( 0, 0, 0 );
	m_Desc[4].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[4].AddTextPiece( FONT_SIZE_12, "★ 페소 " );
	m_Desc[4].SetTextColor( TCT_DEFAULT_BLUE );
	m_Desc[4].AddTextPiece( FONT_SIZE_12, "%s 복구", szPeso );

	m_Desc[5].SetTextStyle( TS_NORMAL );
	m_Desc[5].SetBkColor( 0, 0, 0 );
	m_Desc[5].SetTextColor( TCT_DEFAULT_GREEN );
	m_Desc[5].AddTextPiece( FONT_SIZE_17, "복구되었습니다" );

	ShowWnd();
}

void GrowthAllDownDoneWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void GrowthAllDownDoneWnd::ReBatchButton()
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
	
	HideChildWnd(ID_USE);
	HideChildWnd(ID_GO);

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
			{
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_GROWTH_ALL_DOWN, kSlot) )
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
			}

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
			{
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}

		pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}
}

void GrowthAllDownDoneWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			DWORD dwType = ioEtcItem::EIT_ETC_GROWTH_ALL_DOWN;
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, dwType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( dwType, pUserEtcItem );
			}
		}
		break;
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowSoldierTabDirect( m_iClassType );
			}
		}
		break;
	}
}

void GrowthAllDownDoneWnd::OnRender()
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
		BIG_GAP     = 23,

		DESC_X		= 27,
		DESC_Y		= 206,
		DESC_GAP    = 18,

		END_X       = 129,
		END_Y       = 421,
	};

	// Back
	if( m_pBackEffect )
	{
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	// Icon
	if( m_pIcon )
		m_pIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	// Desc
	m_Desc[0].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, TAT_CENTER );
	m_Desc[1].PrintFullText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y+BIG_GAP, TAT_CENTER );
	m_Desc[2].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y, TAT_LEFT );
	m_Desc[3].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+DESC_GAP, TAT_LEFT );
	m_Desc[4].PrintFullText( iXPos+DESC_X, iYPos+DESC_Y+(DESC_GAP*2), TAT_LEFT );
	m_Desc[5].PrintFullText( iXPos+END_X, iYPos+END_Y, TAT_CENTER );
}