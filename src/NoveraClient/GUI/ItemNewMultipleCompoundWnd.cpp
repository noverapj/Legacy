#include "StdAfx.h"
#include "ItemNewMultipleCompoundWnd.h"

ItemNewMultipleCompoundWnd::ItemNewMultipleCompoundWnd(void)
{
	m_dwEtcItemType = 0;
	m_bAllSelect = false;
	m_dwMaterialType = 0;
	m_vTotalList.clear();
	m_pManual = NULL;
	m_dwCurItemBtn = 0;
	m_dwResultGashaponIndex = 0;
}

ItemNewMultipleCompoundWnd::~ItemNewMultipleCompoundWnd(void)
{
}

void ItemNewMultipleCompoundWnd::iwm_hide()
{
	m_dwEtcItemType = 0;
	m_bAllSelect = false;

	m_vTotalList.clear();

	for ( int i=0 ; i<MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_ITEM1_BTN + i ));
		if( pItemBtn )
			pItemBtn->InitData();

		InventoryIconBtn *pIconBtn = pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
		if( pIconBtn )
			pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
	}
}

void ItemNewMultipleCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ITEM1_BTN:
	case ID_ITEM2_BTN:
	case ID_ITEM3_BTN:
		if( cmd == IOBN_BTNUP )
		{
			CheckCompoundItemBtn( pWnd, dwID );
			UpdateData();
		}
		break;
	case INVENTORY_SELECT_ITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( m_dwCurItemBtn ) );
			if( pTarget && param > 0 )
			{
				ioHashString szName;
				for ( int i=0 ; i<(int)m_vTotalList.size() ; ++i )
				{
					if( m_vTotalList[i].m_iMagicCode == param )
					{
						szName = m_vTotalList[i].m_szName;
						break;
					}
				}
				pTarget->SetSelectMaterial( param, szName );
			}

			m_dwCurItemBtn = 0;

			InventorySelectItemWnd *pInventorySelectItemWnd = dynamic_cast< InventorySelectItemWnd* >( g_GUIMgr.FindWnd(INVENTORY_SELECT_ITEM_WND) );
			if( pInventorySelectItemWnd )
				pInventorySelectItemWnd->HideWnd();
			UpdateData();
		}
		break;
	}
}

void ItemNewMultipleCompoundWnd::SetInfoAndShow( DWORD dwType )
{
	m_vTotalList.clear();

	m_bAllSelect = false;
	m_dwEtcItemType = dwType;

	m_dwMaterialType = 0;
	m_dwResultGashaponIndex = 0;
	ioEtcItemNewMultipleCompound *pEtcItem = dynamic_cast< ioEtcItemNewMultipleCompound * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemType));
	if( pEtcItem )
	{
		m_dwMaterialType = pEtcItem->GetMaterialType();
		m_dwResultGashaponIndex = pEtcItem->GetResultGashaponIndex();
	}

	// Load Total ExtraItemList
	LoadTotalList();

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	for ( int i=0 ; i<MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_ITEM1_BTN + i ));
		if( pItemBtn )
			pItemBtn->InitData();

		InventoryIconBtn *pIconBtn = pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
		if( pIconBtn )
			pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
	}

	if( m_vTotalList.empty() )
	{
		SetChildInActive( ID_ITEM1_BTN );
		SetChildInActive( ID_ITEM2_BTN );
		SetChildInActive( ID_ITEM3_BTN );
	}
	else
	{
		SetChildActive( ID_ITEM1_BTN );
		SetChildInActive( ID_ITEM2_BTN );
		SetChildInActive( ID_ITEM3_BTN );
	}

	UpdateData();
	ShowWnd();
}

void ItemNewMultipleCompoundWnd::LoadTotalList()
{
	switch( m_dwMaterialType )
	{
	case PRESENT_MEDALITEM:
		LoadMedalItemList();
		break;
	case PRESENT_COSTUME:
		LoadCostumeItemList();
		break;
	case PRESENT_EXTRAITEM:
		LoadExtraItemList();
		break;
	}
}

void ItemNewMultipleCompoundWnd::LoadMedalItemList()
{
	ioUserMedalItem *pMedalitem = g_MyInfo.GetUserMedalItem();
	if( !pMedalitem )
		return;

	int iMax = pMedalitem->GetMedalItemCnt();
	for (int i = 0; i < iMax ; i++)
	{
		ioUserMedalItem::MEDALITEMSLOT kSlot;
		if( !pMedalitem->GetMedalItemArray( i , kSlot ) )
			continue;
		//장착 여부 체크
		if( kSlot.m_iEquipClass != 0 )
			continue;
		if( kSlot.m_iCustomIndex != 0 )
			continue;
		//영구, 기간제 체크
		if ( kSlot.m_iPeriodType != ioUserMedalItem::PT_MORTMAIN )
			continue;
		const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( kSlot.m_iItemType );
		if( !pItem )
			continue;

		InventorySelectItem kInfo;
		kInfo.m_iMagicCode = i + 1;
		kInfo.m_szIconName = pItem->m_sIcon;
		kInfo.m_szSubIconName = pItem->m_sSubIcon;
		kInfo.m_szName = pItem->m_sName;

		pMedalitem->GetUseInfo( kSlot.m_iItemType, kSlot.m_iCustomIndex, kInfo.m_szSubText );
		int iLimitLevel = g_MedalItemMgr.GetLevelLimit( kSlot.m_iItemType );
		char szBuf[MAX_PATH];
		sprintf_e( szBuf, "Lv%d 제한", iLimitLevel );
		kInfo.m_szSubText2 = szBuf;
		m_vTotalList.push_back( kInfo );
	}
}

void ItemNewMultipleCompoundWnd::LoadCostumeItemList()
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume ) 
		return;

	int nSize = pCostume->GetCostumeSlotCnt();
	for( int i=0; i<nSize; ++i )
	{
		CostumeSlot kSlot;
		if( !pCostume->GetCostumeSlotArray( i, kSlot ) )
			continue;
		//장착 여부 체크
		if( kSlot.m_bEquip )
			continue;
		//영구, 기간제 체크
		if ( kSlot.m_PeriodType != PCPT_MORTMAIN )
			continue;

		InventorySelectItem kInfo;
		kInfo.m_iMagicCode = i + 1;
		kInfo.m_szName = kSlot.m_Name;
		kInfo.m_szIconName = kSlot.m_IconName;
		kInfo.m_iTabType = kSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE;

		pCostume->GetUseInfo( kSlot.m_nIndex, kInfo.m_szSubText );

		CostumeInfo sCostumeInfo;
		g_CostumeInfoMgr.GetCostumeInfo( kSlot.m_nCostumeCode, sCostumeInfo );
		char szBuf[MAX_PATH];
		if( sCostumeInfo.m_nEnableClass > 0 )
		{
			sprintf_e( szBuf, "%s 전용", g_MyInfo.GetClassName( sCostumeInfo.m_nEnableClass ));
			kInfo.m_szSubText2 = szBuf;
		}
		else
			kInfo.m_szSubText2 = "장착제한없음";

		kInfo.m_nGrade = sCostumeInfo.m_nGradeType;

		m_vTotalList.push_back( kInfo );
	}
}

void ItemNewMultipleCompoundWnd::LoadExtraItemList()
{
	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem ) 
		return;

	int nSize = pExtraItem->GetExtraItemCnt();
	for( int i=0; i<nSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pExtraItem->GetExtraItemArray( i, kSlot) )
			continue;

		if(kSlot.m_bCharEquip)
			continue;

		if( kSlot.m_PeriodType != PCPT_MORTMAIN )
			continue;

		const ioItem* pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode );

		if( !pItem ) 
			continue;

		InventorySelectItem kInfo;
		kInfo.m_iMagicCode = i + 1;
		kInfo.m_szName = kSlot.m_Name;
		kInfo.m_szIconName = kSlot.m_IconName;
		kInfo.m_iTabType = kSlot.m_iItemCode/DEFAULT_BASIC_COSTUME_CODE;

		kInfo.m_szSubText = "영구장비"; //어차피 영구장비니께~

		char szBuf[MAX_PATH];
		if( pItem->GetEnableClass() > 0 )
		{
			SafeSprintf( szBuf, sizeof(szBuf), STR(1), g_MyInfo.GetClassName( pItem->GetEnableClass() ));
			kInfo.m_szSubText2 = szBuf;
		}
		else
			kInfo.m_szSubText2 = STR(2);

		kInfo.m_nGrade = pItem->GetGradeType();
		m_vTotalList.push_back( kInfo);
	}
}

void ItemNewMultipleCompoundWnd::UpdateData()
{
	switch( m_dwMaterialType )
	{
	case PRESENT_MEDALITEM:
		UpdateMedalData();
		break;
	case PRESENT_COSTUME:
		UpdateCostumeData();
		break;
	case PRESENT_EXTRAITEM:
		UpdateExtraItemData();
		break;
	}
}

void ItemNewMultipleCompoundWnd::UpdateMedalData()
{
	ioUserMedalItem::MEDALITEMSLOT kSlot;
	ioUserMedalItem *pMedaltem = g_MyInfo.GetUserMedalItem();
	if( !pMedaltem )
		return;

	m_bAllSelect = false;

	int iSelectedCount = 0;
	for( int i = 0 ; i< MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( ID_ITEM1_BTN + i ) );
		if ( !pItem || pItem->HasWndStyle( IWS_INACTIVE ) )
			continue;

		int iItemIndex = pItem->GetSelectExtraItemSlot();
		if ( iItemIndex == 0 )
			continue;

		kSlot.Init();
		if( pMedaltem->GetMedalItemArray( iItemIndex-1 , kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				ioUIRenderImage *pSubImg = NULL;
				const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( kSlot.m_iItemType );
				if( !pItem )
					continue;

				pImg = g_UIImageSetMgr.CreateImageByFullName( pItem->m_sIcon );
				pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItem->m_sSubIcon );
				pIconBtn->SetIcon( pImg, 0.62f, 0.62f );
				pIconBtn->SetSubIcon( pSubImg );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();

				iSelectedCount++;
			}

			if ( i == 0 )
			{
				SetChildActive(ID_ITEM2_BTN);
				SetChildActive(ID_ITEM3_BTN);
			}
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}
	}

	if ( iSelectedCount == MAX_ITEM_BTN )
		m_bAllSelect = true;
}

void ItemNewMultipleCompoundWnd::UpdateCostumeData()
{
	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume ) 
		return;

	m_bAllSelect = false;

	int iSelectedCount = 0;
	for( int i = 0 ; i< MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( ID_ITEM1_BTN + i ) );
		if ( !pItem || pItem->HasWndStyle( IWS_INACTIVE ) )
			continue;

		int iItemIndex = pItem->GetSelectExtraItemSlot();
		if ( iItemIndex == 0 )
			continue;

		kSlot.Init();
		if( pCostume->GetCostumeSlotArray( iItemIndex-1, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, 0.62f, 0.62f );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();
				pIconBtn->SetCostume( true );
				iSelectedCount++;
			}

			if ( i == 0 )
			{
				SetChildActive(ID_ITEM2_BTN);
				SetChildActive(ID_ITEM3_BTN);
			}
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}
	}

	if ( iSelectedCount == MAX_ITEM_BTN )
		m_bAllSelect = true;
}

void ItemNewMultipleCompoundWnd::UpdateExtraItemData()
{
	EXTRAITEMSLOT kSlot;
	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem ) 
		return;

	m_bAllSelect = false;

	int iSelectedCount = 0;
	for( int i = 0 ; i< MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( ID_ITEM1_BTN + i ) );
		if ( !pItem || pItem->HasWndStyle( IWS_INACTIVE ) )
			continue;

		int iItemIndex = pItem->GetSelectExtraItemSlot();
		if ( iItemIndex == 0 )
			continue;

		kSlot.Init();

		if( pExtraItem->GetExtraItemArray( iItemIndex - 1, kSlot) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, 0.62f, 0.62f );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();
				pIconBtn->SetCostume( true );
				iSelectedCount++;
			}

			if ( i == 0 )
			{
				SetChildActive(ID_ITEM2_BTN);
				SetChildActive(ID_ITEM3_BTN);
			}
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}
	}

	if ( iSelectedCount == MAX_ITEM_BTN )
		m_bAllSelect = true;
}

void ItemNewMultipleCompoundWnd::CheckCompoundItemBtn( ioWnd *pWnd, DWORD dwID )
{
	if( dwID == ID_ITEM1_BTN )
	{
		for ( int i=0 ; i<MAX_ITEM_BTN ; ++i )
		{
			SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_ITEM1_BTN + i ));
			if( pItemBtn )
				pItemBtn->InitData();

			InventoryIconBtn *pIconBtn = pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( ID_ICON1_BTN + i ));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		SetChildInActive( ID_ITEM2_BTN );
		SetChildInActive( ID_ITEM3_BTN );
	}

	InventorySelectItemWnd *pInventorySelectItemWnd = dynamic_cast< InventorySelectItemWnd* >( g_GUIMgr.FindWnd(INVENTORY_SELECT_ITEM_WND) );
	if ( pInventorySelectItemWnd && pInventorySelectItemWnd->IsShow() )
		pInventorySelectItemWnd->HideWnd();
	else if ( pInventorySelectItemWnd )
	{
		m_dwCurItemBtn = dwID;
		ShowInventorySelectItemWnd( pInventorySelectItemWnd );
	}
}

void ItemNewMultipleCompoundWnd::GetItemList( vInventorySelectItemList& vrkList, ioHashStringVec& vRadioNameList )
{
	vRadioNameList.clear();
	ioEtcItemNewMultipleCompound *pEtcItem = dynamic_cast< ioEtcItemNewMultipleCompound * > (g_EtcItemMgr.FindEtcItem(m_dwEtcItemType));
	if( pEtcItem == NULL ) 
		return;

	vRadioNameList = pEtcItem->GetRadionNameList();

	if( m_vTotalList.empty() )
		return;

	std::list<int> vItemIndexList;
	for( int i = 0 ; i< MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( ID_ITEM1_BTN + i ) );
		if ( !pItem || pItem->HasWndStyle( IWS_INACTIVE ) )
			continue;

		vItemIndexList.push_back( pItem->GetSelectExtraItemSlot() );
	}

	for ( int i=0 ; i<(int)m_vTotalList.size() ; ++i )
	{
		bool bEnable = true;
		for ( std::list<int>::iterator iter = vItemIndexList.begin(); 
			iter != vItemIndexList.end() ; ++iter )
		{
			if ( m_vTotalList[i].m_iMagicCode == *iter )
			{
				bEnable = false;
				vItemIndexList.erase( iter );
				break;
			}
		}

		if ( !bEnable )
			continue;

		vrkList.push_back( m_vTotalList[i] );
	}
}

void ItemNewMultipleCompoundWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+290, FONT_SIZE_12 );
}

void ItemNewMultipleCompoundWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );

	if( !pItem ) 
		return;

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

void ItemNewMultipleCompoundWnd::SendItemCompound()
{
	switch( m_dwMaterialType )
	{
	case PRESENT_MEDALITEM:
		SendMedalItemCompound();
		break;
	case PRESENT_COSTUME:
		SendCostumeItemCompound();
		break;
	case PRESENT_EXTRAITEM:
		SendExtraItemCompound();
		break;
	}
}

void ItemNewMultipleCompoundWnd::SendMedalItemCompound()
{
	ioUserMedalItem::MEDALITEMSLOT kSlot;
	ioUserMedalItem *pMedaltem = g_MyInfo.GetUserMedalItem();
	if( !pMedaltem )
		return;

	IntVec vSlotList;

	for ( int i=0 ; i<MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_ITEM1_BTN + i ));
		if( !pItem ) 
			return;

		kSlot.Init();
		int iItemIndex = pItem->GetSelectExtraItemSlot();
		if( pMedaltem->GetMedalItemArray( iItemIndex-1 , kSlot ) )
		{
			//장착 여부 체크
			if( kSlot.m_iEquipClass != 0 )
				continue;
			//영구, 기간제 체크
			if ( kSlot.m_iPeriodType != ioUserMedalItem::PT_MORTMAIN )
				continue;
			vSlotList.push_back( kSlot.m_iItemType );
		}
	}

	if ( vSlotList.size() != MAX_ITEM_BTN )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "재료에 사용될 장비가 선택되지 않았습니다." );
		return;
	}

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwResultGashaponIndex );
	if ( !pItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "result gashapon index error" );
		return;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowGashponWndByNewMultipleCompound( m_dwResultGashaponIndex, m_dwResultGashaponIndex );

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemType;
	kPacket << m_dwMaterialType;
	for ( int i=0; i < (int)vSlotList.size() ; ++i )
		kPacket << vSlotList[i];
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemNewMultipleCompoundWnd::SendCostumeItemCompound()
{
	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume ) 
		return;

	IntVec vSlotList;

	for ( int i=0 ; i<MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_ITEM1_BTN + i ));
		if( !pItem ) 
			return;

		kSlot.Init();
		int iItemIndex = pItem->GetSelectExtraItemSlot();
		if( pCostume->GetCostumeSlotArray( iItemIndex-1, kSlot ) )
		{
			//장착 여부 체크
			if( kSlot.m_bEquip )
				continue;
			//영구, 기간제 체크
			if ( kSlot.m_PeriodType != PCPT_MORTMAIN )
				continue;
			vSlotList.push_back( kSlot.m_nIndex );
		}
	}

	if ( vSlotList.size() != MAX_ITEM_BTN )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "재료에 사용될 장비가 선택되지 않았습니다." );
		return;
	}

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwResultGashaponIndex );
	if ( !pItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "result gashapon index error" );
		return;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowGashponWndByNewMultipleCompound( m_dwResultGashaponIndex, m_dwResultGashaponIndex );

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemType;
	kPacket << m_dwMaterialType;
	for ( int i=0; i < (int)vSlotList.size() ; ++i )
		kPacket << vSlotList[i];
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemNewMultipleCompoundWnd::SendExtraItemCompound()
{
	EXTRAITEMSLOT kSlot;
	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem ) 
		return;

	IntVec vSlotList;
	for ( int i=0 ; i<MAX_ITEM_BTN ; ++i )
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_ITEM1_BTN + i ));
		if( !pItem ) 
			return;

		kSlot.Init();
		int iItemIndex = pItem->GetSelectExtraItemSlot();
		if( pExtraItem->GetExtraItemArray( iItemIndex-1, kSlot) )
		{
			//장착 여부 체크
			if( kSlot.m_bCharEquip )
				continue;
			//영구, 기간제 체크
			if ( kSlot.m_PeriodType != PCPT_MORTMAIN )
				continue;

			vSlotList.push_back( kSlot.m_iIndex );
		}
	}

	if ( vSlotList.size() != MAX_ITEM_BTN )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "재료에 사용될 장비가 선택되지 않았습니다." );
		return;
	}

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwResultGashaponIndex );
	if ( !pItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "result gashapon index error" );
		return;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowGashponWndByNewMultipleCompound( m_dwResultGashaponIndex, m_dwResultGashaponIndex );

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemType;
	kPacket << m_dwMaterialType;
	for ( int i=0; i < (int)vSlotList.size() ; ++i )
		kPacket << vSlotList[i];
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemNewMultipleCompoundWnd::ShowInventorySelectItemWnd( InventorySelectItemWnd *pInventorySelectItemWnd )
{
	ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem(m_dwEtcItemType);
	if ( !pEtcItem )
		return;

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if ( !pUserEtcItem )
		return;

	ioUserEtcItem::ETCITEMSLOT kEtcSlot;
	if( !pUserEtcItem->GetEtcItem( m_dwEtcItemType, kEtcSlot ) )
		return;

	int iEtcitemCount = kEtcSlot.m_iValue1;
	vInventorySelectItemList vItemList;
	ioHashStringVec vRadioNameList;
	GetItemList( vItemList, vRadioNameList );
	if ( vItemList.empty() )
		return;

	bool bIsCostume = false;
	if ( m_dwMaterialType == PRESENT_COSTUME )
		bIsCostume = true;

	pInventorySelectItemWnd->ShowInventorySelectItemWnd( vItemList, this, vRadioNameList, pEtcItem->GetName(), iEtcitemCount, bIsCostume );
}
