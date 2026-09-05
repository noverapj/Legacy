#include "StdAfx.h"

#include "../ioItemRechargeManager.h"
#include "../ioDateHelp.h"

#include "ItemRechargeWnd.h"

//////////////////////////////////////////////////////////////////////////
ItemIconButton::ItemIconButton()
{
	m_pIcon     = NULL;
	m_pIconBack = NULL;

	m_pNumText  = NULL;
	m_pPlusText = NULL;

	m_pStateBlueFrm = NULL;

	m_iNum = 0;

	m_bEquip = false;
	m_bRenderGray = false;
	m_nGradeType = 0;
}

ItemIconButton::~ItemIconButton()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );

	SAFEDELETE( m_pStateBlueFrm );
}

void ItemIconButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemIconButton::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StateBlueFrm" )
	{
		SAFEDELETE( m_pStateBlueFrm );
		m_pStateBlueFrm = pFrame;
	}
	else
	{	
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void ItemIconButton::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemIconButton::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemIconButton::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemIconButton::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemIconButton::RenderBack( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pIconBack )
	{
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void ItemIconButton::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIcon )
	{
		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) || m_bRenderGray )
			eNewType = UI_RENDER_GRAY;

		if ( eNewType != UI_RENDER_SCREEN )
			g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, FLOAT1, eNewType );

		m_pIcon->Render( iNewXPos, iNewYPos, eNewType );
	}
}

void ItemIconButton::RenderOtherInfo( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Mark
	int iMarkX = iNewXPos;
	int iMarkY = iNewYPos;

	int iTextX = iMarkX;
	int iTextY = iMarkY;

	if( m_bEquip && m_pStateBlueFrm )
	{
		// 장착 중
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		m_pStateBlueFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+2;

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, "장착중" );
	}

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	if( m_pNumText && m_pPlusText && m_iNum > 0 )
	{
		int iCurPos = 0;
		int iDefaultPos = iNewXPos;

		int iCurYPos = iNewYPos+REINFORCE_Y;

		int iTotalSize = 0;
		if( m_iNum < 10 )
			iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
		else if( m_iNum < 100 )
			iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
		else
			iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

		iCurPos = iDefaultPos - (iTotalSize / 2);

		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (PLUS_SIZE-NUM_GAP);

		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iNum, -NUM_GAP );
	}
}

bool ItemIconButton::SetIcon( ioUIRenderImage *pIcon, int iNum )
{
	m_iNum = iNum;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	if( !pIcon )
	{
		m_iNum = 0;
		return false;
	}

	m_pIcon = pIcon;

	return true;
}
//////////////////////////////////////////////////////////////////////////
ItemInfoButton::ItemInfoButton()
{
	m_ItemInfo.Init();
	m_iExtraItemIndex = 0;

	m_pEmptyImg = NULL;
}

ItemInfoButton::~ItemInfoButton()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void ItemInfoButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemInfoButton::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ICON_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, IOBN_BTNUP, param );
		}
		break;
	}
}

void ItemInfoButton::SetBlank()
{
	m_ItemInfo.Init();
	m_iExtraItemIndex = 0;

	SetInActive();

	HideChildWnd( ID_ICON_BTN );

	ClearDesc();
}

void ItemInfoButton::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

bool ItemInfoButton::IsIconOver()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_ICON_BTN ) )
		return true;
	return false;
}

void ItemInfoButton::OnRender()
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
	{
		ioButton::OnRender();
	}

	RenderDesc( iXPos, iYPos );
}

void ItemInfoButton::RenderDesc( int iXPos, int iYPos )
{
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
		m_szDescArray[0].PrintFullTextWidthCut( iXPos + 51, iYPos + 105, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos + 51, iYPos + 121, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
	else
	{
		m_szDescArray[0].PrintFullTextWidthCut( iXPos + 53, iYPos + 97,  TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos + 53, iYPos + 113, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
		m_szDescArray[2].PrintFullTextWidthCut( iXPos + 53, iYPos + 128, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
}

void ItemInfoButton::SetInfo( InventoryItemInfo &rkInfo )
{
	SetBlank();
	SetActive();

	m_ItemInfo = rkInfo;
	if( !SetExtraItemInfo() )
	{
		SetBlank();
		return;
	}
}

InventoryItemInfo& ItemInfoButton::GetInfo()
{
	return m_ItemInfo;
}

bool ItemInfoButton::SetExtraItemInfo()
{
	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return false;

	if( !pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	ItemIconButton *pBtn = dynamic_cast< ItemIconButton* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, kSlot.m_iReinforce );

		const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
		if ( pItem )
			pBtn->SetGradeType( pItem->GetGradeType() );

		pBtn->SetEquip( kSlot.m_bCharEquip );
	}

	m_iExtraItemIndex = kSlot.m_iIndex;

	// desc
	ClearDesc();

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", kSlot.m_Name.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );

	char szText1[MAX_PATH] = "";
	Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szText1, sizeof(szText1) );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szText1 );
	
	// Limit
	int iLimitLevel = kSlot.m_iReinforce * g_ExtraItemInfoMgr.GetLevelLimitConst();
	int iEnableClass = 0;
	int iPowerUpItemGrade = ConvertPowerUpItemToItemGrade( kSlot.m_iItemCode );

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
	if( pItem )
		iEnableClass = pItem->GetEnableClass();

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( iEnableClass > 0 )
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "Lv%d %s 제한", iLimitLevel, g_MyInfo.GetClassName(iEnableClass, ConvertPowerUpItemToChar( iPowerUpItemGrade ) ) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "Lv%d 제한", iLimitLevel );

	return true;
}
//////////////////////////////////////////////////////////////////////////
ItemRechargeWnd::ItemRechargeWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

ItemRechargeWnd::~ItemRechargeWnd()
{
	m_vInventoryItemInfoList.clear();
}

void ItemRechargeWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/itemrechargeprocesswnd.xml", this );
}

void ItemRechargeWnd::iwm_show()
{
	ChangeTab( ID_TAB_1_BTN );
}

void ItemRechargeWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	RechargeItemTooltip *pTooltip = dynamic_cast<RechargeItemTooltip*>(FindChildWnd( ID_TOOL_TIP ));
	if( pTooltip )
	{
		ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
		pTooltip->SetTooltipText( kPrinter );
	}
}

void ItemRechargeWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void ItemRechargeWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioMovingWnd::iwm_wheel( zDelta );
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

void ItemRechargeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			ChangeTab( dwID );
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
			ItemRechargeProcessWnd *pWnd = dynamic_cast<ItemRechargeProcessWnd*>( FindChildWnd(ITEM_RECHARGE_PROCESS_WND) );
			if( pWnd )
			{
				ItemInfoButton *pBtn = dynamic_cast<ItemInfoButton*>( FindChildWnd( dwID ) );
				if( pBtn )
				{
					ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
					if( pItem )
					{
						EXTRAITEMSLOT kSlot;
						if( pItem->GetExtraItem( pBtn->GetMagicCode(), kSlot ) )
						{
							if( kSlot.m_PeriodType == EPT_MORTMAIN )
							{
								g_GUIMgr.SetMsgBox( MB_OK, NULL, "영구장비는 기간연장을 할 수 없습니다." );
								return;
							}
							else
							{
								pWnd->SetInfo( this, pBtn->GetMagicCode(), pBtn->GetExtraItemIndex(), m_dwEtcItemIndex );
								return;
							}
						}
					}

					g_GUIMgr.SetMsgBox( MB_OK, this, "장비충전중 예외상황이 발생했습니다." );
				}
			}
		}
		break;
	}
}

void ItemRechargeWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vInventoryItemInfoList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		ItemInfoButton *pItem = dynamic_cast<ItemInfoButton*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray +i;
		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			InventoryItemInfo kInfo;
			pItem->SetInfo( kInfo );
			continue;
		}

		pItem->SetInfo( m_vInventoryItemInfoList[iCurArray] );
	}
}

void ItemRechargeWnd::ChangeTab( DWORD dwID /* = 0 */ )
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );

	m_dwCurTabID = dwID;

	SetExtraItemInfoVec( m_dwCurTabID );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vInventoryItemInfoList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void ItemRechargeWnd::SetExtraItemInfoVec( DWORD dwTabID )
{
	m_vInventoryItemInfoList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem ) return;

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( g_ItemRechargeMgr.GetPromotionTime( kSlot.m_iItemCode ) == 0 )
			continue;

		if( kSlot.m_PeriodType != EPT_TIME )
			continue;

		int iGroup = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
		if( !IsRightTab( dwTabID, iGroup ) )
			continue;

		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );
		int iGapSec = kLimitTime - kCurServerTime;

		enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
		int iDay    = iGapSec/DAY_SEC;
		int iHourSec= iGapSec-(iDay*DAY_SEC);
		int iHour   = iHourSec / HOUR_SEC;
		int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

		if( iDay <= 0 && iHour <= 0 && iMinute <= 0 )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_EXTRAITEM;
		kInfo.m_iMagicCode = kSlot.m_iIndex;
		kInfo.m_bEquip = kSlot.m_bCharEquip;

		int iSlot = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE;
		int iValue = kSlot.m_iReinforce;

		if( !kSlot.m_bCharEquip )
			iValue += 100;

		kInfo.m_iPriorityOrder = iSlot * 10000 + iValue;
		kInfo.m_iPriorityOrder2 = kSlot.m_iItemCode;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwTabID == ID_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
	{
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );
	}
}

void ItemRechargeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessCustomBtnTooltip();
}

void ItemRechargeWnd::ProcessCustomBtnTooltip()
{
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];

	// 장착 툴팁
	int iEquipCharArray = -1;
	bool bShow = false;
	for( int i=ID_ITEM_INFO_WND_1_BTN; i<=ID_ITEM_INFO_WND_8_BTN; ++i )
	{
		ItemInfoButton *pItemWnd = dynamic_cast<ItemInfoButton*>(FindChildWnd( i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsIconOver() ) continue;

		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem ) continue;

		int iMagicCode = pItemWnd->GetMagicCode();
		if( !pExtraItem->GetExtraItem( iMagicCode, kSlot ) ) continue;

		iEquipCharArray = g_MyInfo.FindEquipedExtraItemChar( pItemWnd->GetMagicCode() );
		bShow = true;
		break;
	}

	if( bShow )
	{
		int iLine = 0;

		ItemType eType = GetItemType( kSlot.m_iItemCode );

		kPrinter[iLine].SetTextStyle( TS_NORMAL );
		kPrinter[iLine].SetBkColor( 0, 0, 0 );
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[iLine].AddTextPiece( FONT_SIZE_13, "%s", kSlot.m_Name.c_str() );
		iLine++;

		if( iEquipCharArray != -1 )
		{
			const CHARACTER &rkCharInfo = g_MyInfo.GetCharacter( iEquipCharArray );
			if( rkCharInfo.m_class_type > 0 )
			{
				ioHashString szClassName = g_MyInfo.GetClassName( rkCharInfo.m_class_type, g_MyInfo.GetCharPowerUpGradeByClassType( rkCharInfo.m_class_type ) );

				kPrinter[iLine].SetTextStyle( TS_NORMAL );
				kPrinter[iLine].SetBkColor( 0, 0, 0 );
				if( rkCharInfo.m_sex == 1 )
				{
					kPrinter[iLine].SetTextColor( TCT_DEFAULT_BLUE );
					kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%s - 남성", szClassName.c_str() );
				}
				else
				{
					kPrinter[iLine].SetTextColor( TCT_DEFAULT_RED );
					kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%s - 여성", szClassName.c_str() );
				}
				kPrinter[iLine].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "이 착용중입니다." );
				iLine++;
			}
		}

		kPrinter[iLine].SetTextStyle( TS_NORMAL );
		kPrinter[iLine].SetBkColor( 0, 0, 0 );
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_GRAY );

		switch( eType )
		{
		case IT_NORMAL_WEAPON:
		case IT_EXTRA_WEAPON:
		case IT_RARE_WEAPON:
			kPrinter[iLine].AddTextPiece( FONT_SIZE_13, "무기장비는 " );
			break;
		case IT_NORMAL_ARMOR:
		case IT_EXTRA_ARMOR:
		case IT_RARE_ARMOR:
			kPrinter[iLine].AddTextPiece( FONT_SIZE_13, "갑옷장비는 " );
			break;
		case IT_NORMAL_HELMET:
		case IT_EXTRA_HELMET:
		case IT_RARE_HELMET:
			kPrinter[iLine].AddTextPiece( FONT_SIZE_13, "투구장비는 " );
			break;
		case IT_NORMAL_CLOAK:
		case IT_EXTRA_CLOAK:
		case IT_RARE_CLOAK:
			kPrinter[iLine].AddTextPiece( FONT_SIZE_13, "망토장비는 " );
			break;
		}

		int iPromotionTime = g_ItemRechargeMgr.GetPromotionTime( kSlot.m_iItemCode );

		int iLimitDay = iPromotionTime / 24;
		int iLimitHour= iPromotionTime % 24;
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_RED );
		if( iLimitDay > 0 && iLimitHour > 0 )
		{
			kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%d일 %d시간 연장시 ", iLimitDay, iLimitHour );
		}
		else if( iLimitDay > 0 )
		{
			kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%d일 연장시 ", iLimitDay );
		}
		else
		{
			kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%d시간 연장시 ", iLimitHour );
		}
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "영구장비로 변경됩니다." );
		iLine++;

		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );
		int iGapSec = kLimitTime - kCurServerTime;

		time_t kPromotionTime = iPromotionTime * 3600;
		time_t kNeedTime = kPromotionTime - iGapSec;
		enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
		int iDay    = kNeedTime/DAY_SEC;
		int iHourSec= kNeedTime-(iDay*DAY_SEC);
		int iHour   = iHourSec / HOUR_SEC;
		int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

		kPrinter[iLine].SetTextStyle( TS_NORMAL );
		kPrinter[iLine].SetBkColor( 0, 0, 0 );
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "▶ 남은 기간 : " );
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%d일 %d시간 %d분", iDay, iHour, iMinute );
	}

	RechargeItemTooltip *pTooltip = dynamic_cast<RechargeItemTooltip*>(FindChildWnd( ID_TOOL_TIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter );
	}
}

int ItemRechargeWnd::GetTabType( DWORD dwTabID )
{
	int iArray = ( dwTabID - ID_TAB_1_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	switch( dwTabID )
	{
	case ID_TAB_2_BTN:
		return 1;
	case ID_TAB_3_BTN:
		return 2;
	case ID_TAB_4_BTN:
		return 3;
	case ID_TAB_5_BTN:
		return 4;
	}

	return -1;
}

ItemRechargeWnd::ItemType ItemRechargeWnd::GetItemType( int iItemCode )
{
	int iGroup = iItemCode/100000 + 1;
	int iExtra = iItemCode/10000;
	int iRare  = iItemCode/1000%10;

	switch( iGroup )
	{
	case 1:
		{
			if( iRare > 0 )
				return IT_RARE_WEAPON;
			else if( iExtra > 0 )
				return IT_EXTRA_WEAPON;
			else
				return IT_NORMAL_WEAPON;
		}
	case 2:
		{
			if( iRare > 0 )
				return IT_RARE_ARMOR;
			else if( iExtra > 0 )
				return IT_EXTRA_ARMOR;
			else
				return IT_NORMAL_ARMOR;
		}
	case 3:
		{
			if( iRare > 0 )
				return IT_RARE_HELMET;
			else if( iExtra > 0 )
				return IT_EXTRA_HELMET;
			else
				return IT_NORMAL_HELMET;
		}
	case 4:
		{
			if( iRare > 0 )
				return IT_RARE_CLOAK;
			else if( iExtra > 0 )
				return IT_EXTRA_CLOAK;
			else
				return IT_NORMAL_CLOAK;
		}
	}

	return IT_NONE;
}

bool ItemRechargeWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

void ItemRechargeWnd::Update()
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, m_dwCurTabID );

	SetExtraItemInfoVec( m_dwCurTabID );

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vInventoryItemInfoList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}

	UpdatePage( m_iCurPage );
}

void ItemRechargeWnd::SetInfo( DWORD dwEtcItemIndex )
{
	m_dwEtcItemIndex = dwEtcItemIndex;
	
	if( !IsShow() )
		ShowWnd();
}

void ItemRechargeWnd::SendItemRecharge( int iItemIndex )
{
	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem && pExtraItem->GetExtraItem( iItemIndex, kSlot ) )
	{
		SP2Packet kPacket( CTPK_ETCITEM_USE );
		kPacket << static_cast<int>( m_dwEtcItemIndex );
		kPacket << kSlot.m_iIndex;
		kPacket << kSlot.m_iItemCode;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}
//////////////////////////////////////////////////////////////////////////
ItemRechargeProcessWnd::ItemRechargeProcessWnd()
{
	m_pReturnWnd = NULL;

	m_pIcon     = NULL;
	m_pNumText  = NULL;
	m_pPlusText = NULL;
	m_pIconBackProcess = NULL;
	m_pIconBackResult  = NULL;

	for( int i=0; i<9; ++i )
		m_vRollingEffect[i] = NULL;

	for( int i=0; i<10; ++i )
	{
		m_vStopEffect[i] = NULL;
		m_vSuccessEffect[i] = NULL;
	}

	m_ItemName.Clear();
	m_iReinForce = 0;
	m_iItemCode  = 0;
	m_iItemIndex = 0;

	m_iCurRoll = 0;
	m_iMaxRoll = 0;
	m_nGradeType = 0;
}

ItemRechargeProcessWnd::~ItemRechargeProcessWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pIconBackProcess );
	SAFEDELETE( m_pIconBackResult );

	for( int i=0; i<9; ++i )
		SAFEDELETE( m_vRollingEffect[i] );

	for( int i=0; i<10; ++i )
	{
		SAFEDELETE( m_vStopEffect[i] );
		SAFEDELETE( m_vSuccessEffect[i] );
	}
}

void ItemRechargeProcessWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else if( szType == "icon_back_process" )
	{
		SAFEDELETE( m_pIconBackProcess );
		m_pIconBackProcess = pImage;
	}
	else if( szType == "icon_back_result" )
	{
		SAFEDELETE( m_pIconBackResult );
		m_pIconBackResult = pImage;
	}
	else if( szType == "RollingEffect1" )
	{
		SAFEDELETE( m_vRollingEffect[0] );
		m_vRollingEffect[0] = pImage;
	}
	else if( szType == "RollingEffect2" )
	{
		SAFEDELETE( m_vRollingEffect[1] );
		m_vRollingEffect[1] = pImage;
	}
	else if( szType == "RollingEffect3" )
	{
		SAFEDELETE( m_vRollingEffect[2]);
		m_vRollingEffect[2] = pImage;
	}
	else if( szType == "RollingEffect4" )
	{
		SAFEDELETE( m_vRollingEffect[3] );
		m_vRollingEffect[3] = pImage;
	}
	else if( szType == "RollingEffect5" )
	{
		SAFEDELETE( m_vRollingEffect[4] );
		m_vRollingEffect[4] = pImage;
	}
	else if( szType == "RollingEffect6" )
	{
		SAFEDELETE( m_vRollingEffect[5] );
		m_vRollingEffect[5] = pImage;
	}
	else if( szType == "RollingEffect7" )
	{
		SAFEDELETE( m_vRollingEffect[6] );
		m_vRollingEffect[6] = pImage;
	}
	else if( szType == "RollingEffect8" )
	{
		SAFEDELETE( m_vRollingEffect[7] );
		m_vRollingEffect[7] = pImage;
	}
	else if( szType == "RollingEffect9" )
	{
		SAFEDELETE( m_vRollingEffect[8] );
		m_vRollingEffect[8] = pImage;
	}
	else if( szType == "StopEffect1" )
	{
		SAFEDELETE( m_vStopEffect[0] );
		m_vStopEffect[0] = pImage;
	}
	else if( szType == "StopEffect2" )
	{
		SAFEDELETE( m_vStopEffect[1] );
		m_vStopEffect[1] = pImage;
	}
	else if( szType == "StopEffect3" )
	{
		SAFEDELETE( m_vStopEffect[2] );
		m_vStopEffect[2] = pImage;
	}
	else if( szType == "StopEffect4" )
	{
		SAFEDELETE( m_vStopEffect[3] );
		m_vStopEffect[3] = pImage;
	}
	else if( szType == "StopEffect5" )
	{
		SAFEDELETE( m_vStopEffect[4] );
		m_vStopEffect[4] = pImage;
	}
	else if( szType == "StopEffect6" )
	{
		SAFEDELETE( m_vStopEffect[5] );
		m_vStopEffect[5] = pImage;
	}
	else if( szType == "StopEffect7" )
	{
		SAFEDELETE( m_vStopEffect[6] );
		m_vStopEffect[6] = pImage;
	}
	else if( szType == "StopEffect8" )
	{
		SAFEDELETE( m_vStopEffect[7] );
		m_vStopEffect[7] = pImage;
	}
	else if( szType == "StopEffect9" )
	{
		SAFEDELETE( m_vStopEffect[8] );
		m_vStopEffect[8] = pImage;
	}
	else if( szType == "StopEffect10" )
	{
		SAFEDELETE( m_vStopEffect[9] );
		m_vStopEffect[9] = pImage;
	}
	else if( szType == "SuccessEffect1" )
	{
		SAFEDELETE( m_vSuccessEffect[0] );
		m_vSuccessEffect[0] = pImage;
	}
	else if( szType == "SuccessEffect2" )
	{
		SAFEDELETE( m_vSuccessEffect[1] );
		m_vSuccessEffect[1] = pImage;
	}
	else if( szType == "SuccessEffect3" )
	{
		SAFEDELETE( m_vSuccessEffect[2] );
		m_vSuccessEffect[2] = pImage;
	}
	else if( szType == "SuccessEffect4" )
	{
		SAFEDELETE( m_vSuccessEffect[3] );
		m_vSuccessEffect[3] = pImage;
	}
	else if( szType == "SuccessEffect5" )
	{
		SAFEDELETE( m_vSuccessEffect[4] );
		m_vSuccessEffect[4] = pImage;
	}
	else if( szType == "SuccessEffect6" )
	{
		SAFEDELETE( m_vSuccessEffect[5] );
		m_vSuccessEffect[5] = pImage;
	}
	else if( szType == "SuccessEffect7" )
	{
		SAFEDELETE( m_vSuccessEffect[6] );
		m_vSuccessEffect[6] = pImage;
	}
	else if( szType == "SuccessEffect8" )
	{
		SAFEDELETE( m_vSuccessEffect[7] );
		m_vSuccessEffect[7] = pImage;
	}
	else if( szType == "SuccessEffect9" )
	{
		SAFEDELETE( m_vSuccessEffect[8] );
		m_vSuccessEffect[8] = pImage;
	}
	else if( szType == "SuccessEffect10" )
	{
		SAFEDELETE( m_vSuccessEffect[9] );
		m_vSuccessEffect[9] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ItemRechargeProcessWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwProcessTime      = xElement.GetIntAttribute_e( "ProcessTime" );
	m_dwRollingCheckTime = xElement.GetIntAttribute_e( "RollingCheckTime" );
	m_dwRollingEffectGapTime = xElement.GetIntAttribute_e( "RollingEffectGapTime" );
	m_dwStopEffectGapTime    = xElement.GetIntAttribute_e( "StopEffectGapTime" );

	m_RollingSound = xElement.GetStringAttribute_e( "RollingSound" );
	m_StopSound    = xElement.GetStringAttribute_e( "StopSound" );
	m_SuccessSound = xElement.GetStringAttribute_e( "SuccessSound" );
}

void ItemRechargeProcessWnd::iwm_create()
{
	ioUIRenderImageList vImageList;
	vImageList.clear();
	for( int i=0; i<9; ++i )
		vImageList.push_back( m_vRollingEffect[i] );

	m_RollingEffect.Init();
	m_RollingEffect.AddSpriteList( vImageList );
	m_RollingEffect.SetType( ioSprite::AT_LOOP );
	m_RollingEffect.SetAnimationGapTime( 50 );

	vImageList.clear();
	for( int i=0; i<10; ++i )
		vImageList.push_back( m_vStopEffect[i] );

	m_StopEffect.Init();
	m_StopEffect.AddSpriteList( vImageList );
	m_StopEffect.SetType( ioSprite::AT_PLAY_ONCE );
	m_StopEffect.SetAnimationGapTime( 50 );

	vImageList.clear();
	for( int i=0; i<10; ++i )
		vImageList.push_back( m_vSuccessEffect[i] );

	m_SuccessEffect.Init();
	m_SuccessEffect.AddSpriteList( vImageList );
	m_SuccessEffect.SetType( ioSprite::AT_PLAY_ONCE );
	m_SuccessEffect.SetAnimationGapTime( 50 );
}

void ItemRechargeProcessWnd::iwm_show()
{
	SetUIStartState();
}

void ItemRechargeProcessWnd::iwm_hide()
{
	HideChildWnd( ID_OK_BTN );
	HideChildWnd( ID_CANCEL_BTN );
	HideChildWnd( ID_AGAIN_BTN );
	HideChildWnd( ID_ITEM_LIST_BTN1 );
	HideChildWnd( ID_ITEM_LIST_BTN2 );

	m_RollingEffect.Init();
	m_StopEffect.Init();
	m_SuccessEffect.Init();

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_RollingSound, 0 );
}

void ItemRechargeProcessWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			if( g_MyInfo.GetEtcCoinMoney( m_dwUseEtcItemIndex ) <= 0 )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwUseEtcItemIndex );
				if( pItem )
					g_GUIMgr.SetMsgBox( MB_OK, this, "%s가 없습니다.", pItem->GetName().c_str() );
				else
					g_GUIMgr.SetMsgBox( MB_OK, this, "장비충전중 예외상황이 발생했습니다." );
			}
			else
				SetUIProcessState();
		}
		break;
	case ID_AGAIN_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEtcCoinMoney( m_dwUseEtcItemIndex ) <= 0 )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwUseEtcItemIndex );
				if( pItem )
					g_GUIMgr.SetMsgBox( MB_OK, this, "%s가 없습니다.", pItem->GetName().c_str() );
				else
					g_GUIMgr.SetMsgBox( MB_OK, this, "장비충전중 예외상황이 발생했습니다." );
			}
			else
				SetUIStartState();
		}
		break;
	case ID_ITEM_LIST_BTN1:
	case ID_ITEM_LIST_BTN2:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			if( m_pReturnWnd )
				m_pReturnWnd->HideWnd();

			// UI 갱신
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType() );
				//pInvenWnd->UpdateTab( false, true );
			}
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDNO )
			{
				HideWnd();
				if( m_pReturnWnd )
					m_pReturnWnd->HideWnd();
			}
		}
	}
}

void ItemRechargeProcessWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 124, iYPos + 151, FONT_SIZE_17, m_ItemName.c_str() );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 124, iYPos + 105 );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 124, iYPos + 105, UI_RENDER_NORMAL );

	switch( m_UIState )
	{
	case US_START:
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );	

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kSlot;
				if( pExtraItem->GetExtraItem( m_iItemCode, kSlot ) )
				{
					char szRemain[MAX_PATH] = "";
					Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szRemain, sizeof(szRemain) );
					kPrinter.SetTextColor( TCT_DEFAULT_RED );
					kPrinter.AddTextPiece( FONT_SIZE_16, szRemain );
					kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
					kPrinter.ClearList();
				}
			}
			
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, "장비연장 아이템을 이용하여" );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_13, "기간을 연장 하시겠습니까?" );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
			kPrinter.ClearList();

			m_pIconBackProcess->Render( iXPos + 124, iYPos + 105, UI_RENDER_MULTIPLY );
		}
		break;
	case US_PROCESS:
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );	

			kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter.AddTextPiece( FONT_SIZE_16, m_RollingText[m_iCurRoll].c_str() );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, "장비의 기간을 연장 중입니다." );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_13, "잠시만 기다려주세요." );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
			kPrinter.ClearList();

			m_RollingEffect.Render( iXPos + 124, iYPos + 105, 2.0f, UI_RENDER_ADD );
			
			m_pIconBackProcess->Render( iXPos + 124, iYPos + 105, UI_RENDER_MULTIPLY );
		}
		break;
	case US_END:
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );	

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kSlot;
				if( pExtraItem->GetExtraItem( m_iItemCode, kSlot ) )
				{					
					if( kSlot.m_PeriodType == EPT_MORTMAIN )
					{
						kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
						kPrinter.AddTextPiece( FONT_SIZE_16, "영구장비 달성" );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
						kPrinter.ClearList();

						kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter.AddTextPiece( FONT_SIZE_13, "축하합니다!" );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
						kPrinter.ClearList();

						kPrinter.SetTextColor( TCT_DEFAULT_RED );
						kPrinter.AddTextPiece( FONT_SIZE_13, "영구장비로 변환되었습니다" );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
						kPrinter.ClearList();

						m_SuccessEffect.Render( iXPos + 124, iYPos + 105, 2.0f, UI_RENDER_ADD );
					}
					else
					{
						kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
						kPrinter.AddTextPiece( FONT_SIZE_16, "+ %s", m_RechargeTime.c_str() );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
						kPrinter.ClearList();

						kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
						kPrinter.AddTextPiece( FONT_SIZE_13, "기간연장을 완료했습니다." );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
						kPrinter.ClearList();

						char szRemain[MAX_PATH] = "";
						Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szRemain, sizeof(szRemain) );
						kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter.AddTextPiece( FONT_SIZE_13, "총 잔여시간 " );
						kPrinter.SetTextColor( TCT_DEFAULT_RED );
						kPrinter.AddTextPiece( FONT_SIZE_13, szRemain );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
						kPrinter.ClearList();

						m_StopEffect.Render( iXPos + 124, iYPos + 105, 2.0f, UI_RENDER_ADD );
					}
				}
			}

			m_pIconBackResult->Render( iXPos + 124, iYPos + 105, UI_RENDER_MULTIPLY );
		}
		break;
	}
}

void ItemRechargeProcessWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd::OnProcess( fTimePerSec );

	switch( m_UIState )
	{
	case US_START:
		break;
	case US_PROCESS:
		{
			if( m_dwProcessEndTime > 0 && m_dwProcessEndTime < FRAMEGETTIME() )
			{
				m_dwProcessEndTime = 0;
				ItemRechargeWnd *pWnd = dynamic_cast<ItemRechargeWnd*>( m_pReturnWnd );
				if( pWnd )
					pWnd->SendItemRecharge( m_iItemCode );
			}

			if( m_dwNextRollingTime < FRAMEGETTIME() )
			{
				m_dwNextRollingTime = FRAMEGETTIME() + m_dwRollingCheckTime;
				m_iCurRoll++;
				if( m_iCurRoll >= m_iMaxRoll )
					m_iCurRoll = 0;
			}

			m_RollingEffect.Process();
		}
		break;
	case US_END:
		m_StopEffect.Process();
		m_SuccessEffect.Process();
		break;
	}
}

void ItemRechargeProcessWnd::SetInfo( ioWnd *pReturnWnd, int iItemCode, int iItemIndex, DWORD dwUseEtcItemIndex )
{
	m_pReturnWnd = pReturnWnd;
	m_iItemCode  = iItemCode;
	m_iItemIndex = iItemIndex;
	m_dwUseEtcItemIndex = dwUseEtcItemIndex;
	m_nGradeType = 0;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return;

	if( !pExtraItem->GetExtraItem( iItemCode, kSlot ) )
		return;

	m_ItemName = kSlot.m_Name;
	m_iReinForce = kSlot.m_iReinforce;
	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
	if ( pItem )
		m_nGradeType = pItem->GetGradeType();

	m_UIState = US_START;

	if( !IsShow() )
		ShowWnd();
}

void ItemRechargeProcessWnd::SetUIStartState()
{
	m_UIState = US_START;

	ShowChildWnd( ID_OK_BTN );
	ShowChildWnd( ID_CANCEL_BTN );
	HideChildWnd( ID_PROCESS_BTN );
	HideChildWnd( ID_AGAIN_BTN );
	HideChildWnd( ID_ITEM_LIST_BTN1 );
	HideChildWnd( ID_ITEM_LIST_BTN2 );

	m_dwProcessEndTime = 0;
}

void ItemRechargeProcessWnd::SetUIProcessState()
{
	m_UIState = US_PROCESS;

	HideChildWnd( ID_OK_BTN );
	HideChildWnd( ID_CANCEL_BTN );
	ShowChildWnd( ID_PROCESS_BTN );
	HideChildWnd( ID_AGAIN_BTN );
	HideChildWnd( ID_ITEM_LIST_BTN1 );
	HideChildWnd( ID_ITEM_LIST_BTN2 );

	m_dwProcessEndTime = FRAMEGETTIME() + m_dwProcessTime;
	m_dwNextRollingTime = FRAMEGETTIME() + m_dwRollingCheckTime;

	ItemRechargeWnd *pWnd = dynamic_cast<ItemRechargeWnd*>( m_pReturnWnd );
	if( pWnd )
	{
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwUseEtcItemIndex );
		ioEtcItemRecharge *pRecharge = dynamic_cast<ioEtcItemRecharge*>(pItem);
		if( pRecharge )
		{
			m_iCurRoll = 0;
			m_iMaxRoll = pRecharge->GetMaxRechargeInfoCount();
			m_RollingText.clear();
			m_RollingText.reserve( m_iMaxRoll );

			for( int i=0; i<m_iMaxRoll; ++i )
			{
				char szTime[MAX_PATH] = "";
				ioHashString szText = "????";
				int iPromotionTime = pRecharge->GetRechargeTime(i);
				int iLimitDay = iPromotionTime / 24;
				int iLimitHour= iPromotionTime % 24;
				if( iLimitDay > 0 && iLimitHour > 0 )
				{
					wsprintf( szTime, "+ %d일 %d시간", iLimitDay, iLimitHour );
					szText = szTime;
				}
				else if( iLimitDay > 0 )
				{
					wsprintf( szTime, "+ %d일", iLimitDay );
					szText = szTime;
				}
				else
				{
					wsprintf( szTime, "+ %d시간", iLimitHour );
					szText = szTime;
				}

				m_RollingText.push_back(szText);
			}
		}
	}

	m_RollingEffect.SetPlay();

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_RollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void ItemRechargeProcessWnd::SetUIEndState( int iRechargeHour )
{
	m_UIState = US_END;
	m_iRechargeHour = iRechargeHour;
	
	char szTime[MAX_PATH] = "";
	int iPromotionTime = iRechargeHour;
	int iLimitDay = iPromotionTime / 24;
	int iLimitHour= iPromotionTime % 24;
	if( iLimitDay > 0 && iLimitHour > 0 )
	{
		wsprintf( szTime, "%d일 %d시간", iLimitDay, iLimitHour );
	}
	else if( iLimitDay > 0 )
	{
		wsprintf( szTime, "%d일", iLimitDay );
	}
	else
	{
		wsprintf( szTime, "%d시간", iLimitHour );
	}
	
	m_RechargeTime.Clear();
	m_RechargeTime = szTime;

	HideChildWnd( ID_OK_BTN );
	HideChildWnd( ID_CANCEL_BTN );
	HideChildWnd( ID_PROCESS_BTN );
	ShowChildWnd( ID_AGAIN_BTN );

	ioHashString szItemName;
	szItemName.Clear();
	bool bSuccess = false;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iItemCode, kSlot ) )
		{
			if( kSlot.m_PeriodType == EPT_MORTMAIN )
			{
				bSuccess = true;
				szItemName = kSlot.m_Name;
			}
		}
	}

	if( bSuccess )
	{
		ShowChildWnd( ID_ITEM_LIST_BTN2 );
		m_SuccessEffect.SetPlay();
		g_SoundMgr.PlaySound( m_SuccessSound );
	}
	else
	{
		ShowChildWnd( ID_ITEM_LIST_BTN1 );
		m_StopEffect.SetPlay();
		g_SoundMgr.PlaySound( m_StopSound );
	}

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_RollingSound, 0 );

	m_RollingEffect.Init();
}
//////////////////////////////////////////////////////////////////////////
RechargeItemTooltip::RechargeItemTooltip()
{
}

RechargeItemTooltip::~RechargeItemTooltip()
{
}

void RechargeItemTooltip::ClearList()
{
	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; ++i )
	{
		m_Printer[i].ClearList();
	}
	m_Desc.Release();
	m_iLine = 0;
}

void RechargeItemTooltip::SetTooltipText( ioComplexStringPrinter *rkPrinter )
{
	ClearList();

	int i = 0;
	int iMaxLine = 0;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( rkPrinter[i].IsEmpty() )
			break;

		rkPrinter[i].CopyFullText( m_Printer[i] );
		iMaxLine++;
	}

	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	int iMaxWidth  = 0;
	int iMaxHeight = MIN_HEIGHT;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( m_Printer[i].IsEmpty() )
			break;

		int iCurLineWidth = m_Printer[i].GetFullWidth() + SIDE_TEXT_GAP;
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
	}
	iMaxHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );

	SetSize( iMaxWidth, iMaxHeight );
	ProcessPosition();
}

void RechargeItemTooltip::SetTooltipText( ioComplexStringPrinter *rkPrinter, ioHashString szTooltip, int iLine )
{
	ClearList();
	m_iLine = iLine;

	int i = 0;
	int iMaxLine = 0;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( rkPrinter[i].IsEmpty() )
			break;

		rkPrinter[i].CopyFullText( m_Printer[i] );
		iMaxLine++;
	}
	if( !szTooltip.IsEmpty() )
		m_Desc.Set(0, 0, szTooltip.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
	
	iMaxLine += m_Desc.GetLineSize();
	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	int iMaxWidth  = 0;
	int iMaxHeight = MIN_HEIGHT;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( m_Printer[i].IsEmpty() )
			break;

		int iCurLineWidth = m_Printer[i].GetFullWidth() + SIDE_TEXT_GAP;
		if( iMaxWidth < iCurLineWidth )
			iMaxWidth = iCurLineWidth;
	}
	if( iMaxWidth < m_Desc.GetMaxLineWidth() + SIDE_TEXT_GAP )
		iMaxWidth = m_Desc.GetMaxLineWidth() + SIDE_TEXT_GAP;
		
	iMaxHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );
	SetSize( iMaxWidth, iMaxHeight );
	ProcessPosition();
}

void RechargeItemTooltip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void RechargeItemTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessPosition();
}

void RechargeItemTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;;

	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; i++ )
	{
		if( m_Printer[i].IsEmpty() )
			break;

		m_Printer[i].PrintFullText( iXPos, iYPos + TEXT_LINE_GAP*i, TAT_LEFT );
	}
	m_Desc.OnRender( iXPos, iYPos + TEXT_LINE_GAP * m_iLine );
}
//////////////////////////////////////////////////////////////////////////
AccessoryInfoButton::AccessoryInfoButton()
{
	m_ItemInfo.Init();
	m_nAccItem = 0;

	m_pEmptyImg = NULL;
}

AccessoryInfoButton::~AccessoryInfoButton()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void AccessoryInfoButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void AccessoryInfoButton::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ICON_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, IOBN_BTNUP, param );
		}
		break;
	}
}

void AccessoryInfoButton::SetBlank()
{
	m_ItemInfo.Init();
	m_nAccItem = 0;

	SetInActive();

	HideChildWnd( ID_ICON_BTN );

	ClearDesc();
}

void AccessoryInfoButton::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

bool AccessoryInfoButton::IsIconOver()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_ICON_BTN ) )
		return true;
	return false;
}

void AccessoryInfoButton::OnRender()
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
	{
		ioButton::OnRender();
	}

	RenderDesc( iXPos, iYPos );
}

void AccessoryInfoButton::RenderDesc( int iXPos, int iYPos )
{
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
		m_szDescArray[0].PrintFullTextWidthCut( iXPos + 51, iYPos + 105, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos + 51, iYPos + 121, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
	else
	{
		m_szDescArray[0].PrintFullTextWidthCut( iXPos + 53, iYPos + 97,  TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos + 53, iYPos + 113, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
		m_szDescArray[2].PrintFullTextWidthCut( iXPos + 53, iYPos + 128, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
}

void AccessoryInfoButton::SetInfo( InventoryItemInfo &rkInfo )
{
	SetBlank();
	SetActive();

	m_ItemInfo = rkInfo;
	if( !SetAccessoryInfo() )
	{
		SetBlank();
		return;
	}
}

InventoryItemInfo& AccessoryInfoButton::GetInfo()
{
	return m_ItemInfo;
}

bool AccessoryInfoButton::SetAccessoryInfo()
{
	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	AccessorySlot kSlot;
	ioUserAccessory *pAcc = g_MyInfo.GetUserAccessory();
	if( !pAcc )
		return false;

	if(!pAcc->GetAccessorySlot(m_ItemInfo.m_iMagicCode, kSlot))
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	ItemIconButton *pBtn = dynamic_cast< ItemIconButton* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg );
		pBtn->SetEquip( kSlot.m_bEquip );

		bool bGray = false;
		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		kLimitTime += 30;
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );
		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec <= 0 )
			bGray = true;

		pBtn->SetIconGray( bGray );
	}

	m_nAccItem = kSlot.m_iIndex;

	// desc
	ClearDesc();

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", kSlot.GetName().c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );

	char szText1[MAX_PATH] = "";
	Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szText1, sizeof(szText1), true );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szText1 );
	return true;
}
/////////////////////////////////////////////////////////////////////////
AccessoryRechargeProcessWnd::AccessoryRechargeProcessWnd()
{
	m_pReturnWnd = NULL;

	m_pIcon     = NULL;
	m_pNumText  = NULL;
	m_pPlusText = NULL;
	m_pIconBackProcess = NULL;
	m_pIconBackResult  = NULL;

	for( int i=0; i<9; ++i )
		m_vRollingEffect[i] = NULL;

	for( int i=0; i<10; ++i )
	{
		m_vStopEffect[i] = NULL;
		m_vSuccessEffect[i] = NULL;
	}

	m_ItemName.Clear();
	m_iItemCode  = 0;
	m_iItemIndex = 0;

	m_iCurRoll = 0;
	m_iMaxRoll = 0;
}

AccessoryRechargeProcessWnd::~AccessoryRechargeProcessWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pIconBackProcess );
	SAFEDELETE( m_pIconBackResult );

	for( int i=0; i<9; ++i )
		SAFEDELETE( m_vRollingEffect[i] );

	for( int i=0; i<10; ++i )
	{
		SAFEDELETE( m_vStopEffect[i] );
		SAFEDELETE( m_vSuccessEffect[i] );
	}
}

void AccessoryRechargeProcessWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else if( szType == "icon_back_process" )
	{
		SAFEDELETE( m_pIconBackProcess );
		m_pIconBackProcess = pImage;
	}
	else if( szType == "icon_back_result" )
	{
		SAFEDELETE( m_pIconBackResult );
		m_pIconBackResult = pImage;
	}
	else if( szType == "RollingEffect1" )
	{
		SAFEDELETE( m_vRollingEffect[0] );
		m_vRollingEffect[0] = pImage;
	}
	else if( szType == "RollingEffect2" )
	{
		SAFEDELETE( m_vRollingEffect[1] );
		m_vRollingEffect[1] = pImage;
	}
	else if( szType == "RollingEffect3" )
	{
		SAFEDELETE( m_vRollingEffect[2]);
		m_vRollingEffect[2] = pImage;
	}
	else if( szType == "RollingEffect4" )
	{
		SAFEDELETE( m_vRollingEffect[3] );
		m_vRollingEffect[3] = pImage;
	}
	else if( szType == "RollingEffect5" )
	{
		SAFEDELETE( m_vRollingEffect[4] );
		m_vRollingEffect[4] = pImage;
	}
	else if( szType == "RollingEffect6" )
	{
		SAFEDELETE( m_vRollingEffect[5] );
		m_vRollingEffect[5] = pImage;
	}
	else if( szType == "RollingEffect7" )
	{
		SAFEDELETE( m_vRollingEffect[6] );
		m_vRollingEffect[6] = pImage;
	}
	else if( szType == "RollingEffect8" )
	{
		SAFEDELETE( m_vRollingEffect[7] );
		m_vRollingEffect[7] = pImage;
	}
	else if( szType == "RollingEffect9" )
	{
		SAFEDELETE( m_vRollingEffect[8] );
		m_vRollingEffect[8] = pImage;
	}
	else if( szType == "StopEffect1" )
	{
		SAFEDELETE( m_vStopEffect[0] );
		m_vStopEffect[0] = pImage;
	}
	else if( szType == "StopEffect2" )
	{
		SAFEDELETE( m_vStopEffect[1] );
		m_vStopEffect[1] = pImage;
	}
	else if( szType == "StopEffect3" )
	{
		SAFEDELETE( m_vStopEffect[2] );
		m_vStopEffect[2] = pImage;
	}
	else if( szType == "StopEffect4" )
	{
		SAFEDELETE( m_vStopEffect[3] );
		m_vStopEffect[3] = pImage;
	}
	else if( szType == "StopEffect5" )
	{
		SAFEDELETE( m_vStopEffect[4] );
		m_vStopEffect[4] = pImage;
	}
	else if( szType == "StopEffect6" )
	{
		SAFEDELETE( m_vStopEffect[5] );
		m_vStopEffect[5] = pImage;
	}
	else if( szType == "StopEffect7" )
	{
		SAFEDELETE( m_vStopEffect[6] );
		m_vStopEffect[6] = pImage;
	}
	else if( szType == "StopEffect8" )
	{
		SAFEDELETE( m_vStopEffect[7] );
		m_vStopEffect[7] = pImage;
	}
	else if( szType == "StopEffect9" )
	{
		SAFEDELETE( m_vStopEffect[8] );
		m_vStopEffect[8] = pImage;
	}
	else if( szType == "StopEffect10" )
	{
		SAFEDELETE( m_vStopEffect[9] );
		m_vStopEffect[9] = pImage;
	}
	else if( szType == "SuccessEffect1" )
	{
		SAFEDELETE( m_vSuccessEffect[0] );
		m_vSuccessEffect[0] = pImage;
	}
	else if( szType == "SuccessEffect2" )
	{
		SAFEDELETE( m_vSuccessEffect[1] );
		m_vSuccessEffect[1] = pImage;
	}
	else if( szType == "SuccessEffect3" )
	{
		SAFEDELETE( m_vSuccessEffect[2] );
		m_vSuccessEffect[2] = pImage;
	}
	else if( szType == "SuccessEffect4" )
	{
		SAFEDELETE( m_vSuccessEffect[3] );
		m_vSuccessEffect[3] = pImage;
	}
	else if( szType == "SuccessEffect5" )
	{
		SAFEDELETE( m_vSuccessEffect[4] );
		m_vSuccessEffect[4] = pImage;
	}
	else if( szType == "SuccessEffect6" )
	{
		SAFEDELETE( m_vSuccessEffect[5] );
		m_vSuccessEffect[5] = pImage;
	}
	else if( szType == "SuccessEffect7" )
	{
		SAFEDELETE( m_vSuccessEffect[6] );
		m_vSuccessEffect[6] = pImage;
	}
	else if( szType == "SuccessEffect8" )
	{
		SAFEDELETE( m_vSuccessEffect[7] );
		m_vSuccessEffect[7] = pImage;
	}
	else if( szType == "SuccessEffect9" )
	{
		SAFEDELETE( m_vSuccessEffect[8] );
		m_vSuccessEffect[8] = pImage;
	}
	else if( szType == "SuccessEffect10" )
	{
		SAFEDELETE( m_vSuccessEffect[9] );
		m_vSuccessEffect[9] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void AccessoryRechargeProcessWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwProcessTime      = xElement.GetIntAttribute_e( "ProcessTime" );
	m_dwRollingCheckTime = xElement.GetIntAttribute_e( "RollingCheckTime" );
	m_dwRollingEffectGapTime = xElement.GetIntAttribute_e( "RollingEffectGapTime" );
	m_dwStopEffectGapTime    = xElement.GetIntAttribute_e( "StopEffectGapTime" );

	m_RollingSound = xElement.GetStringAttribute_e( "RollingSound" );
	m_StopSound    = xElement.GetStringAttribute_e( "StopSound" );
	m_SuccessSound = xElement.GetStringAttribute_e( "SuccessSound" );
}

void AccessoryRechargeProcessWnd::iwm_create()
{
	ioUIRenderImageList vImageList;
	vImageList.clear();
	for( int i=0; i<9; ++i )
		vImageList.push_back( m_vRollingEffect[i] );

	m_RollingEffect.Init();
	m_RollingEffect.AddSpriteList( vImageList );
	m_RollingEffect.SetType( ioSprite::AT_LOOP );
	m_RollingEffect.SetAnimationGapTime( 50 );

	vImageList.clear();
	for( int i=0; i<10; ++i )
		vImageList.push_back( m_vStopEffect[i] );

	m_StopEffect.Init();
	m_StopEffect.AddSpriteList( vImageList );
	m_StopEffect.SetType( ioSprite::AT_PLAY_ONCE );
	m_StopEffect.SetAnimationGapTime( 50 );

	vImageList.clear();
	for( int i=0; i<10; ++i )
		vImageList.push_back( m_vSuccessEffect[i] );

	m_SuccessEffect.Init();
	m_SuccessEffect.AddSpriteList( vImageList );
	m_SuccessEffect.SetType( ioSprite::AT_PLAY_ONCE );
	m_SuccessEffect.SetAnimationGapTime( 50 );
}

void AccessoryRechargeProcessWnd::iwm_show()
{
	SetUIStartState();
}

void AccessoryRechargeProcessWnd::iwm_hide()
{
	HideChildWnd( ID_OK_BTN );
	HideChildWnd( ID_CANCEL_BTN );
	HideChildWnd( ID_AGAIN_BTN );
	HideChildWnd( ID_ITEM_LIST_BTN1 );
	HideChildWnd( ID_ITEM_LIST_BTN2 );

	m_RollingEffect.Init();
	m_StopEffect.Init();
	m_SuccessEffect.Init();

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_RollingSound, 0 );
}

void AccessoryRechargeProcessWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(pWnd == NULL)
		return;

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
			if( g_MyInfo.GetEtcCoinMoney( m_dwUseEtcItemIndex ) <= 0 )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwUseEtcItemIndex );
				if( pItem )
					g_GUIMgr.SetMsgBox( MB_OK, this, "%s이 없습니다.", pItem->GetName().c_str() );
				else
					g_GUIMgr.SetMsgBox( MB_OK, this, "액세서리 충전중 예외상황이 발생했습니다." );
			}
			else
				SetUIProcessState();
		}
		break;
	case ID_AGAIN_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_MyInfo.GetEtcCoinMoney( m_dwUseEtcItemIndex ) <= 0 )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwUseEtcItemIndex );
				if( pItem )
					g_GUIMgr.SetMsgBox( MB_OK, this, "%s이 없습니다.", pItem->GetName().c_str() );
				else
					g_GUIMgr.SetMsgBox( MB_OK, this, "액세서리 충전중 예외상황이 발생했습니다." );
			}
			else
				SetUIStartState();
		}
		break;
	case ID_ITEM_LIST_BTN1:
	case ID_ITEM_LIST_BTN2:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			if( m_pReturnWnd )
				m_pReturnWnd->HideWnd();

			// UI 갱신
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->ShowAccessoryTabDirect( g_MyInfo.GetClassType() );
		}
		break;
	case MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDNO )
			{
				HideWnd();
				if( m_pReturnWnd )
					m_pReturnWnd->HideWnd();
			}
		}
	}
}

void AccessoryRechargeProcessWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 124, iYPos + 151, FONT_SIZE_17, m_ItemName.c_str() );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 124, iYPos + 105, UI_RENDER_NORMAL );

	switch( m_UIState )
	{
	case US_START:
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );	

			ioUserAccessory *pAccItem = g_MyInfo.GetUserAccessory();
			if( pAccItem )
			{
				AccessorySlot kSlot;
				if(pAccItem->GetAccessorySlot(m_iItemCode, kSlot))
				{
					char szRemain[MAX_PATH] = "";
					Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szRemain, sizeof(szRemain), true );
					kPrinter.SetTextColor( TCT_DEFAULT_RED );
					kPrinter.AddTextPiece( FONT_SIZE_16, szRemain );
					kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
					kPrinter.ClearList();
				}
			}

			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_13, "액세서리 연장 아이템을 이용하여" );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
			kPrinter.ClearList();

			kPrinter.AddTextPiece( FONT_SIZE_13, "기간을 연장 하시겠습니까?" );
			kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
			kPrinter.ClearList();

			m_pIconBackProcess->Render( iXPos + 124, iYPos + 105, UI_RENDER_MULTIPLY );
		}
		break;
	case US_PROCESS:
		{
			if( !m_RollingText.empty() )
			{
				ioComplexStringPrinter kPrinter;
				kPrinter.SetTextStyle( TS_NORMAL );
				kPrinter.SetBkColor( 0, 0, 0 );	

				kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
				kPrinter.AddTextPiece( FONT_SIZE_16, m_RollingText[m_iCurRoll].c_str() );
				kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
				kPrinter.ClearList();

				kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter.AddTextPiece( FONT_SIZE_13, "액세서리의 기간을 연장 중입니다." );
				kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
				kPrinter.ClearList();

				kPrinter.AddTextPiece( FONT_SIZE_13, "잠시만 기다려주세요." );
				kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
				kPrinter.ClearList();

				m_RollingEffect.Render( iXPos + 124, iYPos + 105, 2.0f, UI_RENDER_ADD );

				m_pIconBackProcess->Render( iXPos + 124, iYPos + 105, UI_RENDER_MULTIPLY );
			}
		}
		break;
	case US_END:
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );	

			ioUserAccessory *pAccItem = g_MyInfo.GetUserAccessory();
			if( pAccItem )
			{
				AccessorySlot kSlot;
				if(pAccItem->GetAccessorySlot(m_iItemCode, kSlot))
				{					
					if( kSlot.m_PeriodType == EPT_MORTMAIN )
					{
						kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
						kPrinter.AddTextPiece( FONT_SIZE_16, "영구 액세서리 달성" );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
						kPrinter.ClearList();

						kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter.AddTextPiece( FONT_SIZE_13, "축하합니다!" );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
						kPrinter.ClearList();

						kPrinter.SetTextColor( TCT_DEFAULT_RED );
						kPrinter.AddTextPiece( FONT_SIZE_13, "영구액세서리로 변환되었습니다" );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
						kPrinter.ClearList();

						m_SuccessEffect.Render( iXPos + 124, iYPos + 105, 2.0f, UI_RENDER_ADD );
					}
					else
					{
						kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
						kPrinter.AddTextPiece( FONT_SIZE_16, "+ %s", m_RechargeTime.c_str() );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 171, TAT_CENTER );
						kPrinter.ClearList();

						kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
						kPrinter.AddTextPiece( FONT_SIZE_13, "기간연장을 완료했습니다." );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 197, TAT_CENTER );
						kPrinter.ClearList();

						char szRemain[MAX_PATH] = "";
						Help::GetRemainTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), szRemain, sizeof(szRemain), true );
						kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter.AddTextPiece( FONT_SIZE_13, "총 잔여시간 " );
						kPrinter.SetTextColor( TCT_DEFAULT_RED );
						kPrinter.AddTextPiece( FONT_SIZE_13, szRemain );
						kPrinter.PrintFullText( iXPos + 124, iYPos + 215, TAT_CENTER );
						kPrinter.ClearList();

						m_StopEffect.Render( iXPos + 124, iYPos + 105, 2.0f, UI_RENDER_ADD );
					}
				}
			}

			m_pIconBackResult->Render( iXPos + 124, iYPos + 105, UI_RENDER_MULTIPLY );
		}
		break;
	}
}

void AccessoryRechargeProcessWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd::OnProcess( fTimePerSec );

	switch( m_UIState )
	{
	case US_START:
		break;
	case US_PROCESS:
		{
			if( m_dwProcessEndTime > 0 && m_dwProcessEndTime < FRAMEGETTIME() )
			{
				m_dwProcessEndTime = 0;
				AccessoryRechargeWnd *pWnd = dynamic_cast<AccessoryRechargeWnd*>( m_pReturnWnd );
				if( pWnd )
					pWnd->SendItemRecharge( m_iItemCode );
			}

			if( m_dwNextRollingTime < FRAMEGETTIME() )
			{
				m_dwNextRollingTime = FRAMEGETTIME() + m_dwRollingCheckTime;
				m_iCurRoll++;
				if( m_iCurRoll >= m_iMaxRoll )
					m_iCurRoll = 0;
			}

			m_RollingEffect.Process();
		}
		break;
	case US_END:
		m_StopEffect.Process();
		m_SuccessEffect.Process();
		break;
	}
}

void AccessoryRechargeProcessWnd::SetInfo( ioWnd *pReturnWnd, int iItemCode, int iItemIndex, DWORD dwUseEtcItemIndex )
{
	m_pReturnWnd = pReturnWnd;
	m_iItemCode  = iItemCode;
	m_iItemIndex = iItemIndex;
	m_dwUseEtcItemIndex = dwUseEtcItemIndex;

	ioUserAccessory *pAccItem = g_MyInfo.GetUserAccessory();
	if( !pAccItem )
		return;

	AccessorySlot kSlot;

	if(!pAccItem->GetAccessorySlot(iItemCode, kSlot))
		return;

	m_ItemName = kSlot.GetName();
	SAFEDELETE( m_pIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );

	m_UIState = US_START;

	if( !IsShow() )
		ShowWnd();
}

void AccessoryRechargeProcessWnd::SetUIStartState()
{
	m_UIState = US_START;

	ShowChildWnd( ID_OK_BTN );
	ShowChildWnd( ID_CANCEL_BTN );
	HideChildWnd( ID_PROCESS_BTN );
	HideChildWnd( ID_AGAIN_BTN );
	HideChildWnd( ID_ITEM_LIST_BTN1 );
	HideChildWnd( ID_ITEM_LIST_BTN2 );

	m_dwProcessEndTime = 0;
}

void AccessoryRechargeProcessWnd::SetUIProcessState()
{
	m_UIState = US_PROCESS;

	HideChildWnd( ID_OK_BTN );
	HideChildWnd( ID_CANCEL_BTN );
	ShowChildWnd( ID_PROCESS_BTN );
	HideChildWnd( ID_AGAIN_BTN );
	HideChildWnd( ID_ITEM_LIST_BTN1 );
	HideChildWnd( ID_ITEM_LIST_BTN2 );

	m_dwProcessEndTime = FRAMEGETTIME() + m_dwProcessTime;
	m_dwNextRollingTime = FRAMEGETTIME() + m_dwRollingCheckTime;

	AccessoryRechargeWnd *pWnd = dynamic_cast<AccessoryRechargeWnd*>( m_pReturnWnd );
	if( pWnd )
	{
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwUseEtcItemIndex );
		ioEtcAccessoryRecharge *pRecharge = dynamic_cast< ioEtcAccessoryRecharge*>(pItem);
		if( pRecharge )
		{
			m_iCurRoll = 0;
			m_iMaxRoll = pRecharge->GetMaxRechargeInfoCount();
			m_RollingText.clear();
			m_RollingText.reserve( m_iMaxRoll );

			for( int i=0; i<m_iMaxRoll; ++i )
			{
				char szTime[MAX_PATH] = "";
				ioHashString szText = "????";
				int iPromotionTime = pRecharge->GetRechargeTime(i);
				int iLimitDay = iPromotionTime / 24;
				int iLimitHour= iPromotionTime % 24;
				if( iLimitDay > 0 && iLimitHour > 0 )
				{
					wsprintf( szTime, "+ %d일 %d시간", iLimitDay, iLimitHour );
					szText = szTime;
				}
				else if( iLimitDay > 0 )
				{
					wsprintf( szTime, "+ %d일", iLimitDay );
					szText = szTime;
				}
				else
				{
					wsprintf( szTime, "+ %d시간", iLimitHour );
					szText = szTime;
				}

				m_RollingText.push_back(szText);
			}
		}
	}

	m_RollingEffect.SetPlay();

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_RollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void AccessoryRechargeProcessWnd::SetUIEndState( int iRechargeHour )
{
	m_UIState = US_END;
	m_iRechargeHour = iRechargeHour;

	char szTime[MAX_PATH] = "";
	int iPromotionTime = iRechargeHour;
	int iLimitDay = iPromotionTime / 24;
	int iLimitHour= iPromotionTime % 24;
	if( iLimitDay > 0 && iLimitHour > 0 )
	{
		wsprintf( szTime, "%d일 %d시간", iLimitDay, iLimitHour );
	}
	else if( iLimitDay > 0 )
	{
		wsprintf( szTime, "%d일", iLimitDay );
	}
	else
	{
		wsprintf( szTime, "%d시간", iLimitHour );
	}

	m_RechargeTime.Clear();
	m_RechargeTime = szTime;

	HideChildWnd( ID_OK_BTN );
	HideChildWnd( ID_CANCEL_BTN );
	HideChildWnd( ID_PROCESS_BTN );
	ShowChildWnd( ID_AGAIN_BTN );

	ioHashString szItemName;
	szItemName.Clear();
	bool bSuccess = false;

	ioUserAccessory *pAccItem = g_MyInfo.GetUserAccessory();
	if( pAccItem )
	{
		AccessorySlot kSlot;
		if(pAccItem->GetAccessorySlot(m_iItemCode, kSlot))
		{
			if( kSlot.m_PeriodType == EPT_MORTMAIN )
			{
				bSuccess = true;
				szItemName = kSlot.GetName();
			}
		}
	}

	if( bSuccess )
	{
		ShowChildWnd( ID_ITEM_LIST_BTN2 );
		m_SuccessEffect.SetPlay();
		g_SoundMgr.PlaySound( m_SuccessSound );
	}
	else
	{
		ShowChildWnd( ID_ITEM_LIST_BTN1 );
		m_StopEffect.SetPlay();
		g_SoundMgr.PlaySound( m_StopSound );
	}

	if( !m_RollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_RollingSound, 0 );

	m_RollingEffect.Init();
}

AccessoryRechargeWnd::AccessoryRechargeWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

AccessoryRechargeWnd::~AccessoryRechargeWnd()
{
	m_vInventoryItemInfoList.clear();
}

void AccessoryRechargeWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/accessoryrechargeprocesswnd.xml", this );
}

void AccessoryRechargeWnd::iwm_show()
{
	ChangeTab( ID_TAB_1_BTN );
}

void AccessoryRechargeWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	RechargeItemTooltip *pTooltip = dynamic_cast<RechargeItemTooltip*>(FindChildWnd( ID_TOOL_TIP ));
	if( pTooltip )
	{
		ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
		pTooltip->SetTooltipText( kPrinter );
	}
}

void AccessoryRechargeWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void AccessoryRechargeWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioMovingWnd::iwm_wheel( zDelta );
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

void AccessoryRechargeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(pWnd == NULL)
		return;


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
			ChangeTab( dwID );
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
			AccessoryRechargeProcessWnd *pRPWnd = dynamic_cast<AccessoryRechargeProcessWnd*>( FindChildWnd(ACCESSORY_RECHARGE_PROCESS_WND) );
			if( pRPWnd )
			{
				AccessoryInfoButton *pBtn = dynamic_cast<AccessoryInfoButton*>( FindChildWnd( dwID ) );
				if( pBtn )
				{
					ioUserAccessory *pAcc = g_MyInfo.GetUserAccessory();

					if( pAcc )
					{
						AccessorySlot kSlot;

						if( pAcc->GetAccessorySlot( pBtn->GetMagicCode(), kSlot ) )
						{
							if( kSlot.m_PeriodType == EPT_MORTMAIN )
							{
								g_GUIMgr.SetMsgBox( MB_OK, NULL, "영구액세서리는 기간연장을 할 수 없습니다." );
								return;
							}
							else
							{
								pRPWnd->SetInfo( this, pBtn->GetMagicCode(), pBtn->GetAccIndex(), m_dwEtcItemIndex );
								return;
							}
						}
					}

					g_GUIMgr.SetMsgBox( MB_OK, this, "액세서리 충전중 예외상황이 발생했습니다." );
				}
			}
		}
		break;
	}
}

void AccessoryRechargeWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vInventoryItemInfoList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		AccessoryInfoButton *pItem = dynamic_cast<AccessoryInfoButton*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray +i;
		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			InventoryItemInfo kInfo;
			pItem->SetInfo( kInfo );
			continue;
		}

		pItem->SetInfo( m_vInventoryItemInfoList[iCurArray] );
	}
}

void AccessoryRechargeWnd::ChangeTab( DWORD dwID /* = 0 */ )
{
	CheckRadioButton( AccessoryRechargeWnd::ID_TAB_1_BTN, AccessoryRechargeWnd::ID_TAB_4_BTN, dwID );

	m_dwCurTabID = dwID;

	SetAccessoryInfoVec( m_dwCurTabID );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( AccessoryRechargeWnd::ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vInventoryItemInfoList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void AccessoryRechargeWnd::SetAccessoryInfoVec( DWORD dwTabID )
{
	m_vInventoryItemInfoList.clear();

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory ) 
		return;

	int nSize = pAccessory->GetAccessorySlotCnt();

	for( int i=0; i<nSize; ++i )
	{
		AccessorySlot kSlot;
		if( !pAccessory->GetAccessorySlotArray( i, kSlot ) )
			continue;

		if( g_AccRechargeMgr.GetPromotionTime( kSlot.m_iAccessoryCode ) == 0 )
			continue;

		if( kSlot.m_PeriodType != ACCPT_TIME )
			continue;

		// 스몰탭 인덱스랑 같아야함
		int nGroup = (kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE);
		if( !IsRightTab( dwTabID, nGroup ) )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_ACCESSORY;
		kInfo.m_iMagicCode = kSlot.m_iIndex;
		kInfo.m_bEquip = kSlot.m_bEquip;

		int nSlot = kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE;
		int nValue = kSlot.m_iValue1;

		if( !kSlot.m_bEquip )
			nValue += 100;

		kInfo.m_iPriorityOrder = nSlot * 10000 + nValue;
		kInfo.m_iPriorityOrder2 = kSlot.m_iAccessoryCode;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwTabID == ID_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );
}

void AccessoryRechargeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessCustomBtnTooltip();
}

void AccessoryRechargeWnd::ProcessCustomBtnTooltip()
{
	ioUserAccessory *pAccessory = NULL;

	AccessorySlot kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	ioHashString szTooltipText;

	// 장착 툴팁
	int iEquipCharArray = -1;
	bool bShow = false;
	for( int i=ID_ITEM_INFO_WND_1_BTN; i<=ID_ITEM_INFO_WND_8_BTN; ++i )
	{
		AccessoryInfoButton *pItemWnd = dynamic_cast<AccessoryInfoButton*>(FindChildWnd( i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsIconOver() ) continue;

		ioUserAccessory *pAcc = g_MyInfo.GetUserAccessory();
		if( !pAcc ) continue;

		int iMagicCode = pItemWnd->GetMagicCode();

		if( !pAcc->GetAccessorySlot( iMagicCode, kSlot ) )
			continue;

		iEquipCharArray = g_MyInfo.FindEquipedAccessoryChar(pItemWnd->GetMagicCode());

		bShow = true;
		pAccessory = pAcc;
		break;
	}

	int iLine = 0;
	if( bShow )
	{
		AccessoryType eType = GetItemType( kSlot.m_iAccessoryCode );

		kPrinter[iLine].SetTextStyle( TS_NORMAL );
		kPrinter[iLine].SetBkColor( 0, 0, 0 );
		kPrinter[iLine].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[iLine].AddTextPiece( FONT_SIZE_13, "%s", kSlot.GetName().c_str() );
		iLine++;

		if( iEquipCharArray != -1 )
		{
			const CHARACTER &rkCharInfo = g_MyInfo.GetCharacter( iEquipCharArray );
			if( rkCharInfo.m_class_type > 0 )
			{
				ioHashString szClassName = g_MyInfo.GetClassName( rkCharInfo.m_class_type, g_MyInfo.GetCharPowerUpGradeByClassType( rkCharInfo.m_class_type ) );

				kPrinter[iLine].SetTextStyle( TS_NORMAL );
				kPrinter[iLine].SetBkColor( 0, 0, 0 );
				if( rkCharInfo.m_sex == 1 )
				{
					kPrinter[iLine].SetTextColor( TCT_DEFAULT_BLUE );
					kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%s - 남성", szClassName.c_str() );
				}
				else
				{
					kPrinter[iLine].SetTextColor( TCT_DEFAULT_RED );
					kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "%s - 여성", szClassName.c_str() );
				}
				kPrinter[iLine].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[iLine].AddTextPiece( FONT_SIZE_12, "이 착용중입니다." );
				iLine++;
			}
		}

		const ioItem* pItem = g_ItemMaker.GetItemConst( kSlot.m_iAccessoryCode, __FUNCTION__ );
		if ( !pItem )
			return;
		int iSlot = ES_RING + kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE - 1;

		char szTooltip[MAX_PATH] = "";
		float fRanNum = kSlot.m_iRandomNumber/FLOAT1000;
		if ( iSlot == ES_RING )
		{
			const ioRingItem* pRing = ToRingItemConst( pItem );
			if ( !pRing )
				return;

			sprintf( szTooltip, pRing->GetToolTipText().c_str(), fRanNum );
		}
		else if ( iSlot == ES_NECKLACE )
		{
			const ioNecklaceItem* pNeck = ToNecklaceItemConst( pItem );
			if ( !pNeck )
				return;

			sprintf( szTooltip, pNeck->GetToolTipText().c_str(), fRanNum );
		}
		else if ( iSlot == ES_BRACELET )
		{
			const ioBraceletItem* pBracelet = ToBraceletItemConst( pItem );
			if ( !pBracelet )
				return;

			sprintf( szTooltip, pBracelet->GetToolTipText().c_str(), fRanNum );
		}
		szTooltipText = szTooltip;
	}

	RechargeItemTooltip *pTooltip = dynamic_cast<RechargeItemTooltip*>(FindChildWnd( ID_TOOL_TIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter, szTooltipText, iLine );
	}
}

int AccessoryRechargeWnd::GetTabType( DWORD dwTabID )
{
	int iArray = ( dwTabID - ID_TAB_1_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	switch( dwTabID )
	{
	case ID_TAB_2_BTN:
		return 1;
	case ID_TAB_3_BTN:
		return 2;
	case ID_TAB_4_BTN:
		return 3;
	}

	return -1;
}

AccessoryRechargeWnd::AccessoryType AccessoryRechargeWnd::GetItemType( int iItemCode )
{
	int iGroup = iItemCode/100000 + 1;
	int iExtra = iItemCode/10000;
	int iRare  = iItemCode/1000%10;

	switch( iGroup )
	{
	case 1:
		return ACC_RING;
	case 2:
		return ACC_NECKLACE;
	case 3:
		return ACC_EARRING;
	}

	return ACC_NONE;
}

bool AccessoryRechargeWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

void AccessoryRechargeWnd::Update()
{
	CheckRadioButton( AccessoryRechargeWnd::ID_TAB_1_BTN, AccessoryRechargeWnd::ID_TAB_4_BTN, m_dwCurTabID );

	SetAccessoryInfoVec( m_dwCurTabID );

	ioScroll *pScroll = (ioScroll*)FindChildWnd( AccessoryRechargeWnd::ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vInventoryItemInfoList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}

	UpdatePage( m_iCurPage );
}

void AccessoryRechargeWnd::SetInfo( DWORD dwEtcItemIndex )
{
	m_dwEtcItemIndex = dwEtcItemIndex;

	if( !IsShow() )
		ShowWnd();
}

void AccessoryRechargeWnd::SendItemRecharge( int iItemIndex )
{
	AccessorySlot kSlot;

	ioUserAccessory *pAcc = g_MyInfo.GetUserAccessory();
	
	if( pAcc && pAcc->GetAccessorySlot( iItemIndex, kSlot ) )
	{
		SP2Packet kPacket( CTPK_ETCITEM_USE );
		kPacket << static_cast<int>( m_dwEtcItemIndex );
		kPacket << kSlot.m_iIndex;
		kPacket << kSlot.m_iAccessoryCode;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
}