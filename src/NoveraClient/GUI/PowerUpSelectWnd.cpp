#include "StdAfx.h"
#include "PowerUpSelectWnd.h"
#include "ReinforceSelectWnd.h"

PowerUpSelectWnd::PowerUpSelectWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;

	m_pEditFrm   = NULL;
	m_pDotLine   = NULL;

	m_iTabLineSize = 0;
	m_iTabPosX     = 0;

	m_SelectPowerUpInfoVec.clear();
	m_CurrSelectPowerUpInfoVec.clear();
}

PowerUpSelectWnd::~PowerUpSelectWnd()
{
	SAFEDELETE( m_pEditFrm );
	SAFEDELETE( m_pDotLine );
	m_SelectPowerUpInfoVec.clear();
	m_CurrSelectPowerUpInfoVec.clear();
}

void PowerUpSelectWnd::ShowSubWnd( int iPowerUpInfoType )
{
	if( !COMPARE( iPowerUpInfoType, ioPowerUpManager::PIT_HERO, ioPowerUpManager::PIT_MAX ) )
		return;

	m_InfoType = (ioPowerUpManager::PowerUpInfoType)iPowerUpInfoType;
	
	SetAllItemInfoVec();

	const TabUISettingVec& TabVec = g_PowerUpManager.GetTabUISettinVec( iPowerUpInfoType );
	int iTabCount = (int)TabVec.size();
	int i = 0;
	if ( m_InfoType == ioPowerUpManager::PIT_HERO )
	{
		for( int iWnd = ID_TAB_1_BTN; iWnd < ID_TAB_1_BTN + MAX_HERO_TAB_BTN; ++iWnd, ++i )
		{
			PowerUpCategoryTab* pButton = dynamic_cast<PowerUpCategoryTab*>( FindChildWnd( iWnd ) );
			if( pButton )
			{
				if( i < (int)TabVec.size() )
					pButton->SetTab( TabVec[i] );
				else
					pButton->ClearTab();
			}
		}

		for( int iWnd = ID_TAB_7_BTN; iWnd < ID_TAB_7_BTN + MAX_ITEM_TAB_BTN; ++iWnd )
			HideChildWnd( iWnd );

		m_iTabLineSize = max(0, MAX_HERO_TAB_BTN - iTabCount) * 73;
		m_iTabPosX     = iTabCount * 73;
		ChangeTab( ID_TAB_1_BTN, "" );
	}
	else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
	{
		for( int iWnd = ID_TAB_7_BTN; iWnd < ID_TAB_7_BTN + MAX_ITEM_TAB_BTN; ++iWnd, ++i )
		{
			PowerUpCategoryTab* pButton = dynamic_cast<PowerUpCategoryTab*>( FindChildWnd( iWnd ) );
			if( pButton )
			{
				if( i < (int)TabVec.size() )
					pButton->SetTab( TabVec[i] );
				else
					pButton->ClearTab();
			}
		}

		for( int iWnd = ID_TAB_1_BTN; iWnd < ID_TAB_1_BTN + MAX_HERO_TAB_BTN; ++iWnd )
			HideChildWnd( iWnd );

		m_iTabLineSize = max(0, MAX_ITEM_TAB_BTN - iTabCount) * 87;
		m_iTabPosX     = iTabCount * 87;
		ChangeTab( ID_TAB_7_BTN, "" );
	}

	m_szCurrFindKeyword.Clear();

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	m_TitlePrinter.ClearList();
	m_TitlePrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_TitlePrinter.SetBkColor( 12, 66, 111 );
	m_TitlePrinter.SetTextColor( TCT_DEFAULT_WHITE );
	if ( m_InfoType == ioPowerUpManager::PIT_HERO )
		m_TitlePrinter.AddTextPiece( FONT_SIZE_18, STR(1) );
	else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
		m_TitlePrinter.AddTextPiece( FONT_SIZE_18, STR(2) );

	ShowWnd();
}

void PowerUpSelectWnd::SetAllItemInfoVec()
{
	char szBuf[MAX_PATH], szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	memset( szBuf, 0, sizeof(szBuf) );
	m_SelectPowerUpInfoVec.clear();

	if ( m_InfoType == ioPowerUpManager::PIT_HERO )
	{
		for( int i = 0; i < g_MyInfo.GetCharCount(); ++i )
		{
			int iClassType = g_MyInfo.GetClassType( i );
			ioHashString szError;
			int iUseMtrlCount,iCurMtrlCount;
			if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,iClassType, iUseMtrlCount, iCurMtrlCount ) )
				continue;
			if ( !g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError) )
				continue;
			int iSetIdx = iClassType - 1;
			const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
			if( !pInfo )
				continue;

			if( !g_MyInfo.IsCharMortmain( iClassType ) )
				continue;

			int iGradType = g_MyInfo.GetCharPowerUpGradeByClassType( iClassType );
			if( iGradType == PUGT_NONE )
				SafeSprintf( szText, sizeof( szText ), STR(1) );
			else
			{
				ioHashString szGradeTitle = g_PowerUpManager.GetCharGradeTitle( iClassType, iGradType );
				if ( szGradeTitle.IsEmpty() )
					return;
				SafeSprintf( szText, sizeof( szText ), STR(3), szGradeTitle.c_str() );
			}

			int iLevel = g_MyInfo.GetClassLevel( iClassType, false );
			SafeSprintf( szBuf, sizeof( szBuf ), "Lv%d %s", iLevel, g_MyInfo.GetClassName( iClassType ) );
			
			PowerUpInfo PowerupInfo;
			PowerupInfo.m_iType = m_InfoType;
			PowerupInfo.m_iTabType = pInfo->GetCharAttackType();
			PowerupInfo.m_iValue = iClassType;
			PowerupInfo.m_szName = szBuf;
			PowerupInfo.m_szSubDesc = szText;
			PowerupInfo.m_szIconName = g_MyInfo.GetMySoldierIconName( iClassType );
			PowerupInfo.m_szSubIconName = g_MyInfo.GetSoldierSubIconName( iClassType );
			PowerupInfo.m_iPriorityOrder = iGradType * 1000000 + iClassType ;
			m_SelectPowerUpInfoVec.push_back( PowerupInfo );
			memset( szText, 0, sizeof(szText) );
			memset( szBuf, 0, sizeof(szBuf) );
		}
	}
	else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
	{
		// solt check
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
			return;

		int cnt = pExtraItem->GetExtraItemCnt();
		for ( int i=0 ; i < cnt ; ++i )
		{
			EXTRAITEMSLOT ExtraItem;
			ExtraItem.Init();
			if ( !pExtraItem->GetExtraItemArray( i, ExtraItem ) )
				continue;

			int iUseMtrlCount,iCurMtrlCount;
			if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM ,ExtraItem.m_iItemCode, iUseMtrlCount, iCurMtrlCount ) )
				continue;

			ioHashString szError;
			if ( !g_PowerUpManager.CheckEnableItemPowerUp( ExtraItem.m_iIndex, szError) )
				continue;

			int iEquipSlotIndex = ExtraItem.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
			int iGradType;
			if ( iEquipSlotIndex == ES_WEAPON )
				iGradType = ConvertPowerUpItemToItemGrade( ExtraItem.m_iItemCode );
			else
				iGradType = ConvertPowerUpRareItemToRareItemGrade( ExtraItem.m_iItemCode );

			const ioItem *pItem = g_ItemMaker.GetItemConst( ExtraItem.m_iItemCode, __FUNCTION__ );
			if( !pItem )
				return;

			int iEnableClass = pItem->GetEnableClass();
			memset( szText, 0, sizeof(szText) );
			if( iGradType == PUGT_ITEM_GRADE_BASE || iGradType == PURIGT_RARE_ITEM_GRADE_BASE )
				SafeSprintf( szText, sizeof( szText ), STR(2) );
			else 
			{
				if ( iEquipSlotIndex == ES_WEAPON )
				{
					ioHashString szGradeTitleName = g_PowerUpManager.GetItemGradeTitle( ExtraItem.m_iItemCode, iGradType );
					if( !szGradeTitleName.IsEmpty() )
						SafeSprintf( szText, sizeof( szText ), STR(3), szGradeTitleName.c_str() );
				}
				else
				{
					ioHashString szGradeTitleName = g_PowerUpManager.GetExtraItemGradeTitle( ExtraItem.m_iItemCode, iGradType );
					if( !szGradeTitleName.IsEmpty() )
						SafeSprintf( szText, sizeof( szText ), STR(3), szGradeTitleName.c_str() );
				}
			}

			PowerUpInfo PowerupInfo;
			PowerupInfo.m_iType = m_InfoType;
			PowerupInfo.m_iTabType = iEquipSlotIndex + 1;
			PowerupInfo.m_iValue = ExtraItem.m_iIndex;
			PowerupInfo.m_iValue2 = ExtraItem.m_iReinforce;
			PowerupInfo.m_szName = ExtraItem.m_Name;
			PowerupInfo.m_szSubDesc = szText;
			PowerupInfo.m_szIconName = ExtraItem.m_IconName;
			PowerupInfo.m_iPriorityOrder = iGradType * 1000000 + ExtraItem.m_iItemCode;

			memset( szText, 0, sizeof(szText) );
			
			if ( iEquipSlotIndex == ES_WEAPON )
			{
				SafeSprintf( szText, sizeof( szText ), STR(4), g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iGradType ) ) );
				PowerupInfo.m_szWeaponDesc = szText;
			}

			m_SelectPowerUpInfoVec.push_back( PowerupInfo );
			memset( szText, 0, sizeof(szText) );
			memset( szBuf, 0, sizeof(szBuf) );
		}
	}
}

void PowerUpSelectWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}
}

void PowerUpSelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void PowerUpSelectWnd::iwm_wheel( int zDelta )
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

void PowerUpSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_TAB_6_BTN:
	case ID_TAB_7_BTN:
	case ID_TAB_8_BTN:
	case ID_TAB_9_BTN:
	case ID_TAB_10_BTN:
	case ID_TAB_11_BTN:
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
			if ( param == PowerUpSelectInfoWnd::ID_SELECT_BTN)
				HideWnd();
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

void PowerUpSelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;
	int iItemSize = m_CurrSelectPowerUpInfoVec.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;

	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		PowerUpSelectInfoWnd *pItem = dynamic_cast<PowerUpSelectInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
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
		
		pItem->SetPowerUpInfo( m_CurrSelectPowerUpInfoVec[ iCurArray ]);
	}
}

void PowerUpSelectWnd::ChangeTab( DWORD dwID, const char *szFind  )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_TAB_1_BTN, ID_TAB_11_BTN, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for (int i = ID_TAB_1_BTN; i < ID_TAB_11_BTN+1 ; i++)
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
		int iItemSize = m_SelectPowerUpInfoVec.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}	
}

void PowerUpSelectWnd::SetCurItemInfoVec( DWORD dwTabID, const char *szFind )
{
	if( m_SelectPowerUpInfoVec.empty() )
	{
		return;
	}
	
	vPowerUpInfo vTotalList = m_SelectPowerUpInfoVec;
	m_CurrSelectPowerUpInfoVec.clear();

	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	int iSize = vTotalList.size();
	for( int i = 0; i < iSize ; i++ )
	{
		const PowerUpInfo& rkInfo = vTotalList[i];
		if( bFind )
		{
			ioHashString szItemName = rkInfo.m_szName;
			if( strstr( szItemName.c_str(), szFind ) == NULL )
				continue;
		}
		else
		{
			if( !IsRightTab( dwTabID, rkInfo.m_iTabType ) )
				continue;
		}

		m_CurrSelectPowerUpInfoVec.push_back( vTotalList[i] );
	}

	if( m_CurrSelectPowerUpInfoVec.empty() )
		return;

	PowerUpCategoryTab* pButton = dynamic_cast<PowerUpCategoryTab*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		const TabUISetting& rkSetting = pButton->GetTab();
		switch( rkSetting.m_iOrder )
		{
		case TabUISetting::ORDER_BY_SETTING:
			{
				std::sort( m_CurrSelectPowerUpInfoVec.begin(), m_CurrSelectPowerUpInfoVec.end(), PowerUpInfoSortByPriority() );
			}
			break;
		default:
			{
				std::sort( m_CurrSelectPowerUpInfoVec.begin(), m_CurrSelectPowerUpInfoVec.end(), PowerUpInfoSortByPriority() );
			}
			break;
		}
	}
}

void PowerUpSelectWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	enum
	{
		X_TITLE_OFFSET = 13,
		Y_TITLE_OFFSET = 11,

		X_OFFSET = 17,
		Y_OFFSET = 474,
	};

	if( m_pEditFrm )
		m_pEditFrm->Render( iXPos+X_OFFSET, iYPos+Y_OFFSET );

	if( m_pDotLine )
	{
		m_pDotLine->SetSize( m_iTabLineSize, 2 );
		m_pDotLine->Render( iXPos + m_iTabPosX, iYPos );
	}

	m_TitlePrinter.PrintFullText( iXPos + X_TITLE_OFFSET, iYPos + Y_TITLE_OFFSET, TAT_LEFT );
}

bool PowerUpSelectWnd::IsRightTab( DWORD dwTabID, int iType )
{
	PowerUpCategoryTab* pButton = dynamic_cast<PowerUpCategoryTab*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		const TabUISetting& rkSetting = pButton->GetTab();
		for( TabFiterVec::const_iterator iter = rkSetting.m_FilterVec.begin(); iter != rkSetting.m_FilterVec.end(); ++iter )
		{
			const TabFiter& Filter = *iter;
			if( Filter.m_iTypeFilter == 0 )
				return true;
			else if( Filter.m_iTypeFilter == iType )
				return true;
		}
	}
	return false;
}

void PowerUpSelectWnd::Update()
{
	UpdatePage( m_iCurPage );
}

void PowerUpSelectWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	ChangeTab( m_dwCurTabID, m_szCurrFindKeyword.c_str() );
}

void PowerUpSelectWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void PowerUpSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TabDot" )
	{
		SAFEDELETE( m_pDotLine );
		m_pDotLine = pImage;		
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void PowerUpSelectWnd::OnProcess( float fTimePerSec )
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PowerUpSelectInfoWnd::PowerUpSelectInfoWnd()
{
	m_pEmptyImg = NULL;
	m_nGradeType = 0;
}

PowerUpSelectInfoWnd::~PowerUpSelectInfoWnd()
{
	SAFEDELETE(m_pEmptyImg);
}

void PowerUpSelectInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_HERO )
			{
				ioHashString szError;
				szError.Clear();
				int iClassType = m_PowerUpInfo.m_iValue;
				int iUseMtrlCount,iCurMtrlCount;
				if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,iClassType, iUseMtrlCount, iCurMtrlCount ) )
				{
					SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
					if ( pSoulstoneBuyWnd )
						pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
					break;
				}
				// PowerUp이 가능한지 체크
				if( !g_PowerUpManager.CheckEnableCharPowerUp( m_PowerUpInfo.m_iValue, szError ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
					break;
				}

				HideWnd();
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->IsShow() )
					pInvenWnd->ShowHeroReinforceDownWnd( iClassType );
			}
			else if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_ITEM )
			{
				ioHashString szError;
				szError.Clear();

				int iUseMtrlCount,iCurMtrlCount;
				ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
				if( !pExtraItem )
					return;

				EXTRAITEMSLOT Slot;
				if( !pExtraItem->GetExtraItem( m_PowerUpInfo.m_iValue, Slot ) )
					return;

				if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM , Slot.m_iItemCode, iUseMtrlCount, iCurMtrlCount ) )
				{
					SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
					if ( pSoulstoneBuyWnd )
						pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
					return;
				}

				if ( !g_PowerUpManager.CheckEnableItemPowerUp( m_PowerUpInfo.m_iValue, szError ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
					return;
				}

				HideWnd();
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->IsShow() )
					pInvenWnd->ShowItemReinforceDownWnd( m_PowerUpInfo.m_iValue );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, this, "타입 에러" );
				break;
			}
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, ID_SELECT_BTN );
		}
		break;
	}
}

void PowerUpSelectInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void PowerUpSelectInfoWnd::SetBlank()
{
	SetInActive();

	m_PowerUpInfo.Clear();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_SELECT_BTN );

	ClearDesc();

	m_nGradeType = 0;
}

void PowerUpSelectInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void PowerUpSelectInfoWnd::OnRender()
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

void PowerUpSelectInfoWnd::RenderDesc( int iXPos, int iYPos )
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

void PowerUpSelectInfoWnd::SetPowerUpInfo( const PowerUpInfo& rkItem )
{
	SetBlank();
	SetActive();

	m_PowerUpInfo = rkItem;

	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}

bool PowerUpSelectInfoWnd::SetInfo()
{
	if( m_PowerUpInfo.IsEmpty() )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	PowerUpSelectIconBtn *pBtn = dynamic_cast< PowerUpSelectIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );

	ioUIRenderImage *pImg = NULL;
	ioHashString szIcon = m_PowerUpInfo.m_szIconName;
	if( !szIcon.IsEmpty() )
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );

	ioUIRenderImage *pSubImg = NULL;
	ioHashString szSubIcon = m_PowerUpInfo.m_szSubIconName;
	if( !szSubIcon.IsEmpty() )
		pSubImg = g_UIImageSetMgr.CreateImageByFullName( szSubIcon );

	pBtn->SetIcon( pImg, FLOAT1, FLOAT1, m_PowerUpInfo.m_iValue2 );
	pBtn->SetSubIcon( pSubImg );
	ShowChildWnd( ID_SELECT_BTN );

	// desc
	ClearDesc();

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", m_PowerUpInfo.m_szName.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "%s", m_PowerUpInfo.m_szSubDesc.c_str() );

	m_szDescArray[2].ClearList();
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "%s", m_PowerUpInfo.m_szWeaponDesc.c_str() );
	
	if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_HERO )
		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_PowerUpInfo.m_iValue );
	else if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_ITEM )
	{
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( pExtraItem )
		{
			EXTRAITEMSLOT ExtraItem;
			if ( pExtraItem->GetExtraItem( m_PowerUpInfo.m_iValue, ExtraItem ) )
			{
				const ioItem *pItem = g_ItemMaker.GetItemConst( ExtraItem.m_iItemCode, __FUNCTION__ );
				if( pItem )
					m_nGradeType = pItem->GetGradeType();
			}
		}
	}

	pBtn->SetGradeType( m_nGradeType );

	return true;
}

void PowerUpSelectInfoWnd::SetItem()
{
	SetBlank();
	SetActive();

	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PowerUpSelectIconBtn::PowerUpSelectIconBtn()
{
	m_pIcon       = NULL;
	m_pSubIcon = NULL;
	m_pIconBack   = NULL;
	m_pNumText = NULL;
	m_pPlusText = NULL;
	m_iItemReinforce = 0;

	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;
	m_nGradeType = 0;
}

PowerUpSelectIconBtn::~PowerUpSelectIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
}

void PowerUpSelectIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
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
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void PowerUpSelectIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void PowerUpSelectIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void PowerUpSelectIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void PowerUpSelectIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void PowerUpSelectIconBtn::RenderBack( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( m_fBackScale );

		if( m_fBackScale != FLOAT1 )
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void PowerUpSelectIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,

		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) )
			eNewType = UI_RENDER_GRAY;

		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eNewType );

		if( m_fScale != FLOAT1 )
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		else
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType );

		if( m_pSubIcon )
		{
			m_pSubIcon->SetScale( m_fScale );
			m_pSubIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		}
		if( m_pNumText && m_pPlusText && m_iItemReinforce > 0 )
		{
			int iCurPos = 0;
			int iDefaultPos = iNewXPos;

			int iCurYPos = iNewYPos+(REINFORCE_Y * m_fScale);

			int iTotalSize = 0;
			if( m_iItemReinforce < 10 )
				iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
			else if( m_iItemReinforce < 100 )
				iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
			else
				iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

			iCurPos = iDefaultPos - (iTotalSize / 2);

			m_pPlusText->Render( iCurPos, iCurYPos );

			iCurPos += (PLUS_SIZE-NUM_GAP);

			m_pNumText->RenderNum( iCurPos, iCurYPos, m_iItemReinforce, -NUM_GAP );
		}
	}
}

bool PowerUpSelectIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int ireinforce )
{
	m_fScale = fScale;
	m_fBackScale = fBackScale;
	m_iItemReinforce = ireinforce;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	if( !pIcon )
	{
		return false;
	}

	m_pIcon = pIcon;

	return true;
}

bool PowerUpSelectIconBtn::SetSubIcon( ioUIRenderImage *pSubIcon )
{
	SAFEDELETE( m_pSubIcon );
	m_pSubIcon = pSubIcon;
	return true;
}
//////////////////////////////////////////////////////////////////////////
PowerUpCategoryTab::PowerUpCategoryTab()
{
}

PowerUpCategoryTab::~PowerUpCategoryTab()
{
}

void PowerUpCategoryTab::ClearTab()
{
	m_Setting.Clear();
	HideWnd();
}

void PowerUpCategoryTab::SetTab( const TabUISetting& rkTab ) 
{
	m_Setting = rkTab;
	SetTitleText( rkTab.m_szTtitle.c_str() );
	ShowWnd();
}