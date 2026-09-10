
#include "StdAfx.h"
#include "InvenSoldierSelectWnd.h"

InvenSoldierSelectBuyWnd::InvenSoldierSelectBuyWnd()
{
	m_dwEtcItemType = 0;

	m_CharInfo.m_class_type = 1;
	m_CharInfo.m_kindred	= 1;
	m_CharInfo.m_sex		= 1;
	m_CharInfo.m_beard	    = 1;
	m_CharInfo.m_face	    = 1;
	m_CharInfo.m_hair	    = 1;
	m_CharInfo.m_skin_color = 1;
	m_CharInfo.m_hair_color = 1;
	m_CharInfo.m_accessories= 1;
	m_CharInfo.m_underwear  = 1;
	m_OverCharInfo = m_CharInfo;

	m_iCurBtnUpClassType    = 0;
	m_iCurBtnOverClassType  = 0;

	m_pManual = NULL;

	m_bSetShowPullDownWnd = false;
}

InvenSoldierSelectBuyWnd::~InvenSoldierSelectBuyWnd()
{
}

void InvenSoldierSelectBuyWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/PackageHelpWnd.xml", this );
}

void InvenSoldierSelectBuyWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
	m_bSetShowPullDownWnd = false;

	// 버튼 세팅
	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );

	if( !kItemList.empty() )
	{
		Help::GetDefaultCharInfo( m_CharInfo, kItemList[0].m_iMagicCode );
		SetBtnTitle( kItemList[0].m_iMagicCode );
	}

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( true, false, true );
		pCharWnd->ShowWnd();
	}
}

void InvenSoldierSelectBuyWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );
}

void InvenSoldierSelectBuyWnd::GetSoldierList( vNewShopPullDownItem &rkList )
{
	rkList.clear();
	
	int iMaxClass = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i <  iMaxClass; i++)
	{
		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo )
			continue;

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;

		// 대여 용병은 선택 가능
		if( g_MyInfo.IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) == false )
		{
			// 영구 용병은 리스트에서 제외
			if( g_MyInfo.IsCharMortmain( iClassType ) )
				 continue;
		}

		ioEtcItemSoldierSelector *pEtcItem = dynamic_cast< ioEtcItemSoldierSelector* > ( g_EtcItemMgr.FindEtcItem( m_dwEtcItemType ) );
		if( pEtcItem )
		{
			if( !pEtcItem->IsRightSoldierCode( iClassType ) )
				continue;

			if( pEtcItem->GetActiveFilter() == ioEtcItemSoldierSelector::AF_ACTIVE )
			{
				if( !g_ClassPrice.IsActiveClass( i ) )
					continue;
			}
		}
		else
		{
			continue;
		}

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = iClassType;
		kItem.m_szName     = pInfo->GetName();
		kItem.m_dwTextColor= g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) );
		rkList.push_back( kItem );
	}
}

void InvenSoldierSelectBuyWnd::SetPullDownMenu( ioWnd *pOpenBtn )
{
	if( !pOpenBtn ) return;

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(FindChildWnd( ID_PULLDOWN_WND ));
	if( pPullDownWnd )
	{
		if( pPullDownWnd->IsShow() )
			pPullDownWnd->HideWnd();
		else
		{
			m_OverCharInfo = m_CharInfo;
			pPullDownWnd->CreatePullDown( pOpenBtn, pOpenBtn->GetXPos(), pOpenBtn->GetYPos() + 24 );

			vNewShopPullDownItem kItemList;
			GetSoldierList( kItemList );

			for(int i = 0;i < (int)kItemList.size();i++)
			{
				pPullDownWnd->InsertItem( kItemList[i] );
			}
			pPullDownWnd->ShowWnd();
			m_bSetShowPullDownWnd = true;

			int iHeight = pPullDownWnd->GetHeight();
			int iEndYPos = pPullDownWnd->GetYPos() + iHeight;
			if( iEndYPos >= GetHeight() )
			{
				int iNewYPos = pOpenBtn->GetYPos() - iHeight - 1;
				pPullDownWnd->SetWndPos( pOpenBtn->GetXPos(), iNewYPos );
			}
		}
	}
}

void InvenSoldierSelectBuyWnd::SetBtnTitle( int iClassType )
{
	ioWnd *pWnd = FindChildWnd( ID_CHAR_SELECT_BTN );
	if( !pWnd )
		return;

	pWnd->SetTitleText( g_MyInfo.GetClassName( iClassType ) );
	pWnd->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) ), 0 );
}

void InvenSoldierSelectBuyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			InvenSoldierSelectBuyAgreeWnd *pAgreeWnd = dynamic_cast< InvenSoldierSelectBuyAgreeWnd* >(g_GUIMgr.FindWnd( INVEN_SOLDIER_SELECT_BUY_AGREE_WND ));
			if( pAgreeWnd )
			{
				pAgreeWnd->HideWnd();
				pAgreeWnd->SetAgree( m_CharInfo.m_class_type, m_dwEtcItemType );
				pAgreeWnd->ShowWnd();
			}
		}
		break;
	case ID_CHAR_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetPullDownMenu( pWnd );
		}
		break;
	case ID_PULLDOWN_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				PullDownBtnUP( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
				PullDownBtnOver( pPullDownWnd->GetOpenBtn(), param );
		}
		break;
	}
}

void InvenSoldierSelectBuyWnd::PullDownBtnUP( ioWnd *pOpenBtn, int iListArray )
{
	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );
	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) return;		
	if( m_iCurBtnUpClassType == kItemList[iListArray].m_iMagicCode ) return;

	m_iCurBtnUpClassType = kItemList[iListArray].m_iMagicCode;
	SetBtnTitle( kItemList[iListArray].m_iMagicCode );
	Help::GetDefaultCharInfo( m_CharInfo, kItemList[iListArray].m_iMagicCode );

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( true, false, true );
	}
}

void InvenSoldierSelectBuyWnd::PullDownBtnOver( ioWnd *pOpenBtn, int iListArray )
{
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( !pCharWnd ) return;		

	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) return;		
	if( m_iCurBtnOverClassType == kItemList[iListArray].m_iMagicCode ) return;

	m_iCurBtnOverClassType = kItemList[iListArray].m_iMagicCode;
	Help::GetDefaultCharInfo( m_OverCharInfo, kItemList[iListArray].m_iMagicCode );

	pCharWnd->SetZPosition( 600.0f );
	pCharWnd->SetChar( m_OverCharInfo );
	pCharWnd->SetEquip( true, false, true );
}

void InvenSoldierSelectBuyWnd::PullDownBtnLeave()
{
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( true, false, true );
	}
}

void InvenSoldierSelectBuyWnd::SetEtcItemType( DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
}

void InvenSoldierSelectBuyWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;	

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>( FindChildWnd( ID_PULLDOWN_WND ));
	if( !pPullDownWnd ) return;

	if( !pPullDownWnd->IsShow() && m_bSetShowPullDownWnd )
	{
		m_bSetShowPullDownWnd = false;
		PullDownBtnLeave();
	}
}

void InvenSoldierSelectBuyWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT_X_OFFSET  = 33,
		TEXT_Y_OFFSET  = 332,
	};

	PrintManual( iXPos+TEXT_X_OFFSET, iYPos+TEXT_Y_OFFSET, FONT_SIZE_12 );
}

void InvenSoldierSelectBuyWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );
	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}
//////////////////////////////////////////////////////////////////////////
InvenSoldierSelectBuyAgreeWnd::InvenSoldierSelectBuyAgreeWnd()
{
	m_pCharIcon     = NULL;
	m_pCharSubIcon  = NULL;
	m_pBackEffect   = NULL;
	m_iClassType    = 0;
	m_dwEtcItemType = 0;
	m_nGradeType = 0;
}

InvenSoldierSelectBuyAgreeWnd::~InvenSoldierSelectBuyAgreeWnd()
{
	SAFEDELETE(m_pCharIcon);
	SAFEDELETE(m_pCharSubIcon);
	SAFEDELETE(m_pBackEffect);
}

void InvenSoldierSelectBuyAgreeWnd::SetAgree( int iClassType, DWORD dwEtcItemType )
{
	m_iClassType    = iClassType;
	m_dwEtcItemType = dwEtcItemType;
	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType );

	// 용병 타입
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon      = g_MyInfo.GetSoldierIcon( m_iClassType, true );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( m_iClassType );
	m_sSelectorTitle = g_MyInfo.GetClassName( m_iClassType );
}

void InvenSoldierSelectBuyAgreeWnd::SendUse()
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem || !pUserEtcItem->IsCanUse( m_dwEtcItemType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else
	{
		SP2Packet kPacket( CTPK_ETCITEM_USE );
		kPacket << m_dwEtcItemType;	
		kPacket << m_iClassType;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}

	HideWnd();
}

void InvenSoldierSelectBuyAgreeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_AGREE:
		if( cmd == IOBN_BTNUP )
		{
			SendUse();
		}
		break;
	}
}

void InvenSoldierSelectBuyAgreeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void InvenSoldierSelectBuyAgreeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 125,
		ICON_Y		= 98,

		TITLE_X		= 125,
		TITLE_Y		= 146,

		FINAL_DESC_X	= 125,
		FINAL_DESC_Y	= 168,
	};

	// icon
	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y );

	if( m_pCharIcon )
		m_pCharIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pCharSubIcon )
		m_pCharSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, m_sSelectorTitle.c_str() );

	// Last
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_13, STR(1) );
}
//////////////////////////////////////////////////////////////////////////
InvenSoldierSelectBuyResultWnd::InvenSoldierSelectBuyResultWnd()
{
	m_pBackEffect = NULL;
	m_pCharIcon = NULL;
	m_pCharSubIcon = NULL;

	m_pManual = NULL;
	m_dwEtcItemType = 0;
	m_nGradeType = 0;
}

InvenSoldierSelectBuyResultWnd::~InvenSoldierSelectBuyResultWnd()
{
	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
}

void InvenSoldierSelectBuyResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void InvenSoldierSelectBuyResultWnd::ShowResult( int iClassType, int iClassTime, DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
	m_pManual       = ioEtcInvenSubManualMgr::GetSingletonPtr();
	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );

	char szBuf[MAX_PATH] = "";
	SafeSprintf( szBuf, sizeof( szBuf ), g_MyInfo.GetClassName( iClassType ) );
	m_TitleName = szBuf;

	m_sClassTime = g_PresentMgr.GetPresentValue2Text( PRESENT_SOLDIER, iClassType, iClassTime );

	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon = g_MyInfo.GetSoldierIcon( iClassType, true );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( iClassType );

	ShowWnd();
}

void InvenSoldierSelectBuyResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowPresentTabDirect();
			}
		}
		break;
	}
}

void InvenSoldierSelectBuyResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		    = 129,
		ICON_Y		    = 105,

		TITLE_X		    = 129,
		TITLE_Y		    = 152,

		TITLE_SECOND_X  = 129,
		TITLE_SECOND_Y  = 175,

		DESC_X		    = 27,
		DESC_Y		    = 206,

		FINAL_DESC_X	= 129,
		FINAL_DESC_Y	= 421,
	};

	// icon
	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y );

	if( m_pCharIcon )
		m_pCharIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pCharSubIcon )
		m_pCharSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, m_TitleName.c_str() );

	// Desc
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos+TITLE_SECOND_X, iYPos+TITLE_SECOND_Y, FONT_SIZE_17, m_sClassTime.c_str() );
	PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

	// Last
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
}

void InvenSoldierSelectBuyResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItemSoldierSelector *pItem =  dynamic_cast<ioEtcItemSoldierSelector*> ( g_EtcItemMgr.FindEtcItem( m_dwEtcItemType ) );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual2() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}
//////////////////////////////////////////////////////////////////////////
InvenSoldierExpBonusWnd::InvenSoldierExpBonusWnd()
{
	m_dwEtcItemType = 0;

	m_CharInfo.m_class_type = 1;
	m_CharInfo.m_kindred	= 1;
	m_CharInfo.m_sex		= 1;
	m_CharInfo.m_beard	    = 1;
	m_CharInfo.m_face	    = 1;
	m_CharInfo.m_hair	    = 1;
	m_CharInfo.m_skin_color = 1;
	m_CharInfo.m_hair_color = 1;
	m_CharInfo.m_accessories= 1;
	m_CharInfo.m_underwear  = 1;
	m_OverCharInfo = m_CharInfo;

	m_iCurBtnUpClassType    = 0;
	m_iCurBtnOverClassType  = 0;

	m_pManual = NULL;
	m_iClassLimitLevel = 0;
}

InvenSoldierExpBonusWnd::~InvenSoldierExpBonusWnd()
{
}

void InvenSoldierExpBonusWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/PackageHelpWnd.xml", this );
}

void InvenSoldierExpBonusWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// 버튼 세팅
	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );

	if( !kItemList.empty() )
	{
		const CHARACTER &rkChar = g_MyInfo.GetCharacterToClassType( kItemList[0].m_iMagicCode );

		m_CharInfo = rkChar;
		m_iCurBtnUpClassType = kItemList[0].m_iMagicCode;
	}

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
		pCharWnd->ShowWnd();
		SetBtnTitle( m_CharInfo.m_class_type );
	}
}

void InvenSoldierExpBonusWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );
}

void InvenSoldierExpBonusWnd::GetSoldierList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	int iMaxClass = g_MyInfo.GetCharCount();
	for (int i = 0; i <  iMaxClass; i++)
	{
		if( !g_MyInfo.IsCharActive( i ) ) continue;
		if( g_MyInfo.IsCharExercise( i ) ) continue;

		DWORD dwCharIndex = g_MyInfo.GetCharIndex( i );
		int iClassType = g_MyInfo.GetClassType( dwCharIndex );
		if( iClassType == -1 ) continue;

		int iClassLevel = g_MyInfo.GetClassLevel( iClassType, true );
		if( iClassLevel >= m_iClassLimitLevel ) continue;


		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = iClassType;

		char szName[MAX_PATH] = "";
		sprintf_e( szName, "Lv%d %s", iClassLevel, g_MyInfo.GetClassName( iClassType ) );

		kItem.m_szName     = szName;
		kItem.m_dwTextColor= g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) );
		rkList.push_back( kItem );
	}
}

void InvenSoldierExpBonusWnd::SetPullDownMenu( ioWnd *pOpenBtn )
{
	if( !pOpenBtn ) return;

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(FindChildWnd( ID_PULLDOWN_WND ));
	if( pPullDownWnd )
	{
		if( pPullDownWnd->IsShow() )
			pPullDownWnd->HideWnd();
		else
		{
			m_OverCharInfo = m_CharInfo;
			pPullDownWnd->CreatePullDown( pOpenBtn, pOpenBtn->GetXPos(), pOpenBtn->GetYPos() + 24 );

			vNewShopPullDownItem kItemList;
			GetSoldierList( kItemList );

			for(int i = 0;i < (int)kItemList.size();i++)
			{
				pPullDownWnd->InsertItem( kItemList[i] );
			}
			pPullDownWnd->ShowWnd();

			int iHeight = pPullDownWnd->GetHeight();
			int iEndYPos = pPullDownWnd->GetYPos() + iHeight;
			if( iEndYPos >= GetHeight() )
			{
				int iNewYPos = pOpenBtn->GetYPos() - iHeight - 1;
				pPullDownWnd->SetWndPos( pOpenBtn->GetXPos(), iNewYPos );
			}
		}
	}
}

void InvenSoldierExpBonusWnd::SetBtnTitle( int iClassType )
{
	ioWnd *pWnd = FindChildWnd( ID_CHAR_SELECT_BTN );
	if( !pWnd )
		return;

	pWnd->SetTitleText( g_MyInfo.GetClassName( iClassType ) );	
	pWnd->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) ), 0 );
}

void InvenSoldierExpBonusWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// MSG 
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), g_MyInfo.GetClassName( m_iCurBtnUpClassType ), g_MyInfo.GetClassLevel( m_iCurBtnUpClassType, true ) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "             " );

			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );
			kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(2), m_iClassLimitLevel );

			kPrinter[3].SetTextStyle( TS_NORMAL );
			kPrinter[3].SetBkColor( 0, 0, 0 );
			kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(3) );

			kPrinter[4].SetTextStyle( TS_NORMAL );
			kPrinter[4].SetBkColor( 0, 0, 0 );
			kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[4].AddTextPiece( FONT_SIZE_13, STR(4) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
		}
		break;
	case ID_CHAR_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetPullDownMenu( pWnd );
		}
		break;
	case ID_PULLDOWN_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				PullDownBtnUP( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
				PullDownBtnOver( pPullDownWnd->GetOpenBtn(), param );
		}
		else if( cmd == IOWN_LEAVED )
		{
			PullDownBtnLeave();
		}
		break;
	case MESSAGE_BOX3: // g_GUIMgr.SetPrevMsgListBox()
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SP2Packet kPacket( CTPK_ETCITEM_USE );
				kPacket << m_dwEtcItemType;	
				kPacket << m_iCurBtnUpClassType;
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );
			}
		}
		break;
	}
}

void InvenSoldierExpBonusWnd::PullDownBtnUP( ioWnd *pOpenBtn, int iListArray )
{
	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );
	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) return;		
	if( m_iCurBtnUpClassType == kItemList[iListArray].m_iMagicCode ) return;

	m_iCurBtnUpClassType = kItemList[iListArray].m_iMagicCode;
	SetBtnTitle( kItemList[iListArray].m_iMagicCode );

	const CHARACTER &rkChar = g_MyInfo.GetCharacterToClassType( kItemList[iListArray].m_iMagicCode );
	
	m_CharInfo = rkChar;
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
	}
}

void InvenSoldierExpBonusWnd::PullDownBtnOver( ioWnd *pOpenBtn, int iListArray )
{
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( !pCharWnd ) return;		

	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) return;		
	if( m_iCurBtnOverClassType == kItemList[iListArray].m_iMagicCode ) return;

	m_iCurBtnOverClassType = kItemList[iListArray].m_iMagicCode;

	const CHARACTER &rkChar = g_MyInfo.GetCharacterToClassType( kItemList[iListArray].m_iMagicCode );

	m_OverCharInfo = rkChar;
	pCharWnd->SetZPosition( 600.0f );
	pCharWnd->SetChar( m_OverCharInfo );
	pCharWnd->SetEquip( false, false, true );
}

void InvenSoldierExpBonusWnd::PullDownBtnLeave()
{
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
	}
}

void InvenSoldierExpBonusWnd::SetEtcItemType( DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
	m_iClassLimitLevel = 999999;
	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS )
	{
		ioEtcItemSoldierExpBonus *pEtcItem = dynamic_cast< ioEtcItemSoldierExpBonus * >( g_EtcItemMgr.FindEtcItem( m_dwEtcItemType ) );
		if( pEtcItem )
		{
			m_iClassLimitLevel = pEtcItem->GetLimitLevel();
		}
	}
}

void InvenSoldierExpBonusWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT_X_OFFSET  = 33,
		TEXT_Y_OFFSET  = 332,
	};

	PrintManual( iXPos+TEXT_X_OFFSET, iYPos+TEXT_Y_OFFSET, FONT_SIZE_12 );
}

void InvenSoldierExpBonusWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );
	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}
//////////////////////////////////////////////////////////////////////////
InvenSoldierExpBonusResultWnd::InvenSoldierExpBonusResultWnd()
{
	m_pBackEffect = NULL;
	m_pCharIcon = NULL;
	m_pCharSubIcon = NULL;

	m_dwEtcItemType = 0;
	m_iClassType =  0;
	m_nGradeType = 0;
}

InvenSoldierExpBonusResultWnd::~InvenSoldierExpBonusResultWnd()
{
	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
}

void InvenSoldierExpBonusResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void InvenSoldierExpBonusResultWnd::ShowResult( int iClassType, DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
	m_iClassType    = iClassType;

	char szBuf[MAX_PATH] = "";
	SafeSprintf( szBuf, sizeof( szBuf ), g_MyInfo.GetClassName( m_iClassType ) );
	m_TitleName = szBuf;

	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
	m_pCharIcon = g_MyInfo.GetSoldierIcon( iClassType, true );
	m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( iClassType );

	m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );
	if( pEtcItem )
	{
		m_EtcItemName = pEtcItem->GetName();
	}

	ShowWnd();
}

void InvenSoldierExpBonusResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
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

void InvenSoldierExpBonusResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		    = 129,
		ICON_Y		    = 105,

		TITLE_X		    = 129,
		TITLE_Y		    = 152,

		TITLE_SECOND_X  = 129,
		TITLE_SECOND_Y  = 175,

		DESC_X		    = 27,
		DESC_Y		    = 206,

		FINAL_DESC_X	= 129,
		FINAL_DESC_Y	= 421,
	};

	// icon
	if( m_pBackEffect )
		m_pBackEffect->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos+ICON_X, iYPos+ICON_Y );

	if( m_pCharIcon )
		m_pCharIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );
	if( m_pCharSubIcon )
		m_pCharSubIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, m_TitleName.c_str() );
	
	// Desc
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos+TITLE_SECOND_X, iYPos+TITLE_SECOND_Y, FONT_SIZE_17, m_EtcItemName.c_str() );

	// Last
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(1) );
}