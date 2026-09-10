#include "StdAfx.h"

#include "MyItemSelecteWnd.h"

#include "../ioPowerUpManager.h"
#include "../ioUserAccessory.h"
#include "../ioUserSpiritManager.h"
//////////////////////////////////////////////////////////////////////////
MyItemSelectRadioButton::MyItemSelectRadioButton()
{
	m_iTabType = 0;
}

MyItemSelectRadioButton::~MyItemSelectRadioButton()
{
}

void MyItemSelectRadioButton::ParseExtraInfo( ioXMLElement &xElement )
{
	ioRadioButton::ParseExtraInfo( xElement );

	m_iTabType = xElement.GetIntAttribute_e( "TabType" );
}
//////////////////////////////////////////////////////////////////////////
ItemSelectInfoWnd::ItemSelectInfoWnd()
{
	m_pEmptyImg = NULL;
	m_pBlueStateFrm = NULL;
}

ItemSelectInfoWnd::~ItemSelectInfoWnd()
{
	SAFEDELETE(m_pEmptyImg);
	SAFEDELETE(m_pBlueStateFrm);
}

void ItemSelectInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void ItemSelectInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "empty_image" )
	{
		SAFEDELETE( m_pEmptyImg );
		m_pEmptyImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ItemSelectInfoWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StateBlueFrm" )
	{
		SAFEDELETE( m_pBlueStateFrm );
		m_pBlueStateFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void ItemSelectInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_ItemInfo.m_iItemCode != 0 )
				GetParent()->iwm_command( this, IOBN_BTNUP, m_ItemInfo.m_iIndex );
			else if( m_AccessoryInfo.m_iIndex != 0 )
				GetParent()->iwm_command( this, IOBN_BTNUP, m_AccessoryInfo.m_iIndex );
			else
				GetParent()->iwm_command( this, IOBN_BTNUP, m_SpiritInfo.m_iCode );
		}
		break;
	}
}

void ItemSelectInfoWnd::SetBlank()
{
	SetInActive();

	m_ItemInfo.Init();
	m_AccessoryInfo.Init();
	m_SpiritInfo.Init();
	m_bSoldier = false;

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_SELECT_BTN );

	ClearDesc();
}

void ItemSelectInfoWnd::SetItemInfo( const EXTRAITEMSLOT& rkItem )
{
	SetBlank();
	SetActive();

	m_ItemInfo = rkItem;
	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}

void ItemSelectInfoWnd::SetAccessoryInfo( AccessorySlot& rkItem )
{
	SetBlank();
	SetActive();

	m_AccessoryInfo = rkItem;
	if( !SetAccessoryInfo() )
	{
		SetBlank();
		return;
	}
}

void ItemSelectInfoWnd::SetSpiritInfo( AlchemicItemInfo& rkItem )
{
	SetBlank();
	SetActive();

	m_SpiritInfo = rkItem;
	if( !SetSpiritInfo() )
	{
		SetBlank();
		return;
	}
}

void ItemSelectInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_AccessoryInfo.m_iAccessoryCode != 0 )
		ProcessAccessoryDesc();
}

void ItemSelectInfoWnd::ProcessAccessoryDesc()
{
	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
	{
		LOG.PrintTimeAndLog( 0, "%s UserAccessoy == NULL", __FUNCTION__ );
		return;
	}

	ioHashString sInfo;
	pAccessory->GetUseInfo( m_AccessoryInfo.m_iIndex, sInfo );
	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_AccessoryInfo.m_iIndex );
		return;
	}

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	if( m_AccessoryInfo.m_PeriodType == ACCPT_MORTMAIN )
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	else
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );

	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, sInfo.c_str() );
}

bool ItemSelectInfoWnd::SetInfo()
{
	if( m_ItemInfo.m_iItemCode == 0 )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
		return false;
	}

	ShowChildWnd( ID_SELECT_BTN );

	ioUIRenderImage *pImg = NULL;
	ioHashString szIcon = m_ItemInfo.m_IconName;
	if( !szIcon.IsEmpty() )
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );

	pBtn->SetIcon( pImg, FLOAT1, FLOAT1, m_ItemInfo.m_iReinforce );

	if( m_ItemInfo.m_iTradeState == EET_NORMAL && m_ItemInfo.m_PeriodType == EPT_MORTMAIN )
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_TRADE_WAIT );
	else if( m_ItemInfo.m_iTradeState == EET_ENABLE && m_ItemInfo.m_PeriodType == EPT_MORTMAIN )
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_TRADE_ENABLE );
	else if( m_ItemInfo.m_PeriodType == EPT_GROW_TIME )
		pBtn->SetExtraItemGrowthCatalyst( true );
	else
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );

	// desc
	ClearDesc();

	const ioItem *pItem = g_ItemMaker.GetItemConst( m_ItemInfo.m_iItemCode, __FUNCTION__ );
	if( !pItem )
		return false;

	pBtn->SetGradeType( pItem->GetGradeType() );

	char szBuf[MAX_PATH], szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	
	int iGradType;
	int iEquipSlotIndex = m_ItemInfo.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	if ( iEquipSlotIndex == ES_WEAPON )
	{
		iGradType = ConvertPowerUpItemToItemGrade( m_ItemInfo.m_iItemCode );
		if ( !IsPowerUpItemGrade( iGradType ) )
			SafeSprintf( szText, sizeof( szText ), STR(1) );
		else 
		{
			ioHashString szGradeTitleName = g_PowerUpManager.GetItemGradeTitle( m_ItemInfo.m_iItemCode, iGradType );
			if( !szGradeTitleName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(2), szGradeTitleName.c_str() );
		}
	}
	else
	{
		iGradType = ConvertPowerUpRareItemToRareItemGrade( m_ItemInfo.m_iItemCode );
		if ( !IsPowerUpRareItemGrade( iGradType ) )
			SafeSprintf( szText, sizeof( szText ), STR(1) );
		else
		{
			ioHashString szGradeTitleName = g_PowerUpManager.GetExtraItemGradeTitle( m_ItemInfo.m_iItemCode, iGradType );
			if( !szGradeTitleName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(2), szGradeTitleName.c_str() );
		}
	}

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", m_ItemInfo.m_Name.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "%s", szText );

	int iSlot = m_ItemInfo.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	int iEnableClass = pItem->GetEnableClass();
	if ( iSlot == ES_WEAPON && iEnableClass > 0 )
	{
		SafeSprintf( szBuf, sizeof( szBuf ), STR(3), g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iGradType ) ) );
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "%s", szBuf );
	}

	return true;
}

bool ItemSelectInfoWnd::SetAccessoryInfo()
{
	if( m_AccessoryInfo.m_iAccessoryCode == 0 )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
		return false;
	}

	ShowChildWnd( ID_SELECT_BTN );

	ioUIRenderImage *pImg = NULL;
	ioHashString szIcon = m_AccessoryInfo.m_IconName;
	if( !szIcon.IsEmpty() )
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );

	pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
	pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );

	// desc
	ClearDesc();

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", m_AccessoryInfo.GetName().c_str() );
	return true;
}

bool ItemSelectInfoWnd::SetSpiritInfo()
{
	if( m_SpiritInfo.m_iCode == 0 )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	m_bSoldier = g_MyInfo.IsCharMortmain( m_SpiritInfo.m_iCode );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
		return false;
	}

	ShowChildWnd( ID_SELECT_BTN );

	ioUIRenderImage *pImg = NULL;
	ioHashString szIcon = m_SpiritInfo.m_szIcon;
	if( !szIcon.IsEmpty() )
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );

	pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
	pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );

	// desc
	ClearDesc();

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", m_SpiritInfo.m_szName.c_str() );

	int iMaxCount = g_SpiritMgr.GetComposeMax( m_SpiritInfo.m_iCode );
	char szBuf[MAX_PATH] = "";
	if( iMaxCount != 0 )
		wsprintf_e( szBuf, "%d/%d", m_SpiritInfo.m_iCount, iMaxCount );
	else
		wsprintf_e( szBuf, "%d", m_SpiritInfo.m_iCount );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szBuf );
	return true;
}

void ItemSelectInfoWnd::RenderDesc( int iXPos, int iYPos )
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

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			m_szDescArray[i].ClearList();
		}

		m_szDescArray[0].SetTextStyle( TS_NORMAL );
		m_szDescArray[0].SetBkColor( 0, 0, 0 );	
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "없음" );
		m_szDescArray[0].PrintFullTextWidthCut( iXPos, iYPos, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "" );
		m_szDescArray[2].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
	else
	{
		int iDescCnt = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			iDescCnt++;
		}

		if( iDescCnt == 3 )
			iYPos += Y_OFFSET_SECOND;

		int iCnt  = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + ( Y_LINE_GAP * iCnt ), TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
			iCnt++;
		}
	}
}

void ItemSelectInfoWnd::OnRender()
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
		if( m_pEmptyImg )
		{
			m_pEmptyImg->Render( iXPos+52, iYPos+49, UI_RENDER_MULTIPLY );
		}
	}
	else
		ioButton::OnRender();

	RenderDesc( iXPos, iYPos );
}

void ItemSelectInfoWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_bSoldier )
	{
		if( m_pBlueStateFrm )
			m_pBlueStateFrm->Render( iXPos + 7, iYPos + 7 );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 27, iYPos + 9, FONT_SIZE_11, "보유중" );
	}
}
//////////////////////////////////////////////////////////////////////////
MyItemSelecteWnd::MyItemSelecteWnd(void)
{
	m_dwCurTabID = ID_TAB_1_BTN;

	m_pEditFrm   = NULL;
	m_vAllItemList.clear();
	m_pPreSelectedWnd = NULL;
}

MyItemSelecteWnd::~MyItemSelecteWnd(void)
{
	SAFEDELETE( m_pEditFrm );
	m_pPreSelectedWnd = NULL;
	m_vAllItemList.clear();
}

void MyItemSelecteWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void MyItemSelecteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_TAB_1_BTN:
	case ID_TAB_2_BTN:
	case ID_TAB_3_BTN:
	case ID_TAB_4_BTN:
	case ID_TAB_5_BTN:
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
			if ( m_pPreSelectedWnd && param != 0 )
			{
				m_pPreSelectedWnd->iwm_command( this, IOBN_BTNUP, param );
				HideWnd();
			}
		}
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFind();
		}
		break;
	}
}

void MyItemSelecteWnd::ShowItemSeletWnd( ExtraItemList vExtraitemList, ioWnd* pPreSelectedWnd )
{
	m_vAllItemList = vExtraitemList;
	m_vCurItemList.clear();
	m_szCurrFindKeyword.Clear();

	ChangeTab( ID_TAB_1_BTN, "" );
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

void MyItemSelecteWnd::OnProcess( float fTimePerSec )
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

void MyItemSelecteWnd::ChangeTab( DWORD dwID, const char *szFind )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for (int i = ID_TAB_1_BTN; i < ID_TAB_5_BTN+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;

			pButton->SetClick( false );
		}
	}

	SetCurItemInfoVec( m_dwCurTabID, szFind );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
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

void MyItemSelecteWnd::SetCurItemInfoVec( DWORD dwTabID, const char *szFind )
{
	if( m_vAllItemList.empty() )
	{
		return;
	}

	ExtraItemList vTotalList = m_vAllItemList;
	m_vCurItemList.clear();

	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	int iSize = vTotalList.size();
	for( int i = 0; i < iSize ; i++ )
	{
		const EXTRAITEMSLOT& rkInfo = vTotalList[i];
		if( bFind )
		{
			ioHashString szItemName = rkInfo.m_Name;
			if( strstr( szItemName.c_str(), szFind ) == NULL )
				continue;
		}
		else
		{
			int iSlot = rkInfo.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
			if( !IsRightTab( dwTabID, iSlot ) )
				continue;
		}

		m_vCurItemList.push_back( vTotalList[i] );
	}

	if( m_vCurItemList.empty() )
		return;
}

bool MyItemSelecteWnd::IsRightTab( DWORD dwTabID, int iType )
{
	MyItemSelectRadioButton* pButton = dynamic_cast<MyItemSelectRadioButton*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		if ( pButton->GetTabType() == -1 )
			return true;
		else if ( pButton->GetTabType() == iType )
			return true;
	}
	return false;
}

void MyItemSelecteWnd::OnRender()
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

void MyItemSelecteWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	ChangeTab( m_dwCurTabID, m_szCurrFindKeyword.c_str() );
}

void MyItemSelecteWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;
	int iItemSize = m_vCurItemList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;

	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		ItemSelectInfoWnd *pItem = dynamic_cast<ItemSelectInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
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

		pItem->SetItemInfo( m_vCurItemList[ iCurArray ]);
	}
}

void MyItemSelecteWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void MyItemSelecteWnd::iwm_wheel( int zDelta )
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
				pScroll->SetScrollPos( m_iCurPage - 1 );

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
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
AccessorySelectWnd::AccessorySelectWnd(void)
{
	m_dwCurTabID = ID_TAB_1_BTN;

	m_pEditFrm   = NULL;
	m_pPreSelectedWnd = NULL;
}

AccessorySelectWnd::~AccessorySelectWnd(void)
{
	SAFEDELETE( m_pEditFrm );
	m_pPreSelectedWnd = NULL;
}

void AccessorySelectWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void AccessorySelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_TAB_1_BTN:
	case ID_TAB_2_BTN:
	case ID_TAB_3_BTN:
	case ID_TAB_4_BTN:
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
			if ( m_pPreSelectedWnd && param != 0 )
			{
				m_pPreSelectedWnd->iwm_command( this, IOBN_BTNUP, param );
				HideWnd();
			}
		}
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFind();
		}
		break;
	}
}

void AccessorySelectWnd::iwm_hide()
{
	if( m_pPreSelectedWnd )
		m_pPreSelectedWnd->iwm_command( this, IOBN_BTNUP, 0 );
}

void AccessorySelectWnd::ShowSeletWnd( ioWnd* pPreSelectedWnd, int iSelectIndex1, int iSelectIndex2, int iSelectIndex3, int iSelectIndex4, bool bShowCompound )
{
	m_szCurrFindKeyword.Clear();
	m_vCurItemList.clear();
	m_iSelectIndex[0] = iSelectIndex1;
	m_iSelectIndex[1] = iSelectIndex2;
	m_iSelectIndex[2] = iSelectIndex3;
	m_iSelectIndex[3] = iSelectIndex4;
	m_bShowCompound = bShowCompound;

	ChangeTab( ID_TAB_1_BTN, "" );
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

void AccessorySelectWnd::OnProcess( float fTimePerSec )
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

void AccessorySelectWnd::ChangeTab( DWORD dwID, const char *szFind )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_TAB_1_BTN, ID_TAB_4_BTN, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for (int i = ID_TAB_1_BTN; i < ID_TAB_4_BTN+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;

			pButton->SetClick( false );
		}
	}

	SetCurItemInfoVec( m_dwCurTabID, szFind );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = 0;
		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void AccessorySelectWnd::SetCurItemInfoVec( DWORD dwTabID, const char *szFind )
{
	m_vCurItemList.clear();

	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory ) 
		return;

	int nSize = pAccessory->GetAccessorySlotCnt();
	for( int i=0; i<nSize; ++i )
	{
		AccessorySlot kSlot;
		if( !pAccessory->GetAccessorySlotArray( i, kSlot ) )
			continue;
		if( kSlot.m_bEquip )
			continue;
		if( kSlot.m_iIndex == m_iSelectIndex[0] || kSlot.m_iIndex == m_iSelectIndex[1] ||
			kSlot.m_iIndex == m_iSelectIndex[2] || kSlot.m_iIndex == m_iSelectIndex[3] )
			continue;
		if( !m_bShowCompound && kSlot.m_iCompoundCode != 0 )
			continue;

		if( bFind )
		{
			ioHashString szItemName = kSlot.GetName();
			if( strstr( szItemName.c_str(), szFind ) == NULL )
				continue;
		}
		else
		{
			int nGroup = (kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE);
			if( !IsRightTab( dwTabID, nGroup ) )
				continue;
		}		

		m_vCurItemList.push_back( kSlot );
	}
	
	if( m_vCurItemList.empty() )
		return;

	if( dwTabID == ID_TAB_1_BTN )
		std::sort( m_vCurItemList.begin(), m_vCurItemList.end(), AccessorySlotSort2() );
	else
		std::sort( m_vCurItemList.begin(), m_vCurItemList.end(), AccessorySlotSort3() );
}

bool AccessorySelectWnd::IsRightTab( DWORD dwTabID, int iType )
{
	MyItemSelectRadioButton* pButton = dynamic_cast<MyItemSelectRadioButton*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		if ( pButton->GetTabType() == 0 )
			return true;
		else if ( pButton->GetTabType() == iType )
			return true;
	}
	return false;
}

void AccessorySelectWnd::OnRender()
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

void AccessorySelectWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	ChangeTab( m_dwCurTabID, m_szCurrFindKeyword.c_str() );
}

void AccessorySelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;
	int iItemSize = m_vCurItemList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		ItemSelectInfoWnd *pItem = dynamic_cast<ItemSelectInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray + i;

		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			pItem->SetBlank();
			continue;
		}
		pItem->SetAccessoryInfo( m_vCurItemList[ iCurArray ]);
	}
}

void AccessorySelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void AccessorySelectWnd::iwm_wheel( int zDelta )
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
				pScroll->SetScrollPos( m_iCurPage - 1 );

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
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
MySpiritSelectWnd::MySpiritSelectWnd(void)
{
	m_pEditFrm   = NULL;
	m_vAllItemList.clear();
	m_pPreSelectedWnd = NULL;
	m_iSpiritCode = 0;
}

MySpiritSelectWnd::~MySpiritSelectWnd(void)
{
	SAFEDELETE( m_pEditFrm );
	m_pPreSelectedWnd = NULL;
	m_vAllItemList.clear();
}

void MySpiritSelectWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void MySpiritSelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szBuf[MAX_PATH];
	for( int i = 0; i < SPIRIT_TYPE_COUNT; ++i )
	{
		wsprintf_e( szBuf, "Name_%d", i + 1 );
		m_szSpiritTypeName[i] = xElement.GetStringAttribute( szBuf );
	}
	m_szDesc = xElement.GetStringAttribute_e( "Desc" );
}

void MySpiritSelectWnd::iwm_create()
{
	for( int i = 0; i < MAX_ITEM_INFO_WND_BTN; ++i )
	{
		int iXIndex = i % PAGE_PER_ITEM_NUM;
		int iYIndex = i / PAGE_PER_ITEM_NUM;
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("ItemSelectInfoWnd"), this, ID_ITEM_INFO_WND_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 17 + iXIndex * 106, 82 + iYIndex * 181 );
	}
}

void MySpiritSelectWnd::iwm_show()
{
	for( int i = 0; i < MAX_ITEM_INFO_WND_BTN; ++i )
	{
		int iXIndex = i % PAGE_PER_ITEM_NUM;
		int iYIndex = i / PAGE_PER_ITEM_NUM;
		ioWnd *pWnd = FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 17 + iXIndex * 106, 82 + iYIndex * 181 );
	}
}

void MySpiritSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			if ( m_pPreSelectedWnd && param != 0 )
			{
				m_pPreSelectedWnd->iwm_command( this, IOBN_BTNUP, param );
				HideWnd();
			}
		}
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFind();
		}
		break;
	}
}

void MySpiritSelectWnd::ShowItemSeletWnd( int iSpiritCode, ioWnd* pPreSelectedWnd )
{
	int iSpiritType = g_SpiritMgr.GetSpiritType( iSpiritCode );
	g_SpiritMgr.GetSpiritInfo( m_vAllItemList, iSpiritType );
	std::sort( m_vAllItemList.begin(), m_vAllItemList.end(), SpiritSort() );

	m_iSpiritCode = iSpiritCode;
	m_vCurItemList.clear();
	m_szCurrFindKeyword.Clear();

	ioWnd *pWnd = FindChildWnd( ID_TAB_BTN );
	if( pWnd && COMPARE( iSpiritType, ioUserSpiritManager::SPT_UNIQUE, ioUserSpiritManager::SPT_NORMAL + 1 ) )
		pWnd->SetTitleText( m_szSpiritTypeName[iSpiritType - 1].c_str() );

	SearchText( "" );
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

void MySpiritSelectWnd::OnProcess( float fTimePerSec )
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

void MySpiritSelectWnd::SearchText( const char *szFind )
{
	SetCurItemInfoVec( szFind );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
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

void MySpiritSelectWnd::SetCurItemInfoVec( const char *szFind )
{
	if( m_vAllItemList.empty() )
	{
		return;
	}

	vAlchemicItemInfoList vTotalList = m_vAllItemList;
	m_vCurItemList.clear();

	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	int iSize = vTotalList.size();
	for( int i = 0; i < iSize ; i++ )
	{
		const AlchemicItemInfo& rkInfo = vTotalList[i];
		if( bFind )
		{
			ioHashString szItemName = rkInfo.m_szName;
			if( strstr( szItemName.c_str(), szFind ) == NULL )
				continue;
		}
		if( m_iSpiritCode == vTotalList[i].m_iCode )
			continue;

		m_vCurItemList.push_back( vTotalList[i] );
	}

	if( m_vCurItemList.empty() )
		return;
}

void MySpiritSelectWnd::OnRender()
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

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_RIGHT );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 459, iYPos + 59, FONT_SIZE_12, m_szDesc.c_str() );
}

void MySpiritSelectWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	SearchText( m_szCurrFindKeyword.c_str() );
}

void MySpiritSelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;
	int iItemSize = m_vCurItemList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;

	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		ItemSelectInfoWnd *pItem = dynamic_cast<ItemSelectInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
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

		pItem->SetSpiritInfo( m_vCurItemList[ iCurArray ]);
	}
}

void MySpiritSelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void MySpiritSelectWnd::iwm_wheel( int zDelta )
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
				pScroll->SetScrollPos( m_iCurPage - 1 );

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
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}