#include "StdAfx.h"

#include <iterator>

#include "../ioPlayStage.h"
#include "../ioDecorationPrice.h"
#include "../ioEventGoodsMgr.h"
#include "../ioSaleManager.h"
#include "../ioSpecialShopMgr.h"
#include "../ioExtendSoundManager.h"
#include "../ioBrowserManager.h"

#include "../Channeling/ioChannelingNodeParent.h"
#include "../Channeling/ioChannelingNodeManager.h"

#include "MonsterCoinWnd.h"
#include "NewShopBuyWnd.h"
#include "NewShopStartWnd.h"
#include "NewShopSearchWnd.h"
#include "NewShopIconBtn.h"
#include "NewShopItemInfoWnd.h"
#include "NewShopToolTip.h"
#include "NewShopSortDefine.h"

#include "SmallTabButton.h"

#include "UISoldierSelectWnd.h"
#include "PetWnd.h"
#include "GlobalWnd.h"

#include "ioShopDataCache.h"

#include "../ioBonusCashManager.h"

bool NewShopWnd::m_bGashaponListRequest = false;
DWORD NewShopWnd::m_dwCashSendTime = 0;

NewShopWnd::NewShopWnd(void)
{
	m_pPlayStage			= NULL;

	//탭
	m_dwCurTabID			= 0;
	m_dwCurSmallTabID		= 0;
	m_dwSmallTabEndID		= 0;
	m_dwDefaultShowTab		= ID_RECOMMEND_TAB_BTN;

	//페이지
	m_iCurPage				= 0;
	m_iSelectCurArray		= 0;
	m_iSelectCurPage		= 0;

	//렌더
	m_pEventMark			= NULL;
	m_bScreen				= false;

	m_dwScreenCheckTime		= 0;
	m_dwScreenGapTime		= 0;
	m_dwScreenTime			= 0;

	//3D 캐릭터 치장관련
	m_iDecoSexType			= 0;
	m_iDecoClassType		= 1;

	m_iDecoReserveSexType	= 0;
	m_bDecoReserve			= false;	
	m_bSetDecoCharInfo		= false;

	//3D 캐릭터 치장장비
	for (int i = 0; i < MAX_DECO_EQUIP ; i++ )
		m_iEquipDecoTypeCodeArray[i] = -1;

	m_bReleaseDecoEquip = false;
	m_dwBonusCashTooltipCheckStartTime = 0;
}

NewShopWnd::~NewShopWnd(void)
{
	for(int i = 0; i < MAX_TAB_BTN; i++ )
	{
		m_vSmallTabNameVecArray[i].clear();
		m_vSmallTabTypeVecArray[i].clear();
	}
	m_vNewShopItemInfoVec.clear();
	SAFEDELETE( m_pEventMark );
}

// 윈도우 생성시 로딩 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TabEventMark" )
	{
		SAFEDELETE( m_pEventMark );
		m_pEventMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

void NewShopWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioHashString sSmallTabName;
	int          iType = 0;
	for (int i = 0; i < MAX_TAB_BTN ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Max_%d", ID_RECOMMEND_TAB_BTN+i );
		int iMax = xElement.GetIntAttribute( szName );
		for (int j = 0; j < iMax ; j++)
		{
			StringCbPrintf_e( szName, sizeof( szName ), "Name_%d_%d", ID_RECOMMEND_TAB_BTN+i, j+1 );
			sSmallTabName = xElement.GetStringAttribute( szName );
			m_vSmallTabNameVecArray[i].push_back( sSmallTabName );

			StringCbPrintf_e( szName, sizeof( szName ), "Type_%d_%d", ID_RECOMMEND_TAB_BTN+i, j+1 );
			iType = xElement.GetIntAttribute( szName );
			m_vSmallTabTypeVecArray[i].push_back( iType );
		}
	}

	m_dwScreenGapTime	= xElement.GetIntAttribute_e( "TabScreenGapTime" );
	m_dwScreenTime		= xElement.GetIntAttribute_e( "TabScreenDuration" );
	m_dwDefaultShowTab	= xElement.GetIntAttribute_e( "DefaultShowTab" );
	m_dwBonusCashTooltipCheckDuration = (DWORD)xElement.GetIntAttribute_e( "BonusCashTooltipCheckDuration" );
	m_szRandUrlFailMsg[0] = xElement.GetStringAttribute_e( "Rand_URL_Fail_Msg1" );
	m_szRandUrlFailMsg[1] = xElement.GetStringAttribute_e( "Rand_URL_Fail_Msg2" );
}

void NewShopWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/newshopstartwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/newshopsearchwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/newshopbuywnd.xml", this );
	g_GUIMgr.AddWnd( "XML/newshopboughtwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/supergashponshopwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/multisupergashponshopwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/MultiSuperGashaponToastPopupShopWnd.xml", this );
		
	ParseShopData();
	UpdateEventItemCnt();

}

void NewShopWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void NewShopWnd::ParseShopData( bool bMustParse /* = false */ )
{
	if( !g_ShopData.GetSingletonPtr() )
	{
		LOG.PrintTimeAndLog(0, "[SHOP ERROR] : %s ioShopDataCache == NULL", __FUNCTION__ );
		return;
	}

	for ( DWORD dwTabID = ID_RECOMMEND_TAB_BTN; dwTabID < ID_TAB_MAX ; dwTabID++ )
	{
		int iSize = m_vSmallTabNameVecArray[dwTabID - ID_RECOMMEND_TAB_BTN].size();

		//상품 정보가 동기화 된후 데이터를 생성
		if( dwTabID == ID_EVENT_GOODS_TAB_BTN || dwTabID == ID_SECRET_TAB_BTN )
			continue;
				
		for ( DWORD dwSamllTabID = ID_SMALL_TAB_1_BTN; dwSamllTabID < ID_SMALL_TAB_10_BTN; dwSamllTabID++ )
		{
			if( !COMPARE( dwSamllTabID, ID_SMALL_TAB_1_BTN, ID_SMALL_TAB_1_BTN + (DWORD)iSize ) )
				continue;
			
			if( dwTabID == ID_DECO_TAB_BTN || (dwTabID == ID_RECOMMEND_TAB_BTN && dwTabID == ID_SMALL_TAB_4_BTN ) )
			{
				//남자 치장
				if( g_ShopData.GetCash( dwTabID, dwSamllTabID, 0 ) == NULL || bMustParse )
					ParseShopDataCache( dwTabID, dwSamllTabID, 0 );
							
				//여자 치장
				if( g_ShopData.GetCash( dwTabID, dwSamllTabID, 1 ) == NULL || bMustParse )
					ParseShopDataCache( dwTabID, dwSamllTabID, 1 );
			}
			else
			{
				if( g_ShopData.GetCash( dwTabID, dwSamllTabID ) == NULL || bMustParse )
					ParseShopDataCache( dwTabID, dwSamllTabID );
			}			
		}
	}
}

void NewShopWnd::ParseShopDataCache( DWORD dwTabID, DWORD dwSamllTabID, int iSexType /* = 0 */ )
{
	if( !g_ShopData.GetSingletonPtr() )
	{
		LOG.PrintTimeAndLog(0, "[SHOP ERROR] : %s ioShopDataCache == NULL", __FUNCTION__ );
		return;
	}

	vNewShopItemInfoVec ShopDataList;
	NewShopItemHelp::ParseItemInfoVec( ShopDataList, dwTabID, dwSamllTabID, this, iSexType );

	NewShopDataCache Cache;
	Cache.dwTab				= dwTabID;
	Cache.dwSamllTab		= dwSamllTabID;
	Cache.m_ShopDataList	= ShopDataList;
	Cache.m_SexType			= iSexType;

	g_ShopData.AddCash( Cache );
}

// 윈도우 Show/Hide 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::iwm_show()
{
	g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_ENTER );

	g_EventGoodsMgr.SyncGoodsList();
	g_SpecialShopMgr.SyncGoodsList();

	SetChildInActive( ID_EVENT_GOODS_TAB_BTN );
	SetChildInActive( ID_SECRET_TAB_BTN );

	MonsterCoinUseWnd *pCoinUseWnd = dynamic_cast<MonsterCoinUseWnd*> (g_GUIMgr.FindWnd( MONSTER_COIN_USE_WND ) );
	if( pCoinUseWnd )
	{
		if( m_pPlayStage->GetModeType() == MT_MONSTER_SURVIVAL )
		{
			if( pCoinUseWnd->GetCoinUseType() != MonsterCoinUseWnd::USETYPE_REVIVAL_COIN )
				pCoinUseWnd->HideWnd();
		}
	}
	PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd(PET_WND) );
	if( pPetWnd && pPetWnd->IsShow() )
		pPetWnd->HideWnd();


	SendGetCash();
	SendRequestGashaponList();

	enum { Y_OFFSET = -20 };

	int iXPos = ( Setting::Width() - GetWidth() ) / 2;
	int iYPos = ( Setting::Height() - GetHeight() ) / 2 + Y_OFFSET;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		iXPos = pInvenWnd->GetDerivedPosX();
		iYPos = pInvenWnd->GetDerivedPosY();
		pInvenWnd->HideWnd();
	}
	ioWnd::SetWndPos( iXPos, iYPos );

	ShowADPage();

	if( g_MyInfo.GetChannelingType() == CNT_NEXON )
	{
		ioWnd* pWnd = FindChildWnd( ID_CHARGE_GOLD_BTN );
		if( pWnd )
			pWnd->SetInActive();
	}
}

void NewShopWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	UpdateSmallTabElement( GetCurrTab() );
}

void NewShopWnd::iwm_hide()
{
	HideChildWnd( ID_START_WND );
	HideChildWnd( ID_SEARCH_WND );
	HideChildWnd( ID_BUY_WND );

	for (int i = 0; i < MAX_BUY_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	g_ShopData.ClearFilterCache();
}

// 윈도우 마우스/키 이벤트 등 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::iwm_gototop()
{
	// 용병 선택창이 항상 상위에 위치한다.
	UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
	if( pUISoliderWnd && pUISoliderWnd->IsShow() )
		pUISoliderWnd->GoToTop();
}

void NewShopWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHARGE_GOLD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_MYROOM )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
			}
			else
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드충전은 광장/개인본부/훈련소에서만 가능합니다." );
					return;
				}
			}

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
				pLocal->OpenFillCashPage();
		}
		break;
	case ID_MY_ITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				switch( m_dwCurTabID )
				{
				case ID_SOLDIER_TAB_BTN:
					pInvenWnd->ShowSoldierTabDirect();
					break;
				case ID_EXTRA_TAB_BTN:
					pInvenWnd->ShowExtraItemTabDirect( -1 );
					break;
				case ID_DECO_TAB_BTN:
					pInvenWnd->ShowDecoTabDirect( -1 );
					break;
				case ID_ETC_TAB_BTN:
					pInvenWnd->ShowEtcTabDirect();
					break;
					// UI코스튬 관련 (상점에서 내아이템가기)
				case ID_COSTUME_TAB_BTN:
					pInvenWnd->ShowCostumeTabDirect( -1 );
					break;
				default:
					pInvenWnd->ShowWnd();
					break;
				}
			}
		}
		break;
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_HOME_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ShowADPage( false );
			}
			else if( cmd == IOWN_OVERED )
			{
				ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );
				kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_12, "상점 첫페이지 바로가기" );

				GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>( FindChildWnd( ID_HOME_TOOLTIP ) );
				if( pTooltip )
					pTooltip->SetTooltipText( kPrinter );
			}
			else if( cmd == IOWN_LEAVED )
			{
				GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>( FindChildWnd( ID_HOME_TOOLTIP ) );
				if( pTooltip )
					pTooltip->HideWnd();
			}
		}
		break;
	case ID_RAND_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_MYROOM )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szRandUrlFailMsg[0].c_str() );
						return;
					}
				}
				else
				{
					if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szRandUrlFailMsg[1].c_str() );
						return;
					}
				}
				
				
				Help::ExcuteWeb( g_BrowserMgr.GetProbabilityURL().c_str() );
			}
		}
		break;
	case ID_RECOMMEND_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			iwm_updatetab( dwID, ID_SMALL_TAB_1_BTN, true, false );
			g_EventGoodsMgr.SyncGoodsList();
			g_SpecialShopMgr.SyncGoodsList();
		}
		break;
	case ID_SOLDIER_TAB_BTN:
	case ID_EXTRA_TAB_BTN:
	case ID_DECO_TAB_BTN:
	case ID_ETC_TAB_BTN:
	case ID_EVENT_GOODS_TAB_BTN:
	case ID_COSTUME_TAB_BTN:
	case ID_SECRET_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			iwm_updatetab( dwID );
			g_EventGoodsMgr.SyncGoodsList();
			g_SpecialShopMgr.SyncGoodsList();
		}
		break;
	case ID_SMALL_TAB_1_BTN:
	case ID_SMALL_TAB_2_BTN:
	case ID_SMALL_TAB_3_BTN:
	case ID_SMALL_TAB_4_BTN:
	case ID_SMALL_TAB_5_BTN:
	case ID_SMALL_TAB_6_BTN:
	case ID_SMALL_TAB_7_BTN:
	case ID_SMALL_TAB_8_BTN:
	case ID_SMALL_TAB_9_BTN:
	case ID_SMALL_TAB_10_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			UpdateSmallTab( dwID, true );
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
			OnBuyInfoBtnUp( dwID );
		}
		break;
	case MESSAGE_BOX3: // g_GUIMgr.SetPrevMsgListBox() OR g_GUIMgr.SetPrevMsgListBoxWithTitle()
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				// 특별 바로 가기
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd )
				{
					pInvenWnd->ShowEtcTabDirect();
				}
			}
		}
		break;
	case ID_SEARCH_SHOW_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioWnd* pWnd = FindChildWnd( ID_SEARCH_WND );
				if( pWnd )
				{					
					ShowChildWnd( ID_SEARCH_WND );
					HideChildWnd( ID_SEARCH_SHOW_BTN );
					ShowChildWnd( ID_SEARCH_HIDE_BTN );
				}
			}
		}
		break;
	case ID_SEARCH_HIDE_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioWnd* pWnd = FindChildWnd( ID_SEARCH_WND );
				if( pWnd )
				{
					HideChildWnd( ID_SEARCH_WND );					
					HideChildWnd( ID_SEARCH_HIDE_BTN );
					ShowChildWnd( ID_SEARCH_SHOW_BTN );
				}
			}
		}
		break;
	case ID_BONUS_CASH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( !g_GUIMgr.IsShow( BONUS_CASH_WND ))
				g_GUIMgr.ShowWnd( BONUS_CASH_WND );
			else
				g_GUIMgr.HideWnd( BONUS_CASH_WND );
		}
		break;
	}
}


void NewShopWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void NewShopWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
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

// 상위 탭 업데이트 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::iwm_updatetab( DWORD dwTabID, int nSmallTabID /* = -1 */, bool bBeforeTabUse /*= true */, bool bAfterTabUse /*= true */, bool bSetFirstItem /*= true */, bool bSearchHide /*= true */ )
{
	UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
	if( pUISoliderWnd && pUISoliderWnd->IsShow() )
		pUISoliderWnd->HideWnd();

	NewShopBuyWnd *pBuyWnd = dynamic_cast<NewShopBuyWnd*> ( FindChildWnd( ID_BUY_WND ) );
	if( pBuyWnd )
	{
		pBuyWnd->InitCharRotate();
		pBuyWnd->HideWnd();
	}

	HideChildWnd( ID_START_WND );
	ShowChildWnd( ID_VERT_SCROLL );
	ShowChildWnd( ID_SEARCH_SHOW_BTN );
	HideChildWnd( ID_SEARCH_HIDE_BTN );

	if( bSearchHide )
		HideChildWnd( ID_SEARCH_WND );

	for (int i = 0; i < MAX_BUY_INFO_WND_BTN; i++ )
		ShowChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	if( bBeforeTabUse )
		UpdateTabBtnUpBefore( dwTabID );

	UpdateTab( dwTabID, bSetFirstItem, nSmallTabID, bSearchHide );

	if( bAfterTabUse )
		UpdateTabBtnUpAfter( dwTabID );
}

void NewShopWnd::UpdateTabBtnUpBefore( DWORD dwTabID )
{
	if( dwTabID == ID_RECOMMEND_TAB_BTN || dwTabID == ID_DECO_TAB_BTN || dwTabID == ID_EXTRA_TAB_BTN || dwTabID == ID_ETC_TAB_BTN || dwTabID == ID_COSTUME_TAB_BTN )
	{
		for ( int i = 0; i < MAX_DECO_EQUIP ; i++ )
			m_iEquipDecoTypeCodeArray[i] = -1;

		//예약된 타입을 기준으로 초기화
		m_bSetDecoCharInfo		= true;
		m_iDecoClassType		= 1;
		m_iDecoSexType			= m_iDecoReserveSexType;

		IntVec vMyClassTypeVec;
		NewShopBuyWnd *pBuyWnd = dynamic_cast<NewShopBuyWnd*> ( FindChildWnd( ID_BUY_WND ) );
		if( pBuyWnd )
			pBuyWnd->GetMyClassTypeVec( vMyClassTypeVec );
		if( !vMyClassTypeVec.empty() )
		{				
			m_iDecoClassType = vMyClassTypeVec[0];
		}

		if( !m_bDecoReserve )
		{
			int iCharArray = g_MyInfo.GetClassArray( m_iDecoClassType );
			if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
				m_iDecoSexType  = g_MyInfo.GetCharacter( iCharArray ).m_sex-1;
		}

		//예약 타입 초기화
		m_iDecoReserveSexType	= 0;
		m_bDecoReserve			= false;
	}
}

void NewShopWnd::UpdateTabBtnUpAfter( DWORD dwTabID, int iSetPage /*=-1*/, int iCurrInfoBtn /*=-1*/ )
{
	if ( dwTabID == ID_RECOMMEND_TAB_BTN || dwTabID == ID_EXTRA_TAB_BTN|| dwTabID == ID_COSTUME_TAB_BTN || dwTabID == ID_SECRET_TAB_BTN )
	{
		if( iSetPage == - 1 && iCurrInfoBtn == -1 )
		{
			// NewShopBuyWnd::SetInfo 발동을 위해서
			OnBuyInfoBtnUp( ID_ITEM_INFO_WND_1_BTN );

			ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
			if( pScroll )
				pScroll->SetScrollPos( 0 );
		}
		else
		{
			m_iCurPage = iSetPage;
			OnBuyInfoBtnUp( ID_ITEM_INFO_WND_1_BTN + iCurrInfoBtn );

			ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage );
		}
	}
}

void NewShopWnd::UpdateTab( DWORD dwTabBtn, bool bSetFirstItem, int nSmallTab /* = -1*/, bool bSearchHide /* = true */ )
{
	HideChildWnd( ID_START_WND );

	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_HOME_BTN ) );
	if( pButton )
		pButton->SetActive();

	if( bSearchHide )
	{
		if( dwTabBtn == ID_RECOMMEND_TAB_BTN )
		{
			HideChildWnd( ID_SEARCH_WND );
			HideChildWnd( ID_SEARCH_SHOW_BTN );
			HideChildWnd( ID_SEARCH_HIDE_BTN );
		}
		else
		{
			HideChildWnd( ID_SEARCH_WND );
			ShowChildWnd( ID_SEARCH_SHOW_BTN );
			HideChildWnd( ID_SEARCH_HIDE_BTN );
		}
	}
	else
	{
		HideChildWnd( ID_SEARCH_SHOW_BTN );
		ShowChildWnd( ID_SEARCH_HIDE_BTN );
	}

	dwTabBtn = UpdateTabInActive( dwTabBtn );
	m_dwCurTabID = dwTabBtn;

	if( g_SpecialShopMgr.IsSpecialShopOpen() )
		CheckRadioButton( ID_RECOMMEND_TAB_BTN, ID_SECRET_TAB_BTN, dwTabBtn );
	else
		CheckRadioButton( ID_RECOMMEND_TAB_BTN, ID_COSTUME_TAB_BTN, dwTabBtn );

	if( !UpdateSmallTabElement( dwTabBtn ) )
		return;	

	int iUpdateTab = UpdateSmallTabInActive();
	int iStartTab = 0;
	if ( nSmallTab != -1 )
		iStartTab = nSmallTab;
	else
		iStartTab = iUpdateTab;
	
	if( iStartTab == 0 )
		UpdateSmallTab( ID_SMALL_TAB_1_BTN, bSetFirstItem );
	else
		UpdateSmallTab( iStartTab, bSetFirstItem );
}

DWORD NewShopWnd::UpdateTabInActive( DWORD dwCurTab )
{
	int iRecommanded = 0;
	iRecommanded += g_SetItemInfoMgr.GetSetItemRecommandedCnt();
	iRecommanded += g_ExtraItemInfoMgr.GetExtraItemRecommandedCnt();
	iRecommanded += g_DecorationPrice.GetDecoRecommandedCnt();
	iRecommanded += g_EtcItemMgr.GetEtcItemRecommandedCnt();
	iRecommanded += g_CostumeShopInfoMgr.GetCostumeRecommandedCnt();

	if( 0 < iRecommanded )
	{	
		SetChildActive( ID_RECOMMEND_TAB_BTN );
	}
	else
	{		
		SetChildInActive( ID_RECOMMEND_TAB_BTN );
		if( dwCurTab == ID_RECOMMEND_TAB_BTN )
			return m_dwDefaultShowTab;
	}

	if( g_SpecialShopMgr.IsSpecialShopOpen() )
		SetChildActive( ID_SECRET_TAB_BTN );
	else
		SetChildInActive( ID_SECRET_TAB_BTN );

	return dwCurTab;
}

// 하위 탭 업데이트 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool NewShopWnd::UpdateSmallTab( DWORD dwSmallTabID, bool bSetFirstItem )
{
	CheckRadioButton( ID_SMALL_TAB_1_BTN, m_dwSmallTabEndID, dwSmallTabID );
	m_dwCurSmallTabID = dwSmallTabID;

	//필터 및 검색관련 인자 가져오기
	std::string			szFind;
	NewShopFilter		eCurrFilter = NSF_NONE;
	NewShopSearchWnd*	pSearch = dynamic_cast<NewShopSearchWnd*>( FindChildWnd( ID_SEARCH_WND ) );
	if( pSearch && pSearch->IsShow() )
	{
		szFind		= pSearch->GetFindKeyWord();
		eCurrFilter = pSearch->GetCurrNewShopFilter();

		if( !szFind.empty() )
			std::transform( szFind.begin(), szFind.end(), szFind.begin(), tolower );
	}

	//상점 상품 구성(필터링 포함)
	m_vNewShopItemInfoVec.clear();
	GetItemInfoVec( m_vNewShopItemInfoVec, m_dwCurTabID, m_dwCurSmallTabID, eCurrFilter, szFind.c_str() );
	
	//필터별 정렬
	switch( eCurrFilter )
	{
	case NSF_HIGH_GOLD_COST:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoHighGoldSort() );
		break;
	case NSF_LOW_GOLD_COST:
		{
			vNewShopItemInfoVec vList;
			NewShopItemHelp::DevideItemListByCoin( m_vNewShopItemInfoVec, SCT_GOLD, 0, vList );
			std::sort( vList.begin(), vList.end(), NewShopInfoLowPesoSort() );
			std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoLowGoldSort() );
			std::copy( vList.begin(), vList.end(), std::back_inserter( m_vNewShopItemInfoVec ) );
		}
		break;
	case NSF_HIGH_PESO_COST:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoHighPesoSort() );
		break;
	case NSF_LOW_PESO_COST:
		{	
			vNewShopItemInfoVec vList;
			NewShopItemHelp::DevideItemListByCoin( m_vNewShopItemInfoVec, SCT_PESO, 0, vList );
			std::sort( vList.begin(), vList.end(), NewShopInfoLowGoldSort() );
			std::sort( m_vNewShopItemInfoVec.begin() , m_vNewShopItemInfoVec.end(), NewShopInfoLowPesoSort() );
			std::copy( vList.begin(), vList.end(), std::back_inserter( m_vNewShopItemInfoVec ) );
		}
		break;
	case NSF_NO_LIMIT:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoNoLimitSort() );
		break;
	case NSF_NEED_HIGH_GRADE:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoSoliderNeedGradeHighSort() );
		break;
	case NSF_NEED_LOW_GRADE:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoSoliderNeedGradeLowSort() );
		break;
	case NSF_NEW_AND_EVENT_MARK:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoNewAndEventSort() );
		break;
	case NSF_EVENT_ITEM_HIGH_COST:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoEventStampHighSort() );
		break;
	case NSF_EVENT_ITEM_LOW_COST:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoEventStampLowSort() );
		break;
	default:
		std::sort( m_vNewShopItemInfoVec.begin(), m_vNewShopItemInfoVec.end(), NewShopInfoDefaultSort() );
		break;
	}

	//검색 윈도우에 현재 스몰탭 이름을 전달
	UpdateSearchWnd();

	//스크롤
	int iItemSize = m_vNewShopItemInfoVec.size();
	ioScroll* pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
	{
		int iMaxPos =  iItemSize / PAGE_PER_BTN_NUM;
		if( iItemSize % PAGE_PER_BTN_NUM )
			iMaxPos++;

		m_iCurPage = 0;
		pScroll->SetScrollRange( 0, iMaxPos );		
		pScroll->SetScrollPos( m_iCurPage );
	}

	if( bSetFirstItem )
		OnBuyInfoBtnUp( ID_ITEM_INFO_WND_1_BTN, false );

#if defined( USE_GA )
	switch( m_dwCurTabID )
	{
	case ID_RECOMMEND_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_POPULAR_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FPOPULAR%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_POPULAR_HREO
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FPOPULAR%2FHERO" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_POPULAR_GEAR
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FPOPULAR%2FGEAR" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_SHOP_POPULAR_COSTUME
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FPOPULAR%2FCOSTUME" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_SHOP_POPULAR_DECO
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FPOPULAR%2FDECO" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_SHOP_POPULAR_SPEACIAL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FPOPULAR%2FSPEACIAL" );
				}
				break;
			}
		}
		break;

	case ID_SOLDIER_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_HERO_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_HERO_PREMIUM
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FPREMIUM" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_HERO_MELEE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FMELEE" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_SHOP_HERO_RANGED
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FRANGED" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_SHOP_HERO_MAGIC
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FMAGIC" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_SHOP_HERO_UNIQUE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FUNIQUE" );
				}
				break;
			case ID_SMALL_TAB_7_BTN:
				{
					// WND_SHOP_HERO_PACKAGE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FHERO%2FPACKAGE" );
				}
				break;
			}
		}
		break;

	case ID_EXTRA_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_GEAR_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_GEAR_SUPPLY
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FSUPPLY" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_GEAR_WEAPON
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FWEAPON" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_SHOP_GEAR_ARMOR
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FARMOR" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_SHOP_GEAR_HELMET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FHELMET" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_SHOP_GEAR_TRINKET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FTRINKET" );
				}
				break;
			case ID_SMALL_TAB_7_BTN:
				{
					// WND_SHOP_GEAR_FEATURES
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FGEAR%2FFEATURES" );
				}
				break;
			}
		}
		break;

	case ID_DECO_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_DECO_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_DECO_GENDER
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FGENDER" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_DECO_HEAD
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FHEAD" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_SHOP_DECO_DYE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FDYE" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_SHOP_DECO_FACE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FFACE" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_SHOP_DECO_SKIN
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FSKIN" );
				}
				break;
			case ID_SMALL_TAB_7_BTN:
				{
					// WND_SHOP_DECO_UNDIES
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FUNDIES" );
				}
				break;
			case ID_SMALL_TAB_8_BTN:
				{
					// WND_SHOP_DECO_MOTION
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FMOTION" );
				}
				break;

			case ID_SMALL_TAB_9_BTN:
				{
					// WND_SHOP_DECO_PACKAGE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FDECO%2FPACKAGE" );
				}
				break;
			}
		}
		break;

	case ID_ETC_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_SPEACIAL_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FSPEACIAL%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_SPEACIAL_SETTING
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FSPEACIAL%2FSETTING" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_SPEACIAL_PREMIUM
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FSPEACIAL%2FPREMIUM" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_SHOP_SPEACIAL_PLAY
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FSPEACIAL%2FPLAY" );
				}
				break;
			}
		}
		break;

	case ID_EVENT_GOODS_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_EVENT_CLOVER
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FEVENT%2FCLOVER" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_EVENT_EVENT
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FEVENT%2FEVENT" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_EVENT_MILEAGE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FEVENT%2FMILEAGE" );
				}
				break;
			}
		}
		break;

	case ID_COSTUME_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_SHOP_COSTUME_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FCOSTUME%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_SHOP_COSTUME_SUPPLY
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FCOSTUME%2FSUPPLY" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_SHOP_COSTUME_ARMOR
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FCOSTUME%2FARMOR" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_SHOP_COSTUME_HELMET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FCOSTUME%2FHELMET" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_SHOP_COSTUME_TRINKET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FCOSTUME%2FTRINKET" );
				}
				break;
			}
		}
		break;

	case ID_SECRET_TAB_BTN:
		{
			// WND_SHOP_SECRET
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FSHOP%2FSECRET" );
		}
		break;
	}
#endif

	return true;
}

bool NewShopWnd::UpdateSmallTabElement( DWORD dwTabBtn )
{
	int iArray = dwTabBtn - ID_RECOMMEND_TAB_BTN;
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )		
		return false;

	int iXOffset = 0;
	int iSize	 = m_vSmallTabNameVecArray[iArray].size();

	for (int i = 0; i < MAX_SMALL_TAB_BTN; i++ )
	{
		SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( ID_SMALL_TAB_1_BTN + i ) );
		if( !pButton )
			return false;

		if( COMPARE( i, 0, iSize ) )
		{
			if( i == 0 )
				iXOffset = pButton->GetXPos();

			if( i == iSize-1 )			
				pButton->SetDisable( true );
			else
				pButton->SetDisable( false );

			ShowChildWnd( ID_SMALL_TAB_1_BTN + i );
			SetChildActive( ID_SMALL_TAB_1_BTN  + i );

			iXOffset += UpdateSmallTabName( pButton, iXOffset, m_vSmallTabNameVecArray[iArray][i] );
		}
		else
		{
			HideChildWnd( ID_SMALL_TAB_1_BTN + i );
		}
	}
	
	int iAddID = (iSize- 1);
	if( !COMPARE( iAddID, 0, MAX_SMALL_TAB_BTN ) )
		iAddID = 0;

	m_dwSmallTabEndID = ID_SMALL_TAB_1_BTN + iAddID;
	return true;
}

int NewShopWnd::UpdateSmallTabInActive()
{
	int iActiveTab = 0;
	switch( m_dwCurTabID )
	{	
	case ID_RECOMMEND_TAB_BTN:
		{
			for( DWORD dwID = ID_SMALL_TAB_1_BTN; dwID < ID_SMALL_TAB_10_BTN; dwID++ )
			{
				SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( dwID ) );
				if( !pButton )
					continue;

				if( !pButton->IsShow() )
					continue;

				vNewShopItemInfoVec vList;
				GetItemInfoVec( vList, m_dwCurTabID, dwID, NSF_NONE, "" );

				if( vList.empty() )
				{
					SetChildInActive( dwID );
				}
				else
				{
					SetChildActive( dwID );
					if( iActiveTab == 0 )
						iActiveTab = dwID;
				}
			}
		}
		break;
	case ID_EVENT_GOODS_TAB_BTN:
		{
			SetChildInActive( ID_SMALL_TAB_1_BTN );
			SetChildInActive( ID_SMALL_TAB_2_BTN );
			SetChildInActive( ID_SMALL_TAB_3_BTN );

			if( g_EventGoodsMgr.IsCloverShopOpen() )
			{
				SetChildActive( ID_SMALL_TAB_1_BTN );
				if( iActiveTab == 0 )
					iActiveTab = ID_SMALL_TAB_1_BTN;
			}

			if( g_EventGoodsMgr.IsEventShopOpen() )
			{
				SetChildActive( ID_SMALL_TAB_2_BTN );
				if( iActiveTab == 0 )
					iActiveTab = ID_SMALL_TAB_2_BTN;
			}

			if( g_EventGoodsMgr.IsMileageShopOpen() )
			{
				SetChildActive( ID_SMALL_TAB_3_BTN );
				if( iActiveTab == 0 )
					iActiveTab = ID_SMALL_TAB_3_BTN;
			}
		}
		break;
	}

	return iActiveTab;
}

int NewShopWnd::UpdateSmallTabName( ioWnd* pWnd, int iXOffset, const ioHashString& szTitle )
{
	if( !pWnd )
		return 0;

	ioUITitle* pTitle = pWnd->GetTitle();
	if( !pTitle )
		return 0;

	if( szTitle.IsEmpty() )
		return 0;

	float fScale	= (float)pTitle->GetSize() / (float)g_FontMgr.GetFontSize();
	int iTextWidth	= g_FontMgr.GetTextWidth( szTitle.c_str(), TS_NORMAL, fScale ) + 11;
	pTitle->SetOffset( iTextWidth/2, 0 );

	pWnd->SetSize( iTextWidth, g_FontMgr.GetFontSize() );
	pWnd->SetWndPos( iXOffset, pWnd->GetYPos() );
	pWnd->SetTitleText( szTitle.c_str() );
	pWnd->ShowWnd();

	return pWnd->GetWidth();
}

void NewShopWnd::GetSmallTabNameList( OUT ioHashStringVec& NameList )
{
	NameList.clear();
	for (int i = 0; i < MAX_SMALL_TAB_BTN; i++ )
	{
		SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( ID_SMALL_TAB_1_BTN + i ) );
		if( !pButton )
			continue;

		if( !pButton->IsShow() )
			continue;

		if( pButton->HasWndStyle( IWS_INACTIVE ) )
		{
			NameList.push_back( "" );
		}
		else
		{
			NameList.push_back( pButton->GetTitleText() );
		}
	}
}

// 검색 윈도우 업데이트 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::UpdateSearchWnd()
{
	NewShopSearchWnd* pSearch = dynamic_cast<NewShopSearchWnd*>( FindChildWnd( ID_SEARCH_WND ) );
	if( pSearch )
	{
		//검색결과 전달
		if( pSearch->IsShow() && m_vNewShopItemInfoVec.empty() )
		{
			pSearch->ChangeSearchType( NewShopSearchWnd::ST_FAIL );
		}

		SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( GetCurrSmallTab() ) );
		if( pButton )
		{
			pSearch->CreateCategoryPullDownTitle( pButton->GetTitleText() );
		}
	}	
}

// 스몰 탭 정보 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int NewShopWnd::GetSmallTabType( DWORD dwTabID, DWORD dwSmallTabID )
{
	int iArray = ( dwTabID - ID_RECOMMEND_TAB_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	int iSmallArray = ( dwSmallTabID - ID_SMALL_TAB_1_BTN );
	if( !COMPARE( iSmallArray, 0, (int) m_vSmallTabTypeVecArray[iArray].size() ) )
		return -1;

	return m_vSmallTabTypeVecArray[iArray][iSmallArray];
}

bool NewShopWnd::IsRightSmallTab( DWORD dwTabID, DWORD dwSmallTabID , int iValue1, int iValue2 )
{
	if( dwTabID == ID_RECOMMEND_TAB_BTN )
	{
		if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_SOLDIER_TAB_BTN )
	{
		if( iValue1 == ioSetItemInfo::CAT_NONE )
			return false;
		else if( dwSmallTabID == ID_SMALL_TAB_1_BTN ) 
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_EXTRA_TAB_BTN )
	{
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_DECO_TAB_BTN )
	{
		if( iValue1 == UID_KINDRED && iValue2 == 0 ) // 0은 남자
			return false;
		else if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_ETC_TAB_BTN )
	{
		if( iValue2 != SHOP_TAB_TYPE_NONE && iValue2 != SHOP_TAB_TYPE_ETC )
			return false;
		else if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_EVENT_GOODS_TAB_BTN )
	{
		if( iValue1 != ioEtcItem::EIT_ETC_SILVER_COIN && iValue1 != ioEtcItem::EIT_ETC_CLOVER && iValue1 != ioEtcItem::EIT_ETC_MILEAGE_COIN )
			return false;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_COSTUME_TAB_BTN )
	{
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_SECRET_TAB_BTN )
	{
		if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}

	return false;
}

// 상점 상품 가져오기 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::GetItemInfoVec(  OUT vNewShopItemInfoVec& rkVec, IN DWORD dwCurTab, IN DWORD dwCurSmallTab, IN int eFilterType, IN const char* szFind )
{
	NewShopDataCache* pCache = g_ShopData.GetCash( dwCurTab, dwCurSmallTab, dwCurTab == ID_DECO_TAB_BTN ? m_iDecoSexType : 0 );
	if( pCache )
	{
		for( vNewShopItemInfoVec::iterator iter = pCache->m_ShopDataList.begin(); iter != pCache->m_ShopDataList.end(); ++iter )
		{
			NewShopItemInfo kInfo = *iter;
		
			//소문자 변환
			std::string szName = kInfo.m_sName.c_str();
			std::transform( szName.begin(), szName.end(), szName.begin(), tolower );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->GetType() == ioLocalManager::LCT_KOREA )
			{
				//조합형 글자 비교(초성 비교 초함)
				if( !Help::Strstr( szName.c_str(), szFind ) )
					continue;
			}
			else
			{
				if( strstr( szName.c_str(), szFind ) == NULL )
					continue;
			}
			
			UpdateShopItemInfoByMyInfo( eFilterType, kInfo );

			rkVec.push_back( kInfo );
		}
	}
}

void NewShopWnd::UpdateShopItemInfoByMyInfo( IN int eFilterType, OUT NewShopItemInfo& kInfo )
{
	switch( kInfo.m_iItemType )			
	{
	case PRESENT_SOLDIER:
		{
			if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE )
			{
				if( g_MyInfo.GetClassArray( kInfo.m_iValue1 ) != -1 )
					kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
			}

			if( eFilterType != NSF_NEED_HIGH_GRADE && eFilterType != NSF_NEED_LOW_GRADE )
			{
				BuyCharNoLevelLimitEventUserNode *pEventNode = dynamic_cast<BuyCharNoLevelLimitEventUserNode*> ( g_MyInfo.GetEventUserMgr().GetEventUserNode( EVT_BUY_CHAR_NO_LEVEL_LIMIT ) );
				if( pEventNode && pEventNode->IsAliveUserNode() )
					kInfo.m_iNeedLevel = 0;
				else if( g_MyInfo.IsCanBuyItem( g_SetItemInfoMgr.GetSetInfoByClass( kInfo.m_iValue1 ) ) )
					kInfo.m_iNeedLevel = 0;
			}
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			if( g_MyInfo.GetGradeLevel() >= kInfo.m_iNeedLevel )
				kInfo.m_iNeedLevel = 0;
		}
		break;
	case PRESENT_DECORATION:
		{
			if( g_MyInfo.GetClassLevel( m_iDecoClassType, true ) >= kInfo.m_iNeedLevel )
				kInfo.m_iNeedLevel = 0;

			if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE )
			{
				int iInvenDecoType = ( m_iDecoClassType * 100000 ) + ( m_iDecoSexType * 1000 ) + kInfo.m_iValue1;
				if( g_MyInfo.IsSlotItem( iInvenDecoType, kInfo.m_iValue2 ) )
					kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
			}
		}
		break;
	case PRESENT_ETC_ITEM:
		{
			if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE )
			{
				ioUserEtcItem::ETCITEMSLOT kSlot;
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( pUserEtcItem )
				{
					if( pUserEtcItem->GetEtcItem( kInfo.m_iValue1, kSlot ) )
						kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
				}
			}
		}
		break;
	case PRESENT_COSTUME:
		{
			if( g_MyInfo.GetGradeLevel() >= kInfo.m_iNeedLevel )
				kInfo.m_iNeedLevel = 0;
		}
		break;
	}
}

void NewShopWnd::GetBonusPesoItemInfoVec( OUT vNewShopItemInfoVec &rvInfoVec )
{
	rvInfoVec.clear();

	GetItemInfoVec( rvInfoVec, ID_RECOMMEND_TAB_BTN, ID_SMALL_TAB_1_BTN, NSF_NONE, "" );
	GetItemInfoVec( rvInfoVec, ID_EXTRA_TAB_BTN, ID_SMALL_TAB_1_BTN,  NSF_NONE, "" );
	GetItemInfoVec( rvInfoVec, ID_DECO_TAB_BTN, ID_SMALL_TAB_1_BTN,  NSF_NONE, "" );
	GetItemInfoVec( rvInfoVec, ID_ETC_TAB_BTN, ID_SMALL_TAB_1_BTN,  NSF_NONE, "" );
	GetItemInfoVec( rvInfoVec, ID_COSTUME_TAB_BTN, ID_SMALL_TAB_1_BTN,  NSF_NONE, "" );

	if( rvInfoVec.empty() )
		return;

	vNewShopItemInfoVec::iterator iter = rvInfoVec.begin();
	while ( iter != rvInfoVec.end() )
	{
		NewShopItemInfo &rInfo = *iter;
		int iBonusPesoSize = rInfo.m_vBonusPeso.size();
		if( rInfo.GetBonusPeso( iBonusPesoSize - 1 ) > 0 )
		{
			++iter;
			rInfo.m_iSortPoint = 1000000000 - rInfo.GetBonusPeso( iBonusPesoSize - 1 ) ; // 보너스페소 순으로 정렬하기 위해서 NewShopInfoDefaultSort()는 적은순으로 정렬됨
		}
		else
		{
			iter = rvInfoVec.erase( iter );
		}
	}
	std::sort( rvInfoVec.begin() , rvInfoVec.end(), NewShopInfoDefaultSort() );
}

// 페이지 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::UpdatePage( int iNewPage )
{
	m_iCurPage = iNewPage;

	int iSize = m_vNewShopItemInfoVec.size();
	int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
	for( int i = 0 ; i < MAX_BUY_INFO_WND_BTN ; i++ )
	{
		NewShopItemInfoWnd *pItem = dynamic_cast<NewShopItemInfoWnd*> ( FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ) );
		if( !pItem )
			continue;

		int iCurArray = iStartArray + i;
		if( !COMPARE( iCurArray, 0, iSize ) )
		{
			// 초기화
			NewShopItemInfo kInfo;
			pItem->SetInfo( kInfo );
			pItem->SetSelect( false );
			pItem->SetDecoEquip( false );
			continue;
		}

		pItem->SetInfo( m_vNewShopItemInfoVec[iCurArray] );
		if( iCurArray == m_iSelectCurArray )
			pItem->SetSelect( true );
		else
			pItem->SetSelect( false );

		pItem->SetDecoEquip( false );
		if( m_dwCurTabID == ID_DECO_TAB_BTN )
		{
			NewShopItemInfo &rkInfo = m_vNewShopItemInfoVec[iCurArray];
			int iDecoTypeCode = ( rkInfo.m_iValue1*100000 ) + rkInfo.m_iValue2;
			for (int i = 0; i < MAX_DECO_EQUIP ; i++)
			{
				if( iDecoTypeCode == m_iEquipDecoTypeCodeArray[i] )
				{
					pItem->SetDecoEquip( true );
					continue;
				}
			}
		}
	}
}

// 현재 선택 정보 접근 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool NewShopWnd::GetCurItemInfo( OUT NewShopItemInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( !COMPARE( dwItemInfoID, ID_ITEM_INFO_WND_1_BTN, ID_ITEM_INFO_WND_8_BTN+1) )
		return false;

	int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
	int iCurArray   = iStartArray + ( dwItemInfoID - ID_ITEM_INFO_WND_1_BTN );
	int iSize       = m_vNewShopItemInfoVec.size();
	if( !COMPARE( iCurArray, 0, iSize ) )
		return false;

	rkInfo = m_vNewShopItemInfoVec[iCurArray];
	return true;
}

int NewShopWnd::GetCurrTab()
{
	return (int)m_dwCurTabID; 
}

int NewShopWnd::GetCurrSmallTab()
{
	return (int)m_dwCurSmallTabID;
}

int NewShopWnd::GetCurrPage()
{	
	return m_iSelectCurPage;
}

int NewShopWnd::GetCurrInfoBtn()
{	
	return m_iSelectCurArray - ( m_iSelectCurPage * PAGE_PER_BTN_NUM );
}

// 구입 버튼 클릭 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::OnBuyInfoBtnUp( DWORD dwItemInfoID, bool bReleaseDecoEquip /*= true */ )
{
	NewShopBuyWnd *pWnd = dynamic_cast<NewShopBuyWnd*> (FindChildWnd( ID_BUY_WND ) );
	if( !pWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s pWnd == NULL.", __FUNCTION__ );
		return;
	}

	NewShopItemInfo kInfo;
	if( !GetCurItemInfo( kInfo, dwItemInfoID ) )
	{
		LOG.PrintTimeAndLog( 0, "%s ItemInfo is error(%d).", __FUNCTION__, dwItemInfoID );
		return;
	}

	// select
	m_iSelectCurArray = 0;
	m_iSelectCurPage = 0;
	for ( int i = 0; i < MAX_BUY_INFO_WND_BTN ; i++ )
	{
		DWORD dwCurItemInfoID = ID_ITEM_INFO_WND_1_BTN + i;
		NewShopItemInfoWnd *pInfoWnd = dynamic_cast<NewShopItemInfoWnd*> ( FindChildWnd( dwCurItemInfoID ) );
		if( !pInfoWnd )
			continue;

		if( dwCurItemInfoID == dwItemInfoID )
		{
			pInfoWnd->SetSelect( true );
			int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
			m_iSelectCurArray = iStartArray + i;
			m_iSelectCurPage = m_iCurPage;

			// deco equip and unequip
			if( kInfo.m_iItemType == PRESENT_DECORATION )
			{
				if( pInfoWnd->IsDecoEquip() && bReleaseDecoEquip )
				{
					SetReleaseDecoEquip( true );
					SetDecoEquipArray( kInfo.m_iValue1, -1 );
					pInfoWnd->SetSelect( false );
					pInfoWnd->SetDecoEquip( false );
					m_iSelectCurArray = -1;
				}
				else
				{
					// 이전값 초기화
					int iDecoTypeCode = GetDecoEquipArray( kInfo.m_iValue1 );
					if( iDecoTypeCode != -1 )
					{
						for (int j = 0; j < MAX_BUY_INFO_WND_BTN ; j++)
						{
							DWORD dwCurItemInfoID = ID_ITEM_INFO_WND_1_BTN + j;
							NewShopItemInfoWnd *pInfoWnd = dynamic_cast<NewShopItemInfoWnd*> ( FindChildWnd( dwCurItemInfoID ) );
							if( !pInfoWnd )
								continue;

							NewShopItemInfo kInfo;
							GetCurItemInfo( kInfo, dwCurItemInfoID );
							if( iDecoTypeCode == ( ( kInfo.m_iValue1*100000 ) + kInfo.m_iValue2 ) )
							{
								pInfoWnd->SetDecoEquip( false );
								break;
							}
						}
					}

					int iEquipCode = g_MyInfo.GetEquipItemCode((m_iDecoClassType * 100000) + ( m_iDecoSexType * 1000) + kInfo.m_iValue1 );
					if( iEquipCode != -1 && iEquipCode != kInfo.m_iValue2 )
					{
						pInfoWnd->SetDecoEquip( true );
						SetDecoEquipArray( kInfo.m_iValue1, ( kInfo.m_iValue1*100000 ) + kInfo.m_iValue2 );
					}
				}
			}
		}
		else
		{
			pInfoWnd->SetSelect( false );
		}
	}

	HideChildWnd( ID_BOUGHT_WND );
	HideChildWnd( ID_SUPERGASHAPON_WND );
	HideChildWnd( ID_MULTI_SUPERGASHAPON_WND );
	HideChildWnd( ID_MULTI_SUPERGASHAPON_TOAST_POPUP_WND );

	pWnd->ShowWnd();
	pWnd->SetInfo( kInfo );
}

void NewShopWnd::SetBoughtMark( int iTabID, int iValue1, int iValue2 )
{
	int iSize = m_vNewShopItemInfoVec.size();
	for( int i=0; i < iSize; ++i )
	{
		NewShopItemInfo &kInfo = m_vNewShopItemInfoVec[i];
		if( kInfo.m_iValue1 != iValue1 )
			continue;
		if( iTabID == ID_DECO_TAB_BTN && kInfo.m_iValue2 != iValue2 )
			continue;

		if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE )
			kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
		break;
	}

	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll )
		pScroll->SetScrollPos( m_iCurPage );
}

void NewShopWnd::SetBoughtMsg( const char *szName, int iBuyPopupType )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), szName );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(3) );
	if( iBuyPopupType == ioEtcItem::BPT_LINK )
		g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(4), STR(5), STR(6), STR(7) );
	else if( iBuyPopupType == ioEtcItem::BPT_SINGLE )
		g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_OK, NULL, kPrinter, STR(8), STR(9), STR(10), STR(11) );
}

// 탭 이동관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::SetDirectTab( int iTabID, int iValue1, int iValue2, bool bAllUpdate /*= true */ )
{
	SetDirectSmallTab( iTabID, ID_SMALL_TAB_1_BTN, iValue1, iValue2, bAllUpdate );
}

void NewShopWnd::SetDirectSmallTab( int iTabID, int iSmallTabID, int iValue1, int iValue2, bool bAllUpdate /*= true */ )
{
	if( bAllUpdate )
	{		
		iwm_updatetab( iTabID, iSmallTabID, true, true, false );
	}
	else
	{
		UpdateTab( iTabID, false, iSmallTabID );
	}

	NewShopBuyWnd *pWnd = dynamic_cast<NewShopBuyWnd*> (FindChildWnd( ID_BUY_WND ) );
	if( !pWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s pWnd == NULL.", __FUNCTION__ );
		return;
	}

	bool bSet = false;
	int iSize = m_vNewShopItemInfoVec.size();
	for( int i=0; i < iSize; ++i )
	{
		NewShopItemInfo &kInfo = m_vNewShopItemInfoVec[i];
		if( kInfo.m_iValue1 != iValue1 )
			continue;
		if( iTabID == ID_DECO_TAB_BTN && kInfo.m_iValue2 != iValue2 )
			continue;

		int iNewPage = i / PAGE_PER_BTN_NUM;
		ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
		if( pScroll )
		{
			if( pScroll->GetMaxPos() < iNewPage )
				iNewPage = pScroll->GetMaxPos();
			pScroll->SetScrollPos( iNewPage );
		}
		bSet = true;
		OnBuyInfoBtnUp( ID_ITEM_INFO_WND_1_BTN + i - ( iNewPage*PAGE_PER_BTN_NUM ), false );
		break;
	}

	if( !bSet && bAllUpdate )
		OnBuyInfoBtnUp( ID_ITEM_INFO_WND_1_BTN, false );
}

void NewShopWnd::RefreshCurrTab()
{
	iwm_updatetab( GetCurrTab(), GetCurrSmallTab(), true, true, true, false );
}

void NewShopWnd::ShowADPage( bool bSecretShopMove /*= true */ )
{
	NewShopBuyWnd *pBuyWnd = dynamic_cast<NewShopBuyWnd*> ( FindChildWnd( ID_BUY_WND ) );
	if( pBuyWnd )
	{
		pBuyWnd->InitCharRotate();
		pBuyWnd->HideWnd();
	}

	HideChildWnd( ID_SEARCH_SHOW_BTN );
	HideChildWnd( ID_SEARCH_HIDE_BTN );
	HideChildWnd( ID_SEARCH_WND );
	HideChildWnd( ID_BUY_WND );
	HideChildWnd( ID_BOUGHT_WND );
	HideChildWnd( ID_SUPERGASHAPON_WND );
	HideChildWnd( ID_MULTI_SUPERGASHAPON_WND );
	HideChildWnd( ID_MULTI_SUPERGASHAPON_TOAST_POPUP_WND );

	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_HOME_BTN ) );
	if( pButton )
		pButton->SetInActive();

	m_bScreen = true;
	m_dwScreenCheckTime = FRAMEGETTIME();

	if( g_SpecialShopMgr.IsSpecialShopOpen() )
		SetChildActive( ID_SECRET_TAB_BTN );
	else
		SetChildInActive( ID_SECRET_TAB_BTN );

	if( g_SpecialShopMgr.IsSpecialShopOpen() && bSecretShopMove )
	{
		iwm_updatetab( ID_SECRET_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
	}
	else
	{
		m_dwCurTabID		= 0;
		m_dwSmallTabEndID	= 0;		

		HideChildWnd( ID_VERT_SCROLL );
		CheckRadioButton( ID_RECOMMEND_TAB_BTN, ID_SECRET_TAB_BTN, 0 );

		for (int i = 0; i < MAX_SMALL_TAB_BTN; ++i )
		{
			HideChildWnd( ID_SMALL_TAB_1_BTN + i );
		}
		for( int i = 0 ; i < MAX_BUY_INFO_WND_BTN ; ++i )
		{
			HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
		}
		ShowChildWnd( ID_START_WND );

		UpdateEventItemCnt();
	}
}

void NewShopWnd::ShowSoldierTab()
{
	if( !IsShow() )
		ShowWnd();

	iwm_updatetab( ID_SOLDIER_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

void NewShopWnd::ShowExtraItemTab()
{
	if( !IsShow() )
		ShowWnd();

	iwm_updatetab( ID_EXTRA_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

void NewShopWnd::ShowDecoItemTab()
{
	if( !IsShow() )
		ShowWnd();

	iwm_updatetab( ID_DECO_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

void NewShopWnd::ShowEtcItemTab()
{
	if( !IsShow() )
		ShowWnd();

	iwm_updatetab( ID_ETC_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

void NewShopWnd::ShowEventGoodsItemTab()
{
	if( !IsShow() )
		ShowWnd();

	iwm_updatetab( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

// UI코스튬 관련 (코스튬 탭)
void NewShopWnd::ShowCostumeItemTab()
{
	if( !IsShow() )
		ShowWnd();

	iwm_updatetab( ID_COSTUME_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

void NewShopWnd::ShowSecretShopItemTab()
{
	if( !IsShow() )
		ShowWnd();

	if( g_SpecialShopMgr.IsSpecialShopOpen() )
		return;

	iwm_updatetab( ID_COSTUME_TAB_BTN, ID_SMALL_TAB_1_BTN, true, true, true );
}

void NewShopWnd::SetTabAndItem( NewShopItemInfo &rkInfo )
{
	if( rkInfo.m_iItemType == PRESENT_SOLDIER )
		UpdateTab( ID_SOLDIER_TAB_BTN, false );
	else if( rkInfo.m_iItemType == PRESENT_EXTRAITEM_BOX )
		UpdateTab( ID_EXTRA_TAB_BTN, false );
	else if( rkInfo.m_iItemType == PRESENT_DECORATION )
		UpdateTab( ID_DECO_TAB_BTN, false );
	else if( rkInfo.m_iItemType == PRESENT_ETC_ITEM )
	{
		if( rkInfo.m_iValue2 == SHOP_TAB_TYPE_SOLDIER )
			UpdateTab( ID_SOLDIER_TAB_BTN, false );
		else if( rkInfo.m_iValue2 == SHOP_TAB_TYPE_EXTRA )
			UpdateTab( ID_EXTRA_TAB_BTN, false );
		else if( rkInfo.m_iValue2 == SHOP_TAB_TYPE_DECO )
			UpdateTab( ID_DECO_TAB_BTN, false );
		else if( rkInfo.m_iValue2 == SHOP_TAB_TYPE_COSTUME )
			UpdateTab( ID_COSTUME_TAB_BTN, false );
		else
			UpdateTab( ID_ETC_TAB_BTN, false );
	}
	else if( rkInfo.m_iItemType == PRESENT_COSTUME_BOX )
		UpdateTab( ID_COSTUME_TAB_BTN, false );
	else
	{
		//ID_RECOMMEND_TAB_BTN, ID_SECRET_TAB_BTN 탭은 다른 탭의 상품을 선별해서 보여주는 특별탭이기 때문에 이벤트 아이템 갯수를 세지 않는다.
		return;
	}

	int iSize = m_vNewShopItemInfoVec.size();
	for( int i=0; i < iSize; ++i )
	{
		NewShopItemInfo &rkCurInfo = m_vNewShopItemInfoVec[i];

		if( rkCurInfo.m_iItemType == rkInfo.m_iItemType && rkCurInfo.m_iValue1 == rkInfo.m_iValue1 &&rkCurInfo.m_iValue2 == rkInfo.m_iValue2 )
		{
			int iNewPage = i / PAGE_PER_BTN_NUM;
			ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
			if( pScroll )
			{
				if( pScroll->GetMaxPos() < iNewPage )
					iNewPage = pScroll->GetMaxPos();
				pScroll->SetScrollPos( iNewPage );
			}
			OnBuyInfoBtnUp( ID_ITEM_INFO_WND_1_BTN + i - ( iNewPage*PAGE_PER_BTN_NUM ) );
			return;
		}
	}
}

// 치장관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::SetDecoEquipArray( int iDecoType, int iDecoTypeCode )
{
	int iDecoTypeArray[MAX_DECO_EQUIP]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };

	for (int i = 0; i < MAX_DECO_EQUIP ; i++)
	{
		if( iDecoTypeArray[i] == iDecoType )
		{
			m_iEquipDecoTypeCodeArray[i] = iDecoTypeCode;
			break;
		}
	}
}

int NewShopWnd::GetDecoEquipArray( int iDecoType )
{
	int iDecoTypeArray[MAX_DECO_EQUIP]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };

	for (int i = 0; i < MAX_DECO_EQUIP ; i++)
	{
		if( iDecoTypeArray[i] == iDecoType )
			return m_iEquipDecoTypeCodeArray[i];
	}

	return -1;
}


void NewShopWnd::ReleaseDecoEquipArray()
{
	int iSize = m_vNewShopItemInfoVec.size();
	int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
	for( int i = 0 ; i < MAX_BUY_INFO_WND_BTN ; i++ )
	{
		NewShopItemInfoWnd *pItem = dynamic_cast<NewShopItemInfoWnd*> ( FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ) );
		if( !pItem )
			continue;

		int iCurArray = iStartArray + i;
		if( !COMPARE( iCurArray, 0, iSize ) )
		{
			// 초기화
			pItem->SetDecoEquip( false );
			continue;
		}

		if( iCurArray != m_iSelectCurArray )
		{
			pItem->SetDecoEquip( false );
		}
	}

	if( !COMPARE( m_iSelectCurArray, 0, iSize ) )
		return;
	NewShopItemInfo &rkInfo = m_vNewShopItemInfoVec[m_iSelectCurArray];
	int iDecoTypeCode = ( rkInfo.m_iValue1*100000 ) + rkInfo.m_iValue2;
	for (int i = 0; i < MAX_DECO_EQUIP ; i++)
	{
		if( m_iEquipDecoTypeCodeArray[i] != iDecoTypeCode )
			m_iEquipDecoTypeCodeArray[i] = -1;
	}
}

void NewShopWnd::SetDecoClassSex( int iDecoClassType, int iDecoSexType, bool bMyCharSex )
{
	m_iDecoClassType     = iDecoClassType;
	m_iDecoSexType       = iDecoSexType;
	m_bSetDecoCharInfo   = true;

	if( !bMyCharSex )
		return;

	int iCharArray = g_MyInfo.GetClassArray( m_iDecoClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_iDecoSexType  = g_MyInfo.GetCharacter( iCharArray ).m_sex-1;
}

void NewShopWnd::SetReserveDecoSex( int iDecoSexType )
{	
	m_iDecoReserveSexType	= iDecoSexType;
	m_bDecoReserve			= true;
}

void NewShopWnd::SetDecoClassSexByRecommendedOrder( int iDecoSexType, int iClassType )
{
	IntVec vMyClassTypeVec;
	if( iClassType == 0 )
	{
		NewShopBuyWnd *pBuyWnd = dynamic_cast<NewShopBuyWnd*> ( FindChildWnd( ID_BUY_WND ) );
		if( pBuyWnd )
			pBuyWnd->GetMyClassTypeVec( vMyClassTypeVec );
		if( !vMyClassTypeVec.empty() )
		{
			m_iDecoClassType = vMyClassTypeVec[0];
		}
	}
	else
	{
		m_iDecoClassType = iClassType;
	}

	m_iDecoSexType       = iDecoSexType;
	m_bSetDecoCharInfo   = true;
}

// 상점 매인탭에 이벤트 마크를 표기하기 위한 관련 함수들 ----------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::UpdateEventItemCnt()
{
	m_EventItemCounter.SetSoldierItem(GetSoldierEventItemCnt());
	m_EventItemCounter.SetDecoItem(GetDecoEventItemCnt());
	m_EventItemCounter.SetExtraItem(GetExtraItemEventItemCnt());
	m_EventItemCounter.SetEtcItem(GetEtcItemEventItemCnt());
	m_EventItemCounter.SetGoodsItem(GetEventGoodsItemEventItemCnt());
	m_EventItemCounter.SetCostumeItem(GetCostumeEventItemCnt());
}

int NewShopWnd::GetTabEventItemCnt( DWORD dwTabID )
{
	switch( dwTabID )
	{
	case ID_RECOMMEND_TAB_BTN:
	case ID_SECRET_TAB_BTN:
		//두 탭은 다른 탭의 상품을 선별해서 보여주는 특별탭이기 때문에 이벤트 아이템 갯수를 세지 않는다.
		return 0;
	case ID_SOLDIER_TAB_BTN:
		return m_EventItemCounter.GetSoldierItem();
	case ID_EXTRA_TAB_BTN:
		return m_EventItemCounter.GetExtraItem();
	case ID_DECO_TAB_BTN:
		return m_EventItemCounter.GetDecoItem();
	case ID_ETC_TAB_BTN:
		return m_EventItemCounter.GetEtcItem();
	case ID_EVENT_GOODS_TAB_BTN:
		return m_EventItemCounter.GetGoodsItem();
	case ID_COSTUME_TAB_BTN:
		return m_EventItemCounter.GetCostume();
	}

	return 0;
}

int NewShopWnd::GetSoldierEventItemCnt()
{
	int iEventCnt = 0;

	vNewShopItemInfoVec vInfoList;
	vInfoList.clear();		
	GetItemInfoVec( vInfoList, ID_SOLDIER_TAB_BTN, ID_SMALL_TAB_1_BTN, NSF_NONE, "" );
	
	if( vInfoList.empty() )
		return iEventCnt;

	vNewShopItemInfoVec::iterator iter = vInfoList.begin();
	while( iter != vInfoList.end() )
	{
		if( (*iter).m_iShopMarkType == SHOP_MARK_TYPE_EVENT || (*iter).m_iShopMarkType == SHOP_MARK_TYPE_DATE )
			iEventCnt++;

		++iter;
	}

	return iEventCnt;
}

int NewShopWnd::GetExtraItemEventItemCnt()
{
	int iEventCnt = 0;

	vNewShopItemInfoVec vInfoList;
	vInfoList.clear();

	GetItemInfoVec( vInfoList, ID_EXTRA_TAB_BTN, ID_SMALL_TAB_1_BTN, NSF_NONE, "" );

	if( vInfoList.empty() )
		return iEventCnt;

	vNewShopItemInfoVec::iterator iter = vInfoList.begin();
	while( iter != vInfoList.end() )
	{
		if( (*iter).m_iShopMarkType == SHOP_MARK_TYPE_EVENT || (*iter).m_iShopMarkType == SHOP_MARK_TYPE_DATE  )
			iEventCnt++;

		++iter;
	}

	return iEventCnt;
}

int NewShopWnd::GetDecoEventItemCnt()
{
	int iEventCnt = 0;

	vNewShopItemInfoVec vInfoList;
	vInfoList.clear();

	GetItemInfoVec( vInfoList, ID_DECO_TAB_BTN, ID_SMALL_TAB_1_BTN, NSF_NONE, "" );

	if( vInfoList.empty() )
		return iEventCnt;

	vNewShopItemInfoVec::iterator iter = vInfoList.begin();
	while( iter != vInfoList.end() )
	{
		if( (*iter).m_iShopMarkType == SHOP_MARK_TYPE_EVENT || (*iter).m_iShopMarkType == SHOP_MARK_TYPE_DATE )
			iEventCnt++;

		++iter;
	}

	return iEventCnt;
}

int NewShopWnd::GetEtcItemEventItemCnt()
{
	int iEventCnt = 0;

	vNewShopItemInfoVec vInfoList;
	vInfoList.clear();

	GetItemInfoVec( vInfoList, ID_ETC_TAB_BTN, ID_SMALL_TAB_1_BTN, NSF_NONE, "" );
	
	if( vInfoList.empty() )
		return iEventCnt;

	vNewShopItemInfoVec::iterator iter = vInfoList.begin();
	while( iter != vInfoList.end() )
	{
		if( (*iter).m_iShopMarkType == SHOP_MARK_TYPE_EVENT || (*iter).m_iShopMarkType == SHOP_MARK_TYPE_DATE )
			iEventCnt++;

		++iter;
	}

	return iEventCnt;
}

int NewShopWnd::GetEventGoodsItemEventItemCnt()
{
	return g_EventGoodsMgr.GetMaxGoods();
}

// UI코스튬 관련 (이벤트 아이템 개수 얻기)
int NewShopWnd::GetCostumeEventItemCnt()
{
	int iEventCnt = 0;

	vNewShopItemInfoVec vInfoList;
	vInfoList.clear();

	GetItemInfoVec( vInfoList, ID_COSTUME_TAB_BTN, ID_SMALL_TAB_1_BTN, NSF_NONE, "" );
	
	if( vInfoList.empty() )
		return iEventCnt;

	vNewShopItemInfoVec::iterator iter = vInfoList.begin();
	while( iter != vInfoList.end() )
	{
		if( (*iter).m_iShopMarkType == SHOP_MARK_TYPE_EVENT || (*iter).m_iShopMarkType == SHOP_MARK_TYPE_DATE )
			iEventCnt++;

		++iter;
	}

	return iEventCnt;
}

void NewShopWnd::GoodsSyncComplete( EventShopType eType, bool bShopClose, DWORD dwEtcItem /*= 0 */, int iValue1 /*= 0 */, int iValue2 /*= 0 */ )
{
	if( dwEtcItem == 0 && iValue1 == 0 && iValue2 == 0 )
	{
		switch( eType )
		{
		case EST_CLOVER:
			{
				ParseShopDataCache( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_1_BTN );

				if( GetCurrTab() ==  ID_EVENT_GOODS_TAB_BTN && GetCurrSmallTab() == ID_SMALL_TAB_1_BTN )
					SetDirectSmallTab( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_1_BTN, iValue1, iValue2 );
			}
			break;
		case EST_EVENT:
			{
				ParseShopDataCache( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_2_BTN );

				if( GetCurrTab() ==  ID_EVENT_GOODS_TAB_BTN && GetCurrSmallTab() == ID_SMALL_TAB_2_BTN )
					SetDirectSmallTab( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_2_BTN, iValue1, iValue2 );
			}
			break;
		case EST_MILEAGE:
			{
				ParseShopDataCache( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_3_BTN );

				if( GetCurrTab() ==  ID_EVENT_GOODS_TAB_BTN && GetCurrSmallTab() == ID_SMALL_TAB_3_BTN )
					SetDirectSmallTab( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_3_BTN, iValue1, iValue2 );
			}
			break;
		case EST_SECRET:			
			{
				ParseShopDataCache( ID_SECRET_TAB_BTN, ID_SMALL_TAB_1_BTN );

				if( GetCurrTab() ==  ID_SECRET_TAB_BTN && GetCurrSmallTab() == ID_SMALL_TAB_1_BTN )
					SetDirectSmallTab( ID_SECRET_TAB_BTN, ID_SMALL_TAB_1_BTN, iValue1, iValue2, false );
			}
			break;
		}
	}
	else
	{
		if( m_dwCurTabID == ID_EVENT_GOODS_TAB_BTN )
		{
			switch( dwEtcItem )
			{
			case ioEtcItem::EIT_ETC_CLOVER:
				{
					ParseShopDataCache( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_1_BTN );
					SetDirectSmallTab( m_dwCurTabID, ID_SMALL_TAB_1_BTN, iValue1, iValue2 );
				}
				break;
			case ioEtcItem::EIT_ETC_SILVER_COIN:
				{
					ParseShopDataCache( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_2_BTN );
					SetDirectSmallTab( m_dwCurTabID, ID_SMALL_TAB_2_BTN, iValue1, iValue2 );
				}			
				break;
			case ioEtcItem::EIT_ETC_MILEAGE_COIN:
				{
					ParseShopDataCache( ID_EVENT_GOODS_TAB_BTN, ID_SMALL_TAB_3_BTN );
					SetDirectSmallTab( m_dwCurTabID, ID_SMALL_TAB_3_BTN, iValue1, iValue2 );
				}			
				break;
			}
		}
		else if( m_dwCurTabID == ID_SECRET_TAB_BTN )
		{
			ParseShopDataCache( ID_SECRET_TAB_BTN, ID_SMALL_TAB_1_BTN );
			SetDirectSmallTab( ID_SECRET_TAB_BTN, ID_SMALL_TAB_1_BTN, iValue1, iValue2, false );
		}
	}
}


// 그외 정보 요청 관련 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NewShopWnd::SendGetCash( bool bMust )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
		return;

	if( bMust )
	{
		m_dwCashSendTime = REALGETTIME();

		SP2Packet kPacket( CTPK_GET_CASH );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );

		if( pLocal->IsMileage() )
		{
			SP2Packet kPacket( CTPK_GET_MILEAGE );
			TCPNetwork::SendToServer( kPacket );
		}
	}
	else
	{
		if( m_dwCashSendTime == 0 )
		{
			m_dwCashSendTime = REALGETTIME();
			return;
		}

		if( REALGETTIME() - m_dwCashSendTime < pLocal->GetCashSendTime() )
			return;

		SP2Packet kPacket( CTPK_GET_CASH );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );

		if( pLocal->IsMileage() )
		{
			SP2Packet kPacket( CTPK_GET_MILEAGE );
			TCPNetwork::SendToServer( kPacket );
		}

		m_dwCashSendTime = REALGETTIME();
	}
}

void NewShopWnd::SendRequestGashaponList()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsSendGashaponList() )
		return;

	if( m_bGashaponListRequest )
		return;

	m_bGashaponListRequest = true;

	MyInventoryWnd *pInventory = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( pInventory )
	{
		if( pInventory->IsGashaponListRequest() )
			return;
	}

	SP2Packet kPacket( CTPK_GASHAPON_LIST );
	kPacket << ioEtcItem::EIT_ETC_GASHAPON14;
	TCPNetwork::SendToServer( kPacket );
}

// 퀘스트 가이드 ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool NewShopWnd::QuestGuide( int iMTabID, int iSTabID, int iBtnValue1, int iBtnValue2, int iBtnValue3, int iBtnValue4, int iBtnValue5, float &rfXPos, float &rfYPos )
{
	// 현재 선택된 내용인지 확인
	NewShopBuyWnd *pBuyWnd = dynamic_cast<NewShopBuyWnd*> (FindChildWnd( ID_BUY_WND ) );
	if( pBuyWnd && pBuyWnd->QuestGuide( iBtnValue1, iBtnValue2, iBtnValue3, iBtnValue4, iBtnValue5, rfXPos, rfYPos ) )
	{
		return true;
	}

	// 버튼에 동일 내용이 있는지 확인
	int iSize = m_vNewShopItemInfoVec.size();
	int iStartArray = m_iCurPage * PAGE_PER_BTN_NUM;
	for( int i = 0 ; i < MAX_BUY_INFO_WND_BTN ; i++ )
	{
		NewShopItemInfoWnd *pItem = dynamic_cast<NewShopItemInfoWnd*> ( FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ) );
		if( !pItem ) continue;

		int iCurArray = iStartArray +i;
		if( COMPARE( iCurArray, 0, iSize ) )
		{
			if( CompareBtnInfo( m_vNewShopItemInfoVec[iCurArray], iBtnValue1, iBtnValue2, iBtnValue3 ) )
			{
				rfXPos = pItem->GetDerivedPosX() + 54;
				rfYPos = pItem->GetDerivedPosY() + 49;
				return true;
			}
		}		
	}

	if( m_dwCurTabID == (DWORD)iMTabID && m_dwCurSmallTabID == (DWORD)iSTabID ) 
	{
		// 탭 버튼이 정상적으로 선태되어있으면 스크롤 UI에 위치
		ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
		if( pScroll )
		{
			rfXPos = pScroll->GetDerivedPosX() + (pScroll->GetWidth() / 2);
			rfYPos = pScroll->GetDerivedPosY();
			return true;
		}
	}
	else if( m_dwCurTabID == (DWORD)iMTabID )	
	{
		ioWnd *pSubTab = FindChildWnd( iSTabID );
		if( pSubTab )
		{
			rfXPos = pSubTab->GetDerivedPosX() + (pSubTab->GetWidth() / 2);
			rfYPos = pSubTab->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		ioWnd *pMainTab = FindChildWnd( iMTabID );
		if( pMainTab )
		{
			rfXPos = pMainTab->GetDerivedPosX() + (pMainTab->GetWidth() / 2);
			rfYPos = pMainTab->GetDerivedPosY();
			return true;
		}
	}
	return false;
}

bool NewShopWnd::CompareBtnInfo( NewShopItemInfo &rkInfo, int iBtnValue1, int iBtnValue2, int iBtnValue3 )
{
	if( rkInfo.m_iItemType == iBtnValue1 )
	{
		if( rkInfo.m_iValue1 == iBtnValue2 )
		{
			if( rkInfo.m_iValue2 == iBtnValue3 )
				return true;
		}
	}
	return false;
}

void NewShopWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd::OnProcess( fTimePerSec );

	if( m_dwScreenCheckTime > 0 && m_dwScreenGapTime > 0 && m_dwScreenTime > 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_bScreen )
		{
			if( m_dwScreenCheckTime+m_dwScreenTime < dwCurTime )
			{
				m_bScreen = false;
				m_dwScreenCheckTime = dwCurTime;
			}
		}
		else
		{
			if( m_dwScreenCheckTime+m_dwScreenGapTime < dwCurTime )
			{
				m_bScreen = true;
				m_dwScreenCheckTime = dwCurTime;
			}
		}
	}

	if( g_EventGoodsMgr.IsSyncEventShop() )
	{
		SetChildActive( ID_EVENT_GOODS_TAB_BTN );
		if( !g_EventGoodsMgr.IsEventShopOpen() && !g_EventGoodsMgr.IsCloverShopOpen() )
		{
			SetChildInActive( ID_EVENT_GOODS_TAB_BTN );
		}
	}

	if( g_SpecialShopMgr.IsSpecialShopOpen() )
	{
		SetChildActive( ID_EVENT_GOODS_TAB_BTN );
	}

	SendGetCash();
	CheckBonusCash();
}

void NewShopWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pSelectedFrm )
		m_pSelectedFrm->Render( iXPos, iYPos );
	if( m_pMultiplyFrm && 0 < m_dwSmallTabEndID )
		m_pMultiplyFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
}

void NewShopWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { X_OFFSET = 594, X_OFFSET2 =578, Y_OFFSET = 15, X_TAB_DOT = 340, Y_TAB_DOT = 112, TAB_DOT_X_GET = 56, };

	// Title 출력
	if( m_pSelectedTitleFrm )
		m_pSelectedTitleFrm->Render( iXPos, iYPos );

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );

	if( m_pUITitle )
		m_pUITitle->PrintNormal( iXPos, iYPos );

	// Title 상에 존재하는 Child 출력
	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWnd *pWnd = NULL;
		pWnd = FindChildWnd( ID_CHARGE_GOLD_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		pWnd = FindChildWnd( ID_MY_ITEM_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		pWnd = FindChildWnd( ID_EXIT_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		pWnd = FindChildWnd( ID_HOME_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		pWnd = FindChildWnd( ID_HOME_TOOLTIP );
		if( pWnd )
			pWnd->RenderWnd();

		pWnd = FindChildWnd( ID_RAND_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}

	char szPeso[MAX_PATH]="";
	char szCash[MAX_PATH]="";
	char szStamp[MAX_PATH]="";
	char szClover[MAX_PATH]="";
	char szMileage[MAX_PATH]="";
	int  iMaxStamp = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_SILVER_COIN );
	int  iMaxClover = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_CLOVER );
	int  iMaxMileage = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_MILEAGE_COIN );

	Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szPeso, sizeof( szPeso ) );
	Help::ConvertNumToStrComma( g_MyInfo.GetTotalCash(), szCash, sizeof( szCash ) );
	Help::ConvertNumToStrComma( iMaxStamp, szStamp, sizeof( szStamp ) );
	Help::ConvertNumToStrComma( iMaxClover, szClover, sizeof( szClover ) );

	int iAdd_OffsetY = 0;
	if( iMaxMileage > 0 )
	{
		Help::ConvertNumToStrComma( iMaxMileage, szMileage, sizeof( szMileage ) );
		iAdd_OffsetY = 7;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );
	if( iMaxStamp > 0 )
	{
		kPrinter.SetTextColor( 130, 198, 255 );
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_SILVER_COIN );
		if( pEtcItem )
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(4), pEtcItem->GetName().c_str(), szStamp );
		else
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(3), szStamp );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_11, " / " );
	}
	if( iMaxClover > 0 )
	{
		kPrinter.SetTextColor( 130, 198, 255 );	
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_CLOVER );
		if( pEtcItem )
			kPrinter.AddTextPiece( FONT_SIZE_11, "%s : %s개", pEtcItem->GetName().c_str(), szClover );
		else
			kPrinter.AddTextPiece( FONT_SIZE_11, "클로버 : %s개", szClover );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_11, " / " );
	}
	kPrinter.SetTextColor( 130, 198, 255 );		
	kPrinter.AddTextPiece( FONT_SIZE_11, STR(1) );
	kPrinter.AddTextPiece( FONT_SIZE_11, szPeso );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_11, " / " );
	kPrinter.SetTextColor( 255, 188, 26 );		
	kPrinter.AddTextPiece( FONT_SIZE_11, STR(2) );
	kPrinter.AddTextPiece( FONT_SIZE_11, szCash );

	ioButton* pBonusCashBtn = dynamic_cast<ioButton*>(FindChildWnd( ID_BONUS_CASH_BTN ));
	if ( pBonusCashBtn && pBonusCashBtn->IsShow() )
	{
		//마일리지가 존재한다면
		if ( iMaxMileage <= 0 )
		{
			pBonusCashBtn->SetWndPos( pBonusCashBtn->GetXPos() - 2 , pBonusCashBtn->GetYPos() + 6 );
			pBonusCashBtn->RenderWnd();
			pBonusCashBtn->SetWndPos( pBonusCashBtn->GetXPos() + 2 , pBonusCashBtn->GetYPos() - 6 );
		}
		else
			pBonusCashBtn->RenderWnd();
		kPrinter.PrintFullText( iXPos + X_OFFSET2, iYPos + Y_OFFSET-iAdd_OffsetY, TAT_RIGHT );
	}
	else
		kPrinter.PrintFullText( iXPos + X_OFFSET, iYPos + Y_OFFSET - iAdd_OffsetY, TAT_RIGHT );
	kPrinter.ClearList();

	if( iMaxMileage > 0 )
	{
		iAdd_OffsetY = 6;
		kPrinter.SetTextColor( 115, 204, 0 );
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_MILEAGE_COIN );
		if( pEtcItem )
		{
			kPrinter.AddTextPiece( FONT_SIZE_11, "%s : %s개", pEtcItem->GetName().c_str(), szMileage );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(4), szMileage );
		}
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
	}

	kPrinter.PrintFullText( iXPos + X_OFFSET, iYPos + Y_OFFSET+iAdd_OffsetY, TAT_RIGHT );
	kPrinter.ClearList();


	// 이벤트 마크(ID_COSTUME_TAB_BTN, ID_SECRET_TAB_BTN 탭에는 이벤트 마크가 붙지 않는다.)	
	MainTabEventMarkRender(ID_SOLDIER_TAB_BTN);
	MainTabEventMarkRender(ID_EXTRA_TAB_BTN);
	MainTabEventMarkRender(ID_DECO_TAB_BTN);
	MainTabEventMarkRender(ID_ETC_TAB_BTN);
	MainTabEventMarkRender(ID_COSTUME_TAB_BTN);

	enum
	{
		X_BONUS_CASH_TOOLTIP_OFFSET	= 63,
		Y_BONUS_CASH_TOOLTIP_OFFSET	= 22,
	};
	ioWnd* pBonusCashToolTip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
	if ( pBonusCashToolTip && pBonusCashToolTip->IsShow() )
	{
		pBonusCashToolTip->SetWndPos( X_OFFSET2 - X_BONUS_CASH_TOOLTIP_OFFSET, Y_OFFSET-iAdd_OffsetY - Y_BONUS_CASH_TOOLTIP_OFFSET );
		pBonusCashToolTip->RenderWnd();
	}

	ioWnd *pWnd = NULL;
	pWnd = FindChildWnd( ID_MULTI_SUPERGASHAPON_TOAST_POPUP_WND );
	if( pWnd && pWnd->IsShow() )
		pWnd->RenderWnd();
}

void NewShopWnd::MainTabEventMarkRender( DWORD dwTabID )
{
	// 이벤트 마크
	ioRadioButton *pBtn = NULL;

	pBtn = dynamic_cast<ioRadioButton*>(FindChildWnd(dwTabID));
	if( pBtn )
	{
		if( GetTabEventItemCnt( dwTabID ) > 0 )
		{
			if( m_pEventMark )
			{
				int iNewXPos = pBtn->GetDerivedPosX();
				int iNewYPos = pBtn->GetDerivedPosY();

				m_pEventMark->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

				if( m_bScreen )
					m_pEventMark->Render( iNewXPos, iNewYPos, UI_RENDER_SCREEN, TFO_BILINEAR );
			}
		}
	}
}

void NewShopWnd::CheckBonusCash()
{
	ioButton* pBonusCashBtn = dynamic_cast<ioButton*>(FindChildWnd( ID_BONUS_CASH_BTN ));
	if ( !pBonusCashBtn )
		return;

	int iBonusCash = g_MyInfo.GetBonusCash();
	if ( iBonusCash <=0 )
	{
		if ( pBonusCashBtn->IsShow() )
			pBonusCashBtn->HideWnd();
		return;
	}
	else
	{
		if ( !pBonusCashBtn->IsShow() )
			pBonusCashBtn->ShowWnd();
	}

	if ( pBonusCashBtn->IsShow() && pBonusCashBtn->IsOver() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if ( pMouse )
			pMouse->SetToolTipCursor();
	}
	/*
	ioWnd* pBonusTooltip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
	if ( pBonusTooltip )
		pBonusTooltip->ShowWnd();
	return;
	*/

	if ( m_dwBonusCashTooltipCheckDuration == 0 )
		return;
	if ( m_dwBonusCashTooltipCheckStartTime + m_dwBonusCashTooltipCheckDuration > FRAMEGETTIME() )
		return;

	m_dwBonusCashTooltipCheckStartTime = FRAMEGETTIME();

	if ( g_BonusCashMgr.CheckAlarmExpireBonusCash() )
	{
		ioWnd* pBonusTooltip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
		if ( pBonusTooltip && !pBonusTooltip->IsShow() )
			pBonusTooltip->ShowWnd();
	}
	else
	{
		ioWnd* pBonusTooltip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
		if ( pBonusTooltip && pBonusTooltip->IsShow() )
			pBonusTooltip->HideWnd();
	}
}

void NewShopWnd::ShowToastPopup()
{
	ShowChildWnd( ID_MULTI_SUPERGASHAPON_TOAST_POPUP_WND );
}