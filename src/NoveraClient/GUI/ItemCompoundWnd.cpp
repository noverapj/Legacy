
#include "StdAfx.h"

#include "ItemCompoundWnd.h"
#include "MyItemSelecteWnd.h"

//////////////////////////////////////////////////////////////////////////////
SelectExtraItemBtn::SelectExtraItemBtn()
{
	m_iCurSelectSlot = 0;
	m_szSelectText.Clear();
}

SelectExtraItemBtn::~SelectExtraItemBtn()
{
}

void SelectExtraItemBtn::iwm_show()
{
	m_iCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;
	m_szSelectText.Clear();
}

void SelectExtraItemBtn::InitData()
{
	m_iCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;
	m_szSelectText.Clear();
}

void SelectExtraItemBtn::SetSelectExtraItemSlot( int iSelectSlot )
{
	m_iCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;
	m_szSelectText.Clear();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		if( pExtraItem->GetExtraItem( iSelectSlot, kSlot ) )
		{
			m_iCurSelectSlot = iSelectSlot;

			char szBuf[MAX_PATH] = "";
			ZeroMemory( szBuf, sizeof( szBuf ) );

			if( kSlot.m_iTradeState == EET_ENABLE )
			{
				SafeSprintf( szBuf, sizeof( szBuf ), STR(2), kSlot.m_Name.c_str(), kSlot.m_iReinforce );
				m_dwCurTitleColor = TCT_DEFAULT_RED;
			}
			else if( kSlot.m_iTradeState == EET_NORMAL )
			{
				SafeSprintf( szBuf, sizeof( szBuf ), STR(3), kSlot.m_Name.c_str(), kSlot.m_iReinforce );
				m_dwCurTitleColor = TCT_DEFAULT_RED;
			}
			else
			{
				SafeSprintf( szBuf, sizeof( szBuf ), STR(1), kSlot.m_Name.c_str(), kSlot.m_iReinforce );
			}
			
			m_szSelectText = szBuf;
		}
	}
}

void SelectExtraItemBtn::SetSelectAccessorySlot( int iSelectSlot )
{
	m_iCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;
	m_szSelectText.Clear();

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( pAccessory )
	{
		AccessorySlot kSlot;
		if( pAccessory->GetAccessorySlot( iSelectSlot, kSlot ) )
		{
			m_iCurSelectSlot = iSelectSlot;

			m_szSelectText = kSlot.GetName();
		}
	}
}

void SelectExtraItemBtn::SetSelectMaterial( int nSelectMaterial, ioHashString szTitle )
{
	m_iCurSelectSlot = nSelectMaterial;	
	m_szSelectText = szTitle;
}

void SelectExtraItemBtn::SetCurTitleColor( DWORD dwTitleColor )
{
	m_dwCurTitleColor = dwTitleColor;
}

void SelectExtraItemBtn::OnRender()
{
	ioButton::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioUITitle *pTitle =  GetTitle();
	if( pTitle )
	{
		pTitle->SetColor( "Normal", m_dwCurTitleColor, 0xFF000000 );
		pTitle->SetColor( "Over", m_dwCurTitleColor, 0xFF000000 );
		pTitle->SetColor( "Push", m_dwCurTitleColor, 0xFF000000 );
	}

	if( !m_szSelectText.IsEmpty() )
		SetTitleText( m_szSelectText.c_str() );
	else
		SetTitleText( m_szDefaultText.c_str() );
}

void SelectExtraItemBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_szDefaultText = xElement.GetStringAttribute_e( "DescText" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ItemCompoundWnd::ItemCompoundWnd(void)
{
	m_bAllSelect = false;
	m_vTotalList.clear();

	m_iTargetReinforce = 0;
	m_iVictimReinforce = 0;
	m_iReUseTargetSlot = 0;

	m_pManual = NULL;

	m_iSelectedBtnType = 0;
}

ItemCompoundWnd::~ItemCompoundWnd(void)
{
}

void ItemCompoundWnd::iwm_create()
{
}

void ItemCompoundWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	ShowWnd();
}

void ItemCompoundWnd::iwm_show()
{
	m_bAllSelect = false;
	m_vTotalList.clear();

	m_iTargetReinforce = 0;
	m_iVictimReinforce = 0;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// Load Total ExtraItemList
	LoadTotalList();
	//

	if( m_vTotalList.empty() )
		SetChildInActive( ID_TARGETITEM_BTN );
	else
		SetChildActive( ID_TARGETITEM_BTN );

	SetChildInActive( ID_VICTIMITEM_BTN );

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget )
		pTarget->InitData();

	SelectExtraItemBtn *pVictim = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIMITEM_BTN));
	if( pVictim )
		pVictim->InitData();

	InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
	if( pTargetIconBtn )
		pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );

	InventoryIconBtn *pVictimIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_VICTIM_ICON));
	if( pVictimIconBtn )
		pVictimIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );
	
	HideChildWnd( ID_TARGETITEM_WND );
	HideChildWnd( ID_VICTIMITEM_WND );
	
	TargetReload();
	UpdateData( true );
}

void ItemCompoundWnd::LoadTotalList()
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
	{
		HideWnd();
		return;
	}

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		InvenItemInfo kInfo;
		kInfo.m_iSlotIndex = kSlot.m_iIndex;
		kInfo.m_iItemCode = kSlot.m_iItemCode;
		kInfo.m_iReinforce = kSlot.m_iReinforce;

		m_vTotalList.push_back( kInfo );
	}

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );
}

void ItemCompoundWnd::GetTargetList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMgr.GetMaxReinforce() )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = kTargetSlot.m_iIndex;

		char szName[MAX_PATH]="";
		if( kTargetSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), STR(1), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kTargetSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), STR(2), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}


void ItemCompoundWnd::GetTargetSeletInfo( ExtraItemList &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMgr.GetMaxReinforce() )
			continue;

		rkList.push_back( kTargetSlot );
	}
}


void ItemCompoundWnd::GetVictimList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	int iSelectTarget = 0;
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget )
		iSelectTarget = pTarget->GetSelectExtraItemSlot();

	if( iSelectTarget == 0 )
		return;

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	EXTRAITEMSLOT kTargetSlot;
	if( !pItem->GetExtraItem( iSelectTarget, kTargetSlot ) )
		return;

	int iTargetType = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	int iSize = m_vTotalList.size();

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort2() );

	for( int i=0; i < iSize; ++i )
	{
		int iSlot = m_vTotalList[i].m_iSlotIndex;
		if( iSlot == iSelectTarget )
			continue;

		// Part Check
		int iType = m_vTotalList[i].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
		if( g_CompoundMgr.EnableSamePart() && iTargetType != iType )
			continue;

		// Code Check
		if( g_CompoundMgr.EnableSameCode() && kTargetSlot.m_iItemCode != m_vTotalList[i].m_iItemCode )
			continue;

		// 동일 장비의 경우 강화값 높은걸 재료로 쓰지 못하게 막는 사항
		/*
		if( kTargetSlot.m_iItemCode == m_vTotalList[i].m_iItemCode &&
			kTargetSlot.m_iReinforce < m_vTotalList[i].m_iReinforce )
			continue;
		*/

		EXTRAITEMSLOT kVictimSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kVictimSlot ) )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = kVictimSlot.m_iIndex;

		char szName[MAX_PATH]="";

		if( kVictimSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), STR(1), kVictimSlot.m_Name.c_str(), kVictimSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kVictimSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), STR(2), kVictimSlot.m_Name.c_str(), kVictimSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kVictimSlot.m_Name.c_str(), kVictimSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void ItemCompoundWnd::GetVictimSeletInfo( ExtraItemList &rkList )
{
	rkList.clear();

	int iSelectTarget = 0;
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget )
		iSelectTarget = pTarget->GetSelectExtraItemSlot();

	if( iSelectTarget == 0 )
		return;

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	EXTRAITEMSLOT kTargetSlot;
	if( !pItem->GetExtraItem( iSelectTarget, kTargetSlot ) )
		return;

	int iTargetType = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	int iSize = m_vTotalList.size();

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort2() );

	for( int i=0; i < iSize; ++i )
	{
		int iSlot = m_vTotalList[i].m_iSlotIndex;
		if( iSlot == iSelectTarget )
			continue;

		// Part Check
		int iType = m_vTotalList[i].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
		if( g_CompoundMgr.EnableSamePart() && iTargetType != iType )
			continue;

		// Code Check
		if( g_CompoundMgr.EnableSameCode() && kTargetSlot.m_iItemCode != m_vTotalList[i].m_iItemCode )
			continue;

		// 동일 장비의 경우 강화값 높은걸 재료로 쓰지 못하게 막는 사항
		/*
		if( kTargetSlot.m_iItemCode == m_vTotalList[i].m_iItemCode &&
			kTargetSlot.m_iReinforce < m_vTotalList[i].m_iReinforce )
			continue;
		*/

		EXTRAITEMSLOT kVictimSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kVictimSlot ) )
			continue;

		char szName[MAX_PATH]="";

		rkList.push_back( kVictimSlot );
	}
}


void ItemCompoundWnd::iwm_hide()
{
	m_bAllSelect = false;
	m_vTotalList.clear();

	m_iTargetReinforce = 0;
	m_iVictimReinforce = 0;

	HideChildWnd( ID_TARGETITEM_WND );
	HideChildWnd( ID_VICTIMITEM_WND );
}

void ItemCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemCompound();
		}
		break;
	case ID_TARGETITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pVictim = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIMITEM_BTN));
			if( pVictim )
				pVictim->InitData();

			InventoryIconBtn *pVictimIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_VICTIM_ICON));
			if( pVictimIconBtn )
				pVictimIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );

			SetChildInActive( ID_VICTIMITEM_BTN );
			HideChildWnd( ID_VICTIMITEM_WND );

			int iSize = m_vTotalList.size();
			
			MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
			if ( pSelectWnd && iSize > 0 )
			{
				m_iSelectedBtnType = ID_TARGETITEM_BTN;
				ExtraItemList rkList;
				GetTargetSeletInfo( rkList );
				pSelectWnd->ShowItemSeletWnd( rkList, this );
			}
		}
		break;
	case ID_VICTIMITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSelectTarget = 0;
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
			if( pTarget )
				iSelectTarget = pTarget->GetSelectExtraItemSlot();
						
			MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
			if ( pSelectWnd )
			{
				m_iSelectedBtnType = ID_VICTIMITEM_BTN;
				ExtraItemList rkList;
				GetVictimSeletInfo( rkList );
				if ( !rkList.empty() )
					pSelectWnd->ShowItemSeletWnd( rkList, this );
				
			}
		}
		break;
	case ID_TARGETITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				TargetListBtnUp( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
			UpdateData( true );
		}
		break;
	case ID_VICTIMITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				VictimListBtnUp( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
			UpdateData( true );
		}
		break;
		
	case MYITEM_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_iSelectedBtnType == 0 && param <= 0 )
				break;
			SelectExtraItemBtn *pBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(m_iSelectedBtnType));
			if( pBtn )
				pBtn->SetSelectExtraItemSlot( param );
			UpdateData( true );
			m_iSelectedBtnType = 0;
		}
		break;
	}
}

void ItemCompoundWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	SuccessRateDescRender();
	PrintManual( iXPos+27, iYPos+261, FONT_SIZE_12 );
}

void ItemCompoundWnd::SuccessRateDescRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		INFO_LEFT_X_OFFSET = 34,
		INFO_LEFT_Y_OFFSET = 373,

		INFO_RIGHT_X_OFFSET = 223,
		INFO_RIGHT_Y_OFFSET = 373,

		INFO_GAP = 21,
	};

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	SelectExtraItemBtn *pVictim = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIMITEM_BTN));
	if( !pVictim ) return;

	if( m_bAllSelect )
	{
		int iTargetSlot = pTarget->GetSelectExtraItemSlot();
		int iVictimSlot = pVictim->GetSelectExtraItemSlot();

		int iSmall = m_iTargetReinforce + g_CompoundMgr.GetCompoundReinforce( CMT_SMALL, m_dwEtcItemCode );
		int iBig = m_iTargetReinforce + g_CompoundMgr.GetCompoundReinforce( CMT_BIG, m_dwEtcItemCode );
		int iDown = m_iTargetReinforce - g_CompoundMgr.GetCompoundReinforce( CMT_DOWN, m_dwEtcItemCode );
		iDown = max( 0, iDown );

		int iSmallRate = g_CompoundMgr.GetCompoundSuccessRate( iTargetSlot, iVictimSlot, CMT_SMALL, m_dwEtcItemCode );
		int iBigRate = g_CompoundMgr.GetCompoundSuccessRate( iTargetSlot, iVictimSlot, CMT_BIG, m_dwEtcItemCode );
		int iFailRate = 100 - (iSmallRate + iBigRate);

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, STR(1), iBig );
		g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET + INFO_GAP, FONT_SIZE_12, STR(2), iSmall );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET + INFO_GAP*2, FONT_SIZE_12, STR(3), iDown );

		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET, FONT_SIZE_12, STR(4), iBigRate );
		g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET + INFO_GAP, FONT_SIZE_12, STR(5), iSmallRate );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET + INFO_GAP*2, FONT_SIZE_12, STR(6), iFailRate );
	}
}

void ItemCompoundWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;


	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

void ItemCompoundWnd::UpdateData( bool bCheckReinforce )
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	SelectExtraItemBtn *pVictim = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIMITEM_BTN));
	if( !pVictim ) return;

	bool bTarget, bVictim;
	bTarget = bVictim = false;

	int iTargetSlot = pTarget->GetSelectExtraItemSlot();
	int iVictimSlot = pVictim->GetSelectExtraItemSlot();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( iTargetSlot, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );

				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();
			}

			bTarget = true;
			m_iTargetReinforce = kSlot.m_iReinforce;
		}

		// Victim Check
		if( pExtraItem->GetExtraItem( iVictimSlot, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_VICTIM_ICON));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, 0.62f, 0.62f, kSlot.m_iReinforce );

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );

				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();
			}

			bVictim = true;
			m_iVictimReinforce = kSlot.m_iReinforce;
		}
	}

	if( bTarget && bVictim )
		m_bAllSelect = true;
	else
		m_bAllSelect = false;

	if( bTarget )
		SetChildActive(ID_VICTIMITEM_BTN);
	else
		SetChildInActive(ID_VICTIMITEM_BTN);

	if( bCheckReinforce && bVictim && m_iTargetReinforce < m_iVictimReinforce )
	{
		pVictim->SetCurTitleColor( TCT_DEFAULT_RED );

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void ItemCompoundWnd::SendItemCompound()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	SelectExtraItemBtn *pVictim = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIMITEM_BTN));
	if( !pVictim ) return;

	bool bTarget, bVictim;
	bTarget = bVictim = false;

	int iTargetSlot = pTarget->GetSelectExtraItemSlot();
	int iVictimSlot = pVictim->GetSelectExtraItemSlot();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( iTargetSlot, kSlot ) )
			bTarget = true;

		// Victim Check
		if( pExtraItem->GetExtraItem( iVictimSlot, kSlot ) )
			bVictim = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	else if( !bVictim )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	m_iReUseTargetSlot = iTargetSlot;

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemCode;
	kPacket << iTargetSlot;
	kPacket << iVictimSlot;
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );

	HideWnd();
}

bool ItemCompoundWnd::CheckWarning( ioWnd *pWnd, int iSlot )
{
	if( !pWnd ) return false;
	if( pWnd->GetID() != ID_VICTIMITEM_WND )
		return false;

	// Victim만 여기로 온다.
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Victim Check
		if( pExtraItem->GetExtraItem( iSlot, kSlot ) )
		{
			if( m_iTargetReinforce < kSlot.m_iReinforce )
				return true;
		}
	}

	return false;
}

void ItemCompoundWnd::TargetListBtnUp( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetTargetList( kItemList );	

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		return;			

	ioUITitle *pTitle =  pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iSlot = rkItem.m_iMagicCode;

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget && iSlot > 0 )
		pTarget->SetSelectExtraItemSlot( iSlot );
}

void ItemCompoundWnd::VictimListBtnUp( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetVictimList( kItemList );	

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		return;			

	ioUITitle *pTitle =  pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iSlot = rkItem.m_iMagicCode;

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIMITEM_BTN));
	if( pTarget && iSlot > 0 )
		pTarget->SetSelectExtraItemSlot( iSlot );
}

void ItemCompoundWnd::TargetReload()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget || m_iReUseTargetSlot <= 0 )
		return;

	vNewShopPullDownItem kItemList;
	GetTargetList( kItemList );	

	vNewShopPullDownItem::iterator iter = kItemList.begin();
	for( ; iter != kItemList.end(); ++iter )
	{
		const NewShopPullDownItem& rkNewShopItem = *iter;
		if( rkNewShopItem.m_iMagicCode == m_iReUseTargetSlot )
		{			
			pTarget->SetSelectExtraItemSlot( m_iReUseTargetSlot );
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
ItemGrowthCatalystWnd::ItemGrowthCatalystWnd(void)
{
	m_vTotalList.clear();

	m_iTargetReinforce = 0;

	m_pManual = NULL;
}

ItemGrowthCatalystWnd::~ItemGrowthCatalystWnd(void)
{
}

void ItemGrowthCatalystWnd::iwm_create()
{
}

void ItemGrowthCatalystWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	ShowWnd();
}

void ItemGrowthCatalystWnd::iwm_show()
{
	m_vTotalList.clear();

	m_iTargetReinforce = 0;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// Load Total ExtraItemList
	LoadTotalList();
	//

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget )
		pTarget->InitData();

	InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
	if( pTargetIconBtn )
	{
		pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );
	}

	HideChildWnd( ID_TARGETITEM_WND );

	UpdateData();
}

void ItemGrowthCatalystWnd::LoadTotalList()
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
	{
		HideWnd();
		return;
	}

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		if( kSlot.m_PeriodType != EPT_GROW_TIME )
			continue;

		InvenItemInfo kInfo;
		kInfo.m_iSlotIndex = kSlot.m_iIndex;
		kInfo.m_iItemCode  = kSlot.m_iItemCode;
		kInfo.m_iReinforce = kSlot.m_iReinforce;

		m_vTotalList.push_back( kInfo );
	}

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );
}

void ItemGrowthCatalystWnd::GetTargetList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode           = kTargetSlot.m_iIndex;
		kItem.m_bGrowthCatalystItem  = true;
		kItem.m_iGrowthCatalystLevel = kTargetSlot.m_iReinforce;

		char szName[MAX_PATH] = "";
		StringCbPrintf( szName, sizeof( szName ), "%s", kTargetSlot.m_Name.c_str() );
		kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void ItemGrowthCatalystWnd::GetTargetSeletInfo( ExtraItemList &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode           = kTargetSlot.m_iIndex;
		kItem.m_bGrowthCatalystItem  = true;
		kItem.m_iGrowthCatalystLevel = kTargetSlot.m_iReinforce;
		rkList.push_back( kTargetSlot );
	}
}

void ItemGrowthCatalystWnd::iwm_hide()
{
	m_vTotalList.clear();

	m_iTargetReinforce = 0;

	HideChildWnd( ID_TARGETITEM_WND );
}

void ItemGrowthCatalystWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemGrowthCatalyst();
		}
		break;
	case ID_TARGETITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSize = m_vTotalList.size();
			if( iSize == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
			MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
			if ( pSelectWnd && iSize > 0 )
			{
				ExtraItemList rkList;
				GetTargetSeletInfo( rkList );
				pSelectWnd->ShowItemSeletWnd( rkList, this );
			}
		}
		break;
	case ID_TARGETITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				TargetListBtnUp( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
			UpdateData();
		}
		break;
	case MYITEM_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
			if( pTarget && param > 0 )
				pTarget->SetSelectExtraItemSlot( param );
			UpdateData();
		}
		break;
	}
}

void ItemGrowthCatalystWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+205, FONT_SIZE_12 );
}

void ItemGrowthCatalystWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;


	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

void ItemGrowthCatalystWnd::UpdateData()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	bool bTarget = false;

	int iTargetSlot = pTarget->GetSelectExtraItemSlot();
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( iTargetSlot, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );
				pIconBtn->SetExtraItemGrowthCatalyst( true );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();
			}

			bTarget = true;
			m_iTargetReinforce = kSlot.m_iReinforce;
		}
	}
}

void ItemGrowthCatalystWnd::SendItemGrowthCatalyst()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	bool bTarget = false;

	int iTargetSlot = pTarget->GetSelectExtraItemSlot();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( iTargetSlot, kSlot ) )
			bTarget = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemCode;
	kPacket << iTargetSlot;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd )
	{
		pInvenWnd->OnItemGrowthCatalystDelay( iTargetSlot );
		pInvenWnd->UpdateTab( false, false );
	}
}

void ItemGrowthCatalystWnd::TargetListBtnUp( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetTargetList( kItemList );	

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		return;			

	ioUITitle *pTitle =  pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iSlot = rkItem.m_iMagicCode;

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget && iSlot > 0 )
		pTarget->SetSelectExtraItemSlot( iSlot );
}

//////////////////////////////////////////////////////////////////////////////
ItemLuckyCoinWnd::ItemLuckyCoinWnd()
{
	m_dwEtcItemCode = 0;
	m_iExchangeIndex = -1;
	m_iExchangeManual = 0;

	m_pManual = NULL;
}

ItemLuckyCoinWnd::~ItemLuckyCoinWnd()
{
}

void ItemLuckyCoinWnd::iwm_create()
{
}

void ItemLuckyCoinWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	ShowWnd();
}

void ItemLuckyCoinWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON_BTN));
	if( pTargetIconBtn )
	{
		pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );
	}

	m_iExchangeIndex = -1;
	ioWnd *pSelectBtn = FindChildWnd( ID_KIND_BTN );
	if( pSelectBtn )
	{
		pSelectBtn->SetTitleText( STR(1) );
	}

	HideChildWnd( ID_KIND_WND );
}

void ItemLuckyCoinWnd::iwm_hide()
{
	HideChildWnd( ID_KIND_WND );
}

void ItemLuckyCoinWnd::GetItemList( vNewShopPullDownItem &rkList )
{
	ioEtcItemLuckyCoin *pEtcItem = dynamic_cast< ioEtcItemLuckyCoin * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode));
	if( pEtcItem == NULL ) return;

	int iMaxExchangeList = pEtcItem->GetExchangeCount();
	for(int i = 0;i < iMaxExchangeList;i++)
	{
		int iExchangeIndex = -1;
		int iExchangeUseManual = 0;
		ioHashString szExchangeName;
		ioHashString szExchangeIcon;
		bool bExchangeCostume;
		int nGradeType = 0;
		pEtcItem->GetExchangeUseData( i, iExchangeIndex, iExchangeUseManual, szExchangeName, szExchangeIcon, bExchangeCostume, nGradeType );

		if( iExchangeIndex == -1 ) continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = i;
		kItem.m_szName = szExchangeName;
		kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;

		rkList.push_back( kItem );
	}
}

void ItemLuckyCoinWnd::GetItemList( vInventorySelectItemList &rkList, ioHashStringVec& vRadioNameList )
{
	vRadioNameList.clear();
	rkList.clear();
	ioEtcItemLuckyCoin *pEtcItem = dynamic_cast< ioEtcItemLuckyCoin * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode));
	if( pEtcItem == NULL ) 
		return;

	vRadioNameList = pEtcItem->GetRadionNameList();

	int iMaxExchangeList = pEtcItem->GetExchangeCount();
	for(int i = 0;i < iMaxExchangeList;i++)
	{
		int iExchangeIndex = -1;
		int iExchangeUseManual = 0;
		ioHashString szExchangeName;
		ioHashString szExchangeIcon;
		bool bExchangeCostume;
		int nGrade = 0;
		pEtcItem->GetExchangeUseData( i, iExchangeIndex, iExchangeUseManual, szExchangeName, szExchangeIcon, bExchangeCostume, nGrade );
		if( iExchangeIndex == -1 ) 
			continue;

		InventorySelectItem kItem;
		kItem.m_iMagicCode = i;
		kItem.m_szName = szExchangeName;
		kItem.m_szIconName = szExchangeIcon;
		pEtcItem->GetExchangeUseTabData( i, kItem.m_szSubText, kItem.m_szSubText2, kItem.m_iTabType, kItem.m_iNeedCoin );
		if ( kItem.m_szSubText.IsEmpty() )
		{
			char szKey[MAX_PATH];
			sprintf_e( szKey, "%d개", kItem.m_iNeedCoin );
			kItem.m_szSubText = szKey;
		}
		kItem.m_nGrade = nGrade;
		rkList.push_back( kItem );
	}
}

void ItemLuckyCoinWnd::SelectListBtnUp( int iSelectIndex )
{
	ioEtcItemLuckyCoin *pEtcItem = dynamic_cast< ioEtcItemLuckyCoin * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode));
	if( pEtcItem == NULL ) return;
	
	m_iExchangeIndex = -1;
	m_bExchangeCostume = false;

	int nGrade = 0;
	pEtcItem->GetExchangeUseData( iSelectIndex, m_iExchangeIndex, m_iExchangeManual, m_szExchangeName, m_szExchangeIcon, m_bExchangeCostume, nGrade );

	if( m_iExchangeIndex == -1 )
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_KIND_BTN );
		if( pSelectBtn )
			pSelectBtn->SetTitleText( STR(1) );
	}
	else
	{
		ioWnd *pSelectBtn = FindChildWnd( ID_KIND_BTN );
		if( pSelectBtn )
			pSelectBtn->SetTitleText( m_szExchangeName.c_str() );

		InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>( FindChildWnd(ID_ICON_BTN) );
		if( pIconBtn )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( m_szExchangeIcon );
			pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1, 0 );
			pIconBtn->SetGradeType( nGrade );
			pIconBtn->SetCostume( m_bExchangeCostume );
			pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pIconBtn->SetActive();
		}
	}
}

void ItemLuckyCoinWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemLuckyCoin();
		}
		break;
	case ID_KIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			InventorySelectItemWnd *pInventorySelectItemWnd = dynamic_cast< InventorySelectItemWnd* >( g_GUIMgr.FindWnd(INVENTORY_SELECT_ITEM_WND) );
			if ( pInventorySelectItemWnd && pInventorySelectItemWnd->IsShow() )
				pInventorySelectItemWnd->HideWnd();
			else if ( pInventorySelectItemWnd )
			{
				ShowInventorySelectItemWnd( pInventorySelectItemWnd );
			}
		}
		break;
	case ID_KIND_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				SelectListBtnUp( param );
				pPullDownWnd->HideWnd();
			}
		}
		break;
	case INVENTORY_SELECT_ITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			InventorySelectItemWnd *pInventorySelectItemWnd = dynamic_cast< InventorySelectItemWnd* >( g_GUIMgr.FindWnd(INVENTORY_SELECT_ITEM_WND) );
			if( pInventorySelectItemWnd )
			{
				SelectListBtnUp( param );
				pInventorySelectItemWnd->HideWnd();
			}
		}
		break;
	}
}

void ItemLuckyCoinWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+33, iYPos+352, FONT_SIZE_12 );
}

void ItemLuckyCoinWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;
	if( m_iExchangeIndex == -1 )
		return;

	ioComplexStringPrinter kPrinter;
	int iSubManual = m_iExchangeManual - 1;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
		a_iYPos += 21;
	}
}

void ItemLuckyCoinWnd::SendItemLuckyCoin()
{
	if( m_iExchangeIndex == -1 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}


	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemCode;
	kPacket << m_iExchangeIndex;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemLuckyCoinWnd::ShowInventorySelectItemWnd( InventorySelectItemWnd *pInventorySelectItemWnd )
{
	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem(m_dwEtcItemCode);
	if ( !pEtcItem )
		return;

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if ( !pUserEtcItem )
		return;

	ioUserEtcItem::ETCITEMSLOT kEtcSlot;
	if( !pUserEtcItem->GetEtcItem( m_dwEtcItemCode, kEtcSlot ) )
		return;

	int iEtcitemCount = kEtcSlot.m_iValue1;
	vInventorySelectItemList vItemList;
	ioHashStringVec vRadioNameList;
	GetItemList( vItemList, vRadioNameList );
	if ( vItemList.empty() )
		return;

	pInventorySelectItemWnd->ShowInventorySelectItemWnd( vItemList, this, vRadioNameList, pEtcItem->GetName(), iEtcitemCount );
}

//////////////////////////////////////////////////////////////////////////////
ItemCompoundExWnd::ItemCompoundExWnd(void)
{
	m_vTotalList.clear();

	m_iTargetReinforce = 0;

	m_pManual = NULL;
}

ItemCompoundExWnd::~ItemCompoundExWnd(void)
{
}

void ItemCompoundExWnd::iwm_create()
{
}

void ItemCompoundExWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	ShowWnd();
}

void ItemCompoundExWnd::iwm_show()
{
	m_vTotalList.clear();

	m_iTargetReinforce = 0;

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// Load Total ExtraItemList
	LoadTotalList();
	//

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget )
		pTarget->InitData();

	InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
	if( pTargetIconBtn )
	{
		pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );
	}

	HideChildWnd( ID_TARGETITEM_WND );

	UpdateData();
}

void ItemCompoundExWnd::LoadTotalList()
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
	{
		HideWnd();
		return;
	}

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		InvenItemInfo kInfo;
		kInfo.m_iSlotIndex = kSlot.m_iIndex;
		kInfo.m_iItemCode  = kSlot.m_iItemCode;
		kInfo.m_iReinforce = kSlot.m_iReinforce;

		m_vTotalList.push_back( kInfo );
	}

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );
}

void ItemCompoundExWnd::GetTargetList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMgr.GetMaxReinforce() )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = kTargetSlot.m_iIndex;

		char szName[MAX_PATH]="";
		if( kTargetSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), STR(1), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kTargetSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), STR(2), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void ItemCompoundExWnd::GetTargetSeletInfo( ExtraItemList &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalList.begin(), m_vTotalList.end(), CompoundItemInfoSort1() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMgr.GetMaxReinforce() )
			continue;

		//NewShopPullDownItem kItem;
		//kItem.m_iMagicCode = kTargetSlot.m_iIndex;

		char szName[MAX_PATH]="";
		if( kTargetSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), STR(1), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			//kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kTargetSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), STR(2), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			//kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			//kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		//kItem.m_szName      = szName;
		rkList.push_back( kTargetSlot );
	}
}

void ItemCompoundExWnd::iwm_hide()
{
	m_vTotalList.clear();

	m_iTargetReinforce = 0;

	HideChildWnd( ID_TARGETITEM_WND );
}

void ItemCompoundExWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemCompoundEx();
		}
		break;
	case ID_TARGETITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSize = m_vTotalList.size();
			if( iSize == 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
			if ( pSelectWnd )
			{
				ExtraItemList rkList;
				GetTargetSeletInfo( rkList );
				pSelectWnd->ShowItemSeletWnd( rkList, this );
			}
		}
		break;
	case ID_TARGETITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				TargetListBtnUp( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
			UpdateData();
		}
		break;
	case MYITEM_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
			if( pTarget && param > 0 )
				pTarget->SetSelectExtraItemSlot( param );
			UpdateData();
		}
		break;
	}
}

void ItemCompoundExWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+205, FONT_SIZE_12 );
}

void ItemCompoundExWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

void ItemCompoundExWnd::UpdateData()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	bool bTarget = false;

	int iTargetSlot = pTarget->GetSelectExtraItemSlot();
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( iTargetSlot, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );

				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();
			}

			bTarget = true;
			m_iTargetReinforce = kSlot.m_iReinforce;
		}
	}
}

void ItemCompoundExWnd::SendItemCompoundEx()
{
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( !pTarget ) return;

	bool bTarget = false;

	int iTargetSlot = pTarget->GetSelectExtraItemSlot();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		// Target Check
		if( pExtraItem->GetExtraItem( iTargetSlot, kSlot ) )
			bTarget = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemCode;
	kPacket << iTargetSlot;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemCompoundExWnd::TargetListBtnUp( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetTargetList( kItemList );	

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		return;			

	ioUITitle *pTitle =  pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iSlot = rkItem.m_iMagicCode;

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGETITEM_BTN));
	if( pTarget && iSlot > 0 )
		pTarget->SetSelectExtraItemSlot( iSlot );
}
//////////////////////////////////////////////////////////////////////////
ItemRainbowMixerWnd::ItemRainbowMixerWnd()
{
	m_dwEtcItemCode = 0;
	m_bPattern		= false;
	m_pManual       = NULL;
	m_dwPatternScreenTime = 0;
	m_bPatternScreen= false;

	m_pBackground		= NULL;
	m_pSlotEquipIcon	= NULL;
	m_pSlotEquipLine1	= NULL;
	m_pSlotEquipLine2	= NULL;
	m_pSlotEquipOver	= NULL;
}

ItemRainbowMixerWnd::~ItemRainbowMixerWnd()
{
	ClearRainbowSlot();
	SAFEDELETE( m_pBackground );
	SAFEDELETE( m_pSlotEquipIcon );
	SAFEDELETE( m_pSlotEquipLine1 );
	SAFEDELETE( m_pSlotEquipLine2 );
	SAFEDELETE( m_pSlotEquipOver );
}

void ItemRainbowMixerWnd::ClearRainbowSlot()
{
	int iSlotSize = m_RainbowSlotList.size();
	for(int i = 0;i < iSlotSize;i++)
	{
		RainbowSlotData &rkSlotData = m_RainbowSlotList[i];
		SAFEDELETE( rkSlotData.m_pIcon );
	}
	m_RainbowSlotList.clear();
}

void ItemRainbowMixerWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SlotEquipIcon" )
	{
		SAFEDELETE( m_pSlotEquipIcon );
		m_pSlotEquipIcon = pImage;
	}
	else if( szType == "SlotEquipLine1" )
	{
		SAFEDELETE( m_pSlotEquipLine1 );
		m_pSlotEquipLine1 = pImage;
	}
	else if( szType == "SlotEquipLine2" )
	{
		SAFEDELETE( m_pSlotEquipLine2 );
		m_pSlotEquipLine2 = pImage;
	}
	else if( szType == "SlotEquipOver" )
	{
		SAFEDELETE( m_pSlotEquipOver );
		m_pSlotEquipOver = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ItemRainbowMixerWnd::SendItemMixer()
{
	if( !m_bPattern )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else
	{
		ioEtcItemRainbowMixer *pEtcItem = dynamic_cast< ioEtcItemRainbowMixer * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
		if( pEtcItem == NULL ) return;

		int i = 0;
		DWORDVec kEquipItem;
		for(i = ID_SLOT_BTN1;i < ID_SLOT_BTN7 + 1;i++)
		{
			ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
			if( pButton )
			{
				int iSlot = i - ID_SLOT_BTN1;
				if( !COMPARE( iSlot, 0, (int)m_RainbowSlotList.size() ) ) continue;

				if( m_RainbowSlotList[iSlot].m_bEquipSlot )
					kEquipItem.push_back( m_RainbowSlotList[iSlot].m_dwEtcItem );
			}
		}

		if( kEquipItem.empty() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		else 
		{
			for(i = 0;i < (int)kEquipItem.size();i++)
			{
				bool bNoneItem = true;
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kEtcItem;
					if( pUserEtcItem->GetEtcItem( kEquipItem[i], kEtcItem ) )
					{
						if( kEtcItem.GetUse() > 0 )
						{
							bNoneItem = false;
						}
					}
				}

				if( bNoneItem )
				{
					ioEtcItem *pEtcItemSlot = g_EtcItemMgr.FindEtcItem( kEquipItem[i] );
					if( pEtcItemSlot )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), pEtcItemSlot->GetName().c_str() );
					else
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
					return;
				}
			}

			int iPresentCode = pEtcItem->GetPatternPresent( kEquipItem );
			if( iPresentCode != -1 )
			{
				SP2Packet kPacket( CTPK_ETCITEM_USE );
				kPacket << (int)m_dwEtcItemCode;			
				kPacket << iPresentCode;
				kPacket << (int)kEquipItem.size();
				for(int k = 0;k < (int)kEquipItem.size();k++)
					kPacket << kEquipItem[k];
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );

				HideWnd();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			}
		}
		kEquipItem.clear();
	}
}

void ItemRainbowMixerWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;
	m_bPattern		= false;

	ioEtcItemRainbowMixer *pEtcItem = dynamic_cast< ioEtcItemRainbowMixer * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
	if( pEtcItem == NULL ) return;
	
	ioHashString kBackImage;
	pEtcItem->GetRandomBackGround( kBackImage );
	SAFEDELETE( m_pBackground );
	m_pBackground = g_UIImageSetMgr.CreateImageByFullName( kBackImage );

	ClearRainbowSlot();
	int iMaxMixerData = pEtcItem->GetMaxMixerList();
	for(int i = 0;i < iMaxMixerData;i++)
	{
		RainbowSlotData kSlotData;		
		ioHashString kEquipIcon, kReleaseIcon;
		kSlotData.m_dwEtcItem = pEtcItem->GetMixerData( i, kEquipIcon, kReleaseIcon );
		if( !kEquipIcon.IsEmpty() )
		{
			kSlotData.m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kEquipIcon );
		}
		m_RainbowSlotList.push_back( kSlotData );
	}
	SetEquipPatternLine();

	ShowWnd();
}

void ItemRainbowMixerWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
}

void ItemRainbowMixerWnd::iwm_hide()
{

}

void ItemRainbowMixerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemMixer();
		}
		break;
	case ID_SLOT_BTN1:
	case ID_SLOT_BTN2:
	case ID_SLOT_BTN3:
	case ID_SLOT_BTN4:
	case ID_SLOT_BTN5:
	case ID_SLOT_BTN6:
	case ID_SLOT_BTN7:
		if( cmd == IOBN_BTNUP )
		{
			SetEquipItem( dwID - ID_SLOT_BTN1 );
		}
		break;
	}
}

void ItemRainbowMixerWnd::SetEquipItem( int iSlot )
{
	if( COMPARE( iSlot, 0, (int)m_RainbowSlotList.size() ) )
	{
		if( m_RainbowSlotList[iSlot].m_bEquipSlot )
		{
			m_RainbowSlotList[iSlot].m_bEquipSlot = false;
		}
		else
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_RainbowSlotList[iSlot].m_dwEtcItem );
			if( pEtcItem )
			{
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kEtcItem;
					if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kEtcItem ) )
					{
						if( kEtcItem.GetUse() > 0 )
						{
							m_RainbowSlotList[iSlot].m_bEquipSlot = true;
						}
					}
				}

				if( !m_RainbowSlotList[iSlot].m_bEquipSlot )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), pEtcItem->GetName().c_str() );
					return;
				}
			}
		}
		CheckEquipPattern();
	}
}

void ItemRainbowMixerWnd::SetEquipPatternLine()
{
	if( m_RainbowSlotList.empty() ) return;

	int iRainbowSlotSize = (int)m_RainbowSlotList.size();
	for(int i = 0;i < iRainbowSlotSize;i++)
	{
		RainbowSlotData &rkData = m_RainbowSlotList[i];

		int iSlotBtn = ID_SLOT_BTN1 + i;
		switch( iSlotBtn )
		{
		case ID_SLOT_BTN1:	// 0
			{
			}
			break;
		case ID_SLOT_BTN2:	// 1
			{
				// 1 + 0 = 127, 158
				rkData.m_Pattern[0].m_iPatternIdx	= 0;
				rkData.m_Pattern[0].m_pPatternLine	= m_pSlotEquipLine1;
				rkData.m_Pattern[0].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[0].m_Point.x       = 127;
				rkData.m_Pattern[0].m_Point.y       = 158;

				// 1 + 2 = 158, 138
				rkData.m_Pattern[1].m_iPatternIdx	= 2;
				rkData.m_Pattern[1].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[1].m_ReverseFormat	= ioUIRenderImage::RF_HORZ_REVERSE;
				rkData.m_Pattern[1].m_Point.x       = 158;
				rkData.m_Pattern[1].m_Point.y       = 138;				
			}
			break;
		case ID_SLOT_BTN3:	// 2
			{
				// 2 + 0 = 157, 179
				rkData.m_Pattern[0].m_iPatternIdx	= 0;
				rkData.m_Pattern[0].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[0].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[0].m_Point.x       = 157;
				rkData.m_Pattern[0].m_Point.y       = 179;

				// 2 + 3 = 199, 199
				rkData.m_Pattern[1].m_iPatternIdx	= 3;
				rkData.m_Pattern[1].m_pPatternLine	= m_pSlotEquipLine1;
				rkData.m_Pattern[1].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[1].m_Point.x       = 199;
				rkData.m_Pattern[1].m_Point.y       = 199;
			}
			break;
		case ID_SLOT_BTN4:	// 3
			{
				// 3 + 0 = 156, 220
				rkData.m_Pattern[0].m_iPatternIdx	= 0;
				rkData.m_Pattern[0].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[0].m_ReverseFormat	= ioUIRenderImage::RF_HORZ_REVERSE;
				rkData.m_Pattern[0].m_Point.x       = 156;
				rkData.m_Pattern[0].m_Point.y       = 220;

				// 3 + 4 = 157, 262
				rkData.m_Pattern[1].m_iPatternIdx	= 4;
				rkData.m_Pattern[1].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[1].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[1].m_Point.x       = 157;
				rkData.m_Pattern[1].m_Point.y       = 262;
			}
			break;
		case ID_SLOT_BTN5:	// 4
			{
				// 4 + 0 = 127, 240
				rkData.m_Pattern[0].m_iPatternIdx	= 0;
				rkData.m_Pattern[0].m_pPatternLine	= m_pSlotEquipLine1;
				rkData.m_Pattern[0].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[0].m_Point.x       = 127;
				rkData.m_Pattern[0].m_Point.y       = 240;

				// 4 + 5 = 85,  262
				rkData.m_Pattern[1].m_iPatternIdx	= 5;
				rkData.m_Pattern[1].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[1].m_ReverseFormat	= ioUIRenderImage::RF_HORZ_REVERSE;
				rkData.m_Pattern[1].m_Point.x       = 85;
				rkData.m_Pattern[1].m_Point.y       = 262;
			}
			break;
		case ID_SLOT_BTN6:	// 5
			{
				// 5 + 0 = 86,  220
				rkData.m_Pattern[0].m_iPatternIdx	= 0;
				rkData.m_Pattern[0].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[0].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[0].m_Point.x       = 86;
				rkData.m_Pattern[0].m_Point.y       = 220;

				// 5 + 6 = 55,  199
				rkData.m_Pattern[1].m_iPatternIdx	= 6;
				rkData.m_Pattern[1].m_pPatternLine	= m_pSlotEquipLine1;
				rkData.m_Pattern[1].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[1].m_Point.x       = 55;
				rkData.m_Pattern[1].m_Point.y       = 199;
			}
			break;
		case ID_SLOT_BTN7:	// 6
			{
				// 6 + 0 = 85,  179
				rkData.m_Pattern[0].m_iPatternIdx	= 0;
				rkData.m_Pattern[0].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[0].m_ReverseFormat	= ioUIRenderImage::RF_HORZ_REVERSE;
				rkData.m_Pattern[0].m_Point.x       = 85;
				rkData.m_Pattern[0].m_Point.y       = 179;

				// 6 + 1 = 84,  138
				rkData.m_Pattern[1].m_iPatternIdx	= 1;
				rkData.m_Pattern[1].m_pPatternLine	= m_pSlotEquipLine2;
				rkData.m_Pattern[1].m_ReverseFormat	= ioUIRenderImage::RF_NONE_REVERSE;
				rkData.m_Pattern[1].m_Point.x       = 84;
				rkData.m_Pattern[1].m_Point.y       = 138;
			}
			break;
		}
	}


}

void ItemRainbowMixerWnd::CheckEquipPattern()
{
	m_bPattern = false;
	ioEtcItemRainbowMixer *pEtcItem = dynamic_cast< ioEtcItemRainbowMixer * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
	if( pEtcItem == NULL ) return;

	DWORDVec kEquipItem;
	for(int i = ID_SLOT_BTN1;i < ID_SLOT_BTN7 + 1;i++)
	{
		ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
		if( pButton )
		{
			int iSlot = i - ID_SLOT_BTN1;
			if( !COMPARE( iSlot, 0, (int)m_RainbowSlotList.size() ) ) continue;

			if( m_RainbowSlotList[iSlot].m_bEquipSlot )
				kEquipItem.push_back( m_RainbowSlotList[iSlot].m_dwEtcItem );
		}
	}

	if( kEquipItem.empty() )
		return;

	if( pEtcItem->GetPatternPresent( kEquipItem ) != -1 )
	{
		m_bPattern = true;
	}
	kEquipItem.clear();    

	if( m_bPattern )
	{
		m_dwPatternScreenTime = FRAMEGETTIME();
		m_bPatternScreen	  = false;
	}
}

void ItemRainbowMixerWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

void ItemRainbowMixerWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bPattern )
	{
		if( FRAMEGETTIME() - m_dwPatternScreenTime > 500 )
		{
			m_dwPatternScreenTime = FRAMEGETTIME();
			m_bPatternScreen = !m_bPatternScreen;
		}
	}
}

void ItemRainbowMixerWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pBackground )
	{
		m_pBackground->SetScale( 1.95f );
		m_pBackground->Render( iXPos + 129, iYPos + 196, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	ioWnd::OnRender();

	OnRenderPattern( iXPos, iYPos );
	OnRenderButtonIcon( iXPos, iYPos );
	PrintManual( iXPos+33, iYPos+352, FONT_SIZE_12 );
}

void ItemRainbowMixerWnd::OnRenderPattern( int iXPos, int iYPos )
{
	if( !m_bPattern ) return;

	int iRainbowSlotSize = (int)m_RainbowSlotList.size();
	for(int i = ID_SLOT_BTN1;i < ID_SLOT_BTN7 + 1;i++)
	{
		ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
		if( pButton )
		{
			// 장착 아이콘
			int iSlot = i - ID_SLOT_BTN1;
			if( COMPARE( iSlot, 0, iRainbowSlotSize ) )
			{
				RainbowSlotData &rkData = m_RainbowSlotList[iSlot];
				if( rkData.m_bEquipSlot )
				{
					for(int k = 0;k < MAX_PATTERN;k++)
					{
						int iNextPatternID = rkData.m_Pattern[k].m_iPatternIdx;
						if( COMPARE( iNextPatternID, 0, iRainbowSlotSize ) )
						{
							if( m_RainbowSlotList[iNextPatternID].m_bEquipSlot )
							{
								// 선 그리기
								if( rkData.m_Pattern[k].m_pPatternLine )
								{
									rkData.m_Pattern[k].m_pPatternLine->SetReverseFormatting( rkData.m_Pattern[k].m_ReverseFormat );
									rkData.m_Pattern[k].m_pPatternLine->Render( iXPos + rkData.m_Pattern[k].m_Point.x, iYPos + rkData.m_Pattern[k].m_Point.y );
								}	
							}
						}
					}
				}
			}
		}
	}
}

void ItemRainbowMixerWnd::OnRenderButtonIcon( int iXPos, int iYPos )
{
	for(int i = ID_SLOT_BTN1;i < ID_SLOT_BTN7 + 1;i++)
	{
		ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
		if( pButton )
		{
			// 장착 아이콘
			int iSlot = i - ID_SLOT_BTN1;
			if( COMPARE( iSlot, 0, (int)m_RainbowSlotList.size() ) )
			{
				if( m_RainbowSlotList[iSlot].m_bEquipSlot )
				{
					if( m_bPattern )
					{
						if( m_pSlotEquipIcon )
							m_pSlotEquipIcon->Render( iXPos + pButton->GetXPos() + 35, iYPos + pButton->GetYPos() + 35 );
					}

					if( m_RainbowSlotList[iSlot].m_pIcon )
						m_RainbowSlotList[iSlot].m_pIcon->Render( iXPos + pButton->GetXPos() + 35, iYPos + pButton->GetYPos() + 35 );

					if( m_bPattern && m_bPatternScreen )
					{
						if( m_RainbowSlotList[iSlot].m_pIcon )
							m_RainbowSlotList[iSlot].m_pIcon->Render( iXPos + pButton->GetXPos() + 35, iYPos + pButton->GetYPos() + 35, UI_RENDER_SCREEN );
					}
				}
			}

			// 마우스 오버
			if( pButton->IsOver() || pButton->IsClicked() )
			{
				if( m_pSlotEquipOver )
				{
					m_pSlotEquipOver->Render( iXPos + pButton->GetXPos() + 35, iYPos + pButton->GetYPos() + 35, UI_RENDER_SCREEN );
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ItemLostSagaMixerWnd::ItemLostSagaMixerWnd()
{
	m_dwEtcItemCode = 0;
	m_pManual       = NULL;

	m_bPattern = false;
	m_dwPatternScreenTime = 0;
	m_bPatternScreen = false;
}

ItemLostSagaMixerWnd::~ItemLostSagaMixerWnd()
{
	ClearLostSagaSlot();
}

void ItemLostSagaMixerWnd::ClearLostSagaSlot()
{
	int iSlotSize = m_LostSagaSlotList.size();
	for(int i = 0;i < iSlotSize;i++)
	{
		LostSagaSlotData &rkSlotData = m_LostSagaSlotList[i];
		SAFEDELETE( rkSlotData.m_pIcon );
		SAFEDELETE( rkSlotData.m_pGrayIcon );
	}
	m_LostSagaSlotList.clear();
}

void ItemLostSagaMixerWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );
}

void ItemLostSagaMixerWnd::SendItemMixer()
{
	if( !m_bPattern )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
	}
	else
	{
		ioEtcItemLostSagaMixer *pEtcItem = dynamic_cast< ioEtcItemLostSagaMixer * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
		if( pEtcItem == NULL ) return;

		int i = 0;
		DWORDVec kEquipItem;
		for(i = ID_SLOT_BTN1;i < ID_SLOT_BTN6 + 1;i++)
		{
			ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
			if( pButton )
			{
				int iSlot = i - ID_SLOT_BTN1;
				if( !COMPARE( iSlot, 0, (int)m_LostSagaSlotList.size() ) ) continue;

				if( m_LostSagaSlotList[iSlot].m_bEquipSlot )
					kEquipItem.push_back( m_LostSagaSlotList[iSlot].m_dwEtcItem );
			}
		}

		if( kEquipItem.empty() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		}
		else 
		{
			for(i = 0;i < (int)kEquipItem.size();i++)
			{
				bool bNoneItem = true;
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kEtcItem;
					if( pUserEtcItem->GetEtcItem( kEquipItem[i], kEtcItem ) )
					{
						if( kEtcItem.GetUse() > 0 )
						{
							bNoneItem = false;
						}
					}
				}

				if( bNoneItem )
				{
					ioEtcItem *pEtcItemSlot = g_EtcItemMgr.FindEtcItem( kEquipItem[i] );
					if( pEtcItemSlot )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), pEtcItemSlot->GetName().c_str() );
					else
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
					return;
				}
			}

			int iPresentCode = pEtcItem->GetPatternPresent( kEquipItem );
			if( iPresentCode != -1 )
			{
				SP2Packet kPacket( CTPK_ETCITEM_USE );
				kPacket << (int)m_dwEtcItemCode;			
				kPacket << iPresentCode;
				kPacket << (int)kEquipItem.size();
				for(int k = 0;k < (int)kEquipItem.size();k++)
					kPacket << kEquipItem[k];
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );

				HideWnd();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			}
		}
		kEquipItem.clear();
	}
}

void ItemLostSagaMixerWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	m_bPattern = false;
	m_dwPatternScreenTime = 0;
	m_bPatternScreen = false;

	ioEtcItemLostSagaMixer *pEtcItem = dynamic_cast< ioEtcItemLostSagaMixer * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
	if( pEtcItem == NULL ) return;

	ClearLostSagaSlot();
	int iMaxMixerData = pEtcItem->GetMaxMixerList();
	for(int i = 0;i < iMaxMixerData;i++)
	{
		LostSagaSlotData kSlotData;		
		ioHashString kEquipIcon, kReleaseIcon;
		kSlotData.m_dwEtcItem = pEtcItem->GetMixerData( i, kEquipIcon, kReleaseIcon );
		if( !kEquipIcon.IsEmpty() )
		{
			kSlotData.m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kEquipIcon );
		}
		if( !kReleaseIcon.IsEmpty() )
		{
			kSlotData.m_pGrayIcon = g_UIImageSetMgr.CreateImageByFullName( kReleaseIcon ); 
		}
		m_LostSagaSlotList.push_back( kSlotData );
	}
	ShowWnd();
}

void ItemLostSagaMixerWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
}

void ItemLostSagaMixerWnd::iwm_hide()
{

}

void ItemLostSagaMixerWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemMixer();
		}
		break;
	case ID_SLOT_BTN1:
	case ID_SLOT_BTN2:
	case ID_SLOT_BTN3:
	case ID_SLOT_BTN4:
	case ID_SLOT_BTN5:
	case ID_SLOT_BTN6:
		if( cmd == IOBN_BTNUP )
		{
			SetEquipItem( dwID - ID_SLOT_BTN1 );
		}
		break;
	}
}

void ItemLostSagaMixerWnd::SetEquipItem( int iSlot )
{
	if( COMPARE( iSlot, 0, (int)m_LostSagaSlotList.size() ) )
	{
		if( m_LostSagaSlotList[iSlot].m_bEquipSlot )
		{
			m_LostSagaSlotList[iSlot].m_bEquipSlot = false;
		}
		else
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_LostSagaSlotList[iSlot].m_dwEtcItem );
			if( pEtcItem )
			{
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kEtcItem;
					if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kEtcItem ) )
					{
						if( kEtcItem.GetUse() > 0 )
						{
							m_LostSagaSlotList[iSlot].m_bEquipSlot = true;
						}
					}
				}

				if( !m_LostSagaSlotList[iSlot].m_bEquipSlot )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), pEtcItem->GetName().c_str() );
					return;
				}
			}
		}
		CheckEquipPattern();
	}
}

void ItemLostSagaMixerWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

void ItemLostSagaMixerWnd::CheckEquipPattern()
{
	m_bPattern = false;
	ioEtcItemLostSagaMixer *pEtcItem = dynamic_cast< ioEtcItemLostSagaMixer * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
	if( pEtcItem == NULL ) return;

	DWORDVec kEquipItem;
	for(int i = ID_SLOT_BTN1;i < ID_SLOT_BTN6 + 1;i++)
	{
		ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
		if( pButton )
		{
			int iSlot = i - ID_SLOT_BTN1;
			if( !COMPARE( iSlot, 0, (int)m_LostSagaSlotList.size() ) ) continue;

			if( m_LostSagaSlotList[iSlot].m_bEquipSlot )
				kEquipItem.push_back( m_LostSagaSlotList[iSlot].m_dwEtcItem );
		}
	}

	if( kEquipItem.empty() )
		return;

	if( pEtcItem->GetPatternPresent( kEquipItem ) != -1 )
	{
		m_bPattern = true;
	}
	kEquipItem.clear();    

	if( m_bPattern )
	{
		m_dwPatternScreenTime = FRAMEGETTIME();
		m_bPatternScreen	  = false;
	}
}

void ItemLostSagaMixerWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bPattern )
	{
		if( FRAMEGETTIME() - m_dwPatternScreenTime > 500 )
		{
			m_dwPatternScreenTime = FRAMEGETTIME();
			m_bPatternScreen = !m_bPatternScreen;
		}
	}
}

void ItemLostSagaMixerWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	OnRenderButtonIcon( iXPos, iYPos );
	PrintManual( iXPos+33, iYPos+352, FONT_SIZE_12 );
}

void ItemLostSagaMixerWnd::OnRenderButtonIcon( int iXPos, int iYPos )
{
	for(int i = ID_SLOT_BTN1;i < ID_SLOT_BTN6 + 1;i++)
	{
		ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( i ) );
		if( pButton )
		{				
			// 장착 아이콘
			int iSlot = i - ID_SLOT_BTN1;
			if( COMPARE( iSlot, 0, (int)m_LostSagaSlotList.size() ) )
			{
				if( m_LostSagaSlotList[iSlot].m_bEquipSlot )
				{
					if( m_LostSagaSlotList[iSlot].m_pIcon )
						m_LostSagaSlotList[iSlot].m_pIcon->Render( iXPos + pButton->GetXPos(), iYPos + pButton->GetYPos() );					

					if( ( m_bPattern && m_bPatternScreen ) || pButton->IsOver() || pButton->IsClicked() )
					{
						if( m_LostSagaSlotList[iSlot].m_pIcon )
							m_LostSagaSlotList[iSlot].m_pIcon->Render( iXPos + pButton->GetXPos(), iYPos + pButton->GetYPos(), UI_RENDER_SCREEN );
					}
				}
				else
				{
					if( m_LostSagaSlotList[iSlot].m_pGrayIcon )
					{
						m_LostSagaSlotList[iSlot].m_pGrayIcon->Render( iXPos + pButton->GetXPos(), iYPos + pButton->GetYPos(), UI_RENDER_GRAY, TFO_NEAREST, false,
																	   UI_RENDER_NORMAL );						
					}

					if( pButton->IsOver() || pButton->IsClicked() )
					{
						if( m_LostSagaSlotList[iSlot].m_pGrayIcon )
						{
							m_LostSagaSlotList[iSlot].m_pGrayIcon->Render( iXPos + pButton->GetXPos(), iYPos + pButton->GetYPos(), UI_RENDER_GRAY, TFO_NEAREST, false,
																		   UI_RENDER_SCREEN );
						}
					}
				}				
			}			
		}
	}
}

//////////////////////////////////////////////////////////////////////////
ItemGoldBoxWnd::ItemGoldBoxWnd()
{
	m_dwEtcItemCode = 0;
	m_bPattern		= false;
	m_pManual       = NULL;

	m_pSlotEquipOver	= NULL;
	m_pBackground	= NULL;

}

ItemGoldBoxWnd::~ItemGoldBoxWnd()
{
	ClearSlot();
	SAFEDELETE( m_pSlotEquipOver );
	SAFEDELETE(m_pBackground);
}

void ItemGoldBoxWnd::ClearSlot()
{
	int iSlotSize = m_GoldBoxSlotList.size();
	for(int i = 0;i < iSlotSize;i++)
	{
		GoldBoxSlotData &rkSlotData = m_GoldBoxSlotList[i];
		SAFEDELETE( rkSlotData.m_pIcon );
	}
	m_GoldBoxSlotList.clear();
}

void ItemGoldBoxWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SlotEquipOver" )
	{
		SAFEDELETE( m_pSlotEquipOver );
		m_pSlotEquipOver = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ItemGoldBoxWnd::SendItemMixer()
{
	if( !m_bPattern )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else
	{
		ioEtcItemGoldBox *pEtcItem = dynamic_cast< ioEtcItemGoldBox * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
		if( pEtcItem == NULL ) return;

		int i = 0;
		DWORDVec kEquipItem;
		ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( ID_SLOT_BTN1 ) );
		if( pButton && !m_GoldBoxSlotList.empty() )
		{
			if( m_GoldBoxSlotList[0].m_bEquipSlot )
				kEquipItem.push_back( m_GoldBoxSlotList[0].m_dwEtcItem );
		}

		if( kEquipItem.empty() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		else 
		{
			for(i = 0;i < (int)kEquipItem.size();i++)
			{
				bool bNoneItem = true;
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kEtcItem;
					if( pUserEtcItem->GetEtcItem( kEquipItem[i], kEtcItem ) )
					{
						if( kEtcItem.GetUse() > 0 )
						{
							bNoneItem = false;
						}
					}
				}

				if( bNoneItem )
				{
					ioEtcItem *pEtcItemSlot = g_EtcItemMgr.FindEtcItem( kEquipItem[i] );
					if( pEtcItemSlot )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), pEtcItemSlot->GetName().c_str() );
					else
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
					return;
				}
			}

			int iPresentCode = pEtcItem->GetPatternPresent( kEquipItem );
			if( iPresentCode != -1 )
			{
				SP2Packet kPacket( CTPK_ETCITEM_USE );
				kPacket << (int)m_dwEtcItemCode;			
				kPacket << iPresentCode;
				kPacket << (int)kEquipItem.size();
				for(int k = 0;k < (int)kEquipItem.size();k++)
					kPacket << kEquipItem[k];
				TCPNetwork::SendToServer( kPacket );
				TCPNetwork::MouseBusy( true );

				HideWnd();
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			}
		}
		kEquipItem.clear();
	}
}

void ItemGoldBoxWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;
	m_bPattern		= false;

	ioEtcItemGoldBox *pEtcItem = dynamic_cast< ioEtcItemGoldBox * > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode ));
	if( pEtcItem == NULL ) return;

	ioHashString kBackImage;
	pEtcItem->GetRandomBackGround( kBackImage );
	SAFEDELETE( m_pBackground );
	m_pBackground = g_UIImageSetMgr.CreateImageByFullName( kBackImage );

	ClearSlot();
	int iMaxMixerData = pEtcItem->GetMaxMixerList();
	for(int i = 0;i < iMaxMixerData;i++)
	{
		GoldBoxSlotData kSlotData;		
		ioHashString kEquipIcon, kReleaseIcon;
		kSlotData.m_dwEtcItem = pEtcItem->GetMixerData( i, kEquipIcon, kReleaseIcon );
		if( !kEquipIcon.IsEmpty() )
		{
			kSlotData.m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kEquipIcon );
		}
		m_GoldBoxSlotList.push_back( kSlotData );
	}

	ShowWnd();
}

void ItemGoldBoxWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();
}

void ItemGoldBoxWnd::iwm_hide()
{

}

void ItemGoldBoxWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_RECV_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemMixer();
		}
		break;
	case ID_SLOT_BTN1:
		if( cmd == IOBN_BTNUP )
		{
			SetEquipItem( dwID - ID_SLOT_BTN1 );
		}
		break;
	}
}

void ItemGoldBoxWnd::SetEquipItem( int iSlot )
{
	if( COMPARE( iSlot, 0, (int)m_GoldBoxSlotList.size() ) )
	{
		if( m_GoldBoxSlotList[iSlot].m_bEquipSlot )
		{
			m_GoldBoxSlotList[iSlot].m_bEquipSlot = false;
			m_bPattern = false;
		}
		else
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_GoldBoxSlotList[iSlot].m_dwEtcItem );
			if( pEtcItem )
			{
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					ioUserEtcItem::ETCITEMSLOT kEtcItem;
					if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kEtcItem ) )
					{
						if( kEtcItem.GetUse() > 0 )
						{
							m_GoldBoxSlotList[iSlot].m_bEquipSlot = true;
							m_bPattern = true;
						}
					}
				}

				if( !m_GoldBoxSlotList[iSlot].m_bEquipSlot )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), pEtcItem->GetName().c_str() );
					return;
				}
			}
		}
	}
}

void ItemGoldBoxWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

//void ItemGoldBoxWnd::OnProcess( float fTimePerSec )
//{
//	if( !IsShow() ) return;
//}

void ItemGoldBoxWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pBackground )
		m_pBackground->Render( iXPos + 13, iYPos + 120, UI_RENDER_NORMAL, TFO_BILINEAR );

	ioWnd::OnRender();

	OnRenderButtonIcon( iXPos, iYPos );
	PrintManual( iXPos+33, iYPos+352, FONT_SIZE_12 );
}

void ItemGoldBoxWnd::OnRenderButtonIcon( int iXPos, int iYPos )
{
	ioButton *pButton = dynamic_cast< ioButton * > ( FindChildWnd( ID_SLOT_BTN1 ) );
	if( pButton && !m_GoldBoxSlotList.empty() )
	{
		// 장착 아이콘
		if( m_GoldBoxSlotList[0].m_bEquipSlot )
		{
			if( m_GoldBoxSlotList[0].m_pIcon )
				m_GoldBoxSlotList[0].m_pIcon->Render( iXPos + pButton->GetXPos() + 35, iYPos + pButton->GetYPos() + 35 );
		}

		// 마우스 오버
		if( pButton->IsOver() || pButton->IsClicked() )
		{
			if( m_pSlotEquipOver )
			{
				m_pSlotEquipOver->Render( iXPos + pButton->GetXPos() + 35, iYPos + pButton->GetYPos() + 35, UI_RENDER_SCREEN );
			}
		}
	}
}
///////////////////////////////////////////////////////////////////
AccessoryCompoundWnd::AccessoryCompoundWnd(void)
{
	m_bAllSelect = false;
	m_iSelectedBtnType = ACI_UNSELECT;

	m_pManual = NULL;
}

AccessoryCompoundWnd::~AccessoryCompoundWnd(void)
{
}

void AccessoryCompoundWnd::ShowWndWithEtcItemCode( DWORD dwCode )
{
	m_dwEtcItemCode = dwCode;

	ShowWnd();
}

void AccessoryCompoundWnd::iwm_show()
{
	m_bAllSelect = false;
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	for( int i = 0; i < ACI_SIZE; i++ )
	{
		SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGET_BTN + i));
		if( pTarget )
			pTarget->InitData();

		InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON + i));
		if( pTargetIconBtn )
			pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );

		m_iSelectedAccessoryIndex[i] = 0;
		if( i != 0 )
			SetChildInActive( ID_TARGET_BTN + i );
	}
}

void AccessoryCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendCompound();
		}
		break;
	case ID_TARGET_BTN:
		if( cmd == IOBN_BTNUP )
		{
			for( int i = 0; i < 3; i++ )
			{
				SelectExtraItemBtn *pVictim = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_VICTIM1_BTN + i));
				if( pVictim )
					pVictim->InitData();

				InventoryIconBtn *pVictimIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_VICTIM1_ICON + i));
				if( pVictimIconBtn )
					pVictimIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );

				SetChildInActive( ID_VICTIM1_BTN + i );
				m_iSelectedAccessoryIndex[ACI_VICTIM1 + i] = 0;
			}
			AccessorySelectWnd *pSelectWnd = dynamic_cast<AccessorySelectWnd*>( g_GUIMgr.FindWnd( ACCESSORY_SELECT_WND ) );
			if ( pSelectWnd )
			{
				m_iSelectedBtnType = ACI_TARGET;
				pSelectWnd->ShowSeletWnd( this, m_iSelectedAccessoryIndex[0], m_iSelectedAccessoryIndex[1],
											m_iSelectedAccessoryIndex[2], m_iSelectedAccessoryIndex[3], false );
			}
		}
		break;
	case ID_VICTIM1_BTN:
	case ID_VICTIM2_BTN:
	case ID_VICTIM3_BTN:
		if( cmd == IOBN_BTNUP )
		{
			AccessorySelectWnd *pSelectWnd = dynamic_cast<AccessorySelectWnd*>( g_GUIMgr.FindWnd( ACCESSORY_SELECT_WND ) );
			if ( pSelectWnd )
			{
				m_iSelectedBtnType = (AccessoryIndexType)(dwID - ID_TARGET_BTN);
				pSelectWnd->ShowSeletWnd( this, m_iSelectedAccessoryIndex[0], m_iSelectedAccessoryIndex[1],
											m_iSelectedAccessoryIndex[2], m_iSelectedAccessoryIndex[3], true );
			}
		}
		break;
	case ACCESSORY_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_iSelectedBtnType == ACI_UNSELECT )
				break;

			float fIconSize = FLOAT1;
			if( m_iSelectedBtnType != ACI_TARGET )
				fIconSize = 0.62f;
			else
			{
				if( m_iSelectedAccessoryIndex[m_iSelectedBtnType] != 0 || param > 0 )
				{
					for( int i = 0; i < 3; i++ )
						SetChildActive( ID_VICTIM1_BTN + i );
				}
			}
			if( param <= 0 )
				break;

			SelectExtraItemBtn *pBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_TARGET_BTN + m_iSelectedBtnType));
			if( pBtn )
				pBtn->SetSelectAccessorySlot( param );

			ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
			if( pAccessory )
			{
				AccessorySlot kSlot;
				if( pAccessory->GetAccessorySlot( param, kSlot ) )
				{
					InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON + m_iSelectedBtnType));
					if( pIconBtn )
					{
						ioUIRenderImage *pImg = NULL;
						pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
						pIconBtn->SetIcon( pImg, fIconSize, fIconSize );
					}
				}
			}
			m_iSelectedAccessoryIndex[m_iSelectedBtnType] = param;
			m_iSelectedBtnType = ACI_UNSELECT;
		}
		break;
	}
}

void AccessoryCompoundWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+309, FONT_SIZE_12 );
}

void AccessoryCompoundWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szCompoundErrMsg[0] = xElement.GetStringAttribute_e( "CompoundErrMsg1" );
	m_szCompoundErrMsg[1] = xElement.GetStringAttribute_e( "CompoundErrMsg2" );
}

void AccessoryCompoundWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemCode );
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

void AccessoryCompoundWnd::SendCompound()
{
	bool bExist[4] = { false, };
	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( pAccessory )
	{
		for( int i = 0; i < ACI_SIZE; i++ )
		{
			AccessorySlot kSlot;
			if( pAccessory->GetAccessorySlot( m_iSelectedAccessoryIndex[i], kSlot ) )
				bExist[i] = true;
			else
				bExist[i] = false;
		}
	}

	if( !bExist[0] )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szCompoundErrMsg[0].c_str() );
		return;
	}
	else if( !bExist[1] || !bExist[2] || !bExist[3] )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szCompoundErrMsg[1].c_str() );
		return;
	}

	SP2Packet kPacket( CTPK_ACCESSORY_COMPOSE );
	kPacket << m_iSelectedAccessoryIndex[0];
	kPacket << m_iSelectedAccessoryIndex[1];
	kPacket << m_iSelectedAccessoryIndex[2];
	kPacket << m_iSelectedAccessoryIndex[3];
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}