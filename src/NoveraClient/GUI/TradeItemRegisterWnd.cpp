#include "stdafx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3dengine/ioUITitle.h"
#include "../io3DEngine/ioEdit.h"

#include "../WndID.h"
#include "../Setting.h"
#include "../ioMyInfo.h"
#include "../ioMyLevelMgr.h"
#include "../EtcHelpFunc.h"
#include "../TextColorTable.h"
#include "../ioItem.h"
#include "../ioItemMaker.h"
#include "../ioUserExtraItem.h"
#include "../ioExtraItemInfoManager.h"
#include "../NetworkWrappingFunc.h"
#include "../ioPresentMgr.h"
#include "../TradeInfoManager.h"

#include "..\Local\ioLocalManager.h"
#include "..\Local\ioLocalParent.h"

#include "ioUI3DRender.h"
#include "ioSP2GUIManager.h"
#include "GlobalWnd.h"

#include "TradeItemRegisterWnd.h"

#include <strsafe.h>
#include "../Local/ioLocalManager.h"


/////////////////////////////////////////////////////////////////////////////////

TradeItemRegisterConfirmWnd::TradeItemRegisterConfirmWnd()
{
	m_iExtraItemIndex = 0;
	m_iPrice = 0;
	m_iTexPrice = 0;

	m_pEditFrm = NULL;
	m_pTexFrm = NULL;

	ClearDesc();
}

TradeItemRegisterConfirmWnd::~TradeItemRegisterConfirmWnd()
{
	SAFEDELETE(m_pEditFrm);
	SAFEDELETE(m_pTexFrm);
}

void TradeItemRegisterConfirmWnd::ClearDesc()
{
	for(int i = 0; i < MAX_DESC; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void TradeItemRegisterConfirmWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "PriceFrm" )
	{
		SAFEDELETE( m_pEditFrm );
		m_pEditFrm = pFrame;
	}
	else if( szType == "TexFrm" )
	{
		SAFEDELETE( m_pTexFrm );
		m_pTexFrm = pFrame;
	}
	else
	{
		ioMovingWnd::AddRenderFrame( szType, pFrame );
	}
}

void TradeItemRegisterConfirmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioMovingWnd::AddRenderImage( szType, pImage );	
}

void TradeItemRegisterConfirmWnd::SetRegisterItemInfo( int iIndex )
{
	m_iExtraItemIndex = 0;
	m_iTexPrice = 0;
	m_iPrice = 0;

	ClearDesc();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		HideWnd();
		return;
	}

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( iIndex, kSlot ) )
	{
		HideWnd();
		return;
	}

	m_iExtraItemIndex = iIndex;

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_17, STR(1), kSlot.m_Name.c_str() );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_17, STR(2) );

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
		m_szDescArray[2].AddTextPiece( FONT_SIZE_13, STR(3), iLimitLevel, g_MyInfo.GetClassName( iEnableClass, iPowerUpItemGrade ) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_13, STR(4), iLimitLevel );

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
	TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
	if( pIconBtn )
	{
		pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kSlot.m_iReinforce );
		if( pItem )
			pIconBtn->SetGradeType( pItem->GetGradeType() );
		pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

		if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
			pIconBtn->SetCustomItem( true );
		else
			pIconBtn->SetCustomItem( false );
	}

	SetPriceEditFocus();

	if( !IsShow() )
		ShowWnd();
}

void TradeItemRegisterConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_REGISTER_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_iPrice = 0;
			ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PRICE_EDIT );
			if( pEdit )
			{
				if( strlen( pEdit->GetText() ) > 0 )
				{
					m_iPrice = atoi( pEdit->GetText() );
					m_iPrice *= 10000;
				}

				__int64 iLimitPeso = g_TradeInfoMgr.GetRegisterLimitPeso();
				if( m_iPrice < iLimitPeso )
				{
					char szPeso[MAX_PATH]="";
					Help::ConvertNumToStrComma( iLimitPeso, szPeso, sizeof( szPeso ) );
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szPeso );
					return;
				}

				pEdit->KillKeyFocus();

				TradeItemRegisterWnd *pRegisterWnd = dynamic_cast<TradeItemRegisterWnd*>(g_GUIMgr.FindWnd(TRADE_REGISTER_WND));
				if( pRegisterWnd )
				{
					pRegisterWnd->SendTradeItemRegister( m_iExtraItemIndex, m_iPrice );
				}

				HideWnd();
			}
		}
		break;
	case ID_CANCEL_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TradeItemRegisterConfirmWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		MAIN_TEXT_X		= 103,
		MAIN_TEXT_Y		= 149,

		PRICE_TEXT_X	= 14,
		PRICE_TEXT_Y	= 219,

		PRICE_X			= 189,
		PRICE_Y			= 221,

		ETC_TEXT_X		= 103,
		ETC_TEXT_Y		= 269,
	};

	if( !m_szDescArray[0].IsEmpty() )
		m_szDescArray[0].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y, TAT_CENTER );

	if( !m_szDescArray[1].IsEmpty() )
		m_szDescArray[1].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y+21, TAT_CENTER );

	if( !m_szDescArray[2].IsEmpty() )
		m_szDescArray[2].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y+41, TAT_CENTER );

	if( m_pEditFrm )
		m_pEditFrm->Render( iXPos+102, iYPos+214 );

	if( m_pTexFrm )
		m_pTexFrm->Render( iXPos+102, iYPos+238, UI_RENDER_MULTIPLY );

	int iTempPrice = 0;

	m_iPrice = 0;
	m_iTexPrice = 0;
	m_fTexRate = 0;

	if( g_MyInfo.GetPCRoomAuthority()  && !g_MyInfo.IsUserEvent())
		m_fTexRate = FLOAT100 * g_TradeInfoMgr.GetPCRoomRegisterTexRate();
	else
		m_fTexRate = FLOAT100 * g_TradeInfoMgr.GetRegisterTexRate();

	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PRICE_EDIT );
	if( pEdit )
	{
		if( strlen( pEdit->GetText() ) > 0 )
		{
			iTempPrice = atoi( pEdit->GetText() );
			m_iPrice = iTempPrice * 10000;

			if( g_MyInfo.GetPCRoomAuthority() && !g_MyInfo.IsUserEvent() )
				m_iTexPrice = m_iPrice * g_TradeInfoMgr.GetPCRoomRegisterTexRate();
			else
				m_iTexPrice = m_iPrice * g_TradeInfoMgr.GetRegisterTexRate();
		}
	}

	char szPeso[MAX_PATH]="";

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );
	kPrinter.PrintFullText( iXPos+PRICE_TEXT_X, iYPos+PRICE_TEXT_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "수수료(%.2f％)", m_fTexRate );
	kPrinter.PrintFullText( iXPos+PRICE_TEXT_X, iYPos+PRICE_TEXT_Y+24, TAT_LEFT );
	kPrinter.ClearList();

	Help::ConvertNumToStrComma( iTempPrice, szPeso, sizeof( szPeso ) );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), szPeso );
	kPrinter.PrintFullText( iXPos+PRICE_X, iYPos+PRICE_Y, TAT_RIGHT );
	kPrinter.ClearList();

	Help::ConvertNumToStrComma( m_iTexPrice, szPeso, sizeof( szPeso ) );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(4), szPeso );
	kPrinter.PrintFullText( iXPos+PRICE_X, iYPos+PRICE_Y+24, TAT_RIGHT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(6) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(7) );
	kPrinter.PrintFullText( iXPos+ETC_TEXT_X, iYPos+ETC_TEXT_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(8) );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(9), g_TradeInfoMgr.GetTradePeriod() );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(10) );
	kPrinter.PrintFullText( iXPos+ETC_TEXT_X, iYPos+ETC_TEXT_Y+18, TAT_CENTER );
	kPrinter.ClearList();
}

void TradeItemRegisterConfirmWnd::SetPriceEditFocus()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_PRICE_EDIT );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->SetKeyFocus();
	}
}

//////////////////////////////////////////////////////////////////////////

TradeItemRegisterResultWnd::TradeItemRegisterResultWnd()
{

}

TradeItemRegisterResultWnd::~TradeItemRegisterResultWnd()
{
}

void TradeItemRegisterResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioMovingWnd::AddRenderImage( szType, pImage );
}

void TradeItemRegisterResultWnd::SetRegisterResultInfo( DWORD dwItemType, DWORD dwMagicCode, DWORD dwValue, DWORD dwMaleCustom, DWORD dwFemaleCustom, __int64 iPrice )
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( dwMagicCode, __FUNCTION__ );
	if( !pItem )
	{
		return;
	}

	m_szName = pItem->GetName();
	m_iPrice = iPrice;

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
	TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
	if( pIconBtn )
	{
		pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, dwValue );
		if( pItem )
			pIconBtn->SetGradeType( pItem->GetGradeType() );
		pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

		if( dwMaleCustom > 0 || dwFemaleCustom > 0 )
			pIconBtn->SetCustomItem( true );
		else
			pIconBtn->SetCustomItem( false );
	}

	if( !IsShow() )
		ShowWnd();
}

void TradeItemRegisterResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_OK_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TradeItemRegisterResultWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		NAME_X		= 103,
		NAME_Y		= 149,

		TEXT_X		= 103,
		TEXT_Y		= 199,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(1), m_szName.c_str() );
	kPrinter.PrintFullText( iXPos+NAME_X, iYPos+NAME_Y, TAT_CENTER );
	kPrinter.ClearList();

	char szPeso[MAX_PATH]="";
	Help::ConvertNumToStrComma( m_iPrice, szPeso, sizeof( szPeso ) );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(2), szPeso );
	kPrinter.PrintFullText( iXPos+NAME_X, iYPos+NAME_Y+21, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+20, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+40, TAT_CENTER );
	kPrinter.ClearList();
}


//////////////////////////////////////////////////////////////////////////

TradeItemRegisterWnd::TradeItemRegisterWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

TradeItemRegisterWnd::~TradeItemRegisterWnd()
{
	m_vInventoryItemInfoList.clear();
}

void TradeItemRegisterWnd::iwm_create()
{
}

void TradeItemRegisterWnd::iwm_show()
{
	ChangeTab( ID_TAB_1_BTN );

	if( g_MyInfo.GetGradeLevel() < g_TradeInfoMgr.GetTradeStateChangeLevel() )
	{
		HideWnd();

		char szName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( g_TradeInfoMgr.GetRegisterLimitLv(), szName, sizeof( szName ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szName );
		return;
	}

	if( m_vInventoryItemInfoList.empty() )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
}

void TradeItemRegisterWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
}

void TradeItemRegisterWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void TradeItemRegisterWnd::iwm_wheel( int zDelta )
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

void TradeItemRegisterWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
		if( cmd == IOBN_BTNUP )
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
			if( param == TradeItemInfoWnd::ID_ACTION_BTN )
			{
				int iSlotIndex = 0;
				TradeItemInfoWnd *pInfoWnd = dynamic_cast<TradeItemInfoWnd*>(FindChildWnd(dwID));
				if( pInfoWnd )
				{
					iSlotIndex = pInfoWnd->GetExtraItemIndex();
				}

				// 세부등록창
				TradeItemRegisterConfirmWnd *pConfirmWnd = dynamic_cast<TradeItemRegisterConfirmWnd*>(g_GUIMgr.FindWnd(TRADE_REGISTER_CONFIRM_WND));
				if( pConfirmWnd )
				{
					pConfirmWnd->SetRegisterItemInfo( iSlotIndex );
				}
			}
		}
		break;
	}
}

void TradeItemRegisterWnd::RegisterWndShow()
{
	if( !IsShow() )
	{
		ShowWnd();
	}
	else
	{
		ChangeTab( ID_TAB_1_BTN );

		if( m_vInventoryItemInfoList.empty() )
		{
			HideWnd();
			return;
		}
	}
}

void TradeItemRegisterWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vInventoryItemInfoList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		TradeItemInfoWnd *pItem = dynamic_cast<TradeItemInfoWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
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

void TradeItemRegisterWnd::ChangeTab( DWORD dwID )
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

void TradeItemRegisterWnd::SetExtraItemInfoVec( DWORD dwTabID )
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

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		if( kSlot.m_iTradeState != EET_ENABLE )
			continue;

		int iGroup = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
		if( !IsRightTab( dwTabID, iGroup ) )
			continue;

		if( kSlot.m_bCharEquip )
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

void TradeItemRegisterWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		TEXT_X		= 17,
		TEXT_Y		= 474,

		LINE_GAP	= 18,
	};

	int iRate = 0;
	if( g_MyInfo.GetPCRoomAuthority() && !g_MyInfo.IsUserEvent() )
		iRate = (int)(g_TradeInfoMgr.GetPCRoomRegisterTexRate() * FLOAT100);
	else
		iRate = (int)(g_TradeInfoMgr.GetRegisterTexRate() * FLOAT100);

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), g_TradeInfoMgr.GetTradePeriod() );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3), iRate );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+18, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+36, TAT_LEFT );
	kPrinter.ClearList();
}

void TradeItemRegisterWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessCustomBtnTooltip();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void TradeItemRegisterWnd::ProcessCustomBtnTooltip()
{
	int i = 0;
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for(i = 0;i < MAX_ITEM_INFO_WND_BTN;i++)
	{
		TradeItemInfoWnd *pItemWnd = dynamic_cast<TradeItemInfoWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN+i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		InventoryItemInfo kItemInfo = pItemWnd->GetInfo();

		ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pUserExtraItem ) continue;

		pUserExtraItem->GetExtraItem( kItemInfo.m_iMagicCode, kSlot );		
		break;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1) );

		if( kSlot.m_dwMaleCustom > 0 && kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(2) );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, ", " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(3) );
		}		
		else if( kSlot.m_dwMaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(4) );
		}
		else if( kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(5) );
		}
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(6) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(7) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(8) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(9) );
	}

	ItemCustomThumbnailTooltip *pTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( TRADE_ITEM_REG_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter, kSlot );
	}
}

int TradeItemRegisterWnd::GetTabType( DWORD dwTabID )
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

bool TradeItemRegisterWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

bool TradeItemRegisterWnd::GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( !COMPARE( dwItemInfoID, ID_ITEM_INFO_WND_1_BTN, ID_ITEM_INFO_WND_8_BTN+1) )
		return false;

	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	int iCurArray   = iStartArray + ( dwItemInfoID - ID_ITEM_INFO_WND_1_BTN );
	int iSize       = m_vInventoryItemInfoList.size();
	if( !COMPARE( iCurArray, 0, iSize ) )
		return false;

	rkInfo = m_vInventoryItemInfoList[iCurArray];
	return true;
}

void TradeItemRegisterWnd::SendTradeItemRegister( int iIndex, __int64 iPrice )
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem ) return;

	EXTRAITEMSLOT kSlot;
	if( !pItem->GetExtraItem( iIndex, kSlot ) )
		return;

	SP2Packet kPacket( CTPK_TRADE_CREATE );
	kPacket << PRESENT_EXTRAITEM;
	kPacket << iIndex;
	kPacket << iPrice;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}