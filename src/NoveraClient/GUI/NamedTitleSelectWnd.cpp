#include "StdAfx.h"

#include "NamedTitleSelectWnd.h"

NamedTitleSelectWnd::NamedTitleSelectWnd()
{
	m_pNamedTitleUiMark = NULL;

	m_NamedTitleList.clear();
	m_CurFindNamedTitleList.clear();
	m_szCurFindKeyword.Clear();

	m_dwNamedTitleCode = 0;

	m_dwCurTabID = ID_TAB_1_BTN;
	m_bEmpty = false;
	m_bNoSearch = false;
	m_szEmpty.ClearList();
	m_szNoSearch.ClearList();
}

NamedTitleSelectWnd::~NamedTitleSelectWnd()
{
	SAFEDELETE( m_pNamedTitleUiMark );

	m_NamedTitleList.clear();
	m_CurFindNamedTitleList.clear();
}

void NamedTitleSelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void NamedTitleSelectWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
	}
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_nCurPage - 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_nCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void NamedTitleSelectWnd::iwm_hide()
{
	m_szEmpty.ClearList();
	m_szNoSearch.ClearList();

	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	if ( pUserNamedTitle->IsNewNamedTitle() )
	{
		pUserNamedTitle->ClearNamedTitleNew();

		SP2Packet kPacket( CTPK_NEW_TITLE_CONFIRM );
		TCPNetwork::SendToServer( kPacket );
	}
}

void NamedTitleSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_TAB_1_BTN:
	case ID_TAB_2_BTN:
	case ID_TAB_3_BTN:
		if( cmd == IOBN_BTNDOWN )
			ChangeTab( dwID, m_szCurFindKeyword.c_str() );
		break;
	case ID_ITEM_INFO_WND_1_BTN:
	case ID_ITEM_INFO_WND_2_BTN:
	case ID_ITEM_INFO_WND_3_BTN:
	case ID_ITEM_INFO_WND_4_BTN:
	case ID_ITEM_INFO_WND_5_BTN:
	case ID_ITEM_INFO_WND_6_BTN:
	case ID_ITEM_INFO_WND_7_BTN:
	case ID_ITEM_INFO_WND_8_BTN:
	case ID_ITEM_INFO_WND_9_BTN:
	case ID_ITEM_INFO_WND_10_BTN:
	case ID_ITEM_INFO_WND_11_BTN:
	case ID_ITEM_INFO_WND_12_BTN:
	case ID_ITEM_INFO_WND_13_BTN:
	case ID_ITEM_INFO_WND_14_BTN:
	case ID_ITEM_INFO_WND_15_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( param == FilterNamedTitleInfoWnd::ID_SELECT_BTN )
			{
				FilterNamedTitleInfoWnd *pSelectInfo = dynamic_cast<FilterNamedTitleInfoWnd*>( FindChildWnd( dwID ) );
				if ( pSelectInfo )
				{
					if ( m_dwNamedTitleCode != pSelectInfo->GetCode() )
					{
						m_dwNamedTitleCode = pSelectInfo->GetCode();

						TCPNetwork::MouseBusy( true );
						SP2Packet kPacket( CTPK_TITLE_CHANGE );
						kPacket << true;
						kPacket << m_dwNamedTitleCode;
						TCPNetwork::SendToServer( kPacket );
					}
				}
			}
		}
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
			SetFind();
		break;
	case ID_RELEASE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				if ( m_dwNamedTitleCode > 0 )
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_TITLE_CHANGE );
					kPacket << false;
					TCPNetwork::SendToServer( kPacket );
				}
			}
		}
		break;
	case ID_FIND_RESET_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFindReset();
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		if( g_InputBox.IsEnter() )
			SetFind();
		else if( g_InputBox.IsKeyDown() || g_InputBox.IsBackSpace() )
			m_szCurFindKeyword = g_InputBox.GetString();
	}
}

void NamedTitleSelectWnd::OnRender()
{
	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	ioWnd::OnRender();
	
	enum
	{
		DESC_X = 154,
		DESC_Y = 183,
	};

	RenderNamedTitle( nXPos, nYPos );
	
	if ( m_bEmpty )
	{
		if( !m_szEmpty.IsEmpty() )
			m_szEmpty.PrintFullText( nXPos+DESC_X, nYPos+DESC_Y, TAT_CENTER );
	}
	else if ( m_bNoSearch )
	{
		if( !m_szNoSearch.IsEmpty() )
			m_szNoSearch.PrintFullText( nXPos+DESC_X, nYPos+DESC_Y, TAT_CENTER );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::RenderNamedTitle( int nXPos, int nYPos )
{
	if ( m_szNamedTitle.IsEmpty() )
		return;

	int nNameWidth = g_FontMgr.GetTextWidth( GetTitleText(), GetTitleTextStyle(), FONT_SIZE_17 );
	g_FontMgr.SetAlignType( GetTitleTextHorzAlign() );
	g_FontMgr.SetTextStyle( GetTitleTextStyle() );
	g_FontMgr.SetBkColor( 12, 66, 111 );

	if ( m_byNamedTitlePremium && m_pNamedTitleUiMark )
	{
		int nMaxWidth = 185;
		int nMarkStartX = nXPos + GetTitleOffsetX() + nNameWidth + 2;
		int nMarkWidth = m_pNamedTitleUiMark->GetWidth();

		m_pNamedTitleUiMark->Render( nMarkStartX + nMarkWidth / 2, nYPos + GetTitleOffsetY(), UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pNamedTitleUiMark->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );

		g_FontMgr.SetTextColor( 240, 189, 48 );
		g_FontMgr.PrintTextWidthCut( nMarkStartX + nMarkWidth, nYPos + GetTitleOffsetY(), FONT_SIZE_17, nMaxWidth, "%s", m_szNamedTitle.c_str() );

		int nNameTitleWidth = g_FontMgr.GetTextWidth( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17 );
		int nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth + nMarkWidth/2 + 2;
		if ( nNameTitleWidth >= nMaxWidth )
		{
			nNameTitleWidth = g_FontMgr.GetTextWidthCutSize( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17, nMaxWidth );
			nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth + nMarkWidth/2 + 6;
		}

		m_pNamedTitleUiMark->Render( nLastXPos, nYPos + GetTitleOffsetY(), UI_RENDER_NORMAL, TFO_BILINEAR );
		m_pNamedTitleUiMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	}
	else
	{
		int nMaxWidth = 229;
		int nMarkStartX = nXPos + GetTitleOffsetX() + nNameWidth + 2;
		int nMarkWidth = g_FontMgr.GetTextWidth( "[", GetTitleTextStyle(), FONT_SIZE_17 );

		g_FontMgr.SetTextColor( 255, 255, 255 );
		g_FontMgr.PrintText( nMarkStartX, nYPos + GetTitleOffsetY(), FONT_SIZE_17, "[" );
		g_FontMgr.PrintTextWidthCut( nMarkStartX + nMarkWidth - 2, nYPos + GetTitleOffsetY(), FONT_SIZE_17, nMaxWidth, "%s", m_szNamedTitle.c_str() );

		int nNameTitleWidth = g_FontMgr.GetTextWidth( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17 );
		int nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth - 4;
		if ( nNameTitleWidth >= nMaxWidth )
		{
			nNameTitleWidth = g_FontMgr.GetTextWidthCutSize( m_szNamedTitle.c_str(), GetTitleTextStyle(), FONT_SIZE_17, nMaxWidth );
			nLastXPos = nMarkStartX + nMarkWidth + nNameTitleWidth + 2;
		}

		g_FontMgr.PrintText( nLastXPos, nYPos + GetTitleOffsetY(), FONT_SIZE_17, "]" );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::Update()
{
	UpdatePage( m_nCurPage );
	UpdateEquipNamedTitle();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::ChangeTab( DWORD dwID, const char *szFind )
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_3_BTN, dwID );
	m_dwCurTabID = dwID;
	
	SetFilterNamedTitleVec( dwID, szFind );

	m_nCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int nItemSize = m_CurFindNamedTitleList.size();

		int nMaxPos = nItemSize / MAX_ITEM_INFO_WND_BTN;
		if( nItemSize % MAX_ITEM_INFO_WND_BTN )
			nMaxPos++;

		pScroll->SetScrollRange( 0, nMaxPos );
		pScroll->SetScrollPos( m_nCurPage );
	}
}

void NamedTitleSelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_nCurPage = iNewPage;

	int nItemSize = m_CurFindNamedTitleList.size();
	int nStartArray = m_nCurPage * MAX_ITEM_INFO_WND_BTN;

	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		FilterNamedTitleInfoWnd *pItem = dynamic_cast<FilterNamedTitleInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int nCurArray = nStartArray + i;

		if( !COMPARE( nCurArray, 0, nItemSize ) )
		{
			pItem->SetBlank();
			continue;
		}

		pItem->SetNamedTitleInfo( m_CurFindNamedTitleList[nCurArray].m_dwCode );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::SetSearchNamedTitleVec()
{
	m_NamedTitleList.clear();
	
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	for ( int i=0; i<pUserNamedTitle->GetNamedTitleSlotCnt(); i++ )
	{
		NamedTitleSlot sNamedTitleSlot;
		if ( !pUserNamedTitle->GetNamedTitleSlotArray( i, sNamedTitleSlot ) )
			continue;

		if ( sNamedTitleSlot.m_byStatus == TITLE_DISABLE )
			continue;

		SearchNamedTitleInfo sSearchNamedTitleInfo;
		sSearchNamedTitleInfo.m_dwCode = sNamedTitleSlot.m_dwCode;
		sSearchNamedTitleInfo.m_byPremium = sNamedTitleSlot.m_byPremium;
		sSearchNamedTitleInfo.m_szName = g_NamedTitleInfoMgr.GetNamedTitleName( sNamedTitleSlot.m_dwCode );

		m_NamedTitleList.push_back( sSearchNamedTitleInfo );
	}
}

void NamedTitleSelectWnd::SetFilterNamedTitleVec( DWORD dwTabID, const char *szFind )
{
	if( m_NamedTitleList.empty() )
		return;

	m_CurFindNamedTitleList.clear();
	m_bEmpty = false;
	m_bNoSearch = false;

	switch( dwTabID )
	{
	case ID_TAB_1_BTN:
		CompareFilterNamedTitle( szFind, true, false );
		break;
	case ID_TAB_2_BTN:
		CompareFilterNamedTitle( szFind, false, false );
		break;
	case ID_TAB_3_BTN:
		CompareFilterNamedTitle( szFind, false, true );
		break;
	}

	if( m_CurFindNamedTitleList.empty() )
	{
		if( strcmp( szFind, "" ) != 0 )
			m_bNoSearch = true;
		else
			m_bEmpty = true;
		return;
	}

	std::sort( m_CurFindNamedTitleList.begin(), m_CurFindNamedTitleList.end(), SearchNamedTitleSortFunc() );
}

void NamedTitleSelectWnd::CompareFilterNamedTitle( const char *szFind, bool bAll, bool bPremium )
{
	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	for( int i=0; i<(int)m_NamedTitleList.size(); i++ )
	{
		if ( !bAll )
		{
			if ( bPremium )
			{
				if ( !m_NamedTitleList[i].m_byPremium )
					continue;
			}
			else
			{
				if ( m_NamedTitleList[i].m_byPremium )
					continue;
			}
		}

		if( bFind )
		{
			ioHashString szItemName = m_NamedTitleList[i].m_szName;
			if( strstr( szItemName.c_str(), szFind ) == NULL )
				continue;
		}

		m_CurFindNamedTitleList.push_back( m_NamedTitleList[i] );
	}
}

void NamedTitleSelectWnd::UpdateEquipNamedTitle()
{
	m_dwNamedTitleCode = 0;
	m_szNamedTitle.Clear();
	m_byNamedTitlePremium = 0;

	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleSlot kEquipSlot;
	if ( !pUserNamedTitle->GetNamedTitleSlotEquip( kEquipSlot) )
		return;

	m_dwNamedTitleCode = kEquipSlot.m_dwCode;
	m_szNamedTitle = g_NamedTitleInfoMgr.GetNamedTitleName( m_dwNamedTitleCode );
	m_byNamedTitlePremium = kEquipSlot.m_byPremium;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	ChangeTab( m_dwCurTabID, m_szCurFindKeyword.c_str() );
}

void NamedTitleSelectWnd::SetFindReset()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_FIND_EDIT ) );
	if ( !pEdit )
		return;

	if ( m_szCurFindKeyword.IsEmpty() )
		return;

	pEdit->ClearString();
	pEdit->SetKeyFocus();
	g_InputBox.SetString( "" );
	pEdit->KillKeyFocus();
	m_szCurFindKeyword.Clear();
	SetFind();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void NamedTitleSelectWnd::ShowSelectWnd()
{	
	m_szCurFindKeyword.Clear();
	m_dwNamedTitleCode = g_MyInfo.GetNamedTitleCode();

	SetSearchNamedTitleVec();

	if ( m_NamedTitleList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "칭호가 없습니다." );
		return;
	}

	ChangeTab( ID_TAB_1_BTN, "" );

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	m_szEmpty.ClearList();
	m_szEmpty.SetTextStyle( TS_NORMAL );
	m_szEmpty.SetBkColor( 0, 0, 0 );
	m_szEmpty.SetTextColor( TCT_DEFAULT_GRAY );
	m_szEmpty.AddTextPiece( FONT_SIZE_12, "칭호가 없습니다" );

	m_szNoSearch.ClearList();
	m_szNoSearch.SetTextStyle( TS_NORMAL );
	m_szNoSearch.SetBkColor( 0, 0, 0 );
	m_szNoSearch.SetTextColor( TCT_DEFAULT_GRAY );
	m_szNoSearch.AddTextPiece( FONT_SIZE_12, "검색결과를 찾을 수 없습니다" );

	SAFEDELETE( m_pNamedTitleUiMark );
	m_pNamedTitleUiMark = g_UIImageSetMgr.CreateImageByFullName( g_NamedTitleInfoMgr.GetNamedTitleUiMark() );

	UpdateEquipNamedTitle();
	ShowWnd();
}

DWORD NamedTitleSelectWnd::GetNamedTitleCode()
{
	return m_dwNamedTitleCode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FilterNamedTitleInfoWnd::FilterNamedTitleInfoWnd()
{
	m_pNewMark = NULL;
	m_dwCode = 0;
	m_NamedTitleSlot.Init();
}

FilterNamedTitleInfoWnd::~FilterNamedTitleInfoWnd()
{
	SAFEDELETE( m_pNewMark );
}

void FilterNamedTitleInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioCheckButton *pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_SELECT_BTN ) );
			if ( !pCheck )
				return;

			pCheck->SetCheck( true );

			NamedTitleSelectWnd *pNamedTitleSelectWnd = dynamic_cast<NamedTitleSelectWnd*>( GetParent() );
			if ( !pNamedTitleSelectWnd )
				return;
			
			if ( m_dwCode > 0 && pNamedTitleSelectWnd->GetNamedTitleCode() != m_dwCode )
				pNamedTitleSelectWnd->iwm_command( this, cmd, ID_SELECT_BTN );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FilterNamedTitleInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "new_mark" )
	{
		SAFEDELETE( m_pNewMark );
		m_pNewMark = pImage;
	}
	else
		ioButton::AddRenderImage( szType, pImage );
}

void FilterNamedTitleInfoWnd::OnRender()
{
	ioButton::OnRender();

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	RenderInfo( nXPos, nYPos );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FilterNamedTitleInfoWnd::RenderInfo( int nXPos, int nYPos )
{
	enum 
	{
		NAMED_TITLE_X = 24,
		NAMED_TITLE_Y = 4,

		NEW_MARK_X = 244,
		NEW_MARK_Y = 5,
	};
	
	if( HasWndStyle( IWS_INACTIVE ) )
		return;

	if( !m_szNamedTitle.IsEmpty() )
		m_szNamedTitle.PrintFullTextWidthCut( nXPos+NAMED_TITLE_X, nYPos+NAMED_TITLE_Y, TAT_LEFT, 218 );

	if ( m_NamedTitleSlot.m_byStatus == TITLE_NEW && m_pNewMark )
		m_pNewMark->Render( nXPos + NEW_MARK_X, nYPos + NEW_MARK_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
}

bool FilterNamedTitleInfoWnd::SetInfo()
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return false;

	ioCheckButton *pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_SELECT_BTN ) );
	if ( !pCheck )
		return false;

	if ( !pUserNamedTitle->GetNamedTitleSlot( m_dwCode, m_NamedTitleSlot ) || m_NamedTitleSlot.m_byStatus == TITLE_DISABLE )
		return false;

	m_szNamedTitle.SetTextStyle( TS_NORMAL );
	m_szNamedTitle.SetBkColor( 0, 0, 0 );
	m_szNamedTitle.SetTextColor( g_NamedTitleInfoMgr.GetNamedTitleTextColor( m_NamedTitleSlot.m_dwCode, m_NamedTitleSlot.m_nLevel, m_NamedTitleSlot.m_byPremium, m_NamedTitleSlot.m_byEquip ) );
	m_szNamedTitle.AddTextPiece( FONT_SIZE_12, "%s", g_NamedTitleInfoMgr.GetNamedTitleName( m_NamedTitleSlot.m_dwCode ).c_str() );
	
	if ( m_NamedTitleSlot.m_byEquip )
		pCheck->SetCheck( true );
	else
		pCheck->SetCheck( false );

	pCheck->ShowWnd();

	return true;
}

void FilterNamedTitleInfoWnd::SetBlank()
{
	SetInActive();
	HideChildWnd( ID_SELECT_BTN );
	m_dwCode = 0;
	m_NamedTitleSlot.Init();
	m_szNamedTitle.ClearList();
}

void FilterNamedTitleInfoWnd::SetNamedTitleInfo( DWORD dwCode )
{
	SetBlank();
	SetActive();

	m_dwCode = dwCode;

	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}

DWORD FilterNamedTitleInfoWnd::GetCode()
{
	return m_dwCode;
}