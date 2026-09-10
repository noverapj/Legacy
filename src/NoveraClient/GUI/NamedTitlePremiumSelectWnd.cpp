#include "StdAfx.h"

#include "NamedTitlePremiumSelectWnd.h"

#include "ioPullDownList.h"

NamedTitlePremiumSelectWnd::NamedTitlePremiumSelectWnd(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pBackEffect = NULL;
	m_pManual = NULL;	
	m_ItemTitle.ClearList();
	m_dwCode = 0;
}

NamedTitlePremiumSelectWnd::~NamedTitlePremiumSelectWnd(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pBackEffect);
	m_ItemTitle.ClearList();
}

void NamedTitlePremiumSelectWnd::iwm_show()
{
	ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
	if( pSelectBtn )
	{
		pSelectBtn->SetTitleText( "칭호를 선택하세요" );
		pSelectBtn->SetTitleColor( TCT_DEFAULT_DARKGRAY, 0xFF000000 );
	}

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
}

void NamedTitlePremiumSelectWnd::iwm_hide()
{
	HideChildWnd( ID_LIST_WND );
}

void NamedTitlePremiumSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			CheckRecvBtn();
		}
		break;
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioPullDownList *pListWnd = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_LIST_WND ) );
			if ( !pListWnd )
				return;

			if( pListWnd->IsShow() )
				pListWnd->HideWnd();
			else
				ShowNamedTitleList();
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
				SendNamedTitleChange();
		}
		break;
	}
}

void NamedTitlePremiumSelectWnd::iwm_pulldown_event( const PullDownEvent& Event )
{
	if( CLASSTYPEQUALS( ClickPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const ClickPullDownEvent& ClickEvent = dynamic_cast<const ClickPullDownEvent&>( Event );
		if( ClickEvent.IsPulldownClick() )
		{			
			const ioPullDownList* pList = ClickEvent.GetEventCaller();
			if( !pList )
				return;

			const NamedTitlePullDown* pItem = dynamic_cast<const NamedTitlePullDown*>( pList->GetPullDown( ClickEvent.iClickIdx ) );
			if( !pItem )
				return;

			m_dwCode = pItem->m_dwCode;

			ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
			if( pSelectBtn )
			{
				pSelectBtn->SetTitleText( pItem->m_szTitle.c_str() );
				pSelectBtn->SetTitleColor( pItem->m_dwTitleColor, 0xFF000000 );
			}
		}
	}
}

void NamedTitlePremiumSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void NamedTitlePremiumSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	enum
	{
		TITLE_X_OFFSET	= 128,
		TITLE_Y_OFFSET	= 152,

		ICON_X		= 129,
		ICON_Y		= 105,

		MANUAL_X = 27,
		MANUAL_Y = 187,
	};

	// Back
	if( m_pBackEffect )
		m_pBackEffect->Render( nXPos+ICON_X, nYPos+ICON_Y, UI_RENDER_MULTIPLY );
	
	// Icon
	if( m_pIcon )
		m_pIcon->Render( nXPos+ICON_X, nYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
	if( m_pSubIcon )
		m_pSubIcon->Render( nXPos+ICON_X, nYPos+ICON_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

	if( !m_ItemTitle.IsEmpty() )
		m_ItemTitle.PrintFullText( nXPos+TITLE_X_OFFSET, nYPos+TITLE_Y_OFFSET, TAT_CENTER );

	PrintManual( nXPos+MANUAL_X, nYPos+MANUAL_Y, FONT_SIZE_12 );
}

void NamedTitlePremiumSelectWnd::ShowSelectWnd( DWORD dwEtcItemCode )
{
	m_dwEtcItemCode = dwEtcItemCode;

	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	HideChildWnd( ID_LIST_WND );
	m_dwCode = 0;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
	if( !pEtcItem ) 
		return;

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );

	m_ItemTitle.ClearList();
	m_ItemTitle.SetTextStyle( TS_NORMAL );
	m_ItemTitle.SetBkColor( 0, 0, 0 );	
	m_ItemTitle.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_ItemTitle.AddTextPiece( FONT_SIZE_17, pEtcItem->GetName().c_str() );

	ioWnd *pSelectBtn = FindChildWnd( ID_SELECT_BTN );
	if( pSelectBtn )
	{
		ioUITitle *pUITitle = pSelectBtn->GetTitle();
		if( pUITitle )
		{
			pUITitle->SetWidthCutSize( 177.0f );
			pUITitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
		}
	}

	ShowWnd();
}

void NamedTitlePremiumSelectWnd::PrintManual( int nXPos, int nYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
	if( !pItem ) 
		return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		nYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( nXPos, nYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void NamedTitlePremiumSelectWnd::GetNamedTitleList( ioPullDownList* pList )
{
	if( !pList )
		return;
	
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	std::vector<NamedTitleSortInfo> SortList;

	for ( int i=0; i<pUserNamedTitle->GetNamedTitleSlotCnt(); i++ )
	{
		NamedTitleSlot sNamedTitleSlot;
		if ( !pUserNamedTitle->GetNamedTitleSlotArray( i, sNamedTitleSlot ) )
			continue;
		
		if ( sNamedTitleSlot.m_byPremium )
			continue;

		NamedTitleSortInfo sNamedTitleSortInfo;
		sNamedTitleSortInfo.m_dwCode = sNamedTitleSlot.m_dwCode;
		sNamedTitleSortInfo.m_szNamedTitle = g_NamedTitleInfoMgr.GetNamedTitleName( sNamedTitleSlot.m_dwCode );
		SortList.push_back( sNamedTitleSortInfo );
	}

	if ( SortList.empty() )
		return;

	std::sort( SortList.begin(), SortList.end(), NamedTitleSortFunc() );

	for ( int i=0; i<(int)SortList.size(); i++ )
	{
		NamedTitleSlot sNamedTitleSlot;
		if ( !pUserNamedTitle->GetNamedTitleSlot( SortList[i].m_dwCode, sNamedTitleSlot ) )
			continue;

		NamedTitlePullDown* pPullDown = new NamedTitlePullDown;
		
		pPullDown->m_dwCode = sNamedTitleSlot.m_dwCode;
		pPullDown->m_szTitle			= SortList[i].m_szNamedTitle.c_str();
		pPullDown->m_dwTitleColor		= g_NamedTitleInfoMgr.GetNamedTitleTextColor( sNamedTitleSlot.m_dwCode, sNamedTitleSlot.m_nLevel, false, sNamedTitleSlot.m_byEquip );
		pList->AddList( pPullDown );
	}
}

void NamedTitlePremiumSelectWnd::ShowNamedTitleList()
{
	ioWnd *pWnd = FindChildWnd( ID_SELECT_BTN );
	if ( !pWnd )
		return;

	ioPullDownList *pListWnd = dynamic_cast<ioPullDownList*>( FindChildWnd( ID_LIST_WND ) );
	if( pListWnd )
	{
		pListWnd->OpenList( pWnd );
		GetNamedTitleList( pListWnd );
	}
}

void NamedTitlePremiumSelectWnd::SendNamedTitleChange()
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleSlot sNamedTitleSlot;
	if ( !pUserNamedTitle->GetNamedTitleSlot( m_dwCode, sNamedTitleSlot ) )
		return;

	HideWnd();

	TCPNetwork::MouseBusy( true );

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwEtcItemCode;
	kPacket << m_dwCode;
	TCPNetwork::SendToServer( kPacket );
}

void NamedTitlePremiumSelectWnd::CheckRecvBtn()
{
	if ( m_dwCode <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "칭호가 선택되지 않았습니다." );
		return;
	}

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "프리미엄 칭호 변경권을" );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "정말 사용하시겠습니까?" );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, "프리미엄칭호", "", "확인", "취소" );
}