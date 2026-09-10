
#include "stdafx.h"

#include "../ioDateHelp.h"
#include "../ioNoviceGradeBenefitManager.h"

#include "GlobalWnd.h"
#include "TradeItemRegisterWnd.h"
#include "ItemCustomWnd.h"
#include "TradeMainWnd.h"
#include "NamedTitleBtn.h"

TradeItemBtn::TradeItemBtn()
{
	m_pEmptyImg = NULL;
	m_nGradeType = 0;
}

TradeItemBtn::~TradeItemBtn()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void TradeItemBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ICON_BTN:
		if( cmd == IOBN_BTNUP )
		{
		}
		break;
	case ID_CUSTOM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ItemCustomGlobalDetailViewWnd *pDetailViewWnd = dynamic_cast< ItemCustomGlobalDetailViewWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_GLOBAL_DETAIL_VIEW_WND ) );
			if( pDetailViewWnd )
			{
				pDetailViewWnd->CreateCharacter( m_ItemInfo.m_dwItemMagicCode, m_ItemInfo.m_dwItemMaleCustom, m_ItemInfo.m_dwItemFemaleCustom );
			}
		}
		break;
	}
}

void TradeItemBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void TradeItemBtn::SetBlank()
{
	m_ItemInfo.Init();

	SetInActive();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_CUSTOM_BTN );

	ClearDesc();

	m_nGradeType = 0;
}

void TradeItemBtn::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void TradeItemBtn::OnRender()
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
			m_pEmptyImg->SetScale( 0.6f, 0.6f );
			m_pEmptyImg->Render( iXPos+37, iYPos+29, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
	else
		ioButton::OnRender();

	RenderDesc( iXPos, iYPos );
}

void TradeItemBtn::SetIsWidthCutDesc( int iArray , bool bWidthCut )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		m_bWidthCutDescArray[iArray] = bWidthCut;
}

bool TradeItemBtn::IsWidthCutDesc( int iArray )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		return m_bWidthCutDescArray[iArray];

	return false;
}

bool TradeItemBtn::IsCustomToolTip()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CUSTOM_BTN ) )
		return true;
	return false;
}

void TradeItemBtn::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET	= 37,
		Y_OFFSET	= 53,
		Y_LINE_GAP	= 12,
	};

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			m_szDescArray[i].ClearList();
		}

		m_szDescArray[0].SetTextStyle( TS_NORMAL );
		m_szDescArray[0].SetBkColor( 0, 0, 0 );	
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_GRAY2 );
		m_szDescArray[0].AddTextPiece( FONT_SIZE_11, STR(1) );
		m_szDescArray[0].PrintFullTextWidthCut( iXPos+X_OFFSET, iYPos+Y_OFFSET, TAT_CENTER, 71 );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY2 );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos+X_OFFSET, iYPos+Y_OFFSET+Y_LINE_GAP, TAT_CENTER, 71 );
	}
	else
	{
		for( int i=0; i < MAX_DESC; ++i )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			m_szDescArray[i].PrintFullTextWidthCut( iXPos+X_OFFSET, iYPos+Y_OFFSET+(Y_LINE_GAP * i), TAT_CENTER, 71 );
		}
	}
}

void TradeItemBtn::SetInfo( TradeItemInfo &rkInfo )
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

TradeItemInfo& TradeItemBtn::GetInfo()
{
	return m_ItemInfo;
}

bool TradeItemBtn::SetExtraItemInfo()
{
	if( m_ItemInfo.m_dwItemMagicCode <= 0 )
		return false;

	ioHashString szItemName, szIconName;
	const ioItem *pItem = g_ItemMaker.GetItemConst( m_ItemInfo.m_dwItemMagicCode, __FUNCTION__ );
	if( pItem )
	{
		szItemName = pItem->GetName();
		szIconName = pItem->GetItemLargeIconName();
		m_nGradeType = pItem->GetGradeType();
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Not Find Item: %d", __FUNCTION__, m_ItemInfo.m_dwItemMagicCode );
		return false;
	}

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		pBtn->SetIcon( pImg, 0.6f, 0.6f, m_ItemInfo.m_dwItemValue, InventoryIconBtn::NT_SMALL );
		pBtn->SetGradeType( m_nGradeType );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}

	if( m_ItemInfo.m_dwItemMaleCustom > 0 || m_ItemInfo.m_dwItemFemaleCustom > 0 )
	{
		ShowChildWnd( ID_CUSTOM_BTN );
	}

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_11, STR(1), szItemName.c_str() );

	// Price
	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_ItemInfo.m_iItemPrice, szConvertNum, sizeof( szConvertNum ) );
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_11, STR(2), szConvertNum );

	return true;
}

void TradeItemBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}

////////////////////////////////////////////////////////////////////////////////

TradeItemBuyWnd::TradeItemBuyWnd()
{

	ClearDesc();
}

TradeItemBuyWnd::~TradeItemBuyWnd()
{

}

void TradeItemBuyWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void TradeItemBuyWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioMovingWnd::AddRenderFrame( szType, pFrame );
}

void TradeItemBuyWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioMovingWnd::AddRenderImage( szType, pImage );	
}

void TradeItemBuyWnd::SetTradItemBuyInfo( TradeItemInfo &rkInfo )
{
	ClearDesc();

	if( g_MyInfo.GetGradeLevel() < g_TradeInfoMgr.GetItemBuyLimitLv() )
	{
		HideWnd();

		char szName[MAX_PATH] = "";
		g_LevelMgr.GetGradeName( g_TradeInfoMgr.GetItemBuyLimitLv(), szName, sizeof( szName ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szName );
		return;
	}

	if( g_MyInfo.IsExtraItemFull( true ) )
	{
		HideWnd();
		return;
	}

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		HideWnd();
		return;
	}

	m_ItemInfo.Init();
	const ioItem *pItem = g_ItemMaker.GetItemConst( rkInfo.m_dwItemMagicCode, __FUNCTION__ );
	if( !pItem )
	{
		return;
	}

	m_ItemInfo = rkInfo;

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_17, STR(2), pItem->GetName().c_str() );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_17, STR(3) );

	// Limit
	int iLimitLevel = m_ItemInfo.m_dwItemValue * g_ExtraItemInfoMgr.GetLevelLimitConst();
	int iEnableClass = pItem->GetEnableClass();
	int iPowerUpItemGrade = ConvertPowerUpItemToItemGrade( rkInfo.m_dwItemMagicCode );

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( iEnableClass > 0 )
		m_szDescArray[2].AddTextPiece( FONT_SIZE_13, STR(4), iLimitLevel, g_MyInfo.GetClassName( iEnableClass, iPowerUpItemGrade ) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_13, STR(5), iLimitLevel );
	
	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
	TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
	if( pIconBtn )
	{
		pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, m_ItemInfo.m_dwItemValue );
		pIconBtn->SetGradeType( pItem->GetGradeType() );
		pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

		if( rkInfo.m_dwItemMaleCustom > 0 || rkInfo.m_dwItemMaleCustom > 0 )
			pIconBtn->SetCustomItem( true );
		else
			pIconBtn->SetCustomItem( false );
	}

	if( !IsShow() )
		ShowWnd();
}

void TradeItemBuyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_TRADE_ITEM );
			kPacket << m_ItemInfo.m_dwTradeIndex;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );

			HideWnd();
		}
		break;
	case ID_EXIT_BTN:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TradeItemBuyWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		MAIN_TEXT_X		= 103,
		MAIN_TEXT_Y		= 149,

		PRICE_TEXT_X	= 17,
		PRICE_TEXT_Y	= 219,

		PRICE_X			= 191,
		PRICE_Y			= 219,
	};

	if( !m_szDescArray[0].IsEmpty() )
		m_szDescArray[0].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y, TAT_CENTER );

	if( !m_szDescArray[1].IsEmpty() )
		m_szDescArray[1].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y+21, TAT_CENTER );

	if( !m_szDescArray[2].IsEmpty() )
		m_szDescArray[2].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y+41, TAT_CENTER );


	char szPeso[MAX_PATH]="";

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );
	kPrinter.PrintFullText( iXPos+PRICE_TEXT_X, iYPos+PRICE_TEXT_Y, TAT_LEFT );
	kPrinter.ClearList();

	float fTexRate;
	if( g_MyInfo.GetPCRoomAuthority() && !g_MyInfo.IsUserEvent() )
		fTexRate = FLOAT100 * g_TradeInfoMgr.GetPCRoomBuyTexRate();
	else
		fTexRate = FLOAT100 * g_TradeInfoMgr.GetBuyTexRate();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "수수료(%.2f％)", fTexRate );
	kPrinter.PrintFullText( iXPos+PRICE_TEXT_X, iYPos+PRICE_TEXT_Y+21, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(3) );
	kPrinter.PrintFullText( iXPos+PRICE_TEXT_X, iYPos+PRICE_TEXT_Y+42, TAT_LEFT );
	kPrinter.ClearList();

	Help::ConvertNumToStrComma( m_ItemInfo.m_iItemPrice, szPeso, sizeof( szPeso ) );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(4), szPeso );
	kPrinter.PrintFullText( iXPos+PRICE_X, iYPos+PRICE_Y, TAT_RIGHT );
	kPrinter.ClearList();

	__int64 iTexPrice;
	if( g_MyInfo.GetPCRoomAuthority() && !g_MyInfo.IsUserEvent() )
		iTexPrice = m_ItemInfo.m_iItemPrice * g_TradeInfoMgr.GetPCRoomBuyTexRate();
	else
		iTexPrice = m_ItemInfo.m_iItemPrice * g_TradeInfoMgr.GetBuyTexRate();

	__int64 iTotalPrice = m_ItemInfo.m_iItemPrice + iTexPrice;

	Help::ConvertNumToStrComma( iTexPrice, szPeso, sizeof( szPeso ) );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(5), szPeso );
	kPrinter.PrintFullText( iXPos+PRICE_X, iYPos+PRICE_Y+21, TAT_RIGHT );
	kPrinter.ClearList();

	Help::ConvertNumToStrComma( iTotalPrice, szPeso, sizeof( szPeso ) );
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(6), szPeso );
	kPrinter.PrintFullText( iXPos+PRICE_X, iYPos+PRICE_Y+42, TAT_RIGHT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(9) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN);
	kPrinter.AddTextPiece( FONT_SIZE_14, STR(10), m_ItemInfo.m_szRegisterUserNick.c_str() );
	kPrinter.PrintFullTextWidthCut( iXPos+102, iYPos+PRICE_TEXT_Y+71, TAT_CENTER, 173 );	
	kPrinter.ClearList();

	// 마감
	enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };

	int iYear = m_ItemInfo.m_dwRegisterStartDate / 10000;
	int iMonth = ( m_ItemInfo.m_dwRegisterStartDate/100 ) % 100;
	int iDay = m_ItemInfo.m_dwRegisterStartDate % 100;
	int iHour = m_ItemInfo.m_dwRegisterEndDate / 10000;
	int iMin = ( m_ItemInfo.m_dwRegisterEndDate/100 ) % 100;
	int iSec = m_ItemInfo.m_dwRegisterEndDate % 100;

	time_t kLimitTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMin, iSec );
	kLimitTime += ( m_ItemInfo.m_dwPeriodTime * DAY_SEC );
	time_t kCurServerTime = g_MyInfo.GetServerDate();

	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	int iGapSec = kLimitTime - kCurServerTime;

	// 남은 시간
	char szTime[MAX_PATH] = "";
	ZeroMemory( szTime, sizeof( szTime ) );

	int iGapDay    = iGapSec/DAY_SEC;
	int iGapHourSec= iGapSec-(iGapDay*DAY_SEC);
	int iGapHour   = iGapHourSec / HOUR_SEC;
	int iGapMinute = ( iGapHourSec-(iGapHour*HOUR_SEC) ) / MINUTE_SEC;
	int iTotalMin = iGapSec / MINUTE_SEC;

	SafeSprintf( szTime, sizeof( szTime ), STR(7), iGapDay, iGapHour, iGapMinute );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8), szTime );
	kPrinter.PrintFullText( iXPos+103, iYPos+367, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

TradeItemBuyResultWnd::TradeItemBuyResultWnd()
{

}

TradeItemBuyResultWnd::~TradeItemBuyResultWnd()
{
}

void TradeItemBuyResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioMovingWnd::AddRenderImage( szType, pImage );
}

void TradeItemBuyResultWnd::SetBuyResultInfo( DWORD dwItemType, DWORD dwMagicCode, DWORD dwValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iPrice )
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
		pIconBtn->SetGradeType( pItem->GetGradeType() );
		pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

		if( dwItemMaleCustom > 0 || dwItemFemaleCustom > 0 )
			pIconBtn->SetCustomItem( true );
		else
			pIconBtn->SetCustomItem( false );
	}

	if( !IsShow() )
		ShowWnd();
}

void TradeItemBuyResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ITEM_GO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowPresentTabDirect();
		}
		break;
	case ID_OK_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TradeItemBuyResultWnd::OnRender()
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

TradeMainWnd::TradeMainWnd()
{
	m_iCurPage			= 1;
	m_iMaxPage			= 1;
	m_eGradeType		= TIT_NONE;
	m_pPreEdit			= NULL;

	SetSortTypeList();
	SetGradeSortTypeList();
	SetPriceSortTypeList();
}

TradeMainWnd::~TradeMainWnd()
{
	m_vTradeItemInfoList.clear();
}

void TradeMainWnd::iwm_create()
{
}

void TradeMainWnd::iwm_show()
{
	m_iCurPage		= 1;
	m_iMaxPage		= 1;
	
	m_eCurrItemType	= 0;
	m_eGradeType	= 0;
	m_ePriceType	= 0;

	ChangeCategory( FindChildWnd( ID_SLOT_SORT_BTN ), ioItem::IT_NONE, m_vSlotSortTypeList, m_eCurrItemType );
	ChangeCategory( FindChildWnd( ID_GRADE_SORT_BTN ), TIT_NONE, m_vGradeSortTypeList, m_eGradeType );
	ChangeCategory( FindChildWnd( ID_PRICE_SORT_BTN ), PRICE_NONE, m_vPriceSortTypeList, m_ePriceType );
	
	OnRefreshTradeItemList();

	// ID_TRADE_ITEM_BTN1 ~ ID_TRADE_ITEM_BTN20 초기화
	InitTradeItemBtn();

	ShowChildWnd( ID_SEARCH_EDIT );
	DisableEditFocus();
}

void TradeMainWnd::iwm_hide()
{
	EnableEditFocus();
	DisableEditFocus();
}

void TradeMainWnd::EnableEditFocus()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_SEARCH_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{
		if( m_pPreEdit != pEdit )
			m_pPreEdit = ioEdit::m_pKeyFocusEdit;

		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		m_CurrFindKeyWord.Clear();
	}
}

void TradeMainWnd::DisableEditFocus()
{
	ioEdit *pCurEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_SEARCH_EDIT ) );
	if( pCurEdit && pCurEdit->IsShow() )
		pCurEdit->KillKeyFocus();

	ioEdit* pEdit = dynamic_cast<ioEdit*>( m_pPreEdit );
	if( pEdit )	
	{
		ioWnd *pParent = pEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
			{
				pEdit->SetKeyFocus();
			}
		}
	}

	m_pPreEdit = NULL;
}

void TradeMainWnd::ProcessKeyWord()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_SEARCH_EDIT ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		if( g_InputBox.IsEnter() )
		{
			m_CurrFindKeyWord = g_InputBox.GetString();
			iwm_command( FindChildWnd( ID_SEARCH_BTN ), IOBN_BTNUP, 0 );
		}
	}
}

void TradeMainWnd::CreatePullDown( ioWnd *pWnd, DWORD dwPullDownWndID, vNewShopPullDownItem& ItemList )
{
	NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>( FindChildWnd( dwPullDownWndID ) );
	if( pListWnd && pListWnd->IsShow() )
	{
		pListWnd->HideWnd();
	}
	else if( pListWnd )
	{
		pListWnd->CreatePullDown( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 24 );

		if( !ItemList.empty() )
		{
			int iSize = ItemList.size();
			for( int i = 0;i < iSize; i++ )
			{
				pListWnd->InsertItem( ItemList[i] );
			}
			pListWnd->ShowWnd();
		}
	}
}

void TradeMainWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_ITEM_REG_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			TradeItemRegisterWnd *pRegisterWnd = dynamic_cast<TradeItemRegisterWnd*>(g_GUIMgr.FindWnd(TRADE_REGISTER_WND));
			if( pRegisterWnd )
			{
				pRegisterWnd->RegisterWndShow();
			}
		}
		break;
	case ID_SLOT_SORT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			CreatePullDown( pWnd, ID_SLOT_SORT_WND, m_vSlotSortTypeList );
		}
		break;
	case ID_SLOT_SORT_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>( pWnd );
			if( pPullDownWnd )
			{
				if( ChangeCategory( pPullDownWnd->GetOpenBtn(), param, m_vSlotSortTypeList, m_eCurrItemType ) )
					OnRefreshTradeItemList();

				pPullDownWnd->HideWnd();
			}
		}
		break;
	case ID_GRADE_SORT_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>( pWnd );
			if( pPullDownWnd )
			{
				if( ChangeCategory( pPullDownWnd->GetOpenBtn(), param, m_vGradeSortTypeList, m_eGradeType ) )
					OnRefreshTradeItemList();

				pPullDownWnd->HideWnd();
			}
		}
		break;
	case ID_PRICE_SORT_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>( pWnd );
			if( pPullDownWnd )
			{
				if( ChangeCategory( pPullDownWnd->GetOpenBtn(), param, m_vPriceSortTypeList, m_ePriceType ) )
					OnRefreshTradeItemList();

				pPullDownWnd->HideWnd();
			}
		}
		break;
	case ID_GRADE_SORT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			CreatePullDown( pWnd, ID_GRADE_SORT_WND, m_vGradeSortTypeList );
		}
		break;
	case ID_PRICE_SORT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			CreatePullDown( pWnd, ID_PRICE_SORT_WND, m_vPriceSortTypeList );
		}
		break;
	case ID_SEARCH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_iCurPage		  = 1;
			m_iMaxPage		  = 1;
			m_CurrFindKeyWord = g_InputBox.GetString();
		}
		break;
	case ID_SEARCH_CLEAR_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_InputBox.Clear();
			m_CurrFindKeyWord.Clear();
		}
		break;
	case ID_REFRESH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			g_TradeInfoMgr.SyncTradeList( true );
		}
		break;
	case ID_TRADE_ITEM_BTN1:
	case ID_TRADE_ITEM_BTN2:
	case ID_TRADE_ITEM_BTN3:
	case ID_TRADE_ITEM_BTN4:
	case ID_TRADE_ITEM_BTN5:
	case ID_TRADE_ITEM_BTN6:
	case ID_TRADE_ITEM_BTN7:
	case ID_TRADE_ITEM_BTN8:
	case ID_TRADE_ITEM_BTN9:
	case ID_TRADE_ITEM_BTN10:
	case ID_TRADE_ITEM_BTN11:
	case ID_TRADE_ITEM_BTN12:
	case ID_TRADE_ITEM_BTN13:
	case ID_TRADE_ITEM_BTN14:
	case ID_TRADE_ITEM_BTN15:
	case ID_TRADE_ITEM_BTN16:
	case ID_TRADE_ITEM_BTN17:
	case ID_TRADE_ITEM_BTN18:
	case ID_TRADE_ITEM_BTN19:
	case ID_TRADE_ITEM_BTN20:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			TradeItemBtn *pBtn = dynamic_cast<TradeItemBtn*>( FindChildWnd(dwID) );
			if( pBtn )
			{
				TradeItemBuyWnd *pBuyWnd = dynamic_cast<TradeItemBuyWnd*>( g_GUIMgr.FindWnd(TRADE_ITEM_BUY_WND) );
				if( pBuyWnd )
				{
					pBuyWnd->SetTradItemBuyInfo( pBtn->GetInfo() );
				}
			}
		}
		break;
	case ID_PRE_PAGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			PrevPage();
		}
		break;
	case ID_NEXT_PAGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			DisableEditFocus();
			NextPage();
		}
		break;
	}
}

void TradeMainWnd::ProcessCustomBtnTooltip()
{
	int i = 0;
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for(i = 0;i < MAX_TRADE_ITEM_LIST;i++)
	{
		TradeItemBtn *pItemWnd = dynamic_cast<TradeItemBtn*>(FindChildWnd( ID_TRADE_ITEM_BTN1+i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		TradeItemInfo kItemInfo = pItemWnd->GetInfo();

		kSlot.m_iItemCode = kItemInfo.m_dwItemMagicCode;
		kSlot.m_iReinforce= kItemInfo.m_dwItemValue;
		kSlot.m_dwMaleCustom = kItemInfo.m_dwItemMaleCustom;
		kSlot.m_dwFemaleCustom = kItemInfo.m_dwItemFemaleCustom;
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

	ItemCustomThumbnailTooltip *pTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_THUMBNAIL_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter, kSlot );
	}
}

void TradeMainWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
	
	ProcessCustomBtnTooltip();
	ProcessKeyWord();
	UpdateTradeItemList();
}

void TradeMainWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iRendPage = min( m_iCurPage, m_iMaxPage );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 201, iYPos + 423, FONT_SIZE_12, STR(1), iRendPage );
}

void TradeMainWnd::OnRefreshTradeItemList()
{	
	g_TradeInfoMgr.SyncTradeList();
}

void TradeMainWnd::InitTradeItemBtn()
{
	for( int i=0; i < MAX_TRADE_ITEM_LIST; ++i )
	{
		TradeItemBtn *pBtn = dynamic_cast<TradeItemBtn*>(FindChildWnd(ID_TRADE_ITEM_BTN1+i));
		if( pBtn )
		{
			pBtn->SetBlank();
		}
	}
}

void TradeMainWnd::UpdatePage()
{
	int iSize = m_vTradeItemInfoList.size();

	for( int i=0; i < iSize; ++i )
	{
		TradeItemBtn *pBtn = dynamic_cast<TradeItemBtn*>(FindChildWnd(ID_TRADE_ITEM_BTN1+i));
		if( pBtn )
		{
			pBtn->SetInfo( m_vTradeItemInfoList[i] );
		}
	}
}

void TradeMainWnd::UpdateTradeItemList()
{
	m_vTradeItemInfoList.clear();

	// ID_TRADE_ITEM_BTN1 ~ ID_TRADE_ITEM_BTN20 초기화
	InitTradeItemBtn();
	
	TradeItemInfoList vList, vBuffer;
	g_TradeInfoMgr.GetTradeItemList( vList );

	// ALL/무/갑/투/무/망 분류
	if( COMPARE( m_eCurrItemType, ioItem::IT_WEAPON, ioItem::IT_OBJECT ) )
	{
		for( auto iter= vList.begin(); iter != vList.end(); ++iter )
		{
			const TradeItemInfo& Info = *iter;
			int iGroup = ( Info.m_dwItemMagicCode / 100000 ) + 1;
			if( iGroup == m_eCurrItemType )
			{
				vBuffer.push_back( Info );
			}
		}
		if( !vBuffer.empty() )
			vList = vBuffer;
		else
			vList.clear();
	}

	// ALL/고급/레어/유니크/아이돌
	if( COMPARE( m_eGradeType, TIT_NORMAL, TIT_IDOL + 1 ) )
	{
		vBuffer.clear();
		for( auto iter = vList.begin(); iter != vList.end(); ++iter )
		{
			const TradeItemInfo& Info = *iter;
			const ioItem* pItem = g_ItemMaker.GetItemConst( Info.m_dwItemMagicCode, __FUNCTION__ );

			//고급 분류는 아이템 등급 설정이 되어있지 않기 때문에 예외처리
			if( pItem )
			{
				if( pItem->GetGradeType() == TIT_NONE && m_eGradeType == TIT_PRIMIUM )
				{
					EquipItemExtendType eExtendType = GetEquipItemExtendType( pItem->GetItemCode() );
					if( eExtendType == EIET_BASIC_EXTREA || eExtendType == EIET_SPECIAL_EXTREA || eExtendType == EIET_EXTRA_POWERUP )
					{
						vBuffer.push_back( Info );
					}
				}
				else if( pItem->GetGradeType() == m_eGradeType )
				{
					vBuffer.push_back( Info );
				}
			}
		}

		if( !vBuffer.empty() )
			vList = vBuffer;
		else
			vList.clear();
	}
	
	//페소 오름차순/내림차순 정렬
	switch( m_ePriceType )
	{
	case PRICE_NONE:
		reverse( vList.begin(), vList.end() );
		break;
	case PRICE_UP:
		std::sort( vList.begin(), vList.end(), TradeHighGoldSort() );
		break;
	case PRICE_DOWN:
		std::sort( vList.begin(), vList.end(), TradeLowGoldSort() );
		break;
	}

	//키워드 검색
	if( !m_CurrFindKeyWord.IsEmpty() )
	{
		vBuffer.clear();
		for( auto iter = vList.begin(); iter != vList.end(); ++iter )
		{
			const TradeItemInfo& Info = *iter;

			const ioItem* pItem = g_ItemMaker.GetItemConst( Info.m_dwItemMagicCode, __FUNCTION__ );
			if( pItem )
			{
				//소문자 변환
				std::string szName = pItem->GetName().c_str();
				std::transform( szName.begin(), szName.end(), szName.begin(), tolower );

				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal && pLocal->GetType() == ioLocalManager::LCT_KOREA )
				{
					//조합형 글자 비교(초성 비교 초함)
					if( !Help::Strstr( szName.c_str(), m_CurrFindKeyWord.c_str() ) )
						continue;
				}
				else
				{
					if( strstr( szName.c_str(), m_CurrFindKeyWord.c_str() ) == NULL )
						continue;
				}

				vBuffer.push_back( Info );
			}
		}

		if( !vBuffer.empty() )
			vList = vBuffer;
		else
			vList.clear();
	}

	if( vList.size() == 0 )
	{
		m_iCurPage = m_iMaxPage = 1;
	}
	else
	{
		m_iMaxPage = (int)vList.size() / MAX_TRADE_ITEM_LIST;
		if( (int)vList.size() % MAX_TRADE_ITEM_LIST != 0 )
			m_iMaxPage++;

		for( int i = 0; i < MAX_TRADE_ITEM_LIST; ++i )
		{
			int nIndex = (m_iCurPage-1) * MAX_TRADE_ITEM_LIST + i;
			if( nIndex < (int)vList.size() )
			{
				const TradeItemInfo& Info = vList[nIndex];
				m_vTradeItemInfoList.push_back( Info );
			}
		}

		UpdatePage();
	}
}

void TradeMainWnd::SetSortTypeList()
{
	m_vSlotSortTypeList.clear();

	char szName[MAX_PATH]="";

	// 전체
	NewShopPullDownItem kItem1;
	kItem1.m_iMagicCode = ioItem::IT_NONE;

	SafeSprintf( szName, sizeof( szName ),"전체 부위" );
	kItem1.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem1.m_szName      = szName;
	m_vSlotSortTypeList.push_back( kItem1 );

	// 무기
	NewShopPullDownItem kItem2;
	kItem2.m_iMagicCode = ioItem::IT_WEAPON;

	SafeSprintf( szName, sizeof( szName ), STR(2) );
	kItem2.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem2.m_szName      = szName;
	m_vSlotSortTypeList.push_back( kItem2 );

	// 갑옷
	NewShopPullDownItem kItem3;
	kItem3.m_iMagicCode = ioItem::IT_ARMOR;

	SafeSprintf( szName, sizeof( szName ), STR(3) );
	kItem3.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem3.m_szName      = szName;
	m_vSlotSortTypeList.push_back( kItem3 );

	// 투구
	NewShopPullDownItem kItem4;
	kItem4.m_iMagicCode = ioItem::IT_HELMET;

	SafeSprintf( szName, sizeof( szName ), STR(4) );
	kItem4.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem4.m_szName      = szName;
	m_vSlotSortTypeList.push_back( kItem4 );

	// 망토
	NewShopPullDownItem kItem5;
	kItem5.m_iMagicCode = ioItem::IT_CLOAK;

	SafeSprintf( szName, sizeof( szName ), STR(5) );
	kItem5.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem5.m_szName      = szName;
	m_vSlotSortTypeList.push_back( kItem5 );
}

void TradeMainWnd::SetGradeSortTypeList()
{
	m_vGradeSortTypeList.clear();

	char szName[MAX_PATH] = "";

	// 전체
	NewShopPullDownItem kItem1;
	kItem1.m_iMagicCode = TIT_NONE;
	SafeSprintf( szName, sizeof( szName ), "전체 등급" );
	kItem1.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem1.m_szName      = szName;
	m_vGradeSortTypeList.push_back( kItem1 );

	// 고급
	NewShopPullDownItem kItem2;
	kItem2.m_iMagicCode = TIT_PRIMIUM;
	SafeSprintf( szName, sizeof( szName ), "고급" );
	kItem2.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem2.m_szName      = szName;
	m_vGradeSortTypeList.push_back( kItem2 );

	// 레어
	NewShopPullDownItem kItem3;
	kItem3.m_iMagicCode = TIT_RARE;
	SafeSprintf( szName, sizeof( szName ), "레어" );
	kItem3.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem3.m_szName      = szName;
	m_vGradeSortTypeList.push_back( kItem3 );

	//유니크
	NewShopPullDownItem kItem4;
	kItem4.m_iMagicCode = TIT_UNIQUE;
	SafeSprintf( szName, sizeof( szName ), "유니크" );
	kItem4.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem4.m_szName      = szName;
	m_vGradeSortTypeList.push_back( kItem4 );

	//아이돌
	NewShopPullDownItem kItem5;
	kItem5.m_iMagicCode = TIT_IDOL;
	SafeSprintf( szName, sizeof( szName ), "아이돌" );
	kItem5.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem5.m_szName      = szName;
	m_vGradeSortTypeList.push_back( kItem5 );
}

void TradeMainWnd::SetPriceSortTypeList()
{
	m_vPriceSortTypeList.clear();

	char szName[MAX_PATH] = "";

	// 전체
	NewShopPullDownItem kItem1;
	kItem1.m_iMagicCode = PRICE_NONE;
	SafeSprintf( szName, sizeof( szName ), "전체 가격" );
	kItem1.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem1.m_szName      = szName;
	m_vPriceSortTypeList.push_back( kItem1 );

	// 고급
	NewShopPullDownItem kItem2;
	kItem2.m_iMagicCode = PRICE_UP;
	SafeSprintf( szName, sizeof( szName ), "높은 가격 순" );
	kItem2.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem2.m_szName      = szName;
	m_vPriceSortTypeList.push_back( kItem2 );

	// 레어
	NewShopPullDownItem kItem3;
	kItem3.m_iMagicCode = PRICE_DOWN;
	SafeSprintf( szName, sizeof( szName ), "낮은 가격 순" );
	kItem3.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
	kItem3.m_szName      = szName;
	m_vPriceSortTypeList.push_back( kItem3 );
}

bool TradeMainWnd::ChangeCategory( IN ioWnd *pWnd, IN int Param, IN vNewShopPullDownItem& ItemList, OUT int& eType )
{
	if( !pWnd )
		return false;

	if( !COMPARE( Param, 0, (int)ItemList.size() ) ) 
		return false;

	ioUITitle *pTitle =  pWnd->GetTitle();
	if( !pTitle )
		return false;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = ItemList[Param];
	if( szTitle == rkItem.m_szName ) 
		return false;

	pTitle->SetText( rkItem.m_szName.c_str(), "" );

	eType		= rkItem.m_iMagicCode;
	m_iCurPage	= 1;

	return true;
}

void TradeMainWnd::PrevPage()
{	
	if(  0 < m_iCurPage - 1 )
		m_iCurPage--;
}

void TradeMainWnd::NextPage()
{
	if( m_iCurPage +1 <= m_iMaxPage )
		m_iCurPage++;
}

//////////////////////////////////////////////////////////////////////////
LobbyTradeItemCustomBtn::LobbyTradeItemCustomBtn()
{
	m_pCustomImg = NULL;
}

LobbyTradeItemCustomBtn::~LobbyTradeItemCustomBtn()
{
	SAFEDELETE( m_pCustomImg );
}

void LobbyTradeItemCustomBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "custom_image" )
	{
		SAFEDELETE( m_pCustomImg );
		m_pCustomImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void LobbyTradeItemCustomBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pCustomImg )
	{
		m_pCustomImg->SetScale( FLOAT05 );
		m_pCustomImg->Render( iXPos - 10, iYPos - 8, UI_RENDER_NORMAL, TFO_BILINEAR );
		if( m_bOver || m_bClicked )
			m_pCustomImg->Render( iXPos - 10, iYPos - 8, UI_RENDER_SCREEN, TFO_BILINEAR );
	}
}

//////////////////////////////////////////////////////////////////////////

LobbyTradeItemBtn::LobbyTradeItemBtn()
{
	m_pEmptyImg = NULL;
	m_nGradeType = 0;
}

LobbyTradeItemBtn::~LobbyTradeItemBtn()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void LobbyTradeItemBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void LobbyTradeItemBtn::SetBlank()
{
	m_ItemInfo.Init();

	SetInActive();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_CUSTOM_BTN );

	ClearDesc();
	
	m_nGradeType = 0;
}

void LobbyTradeItemBtn::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void LobbyTradeItemBtn::OnRender()
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
			m_pEmptyImg->SetScale( 0.50f, 0.50f );
			m_pEmptyImg->Render( iXPos + 25, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
	else
		ioButton::OnRender();

	RenderDesc( iXPos, iYPos );
}

void LobbyTradeItemBtn::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET	= 48,
		Y_OFFSET	= 8,
		Y_LINE_GAP	= 14,
	};

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			m_szDescArray[i].ClearList();
		}

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY2 );
		g_FontMgr.PrintText( iXPos + 48, iYPos + 14, FONT_SIZE_11,STR(1) );
		g_FontMgr.PrintText( iXPos + 48, iYPos + 28, FONT_SIZE_11,STR(2) );
	}
	else
	{
		for( int i=0; i < MAX_DESC; ++i )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			m_szDescArray[i].PrintFullText( iXPos+X_OFFSET, iYPos+Y_OFFSET+(Y_LINE_GAP * i), TAT_LEFT );
		}
	}
}

void LobbyTradeItemBtn::SetTradeInfo( TradeItemInfo &rkInfo )
{
	SetBlank();
	SetActive();

	m_ItemInfo = rkInfo;

	if( m_ItemInfo.m_dwItemMagicCode <= 0 )
		return;

	ioHashString szItemName, szIconName;
	const ioItem *pItem = g_ItemMaker.GetItemConst( m_ItemInfo.m_dwItemMagicCode, __FUNCTION__ );
	if( pItem )
	{
		szItemName = pItem->GetName();
		szIconName = pItem->GetItemLargeIconName();
		m_nGradeType = pItem->GetGradeType();
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Not Find Item: %d", __FUNCTION__, m_ItemInfo.m_dwItemMagicCode );
		return;
	}

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIconName );
		pBtn->SetIcon( pImg, FLOAT05, FLOAT05 );
		pBtn->SetGradeType( m_nGradeType );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
	
	if( m_ItemInfo.m_dwItemMaleCustom > 0 || m_ItemInfo.m_dwItemFemaleCustom > 0 )
	{
		ShowChildWnd( ID_CUSTOM_BTN );
	}

	// desc
	ClearDesc();

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_11, STR(1), szItemName.c_str() );

	if( m_ItemInfo.m_dwItemValue > 0 )
	{
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_BLUE );
		m_szDescArray[0].AddTextPiece( FONT_SIZE_11, STR(2), m_ItemInfo.m_dwItemValue );
	}

	// Price
	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_ItemInfo.m_iItemPrice, szConvertNum, sizeof( szConvertNum ) );
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_11, STR(3), szConvertNum );

	// Period
	enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };

	int iYear = m_ItemInfo.m_dwRegisterStartDate / 10000;
	int iMonth = ( m_ItemInfo.m_dwRegisterStartDate/100 ) % 100;
	int iDay = m_ItemInfo.m_dwRegisterStartDate % 100;
	int iHour = m_ItemInfo.m_dwRegisterEndDate / 10000;
	int iMin = ( m_ItemInfo.m_dwRegisterEndDate/100 ) % 100;
	int iSec = m_ItemInfo.m_dwRegisterEndDate % 100;

	time_t kLimitTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMin, iSec );
	kLimitTime += ( m_ItemInfo.m_dwPeriodTime * DAY_SEC );
	time_t kCurServerTime = g_MyInfo.GetServerDate();

	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	int iGapSec = kLimitTime - kCurServerTime;

	// 남은 시간
	char szTime[MAX_PATH] = "";
	ZeroMemory( szTime, sizeof( szTime ) );

	int iGapDay    = iGapSec/DAY_SEC;
	int iGapHourSec= iGapSec-(iGapDay*DAY_SEC);
	int iGapHour   = iGapHourSec / HOUR_SEC;
	int iGapMinute = ( iGapHourSec-(iGapHour*HOUR_SEC) ) / MINUTE_SEC;
	int iTotalMin = iGapSec / MINUTE_SEC;

	SafeSprintf( szTime, sizeof( szTime ), STR(4), iGapDay, iGapHour, iGapMinute );

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(5), szTime );
}

TradeItemInfo& LobbyTradeItemBtn::GetInfo()
{
	return m_ItemInfo;
}

bool LobbyTradeItemBtn::IsCustomToolTip()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CUSTOM_BTN ) )
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////

LobbyTradeInfoWnd::LobbyTradeInfoWnd()
{
	m_bIsNoviceGrade = false;
}

LobbyTradeInfoWnd::~LobbyTradeInfoWnd()
{

}

void LobbyTradeInfoWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/LobbyOwnerTradeListWnd.xml", this );
}

void LobbyTradeInfoWnd::iwm_show()
{
	ShowChildWnd( ID_OWNER_TRADE_LIST_WND );
	
	OnRefreshTradeItemList();

	HideChildWnd( ID_NOVICE_GRADE_BTN );
	HideChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );

	if ( g_NoviceGradeBenefitMgr.IsBenefitGrade( g_MyInfo.GetGradeLevel() ) )
	{
		ShowChildWnd( ID_NOVICE_GRADE_BTN );
		ShowChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );
		m_bIsNoviceGrade = true;
	}
}

void LobbyTradeInfoWnd::iwm_hide()
{
	HideChildWnd( ID_OWNER_TRADE_LIST_WND );
	//HideChildWnd( ID_LOBBY_USER_LIST_WND );

	HideChildWnd( ID_NOVICE_GRADE_BTN );
	HideChildWnd( ID_NOVICE_GRADE_TOOTIP_BTN );
	m_bIsNoviceGrade = false;
}

void LobbyTradeInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MY_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_App.OpenMannerWnd( g_MyInfo.GetPublicID().c_str(), true );
		}
		break;
	case ID_NOVICE_GRADE_TOOTIP_BTN:
		{
			if( cmd == IOWN_OVERED )
			{
				ShowChildWnd( ID_NOVICE_GRADE_TOOTIP );
			}
			else if( cmd == IOWN_LEAVED )
			{
				HideChildWnd( ID_NOVICE_GRADE_TOOTIP );
			}
		}
		break;
	}
}

void LobbyTradeInfoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	SetNamedTitleSparkle();
}

void LobbyTradeInfoWnd::OnRefreshTradeItemList()
{
	LobbyOwnerTradeListWnd *pTradeListWnd = dynamic_cast<LobbyOwnerTradeListWnd*>(FindChildWnd( ID_OWNER_TRADE_LIST_WND ));
	if( !pTradeListWnd || !pTradeListWnd->IsShow() )
		return;

	pTradeListWnd->OnRefreshTradeItemList();
}

void LobbyTradeInfoWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 12, 66, 111 );
	g_FontMgr.SetTextColor( 255, 255, 255 );

	if ( m_bIsNoviceGrade )
		g_FontMgr.PrintTextWidthCut( iXPos + 59, iYPos + 11, FONT_SIZE_17, 105.0f, g_MyInfo.GetPublicID().c_str() );
	else
	{
		g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos + 14, iYPos + 14, g_MyInfo.GetGradeLevel(), TEAM_PRIVATE_2 );
		g_FontMgr.PrintTextWidthCut( iXPos + 32, iYPos + 11, FONT_SIZE_17, 115.0f, g_MyInfo.GetPublicID().c_str() );
	}
}

void LobbyTradeInfoWnd::SetNamedTitleSparkle()
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleBtn *pNamedTitleBtn = dynamic_cast<NamedTitleBtn*>( FindChildWnd( ID_MY_INFO_BTN ) );
	if ( !pNamedTitleBtn )
		return;

	if ( pUserNamedTitle->IsNewNamedTitle() )
	{
		if ( !pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetStartEffect();
	}
	else
	{
		if ( pNamedTitleBtn->IsEffect() )
			pNamedTitleBtn->SetEndEffect();
	}
}

//////////////////////////////////////////////////////////////////////////

LobbyOwnerTradeListWnd::LobbyOwnerTradeListWnd()
{
	m_iCurPage			= 1;
	m_iMaxPage			= 1;
	m_iPageYPos			= LOBBY_PAGE_Y;
	m_dwMaxTradeList	= OWNER_MAX_TRADE;
}

LobbyOwnerTradeListWnd::~LobbyOwnerTradeListWnd()
{
	m_vTradeItemInfoList.clear();
}

void LobbyOwnerTradeListWnd::PlazaUIMove( int nAddYPos )
{
	m_iPageYPos = QUICK_LOBBY_PAGE_Y + nAddYPos;
	ioWnd* pLeft = FindChildWnd( ID_PRE_BTN );
	if( pLeft )
		pLeft->SetWndPos( pLeft->GetDefaultPosX(), m_iPageYPos );

	ioWnd* pRight = FindChildWnd( ID_NEXT_BTN );
	if( pRight )
		pRight->SetWndPos( pRight->GetDefaultPosX(), m_iPageYPos );
}

void LobbyOwnerTradeListWnd::iwm_show()
{
	m_iCurPage = 1;
	m_iMaxPage = 1;
	InitTradeItemBtn();
}

void LobbyOwnerTradeListWnd::iwm_hide()
{
}

void LobbyOwnerTradeListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{	
	case ID_TRADE_BTN1:
	case ID_TRADE_BTN2:
	case ID_TRADE_BTN3:
	case ID_TRADE_BTN4:
	case ID_TRADE_BTN5:
	case ID_TRADE_BTN6:
	case ID_TRADE_BTN7:
		if( cmd == IOBN_BTNUP )
		{
			LobbyTradeItemBtn *pBtn = dynamic_cast<LobbyTradeItemBtn*>( FindChildWnd( dwID ) );
			if( pBtn )
			{
				TradeItemCancelWnd *pCancelWnd = dynamic_cast<TradeItemCancelWnd*>( g_GUIMgr.FindWnd( TRADE_ITEM_CANCEL_WND ) );
				if( pCancelWnd )
					pCancelWnd->SetTradItemCancelInfo( pBtn->GetInfo() );
			}
		}
		break;
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			PrevPage();
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NextPage();
		}
		break;
	}
}

void LobbyOwnerTradeListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	UpdateTradeItemList();
}

void LobbyOwnerTradeListWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	m_PagePrinter.PrintFullText( iXPos + 105, iYPos + m_iPageYPos, TAT_CENTER );
}

void LobbyOwnerTradeListWnd::OnRefreshTradeItemList()
{
	g_TradeInfoMgr.SyncTradeList();
}

void LobbyOwnerTradeListWnd::InitTradeItemBtn()
{
	for( DWORD dwID = ID_TRADE_BTN1; dwID <= ID_TRADE_BTN7; ++dwID )
	{
		LobbyTradeItemBtn *pBtn = dynamic_cast<LobbyTradeItemBtn*>( FindChildWnd( dwID ) );
		if( pBtn )
		{
			pBtn->SetBlank();

			if( dwID < static_cast<int>( ID_TRADE_BTN1 + m_dwMaxTradeList ) )
				pBtn->ShowWnd();
			else
				pBtn->HideWnd();
		}
	}
}

void LobbyOwnerTradeListWnd::UpdatePage()
{
	int iSize = m_vTradeItemInfoList.size();

	for( int i= 0; i < iSize; ++i )
	{
		LobbyTradeItemBtn *pBtn = dynamic_cast<LobbyTradeItemBtn*>( FindChildWnd( ID_TRADE_BTN1 + i ) );
		if( pBtn )
		{
			pBtn->SetTradeInfo( m_vTradeItemInfoList[i] );
		}
	}

	m_PagePrinter.ClearList();

	m_PagePrinter.SetTextStyle( TS_NORMAL );
	m_PagePrinter.SetBkColor( 0, 0, 0 );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PagePrinter.AddTextPiece( FONT_SIZE_13, "%d", m_iCurPage );
	m_PagePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	m_PagePrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage );
}

void LobbyOwnerTradeListWnd::UpdateTradeItemList()
{
	m_vTradeItemInfoList.clear();

	InitTradeItemBtn();
		
	TradeItemInfoList vList, vBuffer;
	g_TradeInfoMgr.GetTradeItemListByUser( vList, g_MyInfo.GetUserIndex() );

	if( vList.size() == 0 )
	{
		m_iCurPage = m_iMaxPage = 1;
	}
	else
	{		
		m_iMaxPage = (int)vList.size() / OWNER_MAX_TRADE;
		if( (int)vList.size() % OWNER_MAX_TRADE != 0 )
			m_iMaxPage++;

		for( int i = 0; i < OWNER_MAX_TRADE; ++i )
		{
			int nIndex = (m_iCurPage-1) * OWNER_MAX_TRADE + i;
			if( nIndex < (int)vList.size() )
			{
				const TradeItemInfo& Info = vList[nIndex];
				m_vTradeItemInfoList.push_back( Info );
			}
		}

		UpdatePage();
	}
}

void LobbyOwnerTradeListWnd::PrevPage()
{	
	if(  0 < m_iCurPage - 1 )
		m_iCurPage--;
}

void LobbyOwnerTradeListWnd::NextPage()
{
	if( m_iCurPage +1 <= m_iMaxPage )
		m_iCurPage++;
}

//////////////////////////////////////////////////////////////////////////

TradeItemCancelWnd::TradeItemCancelWnd()
{

	ClearDesc();
}

TradeItemCancelWnd::~TradeItemCancelWnd()
{

}

void TradeItemCancelWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void TradeItemCancelWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioMovingWnd::AddRenderFrame( szType, pFrame );
}

void TradeItemCancelWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioMovingWnd::AddRenderImage( szType, pImage );	
}

void TradeItemCancelWnd::SetTradItemCancelInfo( TradeItemInfo &rkInfo )
{
	ClearDesc();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		HideWnd();
		return;
	}

	m_ItemInfo.Init();
	const ioItem *pItem = g_ItemMaker.GetItemConst( rkInfo.m_dwItemMagicCode, __FUNCTION__ );
	if( !pItem )
	{
		return;
	}

	m_ItemInfo = rkInfo;

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_17, STR(1), pItem->GetName().c_str() );

	char szPeso[MAX_PATH]="";
	Help::ConvertNumToStrComma( m_ItemInfo.m_iItemPrice, szPeso, sizeof( szPeso ) );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_17, STR(2), szPeso );


	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( pItem->GetItemLargeIconName() );
	TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
	if( pIconBtn )
	{
		pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, m_ItemInfo.m_dwItemValue );
		pIconBtn->SetGradeType( pItem->GetGradeType() );
		pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

		if( m_ItemInfo.m_dwItemMaleCustom > 0 || m_ItemInfo.m_dwItemFemaleCustom > 0 )
			pIconBtn->SetCustomItem( true );
		else
			pIconBtn->SetCustomItem( false );
	}

	if( !IsShow() )
		ShowWnd();
}

void TradeItemCancelWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ITEM_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SP2Packet kPacket( CTPK_TRADE_CANCEL );
			kPacket << m_ItemInfo.m_dwTradeIndex;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		break;
	case ID_OK_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TradeItemCancelWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		MAIN_TEXT_X		= 103,
		MAIN_TEXT_Y		= 149,

		PRICE_TEXT_X	= 103,
		PRICE_TEXT_Y	= 170,
	};

	if( !m_szDescArray[0].IsEmpty() )
		m_szDescArray[0].PrintFullText( iXPos+MAIN_TEXT_X, iYPos+MAIN_TEXT_Y, TAT_CENTER );

	if( !m_szDescArray[1].IsEmpty() )
		m_szDescArray[1].PrintFullText( iXPos+PRICE_TEXT_X, iYPos+PRICE_TEXT_Y, TAT_CENTER );


	// 마감
	enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };

	int iYear = m_ItemInfo.m_dwRegisterStartDate / 10000;
	int iMonth = ( m_ItemInfo.m_dwRegisterStartDate/100 ) % 100;
	int iDay = m_ItemInfo.m_dwRegisterStartDate % 100;
	int iHour = m_ItemInfo.m_dwRegisterEndDate / 10000;
	int iMin = ( m_ItemInfo.m_dwRegisterEndDate/100 ) % 100;
	int iSec = m_ItemInfo.m_dwRegisterEndDate % 100;

	time_t kLimitTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMin, iSec );
	kLimitTime += ( m_ItemInfo.m_dwPeriodTime * DAY_SEC );
	time_t kCurServerTime = g_MyInfo.GetServerDate();

	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	int iGapSec = kLimitTime - kCurServerTime;

	// 남은 시간
	char szTime[MAX_PATH] = "";
	ZeroMemory( szTime, sizeof( szTime ) );

	int iGapDay    = iGapSec/DAY_SEC;
	int iGapHourSec= iGapSec-(iGapDay*DAY_SEC);
	int iGapHour   = iGapHourSec / HOUR_SEC;
	int iGapMinute = ( iGapHourSec-(iGapHour*HOUR_SEC) ) / MINUTE_SEC;
	int iTotalMin = iGapSec / MINUTE_SEC;

	SafeSprintf( szTime, sizeof( szTime ), STR(1), iGapDay, iGapHour, iGapMinute );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2), szTime );
	kPrinter.PrintFullText( iXPos+103, iYPos+252, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

TradeItemCancelResultWnd::TradeItemCancelResultWnd()
{

}

TradeItemCancelResultWnd::~TradeItemCancelResultWnd()
{
}

void TradeItemCancelResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioMovingWnd::AddRenderImage( szType, pImage );
}

void TradeItemCancelResultWnd::SetCancelResultInfo( DWORD dwItemType, DWORD dwMagicCode, DWORD dwValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iPrice )
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
		pIconBtn->SetGradeType( pItem->GetGradeType() );
		pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

		if( dwItemMaleCustom > 0 || dwItemFemaleCustom > 0 )
			pIconBtn->SetCustomItem( true );
		else
			pIconBtn->SetCustomItem( false );
	}

	if( !IsShow() )
		ShowWnd();
}

void TradeItemCancelResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_ITEM_GO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowPresentTabDirect();
		}
		break;
	case ID_OK_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void TradeItemCancelResultWnd::OnRender()
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
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////