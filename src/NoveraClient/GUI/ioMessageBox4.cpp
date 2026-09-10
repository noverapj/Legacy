#include "StdAfx.h"

#include "ioMessageBox4.h"
#include "ExtraItemFuncWnd.h"

ioMessageBox4Btn::ioMessageBox4Btn()
{
}

ioMessageBox4Btn::~ioMessageBox4Btn()
{
}

void ioMessageBox4Btn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szTitleName  = xElement.GetStringAttribute_e( "TitleName" );

	ioButton::ParseExtraInfo( xElement );
}

void ioMessageBox4Btn::SetTitleName( const char *szTitleName )
{
	m_szTitleName = szTitleName;
}

void ioMessageBox4Btn::AttachTitle()
{	
	ioUITitle *pTitle = GetTitle();
	if( pTitle )
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "%s", m_szTitleName.c_str() );
		SetTitleText( szTitle );
		pTitle->SetOffset( GetWidth() / 2, pTitle->GetTextYOffSet() );	
	}
}
//////////////////////////////////////////////////////////////////////////
ioMessageBox4::ioMessageBox4()
{
	m_pPrevWnd = NULL;
	m_pReturnWnd = NULL;

	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pIconBackEnable = NULL;
	m_pIconBackDisable = NULL;
	m_pDarkLine = NULL;
	m_pLightLine = NULL;

	m_iSelectSlot = 0;
	m_dwSelectItemType = 0;

	m_dwSelectRadioBtnID = 0;
	m_dwLastRadioBtnID = 0;

	m_iYOffset = 0;

	m_bHasItem = false;

	m_pBottomFrm = NULL;

	m_vExpandSlotItemInfoList.clear();
}

ioMessageBox4::~ioMessageBox4()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBackEnable );
	SAFEDELETE( m_pIconBackDisable );
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );

	SAFEDELETE( m_pBottomFrm );

	ClearList();
}

void ioMessageBox4::ClearList()
{
	for( int i=0; i<MAX_MSG_LIST_SIZE; i++ )
	{
		m_PrinterList[i].ClearList();
	}
}

void ioMessageBox4::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( pImage )
	{
		if( szType == "IconBackEnable" )
		{
			SAFEDELETE( m_pIconBackEnable );
			m_pIconBackEnable = pImage;
		}
		else if( szType == "IconBackDisable" )
		{
			SAFEDELETE( m_pIconBackDisable );
			m_pIconBackDisable = pImage;
		}
		else if( szType == "StripGrayDark" )
		{
			SAFEDELETE( m_pDarkLine );
			m_pDarkLine = pImage;
		}
		else if( szType == "StripGrayLight" )
		{
			SAFEDELETE( m_pLightLine );
			m_pLightLine = pImage;
		}
		else
			ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioMessageBox4::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "BottomFrm" )
		{
			SAFEDELETE( m_pBottomFrm );
			m_pBottomFrm = pFrame;
		}
		else
			ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ioMessageBox4::SetPrintMessage( ioWnd *pPrevWnd, ioWnd *pReturnWnd )
{
	m_pPrevWnd = pPrevWnd;
	m_pReturnWnd = pReturnWnd;

	if( !IsShow() )
	{
		ShowWnd();

		int iSize = m_vExpandSlotItemInfoList.size();
		for( int i=0; i<iSize; ++i )
		{
			ShowChildWnd( ID_ITEM1_RBTN+i );
		}

		int iHeight;
		if( iSize == 0 )
			iHeight = MIN_HEIGHT_NONE_BTN;
		else
			iHeight = MIN_HEIGHT;

		m_iYOffset = iSize * 20;
		
		SetSize( GetWidth(), iHeight + m_iYOffset );
		SetWndPos( (Setting::Width() - GetWidth() ) / 2, ( (Setting::Height() - GetHeight() ) / 2 ) - 20 );
		
		UpdateMessage();
		ReBatchButton();
	}
}

void ioMessageBox4::UpdateMessage()
{
	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		return;
	}

	if( !m_bHasItem )
	{
		ioHashString szName = "";
		for( int i=ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT01; i<ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT20 + 1; ++i )
		{
			ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> ( g_EtcItemMgr.FindEtcItem(i) );
			if( pItem &&
				pItem->GetUseSlotNumber() == m_iSelectSlot &&
				pItem->GetLimitTime() > 0 )
			{
				SAFEDELETE( m_pIcon );
				SAFEDELETE( m_pSubIcon );
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
				m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
				szName = pItem->GetName();
				m_dwSelectItemType = pItem->GetType();
			}
		}
		ioComplexStringPrinter MsgPrinter[MAX_MSG_LIST_SIZE];
		MsgPrinter[0].SetTextStyle( TS_NORMAL );
		MsgPrinter[0].SetBkColor( 0, 0, 0 );
		MsgPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof(szTitle), STR(1), szName.c_str() );
		MsgPrinter[0].AddTextPiece( FONT_SIZE_13, szTitle );

		MsgPrinter[1].SetTextStyle( TS_NORMAL );
		MsgPrinter[1].SetBkColor( 0, 0, 0 );
		MsgPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		MsgPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

		MsgPrinter[2].SetTextStyle( TS_NORMAL );
		MsgPrinter[2].SetBkColor( 0, 0, 0 );
		MsgPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
		MsgPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );

		ClearList();
		for( int i=0; i<MAX_MSG_LIST_SIZE; ++i )
		{
			if( MsgPrinter[i].IsEmpty() )
				break;

			MsgPrinter[i].CopyFullText( m_PrinterList[i] );
		}
		return;
	}

	for( int i=ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT01; i<ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT20+1; ++i )
	{
		ioUserEtcItem::ETCITEMSLOT kItemSlot;
		if( !pUserEtcItem->GetEtcItem( i, kItemSlot ) )
			continue;

		ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> ( g_EtcItemMgr.FindEtcItem(i) );
		
		if( pItem )
		{
			if( pItem->GetType() == m_dwSelectItemType )
			{
				SAFEDELETE( m_pIcon );
				SAFEDELETE( m_pSubIcon );
				m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
				m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );

				char szWindowTitle[MAX_PATH] = "";
				SafeSprintf( szWindowTitle, sizeof(szWindowTitle), STR(4), pItem->GetName().c_str() );
				SetTitleText( szWindowTitle );

				ioComplexStringPrinter MsgPrinter[MAX_MSG_LIST_SIZE];
				MsgPrinter[0].SetTextStyle( TS_NORMAL );
				MsgPrinter[0].SetBkColor( 0, 0, 0 );
				MsgPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );

				char szTitle[MAX_PATH] = "";
				SafeSprintf( szTitle, sizeof(szTitle), STR(5), pItem->GetName().c_str() );
				MsgPrinter[0].AddTextPiece( FONT_SIZE_13, szTitle );

				MsgPrinter[1].SetTextStyle( TS_NORMAL );
				MsgPrinter[1].SetBkColor( 0, 0, 0 );
				MsgPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
				MsgPrinter[1].AddTextPiece( FONT_SIZE_13, STR(6) );

				MsgPrinter[2].SetTextStyle( TS_NORMAL );
				MsgPrinter[2].SetBkColor( 0, 0, 0 );
				MsgPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
				MsgPrinter[2].AddTextPiece( FONT_SIZE_13, STR(7) );

				ClearList();
				for( int i=0; i<MAX_MSG_LIST_SIZE; ++i )
				{
					if( MsgPrinter[i].IsEmpty() )
						break;

					MsgPrinter[i].CopyFullText( m_PrinterList[i] );
				}
			}
		}
	}

	MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> (g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
	if( pMyInventoryWnd )
	{
		ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*> (pMyInventoryWnd->FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
		if( pInfoWnd )
			pInfoWnd->SetExpandMedalSlotItemData( m_dwSelectItemType );
	}
}

void ioMessageBox4::ReBatchButton()
{
	if( m_bHasItem )
	{
		ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> ( g_EtcItemMgr.FindEtcItem(m_dwSelectItemType) );
		if( pItem )
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof(szTitle), STR(1), pItem->GetName().c_str() );
			SetTitleText( szTitle );
			SetBtnTitle( STR(2), STR(3) );
		}
	}
	else
	{
		ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> ( g_EtcItemMgr.FindEtcItem(m_dwSelectItemType) );
		if( pItem )
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof(szTitle), STR(4), pItem->GetName().c_str() );
			SetTitleText( szTitle );
			SetBtnTitle( STR(5), STR(6) );
		}
	}

	ioMessageBox4Btn *pYesBtn = dynamic_cast< ioMessageBox4Btn* >( FindChildWnd( ID_YES ) );
	ioMessageBox4Btn *pNoBtn  = dynamic_cast< ioMessageBox4Btn* >( FindChildWnd( ID_NO ) );
	ioWnd *pXBtn = FindChildWnd( ID_X );
	if( pYesBtn && pNoBtn && pXBtn )
	{
		pYesBtn->SetSize( pYesBtn->GetWidth(), pYesBtn->GetHeight() );
		pYesBtn->SetWndPos( BTN_LEFT_XPOS, GetHeight() - BTN_BOTTOM_YPOS );
		pYesBtn->AttachTitle();

		pNoBtn->SetSize( pNoBtn->GetWidth(), pNoBtn->GetHeight() );
		pNoBtn->SetWndPos( pYesBtn->GetWidth() + 19, GetHeight() - BTN_BOTTOM_YPOS );
		pNoBtn->AttachTitle();

		pXBtn->SetWndPos( GetWidth() - XBTN_RIGHT_XPOS, pXBtn->GetYPos() );
	}

	ShowChildWnd( ID_YES );
	ShowChildWnd( ID_NO );
	ShowChildWnd( ID_X );
}

void ioMessageBox4::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_YES:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDYES );
			}
		}
		break;
	case ID_NO:
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd, IDNO );
			}
		}
		break;
	case ID_ITEM1_RBTN:
	case ID_ITEM2_RBTN:
		if( cmd == IOBN_BTNDOWN )
		{
			m_dwSelectRadioBtnID = dwID;
			m_dwSelectItemType = m_vExpandSlotItemInfoList[m_dwSelectRadioBtnID-ID_ITEM1_RBTN].m_dwID;
			CheckRadioButton( ID_ITEM1_RBTN, m_dwLastRadioBtnID, m_dwSelectRadioBtnID );
			UpdateMessage();
		}
		break;
	}
}

void ioMessageBox4::iwm_hide()
{
	if( m_pPrevWnd && !m_pPrevWnd->IsShow() )
		m_pPrevWnd->ShowWnd();

	HideChildWnd( ID_ITEM1_RBTN );
	HideChildWnd( ID_ITEM2_RBTN );
	HideChildWnd( ID_ITEM3_RBTN );
	HideChildWnd( ID_ITEM4_RBTN );

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_vExpandSlotItemInfoList.clear();
	m_iSelectSlot = 0;
	m_dwSelectRadioBtnID = 0;
	m_bHasItem = false;
}

bool ioMessageBox4::iwm_esc()
{
	ioButton *pXBtn = dynamic_cast< ioButton* >( FindChildWnd( ID_X ) );
	if( !pXBtn ) return false;
	if( !pXBtn->IsShow() ) return false;

	iwm_command( pXBtn, IOBN_BTNUP, 0 );
	return true;
}

void ioMessageBox4::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetXPos();
	int iYPos = GetYPos();

	if( m_bHasItem )
	{
		if( m_pIconBackEnable )
			m_pIconBackEnable->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pIconBackDisable )
			m_pIconBackDisable->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	}

	if( m_pIcon )
	{
		m_pIcon->Render( iXPos + 151, iYPos + 98 );
	}
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + 151, iYPos + 98 );

	// Message
	for( int i=0 ; i<MAX_MSG_LIST_SIZE ; i++ )
	{
		if( m_PrinterList[i].IsEmpty() )
			break;

		m_PrinterList[i].PrintFullText( iXPos+TEXT_XPOS, iYPos+TEXT_YPOS+ TEXT_LINE_GAP * i, m_AlignType );
	}

	// RadioBtn Text
	int iSize = m_vExpandSlotItemInfoList.size();
	for( int i=0; i<iSize; ++i )
	{
		if( i%2 == 0 )
		{
			if( m_pDarkLine )
				m_pDarkLine->Render( iXPos + 17, iYPos + 213 + TEXT_LINE_GAP * i, UI_RENDER_MULTIPLY );
		}
		else
		{
			if( m_pLightLine )
				m_pLightLine->Render( iXPos + 17, iYPos + 213 + TEXT_LINE_GAP * i, UI_RENDER_MULTIPLY );
		}
		
		char szPrintMessage[MAX_PATH]="";
		ioHashString &rsName = m_vExpandSlotItemInfoList[i].m_szItemName;
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( pUserEtcItem )
		{
			DWORD dwDummy;
			ioHashString szItemCnt;
			ioHashString szDummy;

			pUserEtcItem->GetUseInfo( m_vExpandSlotItemInfoList[i].m_dwID, szItemCnt, szDummy, dwDummy, dwDummy );

			SafeSprintf( szPrintMessage, sizeof(szPrintMessage), STR(1), m_vExpandSlotItemInfoList[i].m_szItemName.c_str(), szItemCnt );
		}
		else
		{
			SafeSprintf( szPrintMessage, sizeof(szPrintMessage), STR(2), m_vExpandSlotItemInfoList[i].m_szItemName.c_str() );
		}
		
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 39, iYPos + 215 + TEXT_LINE_GAP*i, FONT_SIZE_13, szPrintMessage );
	}
	
	if( m_pBottomFrm )
	{
		m_pBottomFrm->SetSize( GetWidth(), BOTTOM_MULTIPLY_H );
		m_pBottomFrm->Render( GetDerivedPosX(), GetDerivedPosY() + ( GetHeight() - BOTTOM_MULTIPLY_H ), UI_RENDER_MULTIPLY );
	}
}

void ioMessageBox4::SetBtnTitle( const char *szYesTitle, const char *szNoTitle )
{
	ioMessageBox4Btn *pYesBtn = dynamic_cast< ioMessageBox4Btn* > ( FindChildWnd( ID_YES ) );
	if( pYesBtn )
	{
		pYesBtn->SetTitleName( szYesTitle );
		pYesBtn->AttachTitle();
	}

	ioMessageBox4Btn *pNoBtn = dynamic_cast< ioMessageBox4Btn* > ( FindChildWnd( ID_NO ) );
	if( pNoBtn )
	{
		pNoBtn->SetTitleName( szNoTitle );
		pNoBtn->AttachTitle();
	}
}

void ioMessageBox4::SetInfo( int iSlotNumber )
{
	m_iSelectSlot = iSlotNumber;

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		return;
	}

	HideChildWnd( ID_ITEM1_RBTN );
	HideChildWnd( ID_ITEM2_RBTN );
	HideChildWnd( ID_ITEM3_RBTN );
	HideChildWnd( ID_ITEM4_RBTN );

	m_vExpandSlotItemInfoList.clear();
	for( int i=ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT01; i<ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT20+1; ++i )
	{
		ioUserEtcItem::ETCITEMSLOT kItemSlot;
		if( !pUserEtcItem->GetEtcItem( i, kItemSlot ) )
			continue;

		ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> ( g_EtcItemMgr.FindEtcItem(i) );
		if( pItem )
		{
			if( pItem->GetUseSlotNumber() != iSlotNumber )
				continue;

			ExpandSlotItemInfo kInfo;
			kInfo.m_dwID = pItem->GetType();
			kInfo.m_szItemName = pItem->GetName();
			kInfo.m_dwLimitTime = pItem->GetLimitTime();
			m_vExpandSlotItemInfoList.push_back( kInfo );
		}
	}

	if( m_vExpandSlotItemInfoList.empty() )
	{
		m_bHasItem = false;
		return;
	}
	else
		m_bHasItem = true;

	int iSize = m_vExpandSlotItemInfoList.size();
	for ( int i=0; i<iSize; ++i )
	{
		DWORD dwID = ID_ITEM1_RBTN + i ;
		m_dwLastRadioBtnID = dwID;
	}

	m_dwSelectRadioBtnID = ID_ITEM1_RBTN;
	CheckRadioButton( ID_ITEM1_RBTN, m_dwLastRadioBtnID, m_dwSelectRadioBtnID );
	m_dwSelectItemType = m_vExpandSlotItemInfoList[m_dwSelectRadioBtnID-ID_ITEM1_RBTN].m_dwID;
	return;
}