#include "StdAfx.h"
#include "InventorySelectItemWnd.h"

InventorySelectItemRadioButton::InventorySelectItemRadioButton()
{
	m_iTabType = 0;
}

InventorySelectItemRadioButton::~InventorySelectItemRadioButton()
{

}

void InventorySelectItemRadioButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioRadioButton::ParseExtraInfo( xElement );

	m_iTabType = xElement.GetIntAttribute_e( "TabType" );
}

///////////////////////////////////////////////////////////////////////////////////

InventorySelectItemBtn::InventorySelectItemBtn()
{
	m_pIconEmptyImg = NULL;
	m_pIconImg		= NULL;
	m_bIsCostume = false;
}

InventorySelectItemBtn::~InventorySelectItemBtn()
{
	SAFEDELETE( m_pIconEmptyImg );
	SAFEDELETE( m_pIconImg );
}

void InventorySelectItemBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "empty_image" )
	{
		SAFEDELETE( m_pIconEmptyImg );
		m_pIconEmptyImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void InventorySelectItemBtn::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
		m_szDescArray[i].ClearList();
}

void InventorySelectItemBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			GetParent()->iwm_command( this, cmd, m_ItemInfo.m_iMagicCode );
		}
		break;
	case ID_ICON_BTN:
		if( cmd == IOWN_OVERED )
		{
			GetParent()->iwm_command( this, cmd, 0 );
		}
		else if( cmd == IOWN_LEAVED )
		{
			GetParent()->iwm_command( this, cmd, 0 );
		}
		break;
	}
}

void InventorySelectItemBtn::SetItemInfo( const InventorySelectItem& rkItem, int iEtcItemCount, bool bIsCostume )
{
	SetBlank();
	SetActive();

	m_bIsCostume = bIsCostume;

	m_ItemInfo.Init();
	m_ItemInfo = rkItem;

	if ( iEtcItemCount < m_ItemInfo.m_iNeedCoin )
		SetChildInActive( ID_SELECT_BTN );
	else
		SetChildActive( ID_SELECT_BTN );

	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}

bool InventorySelectItemBtn::SetInfo()
{
	if( m_ItemInfo.m_iMagicCode == -1 )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );
	ShowChildWnd( ID_SELECT_BTN );

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
		return false;
	}

	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( m_ItemInfo.m_szIconName );
	ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( m_ItemInfo.m_szSubIconName );

	pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
	pBtn->SetSubIcon( pSubImg );
	pBtn->SetGradeType( m_ItemInfo.m_nGrade );
	pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
	pBtn->SetCostume( m_bIsCostume );

	// desc
	ClearDesc();

	if ( !m_ItemInfo.m_szName.IsEmpty() )
	{
		m_szDescArray[0].SetTextStyle( TS_NORMAL );
		m_szDescArray[0].SetBkColor( 0, 0, 0 );	
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", m_ItemInfo.m_szName.c_str() );
	}

	if ( !m_ItemInfo.m_szSubText.IsEmpty() )
	{
		m_szDescArray[1].ClearList();
		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( m_ItemInfo.m_dwSubTextColor );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "%s", m_ItemInfo.m_szSubText.c_str() );
	}

	if ( !m_ItemInfo.m_szSubText2.IsEmpty() )
	{
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "%s", m_ItemInfo.m_szSubText2.c_str() );
	}

	return true;
}

void InventorySelectItemBtn::SetBlank()
{
	SetInActive();

	m_ItemInfo.Init();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_SELECT_BTN );

	ClearDesc();

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "없음" );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "" );

	m_bIsCostume = false;
}

void InventorySelectItemBtn::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{	
		if( m_pDisableFrm )
			m_pDisableFrm->Render( iXPos, iYPos );
		else if( m_pNormalFrm )
			m_pNormalFrm->Render( iXPos, iYPos );

		// 기타 이미지 추가.
		if( m_pIconEmptyImg )
			m_pIconEmptyImg->Render( iXPos+52, iYPos+49, UI_RENDER_MULTIPLY );
	}
	else
	{
		ioButton::OnRender();
	}

	RenderDesc( iXPos, iYPos );
}

void InventorySelectItemBtn::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET           = 52,
		Y_OFFSET           = 105,
		Y_LINE_GAP         = 16,
		Y_OFFSET_SECOND    = -8,
	};

	iXPos += X_OFFSET;
	iYPos += Y_OFFSET;

	int iDescCnt = 0;
	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if(! m_szDescArray[i].IsEmpty() )
			iDescCnt++;
	}

	if( iDescCnt == 3 )
		iYPos += Y_OFFSET_SECOND;

	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;

		m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + ( Y_LINE_GAP * i ), TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
}

void InventorySelectItemBtn::iwm_lbuttonup( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttonup( mouse );

	if( m_pParent )
	{
		if( IsExDisabled() )
			m_pParent->iwm_command( this, IOEX_BTNUP, -1 );
		else
			m_pParent->iwm_command( this, IOBN_BTNUP, -1 );
	}
}
///////////////////////////////////////////////////////////////////////////////////

InventorySelectItemWnd::InventorySelectItemWnd( void )
{
	m_iCurPage			= 0;
	m_pEditFrm			= NULL;
	m_pPreSelectedWnd	= NULL;
	m_szCurrFindKeyword.Clear();
	m_vAllItemList.clear();
	m_vCurItemList.clear();
	m_bIsCostume = false;
}

InventorySelectItemWnd::~InventorySelectItemWnd( void )
{
	SAFEDELETE( m_pEditFrm );
	m_pPreSelectedWnd = NULL;
}

void InventorySelectItemWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "EditFrm" )
	{
		SAFEDELETE( m_pEditFrm );
		m_pEditFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void InventorySelectItemWnd::ShowInventorySelectItemWnd( vInventorySelectItemList vitemList, ioWnd* pPreSelectedWnd, ioHashStringVec vRadioBtnNameList, ioHashString szTitleName, int iEtcItemCount, bool bIsCostume )
{
	SetRadioBtn( vRadioBtnNameList );

	m_iEtcItemCount = iEtcItemCount;
	m_bIsCostume = bIsCostume;

	if ( szTitleName.IsEmpty() )
		SetTitleText( "선택하기" );
	else
	{
		char szBuf[MAX_PATH];
		wsprintf_e( szBuf, "%s(%d개)", szTitleName.c_str(), m_iEtcItemCount );
		SetTitleText( szBuf );
	}
	
	m_vAllItemList = vitemList;
	m_vCurItemList.clear();
	m_szCurrFindKeyword.Clear();

	ChangeTab( m_dwCurTabID, "" );
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	m_pPreSelectedWnd = NULL;
	if ( pPreSelectedWnd != NULL )
		m_pPreSelectedWnd = pPreSelectedWnd;

	ShowWnd();
}

void InventorySelectItemWnd::SetRadioBtn( const ioHashStringVec& vRadioBtnNameList )
{
	int iBtnSize = vRadioBtnNameList.size();

	switch( iBtnSize )
	{
	case 0 :
		{
			m_dwCurTabID = m_dwCurMinTabID = m_dwCurMaxTabID = ID_TAB1_1_BTN;
			InventorySelectItemRadioButton *pRadioButton = dynamic_cast<InventorySelectItemRadioButton *>( FindChildWnd( ID_TAB1_1_BTN ) );
			if ( !pRadioButton )
				break;
			pRadioButton->SetTitleText( "전체" );
			pRadioButton->ShowWnd();
		}
		break;
	case 1:
		m_dwCurTabID = m_dwCurMinTabID = m_dwCurMaxTabID = ID_TAB1_1_BTN;
		break;
	case 2:
		{
			m_dwCurTabID = m_dwCurMinTabID = ID_TAB2_1_BTN;
			m_dwCurMaxTabID = ID_TAB2_2_BTN;
		}
		break;
	case 3:
		{
			m_dwCurTabID = m_dwCurMinTabID = ID_TAB3_1_BTN;
			m_dwCurMaxTabID = ID_TAB3_3_BTN;
		}
		break;
	case 4:
		{
			m_dwCurTabID = m_dwCurMinTabID = ID_TAB4_1_BTN;
			m_dwCurMaxTabID = ID_TAB4_4_BTN;
		}
		break;
	case 5:
		{
			m_dwCurTabID = m_dwCurMinTabID = ID_TAB5_1_BTN;
			m_dwCurMaxTabID = ID_TAB5_5_BTN;
		}
		break;
	case 6:
		{
			m_dwCurTabID = m_dwCurMinTabID = ID_TAB6_1_BTN;
			m_dwCurMaxTabID = ID_TAB6_6_BTN;
		}
		break;
	}

	for( int i=0 ; i<iBtnSize; ++i )
	{
		InventorySelectItemRadioButton *pRadioButton = dynamic_cast<InventorySelectItemRadioButton *>( FindChildWnd( m_dwCurTabID + i ) );
		if ( !pRadioButton )
			break;

		pRadioButton->SetTitleText( vRadioBtnNameList[i].c_str() );
		pRadioButton->ShowWnd();
	}
}

void InventorySelectItemWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void InventorySelectItemWnd::iwm_wheel( int zDelta )
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
			ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void InventorySelectItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_TAB1_1_BTN:
	case ID_TAB2_1_BTN:
	case ID_TAB2_2_BTN:
	case ID_TAB3_1_BTN:
	case ID_TAB3_2_BTN:
	case ID_TAB3_3_BTN:
	case ID_TAB4_1_BTN:
	case ID_TAB4_2_BTN:
	case ID_TAB4_3_BTN:
	case ID_TAB4_4_BTN:
	case ID_TAB5_1_BTN:
	case ID_TAB5_2_BTN:
	case ID_TAB5_3_BTN:
	case ID_TAB5_4_BTN:
	case ID_TAB5_5_BTN:
	case ID_TAB6_1_BTN:
	case ID_TAB6_2_BTN:
	case ID_TAB6_3_BTN:
	case ID_TAB6_4_BTN:
	case ID_TAB6_5_BTN:
	case ID_TAB6_6_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeTab( dwID, "" );
		}
		break;
	case ID_ITEM_INFO_WND_1_BTN:
	case ID_ITEM_INFO_WND_2_BTN:
	case ID_ITEM_INFO_WND_3_BTN:
	case ID_ITEM_INFO_WND_4_BTN:
	case ID_ITEM_INFO_WND_5_BTN:
	case ID_ITEM_INFO_WND_6_BTN:
	case ID_ITEM_INFO_WND_7_BTN:
	case ID_ITEM_INFO_WND_8_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_pPreSelectedWnd && param != -1 )
			{
				m_pPreSelectedWnd->iwm_command( this, IOBN_BTNUP, param );
				HideWnd();
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			InventorySelectItemBtn *pInfo = dynamic_cast<InventorySelectItemBtn*>( FindChildWnd( dwID ) );
			if ( pInfo )
			{
				if ( !pInfo->GetItemName().IsEmpty() )
				{
					InventorySelectItemToolTip *pTooltip = dynamic_cast<InventorySelectItemToolTip*>( FindChildWnd( ID_TOOLTIP ) );
					if( pTooltip && !pTooltip->IsShow() )
					{
						if( pTooltip->SetInfo( pInfo->GetItemName() ) )
							ShowChildWnd( ID_TOOLTIP );
					}
				}
			}
		}
		else if( cmd == IOWN_LEAVED )
			HideChildWnd( ID_TOOLTIP );
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFind();
		}
		break;
	}
}

void InventorySelectItemWnd::ChangeTab( DWORD dwID, const char *szFind )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( m_dwCurMinTabID, m_dwCurMaxTabID, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for ( DWORD i = m_dwCurMinTabID; i < m_dwCurMaxTabID+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;

			pButton->SetClick( false );
		}
	}

	SetCurItemInfoVec( m_dwCurTabID, szFind );

	m_iCurPage = 0;

	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		int iItemSize = 0;
		iItemSize = m_vCurItemList.size();
		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void InventorySelectItemWnd::SetCurItemInfoVec( DWORD dwTabID, const char *szFind )
{
	if( m_vAllItemList.empty() )
		return;

	vInventorySelectItemList vTotalList = m_vAllItemList;
	m_vCurItemList.clear();

	bool bFind = false;
	std::string	 szFindText = szFind;
	if( !szFindText.empty() )
	{
		bFind = true;
		std::transform( szFindText.begin(), szFindText.end(), szFindText.begin(), tolower );
	}

	int iSize = vTotalList.size();
	for( int i = 0; i < iSize ; i++ )
	{
		const InventorySelectItem& rkInfo = vTotalList[i];
		if( bFind )
		{
			std::string szItemName = rkInfo.m_szName.c_str();
			std::transform( szItemName.begin(), szItemName.end(), szItemName.begin(), tolower );
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->GetType() == ioLocalManager::LCT_KOREA )
			{
				//조합형 글자 비교(초성 비교 초함)
				if( !Help::Strstr( szItemName.c_str(), szFindText.c_str() ) )
					continue;
			}
			else
			{
				if( strstr( szItemName.c_str(), szFindText.c_str() ) == NULL )
					continue;
			}
		}
		else
		{
			if( !IsRightTab( dwTabID, rkInfo.m_iTabType ) )
				continue;
		}

		m_vCurItemList.push_back( vTotalList[i] );
	}
}

bool InventorySelectItemWnd::IsRightTab( DWORD dwTabID, int iType )
{
	InventorySelectItemRadioButton* pButton = dynamic_cast<InventorySelectItemRadioButton*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		if ( pButton->GetTabType() == -1 )
			return true;
		else if ( pButton->GetTabType() == iType )
			return true;
	}
	return false;
}

void InventorySelectItemWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;
	int iItemSize = m_vCurItemList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;

	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		InventorySelectItemBtn *pItem = dynamic_cast<InventorySelectItemBtn*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray + i;

		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			pItem->SetBlank();
			continue;
		}

		pItem->SetItemInfo( m_vCurItemList[ iCurArray ], m_iEtcItemCount, m_bIsCostume );
	}
}

void InventorySelectItemWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	ChangeTab( m_dwCurTabID, m_szCurrFindKeyword.c_str() );
}

void InventorySelectItemWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		if( g_InputBox.IsEnter() )
		{
			iwm_command( FindChildWnd( ID_FIND_BTN ), IOBN_BTNUP, 0 );
		}

		else if( g_InputBox.IsKeyDown() || g_InputBox.IsBackSpace() )
		{
			m_szCurrFindKeyword = g_InputBox.GetString();
			iwm_command( FindChildWnd( ID_FIND_BTN ), IOBN_BTNUP, 0 );
		}
	}
}

void InventorySelectItemWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	enum
	{
		X_OFFSET = 17,
		Y_OFFSET = 474,
	};

	if( m_pEditFrm )
		m_pEditFrm->Render( iXPos+X_OFFSET, iYPos+Y_OFFSET );
}

void InventorySelectItemWnd::iwm_hide()
{
	for ( DWORD i = m_dwCurMinTabID; i < m_dwCurMaxTabID+1 ; i++)
	{
		ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
		if( pButton && pButton->IsShow() )
			pButton->HideWnd();
	}
}

///////////////////////////////////////////////////////////////////

InventorySelectItemToolTip::InventorySelectItemToolTip()
{
	m_szName.ClearList();
	m_szOverSnd.Clear();
}

InventorySelectItemToolTip::~InventorySelectItemToolTip()
{

}

void InventorySelectItemToolTip::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );
	m_szOverSnd = xElement.GetStringAttribute_e( "InfoOverSound" );
}

void InventorySelectItemToolTip::OnProcess( float fTimePerSec )
{
	if ( IsShow() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( !pMouse ) return;
		pMouse->SetToolTipCursor();
		int iXPos = 0;
		int iYPos = 0;
		Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
		SetScreenWndPos( iXPos, iYPos );
		GoToTop();
	}
}

bool InventorySelectItemToolTip::SetInfo( const ioHashString& szName )
{
	if ( szName.IsEmpty() )
	{
		HideWnd();
		return false;
	}

	m_szName.ClearList();
	m_szName.SetTextStyle( TS_NORMAL );
	m_szName.SetBkColor( 0, 0, 0 );	
	m_szName.SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색

	m_szName.AddTextPiece( FONT_SIZE_12, szName.c_str() );

	int iMaxWidth  = m_szName.GetFullWidth() + SIDE_TEXT_GAP;
	int iMaxHeight = MIN_HEIGHT;
	SetSize( iMaxWidth, iMaxHeight );
	//ShowWnd();

	if( !m_szOverSnd.IsEmpty() )
		g_SoundMgr.PlaySound( m_szOverSnd );

	return true;
}

void InventorySelectItemToolTip::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;
	if( !m_szName.IsEmpty() )
		m_szName.PrintFullText( iXPos, iYPos, TAT_LEFT );
}