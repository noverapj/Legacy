#include "StdAfx.h"

#include "EtcItemSlotWnd.h"
#include "ioSpecificDragItem.h"
#include "../ioSlotMgr.h"

#include "GameOptionWnd.h"

//////////////////////////////////////////////////////////////////////////
EtcItemSlotWnd::EtcItemSlotWnd()
{
	m_pBoxLeft = NULL;
	m_pBoxTop = NULL;
}

EtcItemSlotWnd::~EtcItemSlotWnd()
{
	SAFEDELETE(m_pBoxLeft);
	SAFEDELETE(m_pBoxTop);
}

void EtcItemSlotWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "box_left" )
	{
		SAFEDELETE( m_pBoxLeft );
		m_pBoxLeft = pImage;
	}
	else if( szType == "box_top" )
	{
		SAFEDELETE( m_pBoxTop );
		m_pBoxTop = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

void EtcItemSlotWnd::ShowSlotWnd()
{
	ShowWnd();

	ItemSlotBtn *pItemBtn = dynamic_cast<ItemSlotBtn*>(FindChildWnd(ID_SLOT_ITEM_BTN1));

	if( pItemBtn )
		pItemBtn->ShowWnd();

	pItemBtn = dynamic_cast<ItemSlotBtn*>(FindChildWnd(ID_SLOT_ITEM_BTN2));
	if( pItemBtn )
		pItemBtn->ShowWnd();

	pItemBtn = dynamic_cast<ItemSlotBtn*>(FindChildWnd(ID_SLOT_ITEM_BTN3));
	if( pItemBtn )
		pItemBtn->ShowWnd();

	pItemBtn = dynamic_cast<ItemSlotBtn*>(FindChildWnd(ID_SLOT_ITEM_BTN4));
	if( pItemBtn )
		pItemBtn->ShowWnd();

	SlotWnd *pSlotWnd = NULL;

	for(int i = ID_SLOT_INVEN_START; i < ID_SLOT_INVEN_FINISH + 1; i++)
	{
		pSlotWnd = dynamic_cast<SlotWnd*>(FindChildWnd(i));

		if(pSlotWnd)
			pSlotWnd->ShowWnd();
	}

	SetSlotItemBtn();
	SetSlotInvenWnd();
}

void EtcItemSlotWnd::SetSlotItemBtn()
{
	ItemSlotBtn *pItemBtn = NULL;

	for(int i = ID_SLOT_ITEM_BTN1; i < ID_SLOT_ITEM_BTN4+1; i++)
	{
		pItemBtn = dynamic_cast<ItemSlotBtn*>(FindChildWnd(i));
		if( pItemBtn )
			pItemBtn->InitItem(i - ID_SLOT_ITEM_BTN1);
	}
}

void EtcItemSlotWnd::SetSlotInvenWnd()
{
	ioEtcItem *pItem = NULL;
	SlotWnd *pInvenWnd = NULL;

	int nInvenWnd = ID_SLOT_INVEN_START;

	for(int i = ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01; i < ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE + 1; i++)  
	{
		pItem = g_EtcItemMgr.FindEtcItem(i);

		if(pItem)
		{
			pInvenWnd = dynamic_cast<SlotWnd*>(FindChildWnd(nInvenWnd++));

			if(pInvenWnd)
				pInvenWnd->InitItem(pItem);
		}
	}
}


void EtcItemSlotWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BTN_EXIT:
	case ID_BTN_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;

	case ID_BTN_SHORTCUT:
		if( cmd == IOBN_BTNUP )
		{
			if( g_GUIMgr.IsShow( GAME_OPTION_WND ))
			{
				ioWnd *pParent = g_GUIMgr.FindWnd( GAME_OPTION_WND );
				if(pParent)
					pParent->GoToTop();
			}	
			else
			{
				ioWnd *pParent = g_GUIMgr.FindWnd( GAME_OPTION_WND );
				pParent->ShowWnd();
				ioWnd *pKeyWnd = pParent->FindChildWnd(GameOptionWnd::ID_KEYBOARD_TAB_BTN);
				if(pKeyWnd)
					pParent->iwm_command( pKeyWnd,IOBN_BTNDOWN, 0 );
			}
		}
		break;

	case ID_SLOT_ITEM_BTN1:
	case ID_SLOT_ITEM_BTN2:
	case ID_SLOT_ITEM_BTN3:
	case ID_SLOT_ITEM_BTN4:
		if( cmd == IOBN_BTNUP)
		{
			if( g_SlotMgr.GetSlotItemType(dwID - ID_SLOT_ITEM_BTN1) != ioEtcItem::EIT_NONE)
			{
				if(!g_SlotMgr.IsCoolTime(dwID - ID_SLOT_ITEM_BTN1))
				{
					g_SlotMgr.SetInvetToSlot(dwID - ID_SLOT_ITEM_BTN1, ioEtcItem::EIT_NONE);
					g_SlotMgr.InitSlot();
					SetSlotItemBtn();
					SetSlotInvenWnd();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "쿨타임이 남아 있으면#아이템 교체를 할 수 없습니다." );
				}
			}
		}
		break;

	default:
		if( COMPARE(dwID, ID_SLOT_INVEN_START, ID_SLOT_INVEN_FINISH + 1))
		{
			if(cmd == IOBN_BTNUP)
			{
				SlotWnd *pBtn = dynamic_cast<SlotWnd*>( FindChildWnd( dwID ) );

				if(pBtn && pBtn->GetActive() && (pBtn->GetItemCode() != ioEtcItem::EIT_NONE))
				{
					int nEmptySlot = g_SlotMgr.GetEmptySlot();
					if( nEmptySlot != -1)
					{
						g_SlotMgr.SetInvetToSlot(nEmptySlot, pBtn->GetItemCode());
						g_SlotMgr.InitSlot();
						SetSlotItemBtn();
						SetSlotInvenWnd();
					}
				}
			}
		}
		break;
	}
}

void EtcItemSlotWnd::iwm_show()
{
}

void EtcItemSlotWnd::iwm_hide()
{
	g_SlotMgr.InitAllItem();
}

bool EtcItemSlotWnd::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{
	DWORD dwBtnID = CheckDropEnable( mouse, pItem );

	if( dwBtnID == 0 )
		return false;

	ioInvenDragItem *pDragItem = ToInvenDragItem( pItem );
	if( !pDragItem )
		return false;

	if( COMPARE( dwBtnID, ID_SLOT_ITEM_BTN1, ID_SLOT_ITEM_BTN4 + 1 ) )
	{
		ItemSlotBtn *pBtn = dynamic_cast<ItemSlotBtn*>( FindChildWnd( dwBtnID ) );
		if( !pBtn || !pBtn->IsShow() )
			return false;

		if(COMPARE( pDragItem->GetItemDBIndex(), ID_SLOT_ITEM_BTN1, ID_SLOT_ITEM_BTN4 + 1 ) )
		{
			if(!g_SlotMgr.IsCoolTime( pDragItem->GetItemDBIndex() - ID_SLOT_ITEM_BTN1 ) && !g_SlotMgr.IsCoolTime( dwBtnID - ID_SLOT_ITEM_BTN1 ) )
			{
				g_SlotMgr.SetSlotToSlot(pDragItem->GetItemDBIndex() - ID_SLOT_ITEM_BTN1, dwBtnID - ID_SLOT_ITEM_BTN1);
				g_SlotMgr.InitSlot();
				SetSlotItemBtn();
				SetSlotInvenWnd();
				return true;
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "쿨타임이 남아 있으면#아이템 교체를 할 수 없습니다." );
			}
		}
		else
		{
			if( !g_SlotMgr.IsCoolTime( dwBtnID - ID_SLOT_ITEM_BTN1 ) )
			{
				g_SlotMgr.SetInvetToSlot( dwBtnID - ID_SLOT_ITEM_BTN1, pDragItem->GetItemCode() );
				g_SlotMgr.InitSlot();
				SetSlotItemBtn();
				SetSlotInvenWnd();
				return true;
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "쿨타임이 남아 있으면#아이템 교체를 할 수 없습니다." );
			}
		}
	}
	else if( COMPARE( dwBtnID, ID_SLOT_INVEN_START, ID_SLOT_INVEN_FINISH + 1 ) )
	{
		SlotWnd *pBtn = dynamic_cast<SlotWnd*>( FindChildWnd( dwBtnID ) );
		if( !pBtn || !pBtn->IsShow() )
			return false;

		if(COMPARE( pDragItem->GetItemDBIndex(), ID_SLOT_ITEM_BTN1, ID_SLOT_ITEM_BTN4 + 1 ) )
		{
			if( !g_SlotMgr.IsCoolTime( pDragItem->GetItemDBIndex() - ID_SLOT_ITEM_BTN1 ) )
			{
				g_SlotMgr.SetInvetToSlot( pDragItem->GetItemDBIndex() - ID_SLOT_ITEM_BTN1, ioEtcItem::EIT_NONE );
				g_SlotMgr.InitSlot();
				SetSlotItemBtn();
				SetSlotInvenWnd();
				return true;
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "쿨타임이 남아 있으면#아이템 교체를 할 수 없습니다." );
			}
		}
	}
	
	return false;
}

void EtcItemSlotWnd::UpdateAllSlot()
{

}

DWORD EtcItemSlotWnd::CheckDropEnable( const ioMouse& mouse, ioDragItem *pItem )
{
	ioInvenDragItem *pDragItem = ToInvenDragItem( pItem );
	if( !pDragItem )	return 0;

	for( int i = ID_SLOT_ITEM_BTN1; i < ID_SLOT_ITEM_BTN4+1; ++i )
	{
		ItemSlotBtn *pBtn = dynamic_cast<ItemSlotBtn*>( FindChildWnd(i) );
		if( pBtn && pBtn->IsShow() )
		{
			if( pBtn->IsInWndRect( mouse.GetMousePos() ) )
			{
				return i;
			}
			else
			{
				ioMouse NullMouse;
				pBtn->iwm_mouseleave( NullMouse );
			}
		}
	}

	for( int i = ID_SLOT_INVEN_START; i < ID_SLOT_INVEN_FINISH + 1; ++i )
	{
		SlotWnd *pBtn = dynamic_cast<SlotWnd*>( FindChildWnd(i) );
		if( pBtn && pBtn->IsShow() )
		{
			if( pBtn->IsInWndRect( mouse.GetMousePos() ) )
			{
				return i;
			}
			else
			{
				ioMouse NullMouse;
				pBtn->iwm_mouseleave( NullMouse );
			}
		}
	}

	return 0;
}


void EtcItemSlotWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 53, FONT_SIZE_13, "드래그로 원하는 슬롯에 배치가 가능합니다." );

	if( m_pBoxLeft && m_pBoxTop )
	{
		m_pBoxLeft->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pBoxLeft->Render( iXPos+20, iYPos+86 );

		m_pBoxTop->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBoxTop->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pBoxTop->SetSize( 62, 10 );

		m_pBoxTop->Render( iXPos+27, iYPos+86 );
		m_pBoxTop->Render( iXPos+213, iYPos+86 );

		m_pBoxLeft->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pBoxLeft->Render( iXPos+275, iYPos+86 );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+151, iYPos+80, FONT_SIZE_12,  "슬롯 배치 중인 아이템" );

	if( m_pBoxLeft && m_pBoxTop )
	{
		m_pBoxLeft->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
		m_pBoxLeft->Render( iXPos+20, iYPos+182 );

		m_pBoxTop->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBoxTop->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pBoxTop->SetSize( 62, 10 );

		m_pBoxTop->Render( iXPos+27, iYPos+182 );
		m_pBoxTop->Render( iXPos+213, iYPos+182 );

		m_pBoxLeft->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pBoxLeft->Render( iXPos+275, iYPos+182 );
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos+151, iYPos+176, FONT_SIZE_12,  "내가 보유 중인 아이템" );
}
//////////////////////////////////////////////////////////////////////////
ItemSlotBtn::ItemSlotBtn(void)
{
	m_pEmptyImg = NULL;
	m_pItemIcon = NULL;
	m_pItemBackImg = NULL;

	m_pEmptyFrm = NULL;
	m_pPieceFrm = NULL;
	m_pAdditiveFrm = NULL;

	m_dwItemCode = ioEtcItem::EIT_NONE;
	m_nItem = 0;

	m_strIcon = "";

	m_pDragItem = new ioInvenDragItem(SDI_INVEN_ITEM);
}

ItemSlotBtn::~ItemSlotBtn(void)
{
	SAFEDELETE(m_pEmptyImg);
	SAFEDELETE(m_pItemIcon);
	SAFEDELETE(m_pItemBackImg);

	SAFEDELETE( m_pEmptyFrm );
	SAFEDELETE( m_pPieceFrm );
	SAFEDELETE( m_pAdditiveFrm );

	SAFEDELETE(m_pDragItem);
}

void ItemSlotBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "empty_frm" )
	{
		SAFEDELETE( m_pEmptyFrm );
		m_pEmptyFrm = pFrame;
	}
	else if( szType == "piece_frm" )
	{
		SAFEDELETE( m_pPieceFrm );
		m_pPieceFrm = pFrame;
	}
	else if( szType == "additive_frm" )
	{
		SAFEDELETE( m_pAdditiveFrm );
		m_pAdditiveFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void ItemSlotBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemSlotBtn::InitItem(const int nSlot)
{
	SAFEDELETE(m_pItemIcon);	// 이 이미지의 유무로 빈 슬롯인지 아닌지를 구분한다.

	char szKeyText[MAX_PATH] = "";
	StringCbCopy( szKeyText, sizeof( szKeyText ), g_KeyManager.GetKeyText(g_KeyManager.GetCurrentConsumptionKey(nSlot)));

	if( strcmp( szKeyText, "지정되지 않음" ) == 0 )
	{
		strcpy(szKeyText, "없음");
	}
	else
	{
		szKeyText[2] = '\0';
	}

	m_strKey = szKeyText;

	ioSlotItem *pItem = NULL;

	if( pItem = g_SlotMgr.GetSlot(nSlot))
	{
		m_strIcon = pItem->GetImg();
		m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName(m_strIcon);

		SAFEDELETE(m_pItemBackImg);
		m_pItemBackImg = g_UIImageSetMgr.CreateImageByFullName(pItem->GetBackImg());

		m_nItem = pItem->GetCountItem();
		m_dwItemCode = pItem->GetType();
	}
}

void ItemSlotBtn::iwm_mouseleave( const ioMouse& mouse )
{
	if(m_pItemIcon && m_bClicked)
	{
		POINT ptOffset = { 0, 0};
		if(m_pDragItem)
		{
			m_pDragItem->SetItemData(GetID(), m_dwItemCode);
			m_pDragItem->SetDragImageName(m_strIcon);
			m_pDragItem->DoDragDrop(GetParent(), m_rcPos, ptOffset);
		}
	}

	ioButton::iwm_mouseleave( mouse );
}
void ItemSlotBtn::OnRender()
{
	enum 
	{
		CENTER_X	= 32,
		CENTER_Y	= 32,
	};

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( (m_pItemIcon) && (m_dwItemCode != ioEtcItem::EIT_NONE) && (m_nItem > 0)) 		
	{
		if( m_pPieceFrm )
			m_pPieceFrm->Render( iXPos, iYPos );

		if(m_pItemBackImg)
		{
			m_pItemBackImg->SetScale( 0.75f );
			m_pItemBackImg->Render( iXPos+32, iYPos+32, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		
		m_pItemIcon->SetScale( 0.75f );
		m_pItemIcon->Render( iXPos+32, iYPos+32, UI_RENDER_NORMAL, TFO_BILINEAR );

		if(!m_strKey.IsEmpty())
		{
			g_FontMgr.SetAlignType(TAT_LEFT);
			g_FontMgr.SetTextStyle(TS_OUTLINE_FULL_2X );
			g_FontMgr.SetBkColor(18, 78, 148);
			g_FontMgr.SetTextColor(TCT_DEFAULT_WHITE);
			g_FontMgr.PrintText( iXPos + 4, iYPos + 3, FONT_SIZE_12, m_strKey.c_str() );

			g_FontMgr.SetAlignType(TAT_RIGHT);
			g_FontMgr.SetTextStyle(TS_OUTLINE_FULL_3X );
			g_FontMgr.SetBkColor(58, 58, 58);
			g_FontMgr.SetTextColor(TCT_DEFAULT_WHITE);
			g_FontMgr.PrintText( iXPos + 61, iYPos + 45, FONT_SIZE_12, "%d", m_nItem );
		}

		if( IsOver() )
			OnDrawOvered( iXPos, iYPos );
	}
	else
	{
		if( m_pEmptyFrm )
		{
			m_pEmptyFrm->Render( iXPos, iYPos );

			if( m_pEmptyImg )
				m_pEmptyImg->Render( iXPos, iYPos);
		}

		if(!m_strKey.IsEmpty())
		{
			g_FontMgr.SetAlignType(TAT_CENTER);
			g_FontMgr.SetTextStyle(TS_NORMAL);
			g_FontMgr.SetTextColor(153, 153, 153);
			char strTmp[MAX_PATH] = "단축키 ";
			StringCbCat( strTmp, sizeof(strTmp), m_strKey.c_str());
			g_FontMgr.PrintText( iXPos + 32, iYPos + 44, FONT_SIZE_12, strTmp);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
SlotWnd::SlotWnd()
{
	m_pEmptyImg = NULL;
	m_pItemBackImg = NULL;

	m_pEmptyFrm = NULL;
	m_pPieceFrm = NULL;
	m_pAdditiveFrm = NULL;

	m_pItemIcon = NULL;
	m_pSubIcon = NULL;

	m_dwItemCode = ioEtcItem::EIT_NONE;

	m_pDragItem = new ioInvenDragItem(SDI_INVEN_ITEM);
}

SlotWnd::~SlotWnd()
{
	SAFEDELETE(m_pEmptyImg);
	SAFEDELETE(m_pItemBackImg);

	SAFEDELETE(m_pEmptyFrm);
	SAFEDELETE(m_pPieceFrm);
	SAFEDELETE(m_pAdditiveFrm);

	SAFEDELETE(m_pItemIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pDragItem);
}

void SlotWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void SlotWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "empty_frm" )
		{
			SAFEDELETE( m_pEmptyFrm );
			m_pEmptyFrm = pFrame;
		}
		else if( szType == "piece_frm" )
		{
			SAFEDELETE( m_pPieceFrm );
			m_pPieceFrm = pFrame;
		}
		else if( szType == "additive_frm" )
		{
			SAFEDELETE( m_pAdditiveFrm );
			m_pAdditiveFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void SlotWnd::InitItem(const ioEtcItem *pItem)
{
	SAFEDELETE(m_pItemIcon);
	SAFEDELETE(m_pSubIcon);

	if(!pItem) return;

	m_strIcon = pItem->GetIconName();
	m_sSubIcon = pItem->GetSubIconName();
	DWORD dwType = pItem->GetType();
	
	if(COMPARE(dwType, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF64+1))
	{
		ioEtcItemConsumption * pEtcItem = (ioEtcItemConsumption*) pItem;

		m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName(m_strIcon);
		m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName(m_sSubIcon);

		SAFEDELETE(m_pItemBackImg);
		m_pItemBackImg = g_UIImageSetMgr.CreateImageByFullName(pEtcItem->GetIconBackImg());
		m_dwItemCode = pItem->GetType();
	}
	else
	{
		if(dwType == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
		{
			ioEtcItemRevive * pEtcItem = (ioEtcItemRevive*) pItem;

			m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName(m_strIcon);
			m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName(m_sSubIcon);

			SAFEDELETE(m_pItemBackImg);
			m_pItemBackImg = g_UIImageSetMgr.CreateImageByFullName(pEtcItem->GetIconBackImg());
			m_dwItemCode = pItem->GetType();
		}
	}

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;

	m_nItem = 0;

	if(pUserEtcItem->GetEtcItem( dwType, kSlot ) )
		m_nItem = kSlot.m_iValue1;

	m_bActive = true;

	for(int i = 0; i < ioSlotMgr::MAX_SLOT; i++)
	{
		if( (g_SlotMgr.GetSlotItemType(i) == dwType) || (m_nItem == 0) )
		{
			m_bActive = false;
			break;
		}
	}
}

void SlotWnd::iwm_mouseleave( const ioMouse& mouse )
{
	if(m_pItemIcon && m_bClicked)
	{
		POINT ptOffset = { 0, 0};
		if(m_bActive && m_pDragItem)
		{
			m_pDragItem->SetItemData(GetID(), m_dwItemCode);
			m_pDragItem->SetDragImageName(m_strIcon);
			m_pDragItem->SetDragSubImageName(m_sSubIcon);
			m_pDragItem->DoDragDrop(GetParent(), m_rcPos, ptOffset);
		}
	}

	ioButton::iwm_mouseleave( mouse );
}

void SlotWnd::OnRender()
{
	enum 
	{
		CENTER_X	= 32,
		CENTER_Y	= 32,
	};

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( (m_pItemIcon) && (m_dwItemCode != ioEtcItem::EIT_NONE) )
	{
		UIRenderType eType = UI_RENDER_NORMAL;

		if(m_bActive)
		{
			if( m_pPieceFrm )
				m_pPieceFrm->Render( iXPos, iYPos );
		}
		else
		{
			if( m_pDisableFrm )
				m_pDisableFrm->Render( iXPos, iYPos );

			eType = UI_RENDER_GRAY;
		}

		
		if( m_pItemBackImg )
		{
			m_pItemBackImg->SetScale( 0.75f );
			m_pItemBackImg->Render( iXPos+CENTER_X, iYPos+CENTER_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}

		m_pItemIcon->SetScale( 0.75f );
		m_pItemIcon->Render( iXPos+CENTER_X, iYPos+CENTER_Y, eType, TFO_BILINEAR );

		if( m_pSubIcon )
		{
			m_pSubIcon->SetScale( 0.75f );
			m_pSubIcon->Render( iXPos+CENTER_X, iYPos+CENTER_Y, eType, TFO_BILINEAR );
		}
		g_FontMgr.SetAlignType(TAT_RIGHT);
		g_FontMgr.SetTextStyle(TS_OUTLINE_FULL_3X );
		g_FontMgr.SetBkColor(58, 58, 58);
		g_FontMgr.SetTextColor(TCT_DEFAULT_WHITE);
		g_FontMgr.PrintText( iXPos + 61, iYPos + 45, FONT_SIZE_12, "%d", m_nItem );

		if( IsOver() )
			OnDrawOvered( iXPos, iYPos );
	}
	else
	{
		if( m_pEmptyFrm )
			m_pEmptyFrm->Render( iXPos, iYPos );

		if( m_pEmptyImg )
		{
			m_pEmptyImg->SetScale( 0.75f );
			m_pEmptyImg->Render( iXPos+CENTER_X, iYPos+CENTER_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}