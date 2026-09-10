#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"

#include "../NetworkWrappingFunc.h"
#include "../TextColorTable.h"
#include "../ioApplication.h"
#include "../EtcHelpFunc.h"

#include "../ioMyInfo.h"
#include "../ioItemCompoundManager.h"
#include "../ioEtcItem.h"
#include "../ioEtcItemManager.h"

#include "GlobalWnd.h"
#include "ManualWnd.h"
#include "MyInventoryInfoWnd.h"

#include "../WndID.h"
#include "ioMessageBox.h"
#include "ioSP2GUIManager.h"

#include ".\ItemMultipleCompoundWnd.h"
#include <strsafe.h>
#include "MyItemSelecteWnd.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ItemMultipleCompoundWnd::ItemMultipleCompoundWnd(void)
{
	m_dwEtcItemType = 0;
	m_bAllSelect = false;

	m_vTotalList.clear();
	m_pManual = NULL;
}

ItemMultipleCompoundWnd::~ItemMultipleCompoundWnd(void)
{
}

void ItemMultipleCompoundWnd::iwm_create()
{
}

void ItemMultipleCompoundWnd::SetInfoAndShow( DWORD dwType )
{
	m_vTotalList.clear();

	m_bAllSelect = false;
	m_dwEtcItemType = dwType;

	// Load Total ExtraItemList
	LoadTotalList();

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// 초기화
	SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	InventoryIconBtn *pIconBtn = NULL;
	pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON1_BTN));
	if( pIconBtn )
		pIconBtn->SetIcon( NULL, 0.62f, 0.62f );

	pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON2_BTN));
	if( pIconBtn )
		pIconBtn->SetIcon( NULL, 0.62f, 0.62f );

	pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON3_BTN));
	if( pIconBtn )
		pIconBtn->SetIcon( NULL, 0.62f, 0.62f );


	if( m_vTotalList.empty() )
	{
		SetChildInActive( ID_ITEM1_BTN );
		SetChildInActive( ID_ITEM2_BTN );
		SetChildInActive( ID_ITEM3_BTN );
	}
	else
	{
		if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
			m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
			m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
		{
			SetChildActive( ID_ITEM1_BTN );
			SetChildInActive( ID_ITEM2_BTN );
			SetChildInActive( ID_ITEM3_BTN );
		}
		else
		{
			SetChildActive( ID_ITEM1_BTN );
			SetChildActive( ID_ITEM2_BTN );
			SetChildActive( ID_ITEM3_BTN );
		}
	}

	HideChildWnd( ID_ITEM_LIST_WND );

	UpdateData();

	ShowWnd();
}

void ItemMultipleCompoundWnd::LoadTotalList()
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

void ItemMultipleCompoundWnd::GetCurList( DWORD dwID, vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();

	SelectExtraItemBtn *pTarget = NULL;
	int iItemIndex1, iItemIndex2, iItemIndex3;
	int iItemType1, iItemType2, iItemType3;

	iItemIndex1 = iItemIndex2 = iItemIndex3 = 0;
	iItemType1 = iItemType2 = iItemType3 = -1;

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pTarget )
	{
		iItemIndex1 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex1, kTargetSlot ) )
		{
			iItemType1 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pTarget )
	{
		iItemIndex2 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex2, kTargetSlot ) )
		{
			iItemType2 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pTarget )
	{
		iItemIndex3 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex3, kTargetSlot ) )
		{
			iItemType3 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	bool bTotal = false;
	if( iItemIndex1 == 0 && iItemIndex2 == 0 && iItemIndex3 == 0 )
		bTotal = true;

	for( int i=0; i < iSize; ++i )
	{
		int iCurIndex = m_vTotalList[i].m_iSlotIndex;
		int iCurType = m_vTotalList[i].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

		if( !bTotal )
		{
			// Equal Index
			if( iCurIndex == iItemIndex1 || iCurIndex == iItemIndex2 || iCurIndex == iItemIndex3 )
				continue;

			// Part Check
			if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
			{
				if( iItemType1 != -1 && iItemType1 != iCurType )
					continue;

				if( iItemType2 != -1 && iItemType2 != iCurType )
					continue;

				if( iItemType3 != -1 && iItemType3 != iCurType )
					continue;
			}
		}

		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( iCurIndex, kTargetSlot ) )
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

void ItemMultipleCompoundWnd::GetCurSeletedBtnList( DWORD dwID, ExtraItemList &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();

	SelectExtraItemBtn *pTarget = NULL;
	int iItemIndex1, iItemIndex2, iItemIndex3;
	int iItemType1, iItemType2, iItemType3;

	iItemIndex1 = iItemIndex2 = iItemIndex3 = 0;
	iItemType1 = iItemType2 = iItemType3 = -1;

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pTarget )
	{
		iItemIndex1 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex1, kTargetSlot ) )
		{
			iItemType1 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pTarget )
	{
		iItemIndex2 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex2, kTargetSlot ) )
		{
			iItemType2 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pTarget )
	{
		iItemIndex3 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex3, kTargetSlot ) )
		{
			iItemType3 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	bool bTotal = false;
	if( iItemIndex1 == 0 && iItemIndex2 == 0 && iItemIndex3 == 0 )
		bTotal = true;

	for( int i=0; i < iSize; ++i )
	{
		int iCurIndex = m_vTotalList[i].m_iSlotIndex;
		int iCurType = m_vTotalList[i].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

		if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4) //유니크 장비 합성기면
		{
			const ioItem* pEtcItem = g_ItemMaker.GetItemConst(  m_vTotalList[i].m_iItemCode );

			if( !pEtcItem || pEtcItem->GetGradeType()  != 4 )
				continue;
		}

		if( !bTotal )
		{
			// Equal Index
			if( iCurIndex == iItemIndex1 || iCurIndex == iItemIndex2 || iCurIndex == iItemIndex3 )
				continue;

			// Part Check
			if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
			{
				if( iItemType1 != -1 && iItemType1 != iCurType )
					continue;

				if( iItemType2 != -1 && iItemType2 != iCurType )
					continue;

				if( iItemType3 != -1 && iItemType3 != iCurType )
					continue;
			}
		}

		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( iCurIndex, kTargetSlot ) )
			continue;

		rkList.push_back( kTargetSlot );
	}
}


void ItemMultipleCompoundWnd::iwm_show()
{
}

void ItemMultipleCompoundWnd::iwm_hide()
{
	m_dwEtcItemType = 0;
	m_bAllSelect = false;

	m_vTotalList.clear();

	SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pItemBtn )
		pItemBtn->InitData();
}

void ItemMultipleCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ITEM_LIST_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				CheckItemListWnd( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}

			UpdateData();
		}
		break;
	case MYITEM_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( m_dwCurItemBtn ) );
			if( pTarget && param > 0 )
				pTarget->SetSelectExtraItemSlot( param );

			m_dwCurItemBtn = 0;
			UpdateData();
		}
		break;
	}
}

void ItemMultipleCompoundWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+290, FONT_SIZE_12 );
}

void ItemMultipleCompoundWnd::UpdateData()
{
	SelectExtraItemBtn *pItem1 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( !pItem1 ) return;

	SelectExtraItemBtn *pItem2 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( !pItem2 ) return;

	SelectExtraItemBtn *pItem3 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( !pItem3 ) return;

	int iItemIndex1 = pItem1->GetSelectExtraItemSlot();
	int iItemIndex2 = pItem2->GetSelectExtraItemSlot();
	int iItemIndex3 = pItem3->GetSelectExtraItemSlot();

	m_bAllSelect = false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		bool bSelect1 = false;
		if( pExtraItem->GetExtraItem( iItemIndex1, kSlot ) )
		{
			bSelect1 = true;
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON1_BTN));
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
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON1_BTN));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		bool bSelect2 = false;
		if( pExtraItem->GetExtraItem( iItemIndex2, kSlot ) )
		{
			bSelect2 = true;
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON2_BTN));
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
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON2_BTN));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		bool bSelect3 = false;
		if( pExtraItem->GetExtraItem( iItemIndex3, kSlot ) )
		{
			bSelect3 = true;
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON3_BTN));
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
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON3_BTN));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		if( bSelect1 && bSelect2 && bSelect3 )
		{
			m_bAllSelect = true;
		}
	}

	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
	{
		if( pExtraItem->GetExtraItem( iItemIndex1, kSlot ) )
		{
			SetChildActive(ID_ITEM2_BTN);
			SetChildActive(ID_ITEM3_BTN);
		}
	}
}

void ItemMultipleCompoundWnd::SendItemCompound()
{
	SelectExtraItemBtn *pItem1 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( !pItem1 ) return;

	SelectExtraItemBtn *pItem2 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( !pItem2 ) return;

	SelectExtraItemBtn *pItem3 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( !pItem3 ) return;

	int iItemIndex1 = pItem1->GetSelectExtraItemSlot();
	int iItemIndex2 = pItem2->GetSelectExtraItemSlot();
	int iItemIndex3 = pItem3->GetSelectExtraItemSlot();

	int iItemType = -1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		if( !pExtraItem->GetExtraItem( iItemIndex1, kSlot ) ||
			!pExtraItem->GetExtraItem( iItemIndex2, kSlot ) ||
			!pExtraItem->GetExtraItem( iItemIndex3, kSlot ) )
		{
			if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}
		else
		{
			iItemType = kSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ItemMultipleCompoundWnd::SendItemCompound - Exception Error(1)" );
		return;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemType;
	kPacket << iItemIndex1;
	kPacket << iItemIndex2;
	kPacket << iItemIndex3;
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void ItemMultipleCompoundWnd::CheckCompoundItemBtn( ioWnd *pWnd, DWORD dwID )
{
	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
	{
		if( dwID == ID_ITEM1_BTN )
		{
			SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
			if( pItemBtn )
				pItemBtn->InitData();

			pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
			if( pItemBtn )
				pItemBtn->InitData();

			pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
			if( pItemBtn )
				pItemBtn->InitData();

			SetChildInActive( ID_ITEM2_BTN );
			SetChildInActive( ID_ITEM3_BTN );
		}
	}

	MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
	if ( pSelectWnd )
	{
		ExtraItemList rkList;
		GetCurSeletedBtnList( dwID, rkList );
		if ( !rkList.empty() )
		{
			m_dwCurItemBtn = dwID;
			pSelectWnd->ShowItemSeletWnd( rkList, this );
			UpdateData();
		}
	}
	/*
	NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_ITEM_LIST_WND));
	if( pListWnd && pListWnd->IsShow() )
	{
		pListWnd->HideWnd();
	}
	else if( pListWnd )
	{
		m_dwCurItemBtn = dwID;

		int iXPos, iYPos;
		iXPos = GetDerivedPosX();
		iYPos = GetDerivedPosY();

		pListWnd->CreatePullDown( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 25 );

		vNewShopPullDownItem kItemList;
		GetCurList( dwID, kItemList );

		if( !kItemList.empty() )
		{
			int iSize = kItemList.size();
			for(int i = 0;i < iSize; i++)
			{
				pListWnd->InsertItem( kItemList[i] );
			}
			pListWnd->ShowWnd();

			UpdateData();
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
	}
	*/

}

void ItemMultipleCompoundWnd::CheckItemListWnd( ioWnd *pWnd, int iArray )
{
	if( !pWnd )  return;

	vNewShopPullDownItem kItemList;
	DWORD dwID = pWnd->GetID();

	GetCurList( dwID, kItemList );

	if( !COMPARE( iArray, 0, (int)kItemList.size() ) ) 
		return;

	ioUITitle *pTitle =  pWnd->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iSlot = rkItem.m_iMagicCode;
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(pWnd);
	if( pTarget && iSlot > 0 )
	{
		pTarget->SetSelectExtraItemSlot( iSlot );
	}

	m_dwCurItemBtn = 0;
}

void ItemMultipleCompoundWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;

	pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );

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


UniqueItemMultipleCompoundWnd::UniqueItemMultipleCompoundWnd(void)
{
	m_dwEtcItemType = 0;
	m_bAllSelect = false;

	m_vTotalList.clear();
	m_pManual = NULL;
}

UniqueItemMultipleCompoundWnd::~UniqueItemMultipleCompoundWnd(void)
{
}

void UniqueItemMultipleCompoundWnd::iwm_create()
{
}

void UniqueItemMultipleCompoundWnd::SetInfoAndShow( DWORD dwType )
{
	m_vTotalList.clear();

	m_bAllSelect = false;
	m_dwEtcItemType = dwType;

	// Load Total ExtraItemList
	LoadTotalList();

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// 초기화
	SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	InventoryIconBtn *pIconBtn = NULL;
	pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON1_BTN));
	if( pIconBtn )
		pIconBtn->SetIcon( NULL, 0.62f, 0.62f );

	pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON2_BTN));
	if( pIconBtn )
		pIconBtn->SetIcon( NULL, 0.62f, 0.62f );

	pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON3_BTN));
	if( pIconBtn )
		pIconBtn->SetIcon( NULL, 0.62f, 0.62f );


	if( m_vTotalList.empty() )
	{
		SetChildInActive( ID_ITEM1_BTN );
		SetChildInActive( ID_ITEM2_BTN );
		SetChildInActive( ID_ITEM3_BTN );
	}
	else
	{
		if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
			m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
			m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
		{
			SetChildActive( ID_ITEM1_BTN );
			SetChildInActive( ID_ITEM2_BTN );
			SetChildInActive( ID_ITEM3_BTN );
		}
		else
		{
			SetChildActive( ID_ITEM1_BTN );
			SetChildActive( ID_ITEM2_BTN );
			SetChildActive( ID_ITEM3_BTN );
		}
	}

	HideChildWnd( ID_ITEM_LIST_WND );

	UpdateData();

	ShowWnd();
}

void UniqueItemMultipleCompoundWnd::LoadTotalList()
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
	{
		HideWnd();
		return;
	}

	ioUserEtcItem* UserEtcItem = g_MyInfo.GetUserEtcItem();
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

void UniqueItemMultipleCompoundWnd::GetCurList( DWORD dwID, vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();

	SelectExtraItemBtn *pTarget = NULL;
	int iItemIndex1, iItemIndex2, iItemIndex3;
	int iItemType1, iItemType2, iItemType3;

	iItemIndex1 = iItemIndex2 = iItemIndex3 = 0;
	iItemType1 = iItemType2 = iItemType3 = -1;

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pTarget )
	{
		iItemIndex1 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex1, kTargetSlot ) )
		{
			iItemType1 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pTarget )
	{
		iItemIndex2 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex2, kTargetSlot ) )
		{
			iItemType2 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pTarget )
	{
		iItemIndex3 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex3, kTargetSlot ) )
		{
			iItemType3 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	bool bTotal = false;
	if( iItemIndex1 == 0 && iItemIndex2 == 0 && iItemIndex3 == 0 )
		bTotal = true;

	for( int i=0; i < iSize; ++i )
	{
		int iCurIndex = m_vTotalList[i].m_iSlotIndex;
		int iCurType = m_vTotalList[i].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

		if( !bTotal )
		{
			// Equal Index
			if( iCurIndex == iItemIndex1 || iCurIndex == iItemIndex2 || iCurIndex == iItemIndex3 )
				continue;

			// Part Check
			if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
			{
				if( iItemType1 != -1 && iItemType1 != iCurType )
					continue;

				if( iItemType2 != -1 && iItemType2 != iCurType )
					continue;

				if( iItemType3 != -1 && iItemType3 != iCurType )
					continue;
			}
		}

		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( iCurIndex, kTargetSlot ) )
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
			StringCbPrintf_e( szName, sizeof( szName ), "%s +%d", kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void UniqueItemMultipleCompoundWnd::GetCurSeletedBtnList( DWORD dwID, ExtraItemList &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalList.size();

	SelectExtraItemBtn *pTarget = NULL;
	int iItemIndex1, iItemIndex2, iItemIndex3;
	int iItemType1, iItemType2, iItemType3;

	iItemIndex1 = iItemIndex2 = iItemIndex3 = 0;
	iItemType1 = iItemType2 = iItemType3 = -1;

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pTarget )
	{
		iItemIndex1 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex1, kTargetSlot ) )
		{
			iItemType1 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pTarget )
	{
		iItemIndex2 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex2, kTargetSlot ) )
		{
			iItemType2 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pTarget )
	{
		iItemIndex3 = pTarget->GetSelectExtraItemSlot();

		EXTRAITEMSLOT kTargetSlot;
		if( pItem->GetExtraItem( iItemIndex3, kTargetSlot ) )
		{
			iItemType3 = kTargetSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;		
		}
	}

	bool bTotal = false;
	if( iItemIndex1 == 0 && iItemIndex2 == 0 && iItemIndex3 == 0 )
		bTotal = true;

	for( int i=0; i < iSize; ++i )
	{
		int iCurIndex = m_vTotalList[i].m_iSlotIndex;
		int iCurType = m_vTotalList[i].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;

		if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4) //유니크 장비 합성기면
		{
			const ioItem* pEtcItem = g_ItemMaker.GetItemConst(  m_vTotalList[i].m_iItemCode );

			if( !pEtcItem || pEtcItem->GetGradeType()  != 4 ) //유니크 장비
				continue;
		}

		if( !bTotal )
		{
			// Equal Index
			if( iCurIndex == iItemIndex1 || iCurIndex == iItemIndex2 || iCurIndex == iItemIndex3 )
				continue;

			// Part Check
			if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
			{
				if( iItemType1 != -1 && iItemType1 != iCurType )
					continue;

				if( iItemType2 != -1 && iItemType2 != iCurType )
					continue;

				if( iItemType3 != -1 && iItemType3 != iCurType )
					continue;
			}
		}

		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( iCurIndex, kTargetSlot ) )
			continue;

		rkList.push_back( kTargetSlot );
	}
}


void UniqueItemMultipleCompoundWnd::iwm_show()
{
}

void UniqueItemMultipleCompoundWnd::iwm_hide()
{
	m_dwEtcItemType = 0;
	m_bAllSelect = false;

	m_vTotalList.clear();

	SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( pItemBtn )
		pItemBtn->InitData();

	pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( pItemBtn )
		pItemBtn->InitData();
}

void UniqueItemMultipleCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_ITEM_LIST_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				CheckItemListWnd( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}

			UpdateData();
		}
		break;
	case MYITEM_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>( FindChildWnd( m_dwCurItemBtn ) );
			if( pTarget && param > 0 )
				pTarget->SetSelectExtraItemSlot( param );

			m_dwCurItemBtn = 0;
			UpdateData();
		}
		break;
	}
}

void UniqueItemMultipleCompoundWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+290, FONT_SIZE_12 );
}

void UniqueItemMultipleCompoundWnd::UpdateData()
{
	SelectExtraItemBtn *pItem1 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( !pItem1 ) return;

	SelectExtraItemBtn *pItem2 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( !pItem2 ) return;

	SelectExtraItemBtn *pItem3 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( !pItem3 ) return;

	int iItemIndex1 = pItem1->GetSelectExtraItemSlot();
	int iItemIndex2 = pItem2->GetSelectExtraItemSlot();
	int iItemIndex3 = pItem3->GetSelectExtraItemSlot();

	m_bAllSelect = false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		bool bSelect1 = false;
		if( pExtraItem->GetExtraItem( iItemIndex1, kSlot ) )
		{
			bSelect1 = true;
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON1_BTN));
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
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON1_BTN));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		bool bSelect2 = false;
		if( pExtraItem->GetExtraItem( iItemIndex2, kSlot ) )
		{
			bSelect2 = true;
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON2_BTN));
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
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON2_BTN));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		bool bSelect3 = false;
		if( pExtraItem->GetExtraItem( iItemIndex3, kSlot ) )
		{
			bSelect3 = true;
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON3_BTN));
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
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ICON3_BTN));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.62f, 0.62f );
		}

		if( bSelect1 && bSelect2 && bSelect3 )
		{
			m_bAllSelect = true;
		}
	}

	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
	{
		if( pExtraItem->GetExtraItem( iItemIndex1, kSlot ) )
		{
			SetChildActive(ID_ITEM2_BTN);
			SetChildActive(ID_ITEM3_BTN);
		}
	}
}

void UniqueItemMultipleCompoundWnd::SendItemCompound()
{
	SelectExtraItemBtn *pItem1 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
	if( !pItem1 ) return;

	SelectExtraItemBtn *pItem2 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
	if( !pItem2 ) return;

	SelectExtraItemBtn *pItem3 = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
	if( !pItem3 ) return;

	int iItemIndex1 = pItem1->GetSelectExtraItemSlot();
	int iItemIndex2 = pItem2->GetSelectExtraItemSlot();
	int iItemIndex3 = pItem3->GetSelectExtraItemSlot();

	int iItemType = -1;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;

		if( !pExtraItem->GetExtraItem( iItemIndex1, kSlot ) ||
			!pExtraItem->GetExtraItem( iItemIndex2, kSlot ) ||
			!pExtraItem->GetExtraItem( iItemIndex3, kSlot ) )
		{
			if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
				m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}
		else
		{
			iItemType = kSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "UniqueItemMultipleCompoundWnd::SendItemCompound - Exception Error(1)" );
		return;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemType;
	kPacket << iItemIndex1;
	kPacket << iItemIndex2;
	kPacket << iItemIndex3;
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void UniqueItemMultipleCompoundWnd::CheckCompoundItemBtn( ioWnd *pWnd, DWORD dwID )
{
	if( m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 ||
		m_dwEtcItemType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
	{
		if( dwID == ID_ITEM1_BTN )
		{
			SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM1_BTN));
			if( pItemBtn )
				pItemBtn->InitData();

			pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM2_BTN));
			if( pItemBtn )
				pItemBtn->InitData();

			pItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_ITEM3_BTN));
			if( pItemBtn )
				pItemBtn->InitData();

			SetChildInActive( ID_ITEM2_BTN );
			SetChildInActive( ID_ITEM3_BTN );
		}
	}

	MyItemSelecteWnd *pSelectWnd = dynamic_cast<MyItemSelecteWnd*>( g_GUIMgr.FindWnd( MYITEM_SELECT_WND ) );
	if ( pSelectWnd )
	{
		ExtraItemList rkList;
		GetCurSeletedBtnList( dwID, rkList );
		if ( !rkList.empty() )
		{
			m_dwCurItemBtn = dwID;
			pSelectWnd->ShowItemSeletWnd( rkList, this );
			UpdateData();
		}
	}
}

void UniqueItemMultipleCompoundWnd::CheckItemListWnd( ioWnd *pWnd, int iArray )
{
	if( !pWnd )  return;

	vNewShopPullDownItem kItemList;
	DWORD dwID = pWnd->GetID();

	GetCurList( dwID, kItemList );

	if( !COMPARE( iArray, 0, (int)kItemList.size() ) ) 
		return;

	ioUITitle *pTitle =  pWnd->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iSlot = rkItem.m_iMagicCode;
	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(pWnd);
	if( pTarget && iSlot > 0 )
	{
		pTarget->SetSelectExtraItemSlot( iSlot );
	}

	m_dwCurItemBtn = 0;
}

void UniqueItemMultipleCompoundWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;

	pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );

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