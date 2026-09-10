#include "stdafx.h"

#include "ioEtcItem.h"
#include "ioBingo.h"
#include "ioDateHelp.h"
#include "ioPriateRullet.h"
#include "ioMatchingGame.h"
#include "ioDiceGame.h"

#include "GUI/ItemResellWnd.h"
#include "GUI/ItemCustomWnd.h"
#include "GUI/CustomTournamentCreateWnd.h"
#include "GUI/BingoWnd.h"
#include "GUI/SuperGashaponWnd.h"
#include "GUI/MultiSuperGashaponWnd.h"
#include "GUI/UsePresentEtcItemWnd.h"
#include "GUI/ItemRechargeWnd.h"
#include "GUI/MyInventoryInfoWnd.h"
#include "GUI/FishingInventoryWnd.h"
#include "GUI/IDChangeWnd.h"
#include "GUI/ChangeTradeStateWnd.h"
#include "GUI/ReinforceSelectWnd.h"
#include "GUI/CostumeCustomWnd.h"
#include "GUI/SelectExtraGashaponWnd.h"
#include "GUI/SelectGashaponWnd.h"
#include "GUI/ExcavationShovelWnd.h"
#include "GUI/DiceGameWnd.h"

ioEtcItem::ioEtcItem()
{
	m_dwType			= EIT_NONE;
	m_eUseType			= UT_NONE;
	m_iMaxUse			= 0;
	m_iBuyPopupType		= 0;
	m_iGroup			= GROUP_NONE;

	m_vValueList.reserve( 10 );
	m_vPesoList.reserve( 10 );
	m_vCashList.reserve( 10 );
	m_vBonusPesoList.reserve( 10 );
	m_vMainManualList.reserve( 10 );
	m_vSubManualList.reserve( 10 );
	m_vShopOrderList.reserve( 10 );
	m_vRecommandedShopOrderList.reserve( 10 );
	m_vShowPesoList.reserve( 10 );
	m_vShopNameList.reserve( 10 );
	m_vActiveList.reserve( 10 );
	m_vShopMarkTypeList.reserve( 10 );
	m_vShopLimitDateList.reserve( 10 );
	m_vShopTabTypeList.reserve( 10 );

	m_vCanMortmainList.reserve( 10 );
	m_vSubscriptionList.reserve( 10 );

	m_bOnlyBuyBtn = false;
	m_bOnlyGiftBtn = false;
}

ioEtcItem::~ioEtcItem()
{
	m_vValueList.clear();
	m_vPesoList.clear();
	m_vCashList.clear();
	m_vBonusPesoList.clear();
	m_vMainManualList.clear();
	m_vSubManualList.clear();
	m_vShopOrderList.clear();
	m_vRecommandedShopOrderList.clear();
	m_vShowPesoList.clear();
	m_vShopNameList.clear();
	m_vActiveList.clear();
	m_vShopMarkTypeList.clear();
	m_vShopLimitDateList.clear();
	m_vShopTabTypeList.clear();

	m_vCanMortmainList.clear();

	m_vSubscriptionList.clear();
}

void ioEtcItem::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH]="";
	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
	m_Name = szBuf;

	rkLoader.LoadString_e( "extra_name", "", szBuf, MAX_PATH );
	m_ExtraName = szBuf;

	m_dwType				= rkLoader.LoadInt_e( "type", 0 );
	m_eUseType				= ioEtcItem::UseType(m_dwType/ioEtcItem::USE_TYPE_CUT_VALUE);
	m_iMaxUse				= rkLoader.LoadInt_e( "max_use", 0);
	m_iSellPeso				= rkLoader.LoadInt_e( "sell_peso", 0 );
	m_iMortmainSellPeso		= rkLoader.LoadInt_e( "mortmain_sell_peso", 0 );	
	m_iExtraType			= rkLoader.LoadInt_e( "extra_type", 0 );
	m_iBuyPopupType			= rkLoader.LoadInt_e( "buy_popup_type", 0 );
	m_iGroup				= rkLoader.LoadInt_e( "group", GROUP_NONE );
	m_iSecretShopGroup		= rkLoader.LoadInt_e( "secret_group", GROUP_NONE );
	m_iInventoryManual		= rkLoader.LoadInt_e( "inventory_manual", 0 );
	m_iInventorySubManual	= rkLoader.LoadInt_e( "inventory_sub_manual", 0 );
	m_iInventoryOrder		= rkLoader.LoadInt_e( "inventory_order", 0 );
	m_iLimitChannelingType	= rkLoader.LoadInt_e( "limit_channeling_type", CNT_NONE );
	m_bCanExtendUse			= rkLoader.LoadBool_e( "extend_use", false );
	m_bCanPresentUse		= rkLoader.LoadBool_e( "present_use", false );
	m_bCanSell				= rkLoader.LoadBool_e( "can_sell", false );
	m_bBlockBuying			= rkLoader.LoadBool_e( "block_buying", false );
	m_bCanOnOff				= rkLoader.LoadBool_e( "can_onoff", false );
	m_bDecorationMaxCheck	= rkLoader.LoadBool_e( "decoration_max_check", false );
	m_bExtraItemMaxCheck	= rkLoader.LoadBool_e( "extraitem_max_check", false );
	m_bMedalItemMaxCheck	= rkLoader.LoadBool_e( "medalitem_max_check", false );
	

	char szKeyName[MAX_PATH]="";
	int  iKeyValue = -1;
	enum { MAX_LOOP = 100, };
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "value%d", i+1 );
		iKeyValue = rkLoader.LoadInt( szKeyName, -1 );
		if( iKeyValue == -1 )
			break;
		m_vValueList.push_back( iKeyValue );

#if defined( USE_GA )		
		int iSubCode = iKeyValue;
#endif

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "peso%d", i+1 );
		m_vPesoList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "cash%d", i+1 );
		m_vCashList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

#if defined( USE_GA )
		int iCash = rkLoader.LoadInt( szKeyName, 0 );
#endif

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "bonus_peso%d", i+1 );
		m_vBonusPesoList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );	
	
		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "main_manual%d", i+1 );
		m_vMainManualList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "sub_manual%d", i+1 );
		m_vSubManualList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "shop_order%d", i+1 );
		m_vShopOrderList.push_back( rkLoader.LoadInt( szKeyName, ORDER_DEFAULT ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "recommended_shop_order%d", i+1 );
		m_vRecommandedShopOrderList.push_back( rkLoader.LoadInt( szKeyName, ORDER_DEFAULT ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "show_peso%d", i+1 );
		m_vShowPesoList.push_back( rkLoader.LoadInt( szKeyName, 1 ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "shop_name%d", i+1 );
		rkLoader.LoadString( szKeyName, "", szBuf, MAX_PATH );
		ioHashString sShopName = szBuf;
		m_vShopNameList.push_back( sShopName );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "active%d", i+1 );
		m_vActiveList.push_back( rkLoader.LoadInt( szKeyName, 1 ) );

#if defined( USE_GA )
		// 상점 목록에 활성화 되어있는 품목만 넣도록 한다
		int iActive = rkLoader.LoadInt( szKeyName, 1 );
		if( iCash > 0 && iActive == 1 )
			g_App.SetGAEtcSubInfo( iSubCode, iCash );
#endif

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "shop_mark_type%d", i+1 );
		m_vShopMarkTypeList.push_back( rkLoader.LoadInt( szKeyName, 0 ) ); // SHOP_MARK_TYPE_NONE

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "shop_limit_date%d", i+1 );
		m_vShopLimitDateList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "shop_tab_type%d", i+1 );
		m_vShopTabTypeList.push_back( rkLoader.LoadInt( szKeyName, 0 ) ); // SHOP_TAB_TYPE_NONE

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "can_mortmain%d", i+1 );
		m_vCanMortmainList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "subscription_type%d", i+1 );
		m_vSubscriptionList.push_back( rkLoader.LoadInt( szKeyName, 0 ) );

		g_SaleMgr.LoadINI( true, rkLoader, ioSaleManager::IT_ETC, m_dwType, i , i );
	}

	rkLoader.LoadString_e( "icon_name", "", szBuf, MAX_PATH );
	m_IconName = szBuf;	
	rkLoader.LoadString_e( "subicon_name", "", szBuf, MAX_PATH );
	m_SubIconName = szBuf;

	rkLoader.LoadString_e( "alarm_ment1", "", szBuf, MAX_PATH );
	m_szAlarmMent1 = szBuf; 
	rkLoader.LoadString_e( "alarm_ment2", "", szBuf, MAX_PATH );
	m_szAlarmMent2 = szBuf; 
	rkLoader.LoadString_e( "alarm_ment3", "", szBuf, MAX_PATH );
	m_szAlarmMent3 = szBuf; 

#if defined( USE_GA )
	g_App.SetGAEtcMainInfo( m_dwType );
#endif

	int nOnlyBtnType = rkLoader.LoadInt_e( "buy_type", 0 );
	switch ( nOnlyBtnType )
	{
	case 1:
		m_bOnlyBuyBtn = true;
		break;
	case 2:
		m_bOnlyGiftBtn = true;
		break;
	}

	m_nGradeType = rkLoader.LoadInt_e( "grade_type", 0);
}

int ioEtcItem::GetValue( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vValueList.size() ) )
		return 0;

	return m_vValueList[iArray];
}

int ioEtcItem::GetPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vPesoList.size() ) )
		return 0;

	return m_vPesoList[iArray];
}

int ioEtcItem::GetCash( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vCashList.size() ) )
		return 0;

	return m_vCashList[iArray];
}

int ioEtcItem::GetBonusPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vBonusPesoList.size() ) )
		return 0;

	return m_vBonusPesoList[iArray];
}

int ioEtcItem::GetSubscriptionType( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vSubscriptionList.size() ) )
		return 0;

	return m_vSubscriptionList[iArray];
}

ioHashString ioEtcItem::GetShopName( int iArray )
{
	ioHashString sAddName;
	if( !COMPARE( iArray, 0, (int) m_vShopNameList.size() ) )
		return sAddName;

	return m_vShopNameList[iArray];
}

int ioEtcItem::GetMainManual( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vMainManualList.size() ) )
		return 0;

	return m_vMainManualList[iArray];
}

int ioEtcItem::GetSubManual( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vSubManualList.size() ) )
		return 0;

	return m_vSubManualList[iArray];
}

int ioEtcItem::GetShopOrder( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vShopOrderList.size() ) )
		return 0;

	return m_vShopOrderList[iArray];
}

int ioEtcItem::GetRecommandedShopOrder( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vRecommandedShopOrderList.size() ) )
		return 0;

	return m_vRecommandedShopOrderList[iArray];
}

int ioEtcItem::GetRecommandedShopOrderSize() const
{
	return (int)m_vRecommandedShopOrderList.size();
}

bool ioEtcItem::IsShowPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vShowPesoList.size() ) )
		return false;

	if( m_vShowPesoList[iArray] == 1 )
		return true;

	return false;
}

bool ioEtcItem::IsActive( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vActiveList.size() ) )
		return false;

	if( m_vActiveList[iArray] == 1 )
		return true;

	return false;
}

int ioEtcItem::GetShopMarkType( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vShopMarkTypeList.size() ) )
		return 0;

	return m_vShopMarkTypeList[iArray];
}

int ioEtcItem::GetLimitDate( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vShopLimitDateList.size() ) )
		return 0;

	return m_vShopLimitDateList[iArray];
}


int ioEtcItem::GetShopTabType( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vShopTabTypeList.size() ) )
		return 0;

	return m_vShopTabTypeList[iArray];
}

int ioEtcItem::GetShopTabTypeSize()
{
	return m_vShopTabTypeList.size();
}


bool ioEtcItem::IsValid() const
{
	if( !m_Name.IsEmpty() && m_dwType != 0 )
		return true;

	return false;
}

void ioEtcItem::OnSell( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	int iResellPeso;
	__int64 iMyMoney;
	rkPacket >> iResellPeso >> iMyMoney;
	g_MyInfo.SetMoney( iMyMoney );

	DWORD sInfo1Color;
	DWORD sInfo2Color;
	ioHashString sInfo1;
	ioHashString sInfo2;
	pUserEtcItem->GetUseInfo( iType, sInfo1, sInfo2, sInfo1Color, sInfo2Color );

	char szConvertNum[MAX_PATH] = "";
	ioHashString szIconName = GetIconName();
	ioHashString szSubIconName = GetSubIconName();
	Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

	ComplexStringPrinterVec vTitle;
	ComplexStringPrinterVec vDesc;

	ioComplexStringPrinter kTitle1;
	kTitle1.SetTextStyle( TS_NORMAL );
	kTitle1.SetBkColor( 0, 0, 0 );
	kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kTitle1.AddTextPiece( FONT_SIZE_17, STR(1), GetName().c_str(), sInfo1.c_str() );
	vTitle.push_back( kTitle1 );

	ioComplexStringPrinter kTitle2;
	kTitle2.SetTextStyle( TS_NORMAL );
	kTitle2.SetBkColor( 0, 0, 0 );
	kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kTitle2.AddTextPiece( FONT_SIZE_17, STR(2), szConvertNum );
	vTitle.push_back( kTitle2 );
	
	ioComplexStringPrinter kDesc1;
	kDesc1.SetTextStyle( TS_NORMAL );
	kDesc1.SetBkColor( 0, 0, 0 );	
	kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kDesc1.AddTextPiece( FONT_SIZE_12, STR(3), GetName().c_str(), sInfo1.c_str() );
	vDesc.push_back( kDesc1 );

	ioComplexStringPrinter kDesc2;
	kDesc2.SetTextStyle( TS_NORMAL );
	kDesc2.SetBkColor( 0, 0, 0 );
	kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kDesc2.AddTextPiece( FONT_SIZE_12, STR(4), szConvertNum );
	vDesc.push_back( kDesc2 );

	pUserEtcItem->DeleteEtcItem( iType );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
		pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_SELL, ItemRecvSellInfoWnd::ITEM_ETCITEM, szIconName, szSubIconName, vTitle, vDesc, -1, 0, false, false, GetGradeType() );
	}

#if defined( USE_GA )
	char chItemIndex[32]	= {0,};
	char chSlash[16]		= {0,};
	char chPostData[256]	= {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		sprintf_e( chItemIndex, "%d", iType );
		sprintf_e( chSlash, "%%2F" );
		sprintf_e( chPostData, "%sPESO_GET_BUY_ETC_ITEM%s%s", chSlash, chSlash, chItemIndex );
	}
	else
	{
		SafeSprintf( chItemIndex, sizeof(chItemIndex), "%1", iType );
		SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
		SafeSprintf( chPostData, sizeof(chPostData), "%1PESO_GET_BUY_ETC_ITEM%2%3", chSlash, chSlash, chItemIndex );
	}	

	// PESO_GET_BUY_ETC_ITEM
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Peso"
		, "Etc"
		, ""
		, iResellPeso
		, chPostData );
#endif

}

bool ioEtcItem::OnSellBtnPush( int iType, ioUserEtcItem *pUserEtcItem, ioWnd *pReturnWnd )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( !IsCanSell() )
		return false;

	int  iValue1 = 0;
	if( GetUseType() == UT_TIME )
	{
		int iHour = kItemSlot.GetUse() / 60;      
		iValue1 = iHour;
	}
	else if( GetUseType() == UT_DATE )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kItemSlot.GetYear(), kItemSlot.GetMonth(), kItemSlot.GetDay(), kItemSlot.GetHour(), kItemSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		enum { DAY_SEC = 86400 };
		int iDay    = iGapSec/DAY_SEC;
		iValue1 = max( 0, iDay );
	}
	else 
	{
		iValue1 = kItemSlot.GetUse();
	}

	DWORD sInfo1Color;
	DWORD sInfo2Color;
	ioHashString sInfo1;
	ioHashString sInfo2;
	pUserEtcItem->GetUseInfo( iType, sInfo1,sInfo2, sInfo1Color, sInfo2Color );

	int iResellPeso  = GetSellPeso() * iValue1;
	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

	if( iResellPeso < 0 )
		return false;

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, STR(1), GetName().c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, STR(2), sInfo1.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(5), szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, STR(6) );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( GetIconName() );
		ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( GetSubIconName() );
		pResellWnd->SetInfoAndShow( STR(7), pImg, pSubImg, 0, kPrinter, pReturnWnd, SELL_EDIT_NONE, ACST_SELL, false, GetGradeType() );
	}

	return true;
}

void ioEtcItem::SetCash( int iArray , int iCash )
{
	if( !COMPARE( iArray, 0, (int) m_vCashList.size() ) )
		return;

	m_vCashList[iArray] = iCash;
}

void ioEtcItem::SetPeso( int iArray , int iPeso )
{
	if( !COMPARE( iArray, 0, (int) m_vPesoList.size() ) )
		return;

	m_vPesoList[iArray] = iPeso;
}

void ioEtcItem::SetActive( int iArray , bool bActive )
{
	if( !COMPARE( iArray, 0, (int) m_vActiveList.size() ) )
		return;

	m_vActiveList[iArray] = bActive;
}

void ioEtcItem::SetShopOrder( int iArray, int iShopOrder )
{
	if( !COMPARE( iArray, 0, (int) m_vShopOrderList.size() ) )
		return;

	m_vShopOrderList[iArray] = iShopOrder;
}

void ioEtcItem::SetRecommandedShopOrder( int iArray, int iRecommandedShopOrder )
{
	if( !COMPARE( iArray, 0, (int) m_vRecommandedShopOrderList.size() ) )
		return;

	m_vRecommandedShopOrderList[iArray] = iRecommandedShopOrder;
}

void ioEtcItem::SetShopMarkType( int iArray, int iShopMarkType )
{
	if( !COMPARE( iArray, 0, (int) m_vShopMarkTypeList.size() ) )
		return;

	m_vShopMarkTypeList[iArray] = iShopMarkType;
}

void ioEtcItem::SetShopLimitDate( int iArray, int iLimitDate )
{
	if( !COMPARE( iArray, 0, (int) m_vShopLimitDateList.size() ) )
		return;

	m_vShopLimitDateList[iArray] = iLimitDate;
}

void ioEtcItem::SetCanMortmain( int iArray , bool bCanMortmain )
{
	if( !COMPARE( iArray, 0, (int) m_vCanMortmainList.size() ) )
		return;

	m_vCanMortmainList[iArray] = (int)bCanMortmain;
}

bool ioEtcItem::IsCanMortMain( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vCanMortmainList.size() ) )
		return false;

	if( m_vCanMortmainList[iArray] == 0 )
		return false;

	return true;
}

void ioEtcItem::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	int iUseCount = 1;
	ioUserEtcItem::ETCITEMSLOT kPrevSlot;
	if( pUserEtcItem->GetEtcItem( kSlot.m_iType, kPrevSlot ) )
		iUseCount = kPrevSlot.m_iValue1 - kSlot.m_iValue1;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType, iUseCount );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemWholeChat::ioEtcItemWholeChat()
{

}

ioEtcItemWholeChat::~ioEtcItemWholeChat()
{

}

void ioEtcItemWholeChat::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemWholeChat::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	if( kSlot.GetUse() == 0 )
	{
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemWholeChat::OnBuy()
{
}

bool ioEtcItemWholeChat::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(7), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[6].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[7].SetTextStyle( TS_NORMAL );
	kPrinter[7].SetBkColor( 0, 0, 0 );	
	kPrinter[7].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[7].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}

void ioEtcItemWholeChat::OnSell( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem )
{
	ioEtcItem::OnSell( iType, rkPacket, pUserEtcItem );

//	GameChatInput *pWnd = dynamic_cast<GameChatInput*> ( g_GUIMgr.FindWnd( GAME_CHAT_INPUT ) );
//	if( pWnd )
//		pWnd->SetChangeChatBtn( true );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGrowthDown::ioEtcItemGrowthDown()
{
}

ioEtcItemGrowthDown::~ioEtcItemGrowthDown()
{
}

void ioEtcItemGrowthDown::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemGrowthDown::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowGrowthDownWnd();
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExtraItemCompound::ioEtcItemExtraItemCompound()
{
}

ioEtcItemExtraItemCompound::~ioEtcItemExtraItemCompound()
{
}

void ioEtcItemExtraItemCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExtraItemCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	// 합성 창
	if( g_CompoundMgr.GetNeedLevel() > g_MyInfo.GetGradeLevel() )
	{
		char szHelp[MAX_PATH]="";
		g_LevelMgr.GetGradeName( g_CompoundMgr.GetNeedLevel(), szHelp, sizeof( szHelp ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szHelp );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowItemCompoundWnd( GetType() );
	}

	return true;
}

////////////
ioEtcItemExtraItemCompound2::ioEtcItemExtraItemCompound2()
{
}

ioEtcItemExtraItemCompound2::~ioEtcItemExtraItemCompound2()
{
}

void ioEtcItemExtraItemCompound2::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExtraItemCompound2::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	// 합성 창
	if( g_CompoundMgr.GetNeedLevel() > g_MyInfo.GetGradeLevel() )
	{
		char szHelp[MAX_PATH]="";
		g_LevelMgr.GetGradeName( g_CompoundMgr.GetNeedLevel(), szHelp, sizeof( szHelp ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szHelp );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowItemCompoundWnd( GetType() );
	}

	return true;
}

//////////////
ioEtcItemExtraItemCompound3::ioEtcItemExtraItemCompound3()
{
}

ioEtcItemExtraItemCompound3::~ioEtcItemExtraItemCompound3()
{
}

void ioEtcItemExtraItemCompound3::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExtraItemCompound3::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	// 합성 창
	if( g_CompoundMgr.GetNeedLevel() > g_MyInfo.GetGradeLevel() )
	{
		char szHelp[MAX_PATH]="";
		g_LevelMgr.GetGradeName( g_CompoundMgr.GetNeedLevel(), szHelp, sizeof( szHelp ), false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), szHelp );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowItemCompoundWnd( GetType() );
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMultipleCompound::ioEtcItemMultipleCompound()
{
}

ioEtcItemMultipleCompound::~ioEtcItemMultipleCompound()
{
}

void ioEtcItemMultipleCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}

////////////
ioEtcItemMultipleCompound2::ioEtcItemMultipleCompound2()
{
}

ioEtcItemMultipleCompound2::~ioEtcItemMultipleCompound2()
{
}

void ioEtcItemMultipleCompound2::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleCompound2::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}

///////////
ioEtcItemMultipleCompound3::ioEtcItemMultipleCompound3()
{
}

ioEtcItemMultipleCompound3::~ioEtcItemMultipleCompound3()
{
}

void ioEtcItemMultipleCompound3::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleCompound3::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}

///////////
ioEtcItemMultipleCompound4::ioEtcItemMultipleCompound4()
{
}

ioEtcItemMultipleCompound4::~ioEtcItemMultipleCompound4()
{
}

void ioEtcItemMultipleCompound4::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleCompound4::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}


///////////
ioEtcItemMultipleCompound5::ioEtcItemMultipleCompound5()
{
}

ioEtcItemMultipleCompound5::~ioEtcItemMultipleCompound5()
{
}

void ioEtcItemMultipleCompound5::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleCompound5::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMultipleEqualCompound::ioEtcItemMultipleEqualCompound()
{
}

ioEtcItemMultipleEqualCompound::~ioEtcItemMultipleEqualCompound()
{
}

void ioEtcItemMultipleEqualCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleEqualCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}

///////////////
ioEtcItemMultipleEqualCompound2::ioEtcItemMultipleEqualCompound2()
{
}

ioEtcItemMultipleEqualCompound2::~ioEtcItemMultipleEqualCompound2()
{
}

void ioEtcItemMultipleEqualCompound2::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleEqualCompound2::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}

///////////////
ioEtcItemMultipleEqualCompound3::ioEtcItemMultipleEqualCompound3()
{
}

ioEtcItemMultipleEqualCompound3::~ioEtcItemMultipleEqualCompound3()
{
}

void ioEtcItemMultipleEqualCompound3::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMultipleEqualCompound3::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowMultipleCompoundWnd( GetType() );
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingBait::ioEtcItemFishingBait()
{

}

ioEtcItemFishingBait::~ioEtcItemFishingBait()
{

}

void ioEtcItemFishingBait::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemFishingBait::OnBuy()
{
	FishingInventoryWnd *pFishingWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
	if( pFishingWnd && pFishingWnd->IsShow() )
	{
		pFishingWnd->UpdateBait();
	}
}

bool ioEtcItemFishingBait::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingMoonBait::ioEtcItemFishingMoonBait()
{

}

ioEtcItemFishingMoonBait::~ioEtcItemFishingMoonBait()
{

}

void ioEtcItemFishingMoonBait::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemFishingMoonBait::OnBuy()
{
	FishingInventoryWnd *pFishingWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
	if( pFishingWnd && pFishingWnd->IsShow() )
	{
		pFishingWnd->UpdateBait();
	}
}

bool ioEtcItemFishingMoonBait::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingRod::ioEtcItemFishingRod()
{

}

ioEtcItemFishingRod::~ioEtcItemFishingRod()
{

}

bool ioEtcItemFishingRod::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingMoonRod::ioEtcItemFishingMoonRod()
{

}

ioEtcItemFishingMoonRod::~ioEtcItemFishingMoonRod()
{

}

bool ioEtcItemFishingMoonRod::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFriendSlotExtend::ioEtcItemFriendSlotExtend()
{

}

ioEtcItemFriendSlotExtend::~ioEtcItemFriendSlotExtend()
{

}

bool ioEtcItemFriendSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

bool ioEtcItemFriendSlotExtend::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(7), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(2), GetName().c_str() );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(5), GetName().c_str() );

	kPrinter[7].SetTextStyle( TS_NORMAL );
	kPrinter[7].SetBkColor( 0, 0, 0 );	
	kPrinter[7].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[7].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCharSlotExtend::ioEtcItemCharSlotExtend()
{

}

ioEtcItemCharSlotExtend::~ioEtcItemCharSlotExtend()
{

}

bool ioEtcItemCharSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

bool ioEtcItemCharSlotExtend::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_ORANGE ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_15,STR(5), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_15,STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_15," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_15,STR(2), GetName().c_str() );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_15,STR(3) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_ORANGE ); // 주황
	kPrinter[4].AddTextPiece( FONT_SIZE_15,STR(4) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingSlotExtend::ioEtcItemFishingSlotExtend()
{

}

ioEtcItemFishingSlotExtend::~ioEtcItemFishingSlotExtend()
{

}

bool ioEtcItemFishingSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

bool ioEtcItemFishingSlotExtend::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_ORANGE ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_15,STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_15,STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_15," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_15,STR(3), GetName().c_str() );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_15,STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_ORANGE ); // 주황
	kPrinter[4].AddTextPiece( FONT_SIZE_15,STR(5) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemChangeID::ioEtcItemChangeID()
{

}

ioEtcItemChangeID::~ioEtcItemChangeID()
{

}

void ioEtcItemChangeID::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	ioHashString szNewName;
	rkPacket >> szNewName;

	TCPNetwork::MouseBusy( false );
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnChangeNameResultWnd(  szNewName );
	}

	IDChangeWnd *pWnd = dynamic_cast<IDChangeWnd*> ( g_GUIMgr.FindWnd( IDCHANGE_WND ) );
	if( pWnd )
		pWnd->SetInfoAndShow(szNewName);
	
	if( pUserEtcItem->GetEtcItem( kSlot.m_iType , kSlot ) )
	{
		kSlot.SetUse( true ); // 사용한것으로 체크
		pUserEtcItem->SetEtcItem( kSlot );
	}
	
	g_App.SetWebLogOutForLuncher( true );
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemChangeID::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;
	
	if( kItemSlot.IsUse() )
		return false;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowChangeNameWnd( GetType() );
	
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCustomSound::ioEtcItemCustomSound()
{

}

ioEtcItemCustomSound::~ioEtcItemCustomSound()
{

}

void ioEtcItemCustomSound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemCustomSound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( !kItemSlot.IsUse() )
		return false;
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBuyMortmainChar::ioEtcItemBuyMortmainChar()
{

}

ioEtcItemBuyMortmainChar::~ioEtcItemBuyMortmainChar()
{

}

void ioEtcItemBuyMortmainChar::SetUsed( ioUserEtcItem *pUserEtcItem )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR, kSlot ) )
		return;

	kSlot.AddUse( -1 );
	if( kSlot.GetUse() == 0 )
		pUserEtcItem->DeleteEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR );
	else
		pUserEtcItem->SetEtcItem( kSlot );

	return;
}

bool ioEtcItemBuyMortmainChar::AddEtcItem( ioUserEtcItem *pUserEtcItem, int iGradeLevel, int iBonusLevel )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL", __FUNCTION__ );
		return false;
	}

	if( iGradeLevel != iBonusLevel )
		return false;

	ioUserEtcItem::ETCITEMSLOT kEtcItemSlot;
	pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR, kEtcItemSlot );

	kEtcItemSlot.m_iType = ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR;   
	kEtcItemSlot.AddUse( 1 );
	kEtcItemSlot.SetUse( true );

	pUserEtcItem->AddEtcItem( kEtcItemSlot );
	return true;
}

bool ioEtcItemBuyMortmainChar::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	g_GUIMgr.ShowWnd( MORTMAIN_CHAR_BUY_WND );
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemGuildCreate::ioEtcItemGuildCreate()
{

}

ioEtcItemGuildCreate::~ioEtcItemGuildCreate()
{

}

void ioEtcItemGuildCreate::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );
	// 사용 처리는 길드생성 패킷이 따로 도착하므로 여기서는 사용한것만 처리

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemGuildCreate::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	// 길드 생성 UI 
	g_GUIMgr.ShowWnd( GUILD_CREATE_WND );
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemGuildMarkChange::ioEtcItemGuildMarkChange()
{

}

ioEtcItemGuildMarkChange::~ioEtcItemGuildMarkChange()
{

}

void ioEtcItemGuildMarkChange::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );
	// 사용 처리는 길드마크변경 패킷이 따로 도착하므로 여기서는 사용한것만 처리

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemGuildMarkChange::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	g_App.OpenGuildMarkWnd();
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemGuildNameChange::ioEtcItemGuildNameChange()
{

}

ioEtcItemGuildNameChange::~ioEtcItemGuildNameChange()
{

}

void ioEtcItemGuildNameChange::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );
	// 사용 처리는 길드명변경 패킷이 따로 도착하므로 여기서는 사용한것만 처리

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemGuildNameChange::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	// 길드와 길드장 체크
	if( !g_GuildInfo.IsGuild() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}
	else if( !g_GuildInfo.IsGuildMaster() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	// 길드명 변경 UI
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowChangeNameWnd( GetType() );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPesoBonus::ioEtcItemPesoBonus()
{

}

ioEtcItemPesoBonus::~ioEtcItemPesoBonus()
{

}

bool ioEtcItemPesoBonus::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(4), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

 	kPrinter[1].SetTextStyle( TS_NORMAL );
 	kPrinter[1].SetBkColor( 0, 0, 0 );	
 	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
 	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );
 
 	kPrinter[2].SetTextStyle( TS_NORMAL );
 	kPrinter[2].SetBkColor( 0, 0, 0 );	
 	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(2) );
 
 	kPrinter[3].SetTextStyle( TS_NORMAL );
 	kPrinter[3].SetBkColor( 0, 0, 0 );	
 	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
 	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(3) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExpBonus::ioEtcItemExpBonus()
{

}

ioEtcItemExpBonus::~ioEtcItemExpBonus()
{

}

bool ioEtcItemExpBonus::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(4), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(3) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemSoldierPackage::ioEtcItemSoldierPackage()
{
	m_iActiveFilter	= AF_ACTIVE;
}

ioEtcItemSoldierPackage::~ioEtcItemSoldierPackage()
{

}

void ioEtcItemSoldierPackage::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	// 상황 전개
	CHARACTER kCharInfo;
	int iBonusPeso;
	__int64 iMyMoney;
	rkPacket >> kCharInfo >> iBonusPeso >> iMyMoney;

	int i;
	ITEMSLOTVec vItemSlot;	
	{   // 기본 치장 세팅 : 기본 치장에서 종족은 제외시킨다.
		g_DecorationPrice.GetPackageBuyDefaultList( kCharInfo, vItemSlot );
		int iDefaultSlotItem = vItemSlot.size();
		for(i = 0;i < iDefaultSlotItem;i++)
		{
			ITEMSLOT kSlot = vItemSlot[i];
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}
		vItemSlot.clear();
	}

	{   // 선택한 치장을 지급한다.		
		enum 
		{
			MAX_DECO_ITEM = 6,
		};
		int iDecoTypeList[MAX_DECO_ITEM]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
		for(i = 0;i < MAX_DECO_ITEM;i++)
		{
			ITEMSLOT kSlot = g_DecorationPrice.GetItemSlot( kCharInfo, iDecoTypeList[i] );
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}	
	}

	// 페소 적용
	g_MyInfo.SetMoney( iMyMoney );
	DWORD dwCharIndex = g_MyInfo.ApplyCharPackage( kCharInfo );
	if( dwCharIndex > 0 && pPlayStage )
	{
		pPlayStage->ReLoadCharInfo( dwCharIndex, kCharInfo );
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnPackageItemSoldierResult( kCharInfo, iBonusPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemSoldierPackage::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	// 일단 UI 띄우고, 실제 사용하려할때 막자.
	if( g_MyInfo.GetMortmainCharCount() + g_MyInfo.GetExerciseCharCount() >= g_MyInfo.GetCurMaxCharSlot() )
	{
		// 전부 영구 용병이거나 무료 용병이다.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	// 영구 용병 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( pWnd && pWnd->IsShow() )
	{
		if( m_dwType == ioEtcItem::EIT_ETC_SOLDIER_PACKAGE || m_dwType == ioEtcItem::EIT_ETC_SOLDIER_PACKAGE2 || COMPARE( m_dwType, ioEtcItem::EIT_ETC_SOLDIER_PACKAGE3, ioEtcItem::EIT_ETC_SOLDIER_PACKAGE10 + 1 ) )
			pWnd->ShowPackageItemWnd( PackageItemBuyWnd::PACKAGE_SOLDIER, m_dwType, m_iActiveFilter );
		else if( m_dwType == ioEtcItem::EIT_ETC_PREMIUM_SOLDIER_PACKAGE )
			pWnd->ShowPackageItemWnd( PackageItemBuyWnd::PACKAGE_PREMIUM_SOLDIER, m_dwType, m_iActiveFilter );
		else
			LOG.PrintTimeAndLog( 0, "%s Type Error %d", __FUNCTION__, m_dwType );

	}

	return true;
}

bool ioEtcItemSoldierPackage::IsRightSoldierCode( int iClassType )
{
	if( m_vSoldierCode.empty() )
		return true;

	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( iClassType == m_vSoldierCode[i] )
		{
			return true;
		}
	}

	return false;
}

void ioEtcItemSoldierPackage::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iLimitClassTypeNum	= rkLoader.LoadInt_e( "limit_class_num", 1 );
	m_iActiveFilter			= rkLoader.LoadInt_e( "limit_active_filter", static_cast<int>( AF_ACTIVE ) );

	// Soldier
	char szKeyName[MAX_PATH]="";
	int iMax  = rkLoader.LoadInt( "max_soldier", 0 );
	m_vSoldierCode.clear();
	m_vSoldierCode.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "soldier%d", i+1 );
		int iSoldierCode  = rkLoader.LoadInt( szKeyName, 0 );
		if( iSoldierCode != 0 )
			m_vSoldierCode.push_back( iSoldierCode );
	}

}
//////////////////////////////////////////////////////////////////////////
ioEtcItemDecorationPackage::ioEtcItemDecorationPackage()
{

}

ioEtcItemDecorationPackage::~ioEtcItemDecorationPackage()
{

}

void ioEtcItemDecorationPackage::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	// 상황 전개
	CHARACTER kCharInfo;
	int iBonusPeso;
	__int64 iMyMoney;
	rkPacket >> kCharInfo >> iBonusPeso >> iMyMoney;

	int i;
	ITEMSLOTVec vItemSlot;	
	{   // 기본 치장 세팅 : 기본 치장에서 종족은 제외시킨다.
		g_DecorationPrice.GetPackageBuyDefaultList( kCharInfo, vItemSlot );
		int iDefaultSlotItem = vItemSlot.size();
		for(i = 0;i < iDefaultSlotItem;i++)
		{
			ITEMSLOT kSlot = vItemSlot[i];
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}
		vItemSlot.clear();
	}

	{   // 선택한 치장을 지급한다.		
		enum 
		{
			MAX_DECO_ITEM = 6,
		};
		int iDecoTypeList[MAX_DECO_ITEM]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
		for(i = 0;i < MAX_DECO_ITEM;i++)
		{
			ITEMSLOT kSlot = g_DecorationPrice.GetItemSlot( kCharInfo, iDecoTypeList[i] );
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}	
	}

	// 페소 적용
	g_MyInfo.SetMoney( iMyMoney );
	DWORD dwCharIndex = g_MyInfo.ApplyCharPackage( kCharInfo );
	if( dwCharIndex > 0 && pPlayStage )
	{
		pPlayStage->ReLoadCharInfo( dwCharIndex, kCharInfo );
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnPackageItemDecoResult( kCharInfo, iBonusPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemDecorationPackage::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	if( g_MyInfo.GetExerciseCharCount() >= g_MyInfo.GetCharCount() )
	{
		// 보유중인 용병이 없다.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	// 치장 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ShowPackageItemWnd( PackageItemBuyWnd::PACKAGE_DECORATION, iType );

	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGoldMonsterCoin::ioEtcItemGoldMonsterCoin()
{

}

ioEtcItemGoldMonsterCoin::~ioEtcItemGoldMonsterCoin()
{

}

void ioEtcItemGoldMonsterCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemGoldMonsterCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	int iUseCount = 1;
	ioUserEtcItem::ETCITEMSLOT kPrevSlot;
	if( pUserEtcItem->GetEtcItem( kSlot.m_iType, kPrevSlot ) )
		iUseCount = kPrevSlot.m_iValue1 - kSlot.m_iValue1;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}	

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType, iUseCount );
}

void ioEtcItemGoldMonsterCoin::OnBuy()
{
	g_BattleRoomMgr.SyncMonsterCoin();
}

bool ioEtcItemGoldMonsterCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13, STR(5) );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[6].AddTextPiece( FONT_SIZE_13, STR(6) );

	kPrinter[7].SetTextStyle( TS_NORMAL );
	kPrinter[7].SetBkColor( 0, 0, 0 );	
	kPrinter[7].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[7].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[8].SetTextStyle( TS_NORMAL );
	kPrinter[8].SetBkColor( 0, 0, 0 );	
	kPrinter[8].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[8].AddTextPiece( FONT_SIZE_13, STR(7) );

	kPrinter[9].SetTextStyle( TS_NORMAL );
	kPrinter[9].SetBkColor( 0, 0, 0 );	
	kPrinter[9].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[9].AddTextPiece( FONT_SIZE_13, STR(8) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMonsterCoin::ioEtcItemMonsterCoin()
{

}

ioEtcItemMonsterCoin::~ioEtcItemMonsterCoin()
{

}

void ioEtcItemMonsterCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemMonsterCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	int iUseCount = 1;
	ioUserEtcItem::ETCITEMSLOT kPrevSlot;
	if( pUserEtcItem->GetEtcItem( kSlot.m_iType, kPrevSlot ) )
		iUseCount = kPrevSlot.m_iValue1 - kSlot.m_iValue1;

	pUserEtcItem->SetEtcItem( kSlot );          // 0개 이하가 되어도 삭제하지 않는다.

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType, iUseCount );
}

void ioEtcItemMonsterCoin::OnBuy()
{
	g_BattleRoomMgr.SyncMonsterCoin();
}

bool ioEtcItemMonsterCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13, " " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13, STR(6) );

	kPrinter[7].SetTextStyle( TS_NORMAL );
	kPrinter[7].SetBkColor( 0, 0, 0 );	
	kPrinter[7].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[7].AddTextPiece( FONT_SIZE_13,STR(7) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemGashapon::ioEtcItemGashapon()
{

}

ioEtcItemGashapon::~ioEtcItemGashapon()
{

}

void ioEtcItemGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlram    = false;
	int   iPresentPeso   = 0;

	rkPacket >> iPresentType;
	rkPacket >> iPresentValue1;
	rkPacket >> iPresentValue2;
	rkPacket >> iPresentValue3;
	rkPacket >> iPresentValue4;
	rkPacket >> bWholeAlram;
	rkPacket >> iPresentPeso;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnGashponResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlram, iPresentPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.m_iValue1 <= 0 )
		return false;

	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowGashponWnd( GetType(), GetType() );
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

void ioEtcItemGashapon::OnBuy()
{
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemRandomDecoM::ioEtcItemRandomDecoM()
{
}

ioEtcItemRandomDecoM::~ioEtcItemRandomDecoM()
{
}

void ioEtcItemRandomDecoM::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlram    = false;
	int   iPresentPeso   = 0;

	rkPacket >> iPresentType;
	rkPacket >> iPresentValue1;
	rkPacket >> iPresentValue2;
	rkPacket >> iPresentValue3;
	rkPacket >> iPresentValue4;
	rkPacket >> bWholeAlram;
	rkPacket >> iPresentPeso;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd )
	{
		pInvenWnd->OnRandomDecoResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlram, iPresentPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemRandomDecoM::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.m_iValue1 <= 0 )
		return false;

	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd )
	{
		pInvenWnd->ShowRandomDecoWnd( true );
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemRandomDecoW::ioEtcItemRandomDecoW()
{
}

ioEtcItemRandomDecoW::~ioEtcItemRandomDecoW()
{
}

void ioEtcItemRandomDecoW::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlram    = false;
	int   iPresentPeso   = 0;

	rkPacket >> iPresentType;
	rkPacket >> iPresentValue1;
	rkPacket >> iPresentValue2;
	rkPacket >> iPresentValue3;
	rkPacket >> iPresentValue4;
	rkPacket >> bWholeAlram;
	rkPacket >> iPresentPeso;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd )
	{
		pInvenWnd->OnRandomDecoResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlram, iPresentPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemRandomDecoW::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.m_iValue1 <= 0 )
		return false;

	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd )
	{
		pInvenWnd->ShowRandomDecoWnd( false );
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemPackage::ioEtcItemPackage()
{

}

ioEtcItemPackage::~ioEtcItemPackage()
{

}

bool ioEtcItemPackage::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

void ioEtcItemPackage::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemDecoUnderwearPackage::ioEtcItemDecoUnderwearPackage()
{

}

ioEtcItemDecoUnderwearPackage::~ioEtcItemDecoUnderwearPackage()
{

}

void ioEtcItemDecoUnderwearPackage::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	// 상황 전개
	CHARACTER kCharInfo;
	int iBonusPeso;
	__int64 iMyMoney;
	rkPacket >> kCharInfo >> iBonusPeso >> iMyMoney;

	int i;
	ITEMSLOTVec vItemSlot;	
	{   // 기본 치장 세팅 : 기본 치장에서 종족은 제외시킨다.
		g_DecorationPrice.GetPackageBuyDefaultList( kCharInfo, vItemSlot );
		int iDefaultSlotItem = vItemSlot.size();
		for(i = 0;i < iDefaultSlotItem;i++)
		{
			ITEMSLOT kSlot = vItemSlot[i];
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}
		vItemSlot.clear();
	}

	{   // 선택한 치장을 지급한다.		
		enum 
		{
			MAX_DECO_ITEM = 6,
		};
		int iDecoTypeList[MAX_DECO_ITEM]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
		for(i = 0;i < MAX_DECO_ITEM;i++)
		{
			ITEMSLOT kSlot = g_DecorationPrice.GetItemSlot( kCharInfo, iDecoTypeList[i] );
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}	
	}

	// 페소 적용
	g_MyInfo.SetMoney( iMyMoney );
	DWORD dwCharIndex = g_MyInfo.ApplyCharPackage( kCharInfo );
	if( dwCharIndex > 0 && pPlayStage )
	{
		pPlayStage->ReLoadCharInfo( dwCharIndex, kCharInfo );
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnPackageItemDecoResult( kCharInfo, iBonusPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemDecoUnderwearPackage::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	if( g_MyInfo.GetExerciseCharCount() >= g_MyInfo.GetCharCount() )
	{
		// 보유중인 용병이 없다.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	// 치장 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ShowPackageItemWnd( PackageItemBuyWnd::PACKAGE_DECO_UNDERWEAR, iType );

	}
	return true;
}
////////////////////////////////////////////////////////////////////////////
ioEtcItemExcavatingKit::ioEtcItemExcavatingKit()
{

}

ioEtcItemExcavatingKit::~ioEtcItemExcavatingKit()
{

}

void ioEtcItemExcavatingKit::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	TCPNetwork::MouseBusy( false );

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;
	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	pUserEtcItem->SetEtcItem( kSlot );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExcavatingKit::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

bool ioEtcItemExcavatingKit::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{	
	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTradeStateChange::ioEtcItemTradeStateChange()
{
}

ioEtcItemTradeStateChange::~ioEtcItemTradeStateChange()
{
}

void ioEtcItemTradeStateChange::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemTradeStateChange::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	// 선택 창
	ChangeTradeStateWnd *pStateWnd = dynamic_cast<ChangeTradeStateWnd*>(g_GUIMgr.FindWnd(CHANGE_TRADE_STATE_WND));
	if( pStateWnd )
	{
		if( pStateWnd->IsShow() )
			pStateWnd->HideWnd();

		pStateWnd->ShowWnd();
	}
	//

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemQuestEvent::ioEtcItemQuestEvent()
{

}

ioEtcItemQuestEvent::~ioEtcItemQuestEvent()
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemSilverCoin::ioEtcItemSilverCoin()
{

}

ioEtcItemSilverCoin::~ioEtcItemSilverCoin()
{

}

void ioEtcItemSilverCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemSilverCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemSilverCoin::OnBuy()
{
}

bool ioEtcItemSilverCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMileageCoin::ioEtcItemMileageCoin()
{

}

ioEtcItemMileageCoin::~ioEtcItemMileageCoin()
{

}

void ioEtcItemMileageCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemMileageCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemMileageCoin::OnBuy()
{
}

bool ioEtcItemMileageCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBattleRecordInit::ioEtcItemBattleRecordInit()
{

}

ioEtcItemBattleRecordInit::~ioEtcItemBattleRecordInit()
{

}

void ioEtcItemBattleRecordInit::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}
	// 전투 전적 초기화 - 승/패/킬/데스
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnRecordInitResultWnd( GetType() );
	}
	g_MyInfo.InitEtcItemUseBattleRecord();
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemBattleRecordInit::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowRecordInitWnd( GetType() );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemLadderRecordInit::ioEtcItemLadderRecordInit()
{

}

ioEtcItemLadderRecordInit::~ioEtcItemLadderRecordInit()
{

}

void ioEtcItemLadderRecordInit::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}
	// 진영 전적 초기화 - 승/패/킬/데스
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnRecordInitResultWnd( GetType() );
	}
	g_MyInfo.InitEtcItemUseLadderRecord();
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemLadderRecordInit::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowRecordInitWnd( GetType() );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemHeroRecordInit::ioEtcItemHeroRecordInit()
{

}

ioEtcItemHeroRecordInit::~ioEtcItemHeroRecordInit()
{

}

void ioEtcItemHeroRecordInit::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}
	// 래더 전적 초기화 - 승/패/킬/데스
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnRecordInitResultWnd( GetType() );
	}
	g_MyInfo.InitEtcItemUseHeroRecord();
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemHeroRecordInit::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowRecordInitWnd( GetType() );

	return true;
}
////////////////////////////////////////////////////////////////////////////
ioEtcItemSkeleton::ioEtcItemSkeleton()
{

}

ioEtcItemSkeleton::~ioEtcItemSkeleton()
{

}

void ioEtcItemSkeleton::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "skeleton_hm", "", szBuf, MAX_PATH );
	m_szHumanManSkl = szBuf;

	rkLoader.LoadString_e( "skeleton_hw", "", szBuf, MAX_PATH );
	m_szHumanWomanSkl = szBuf;
}

bool ioEtcItemSkeleton::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

bool ioEtcItemSkeleton::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}

bool ioEtcItemSkeleton::OnUseSwitchBtnPush( ioUserEtcItem *pUserEtcItem, bool bUseOn, ioPlayStage *pPlayStage )
{
	if( !IsCanOnOff() ) 
		return false;

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( GetType(), kItemSlot ) )
		return false;

	if( !pPlayStage )
		return false;

	bool bStateOK = false;
	ioBaseChar *pOwnerChar = pPlayStage->GetOwnerChar();
	if( pOwnerChar )
	{
		if( pOwnerChar->IsObejctEquipState() )
		{
			bStateOK = false;
		}
		else if( pOwnerChar->GetState() == CS_SPECIAL_MOTION || pOwnerChar->GetState() == CS_OBSERVER || pOwnerChar->GetState() == CS_VIEW || 
				 pOwnerChar->GetState() == CS_LOADING || pOwnerChar->GetState() == CS_READY || pOwnerChar->GetState() == CS_DELAY || pOwnerChar->GetState() == CS_ETCITEM_MOTION )
		{
			bStateOK = true;
		}
	}
	else
	{
		bStateOK = true;
	}

	if( bStateOK )
	{
		SP2Packet kPacket( CTPK_ETCITEM_SWITCH );
		kPacket << GetType() << bUseOn;
		TCPNetwork::SendToServer( kPacket );
		return true;
	}
	return false;
}

void ioEtcItemSkeleton::OnUseSwitch( SP2Packet &rkPacket, ioPlayStage *pPlayStage )
{
	if( !pPlayStage ) return;

	bool bUse;
	rkPacket >> bUse;

	ioHashString kName;
	rkPacket >> kName;
	if( kName == g_MyInfo.GetPublicID() )
	{
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( pUserEtcItem )
		{
			ioUserEtcItem::ETCITEMSLOT kItemSlot;
			if( pUserEtcItem->GetEtcItem( GetType(), kItemSlot ) )
			{
				kItemSlot.SetUse( bUse );
				pUserEtcItem->SetEtcItem( kItemSlot );
			}

			// 링크된 다른 아이템은 해제한다.
			if( bUse )
			{
				IntVec vLinkItem;
				vLinkItem.push_back( EIT_ETC_SKELETON_BIG );
				vLinkItem.push_back( EIT_ETC_SKELETON_BIGHEAD );
				vLinkItem.push_back( EIT_ETC_SKELETON_SMALL );
				for(int i = 0;i < (int)vLinkItem.size();i++)
				{
					if( GetType() == vLinkItem[i] ) continue;
					if( !pUserEtcItem->GetEtcItem( vLinkItem[i], kItemSlot ) ) continue;

					kItemSlot.SetUse( false );
					pUserEtcItem->SetEtcItem( kItemSlot );
				}
				vLinkItem.clear();
			}

			MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd( MY_INVENTORY_WND ));
			if( pMyInventoryWnd && pMyInventoryWnd->IsShow() )
				pMyInventoryWnd->UpdateTab( false, true, false );
		}		
	}	

	// 광장이면 용병에게 적용
	if( pPlayStage->GetModeType() == MT_TRAINING )
	{
		if( bUse )
		{
			// 교체
			pPlayStage->SkeletonEtcItemSync( kName, GetType() );
		}
		else
		{
			// 원래 용병으로 복구
			pPlayStage->SkeletonEtcItemSync( kName, 0 );
		}
	}
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemMotion::ioEtcItemMotion()
{
	m_bJoinUserSync = false;
	m_bDisplayMotion= false;
}

ioEtcItemMotion::~ioEtcItemMotion()
{

}

void ioEtcItemMotion::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "motion_hm", "", szBuf, MAX_PATH );
	m_szHumanManMotion = szBuf;

	rkLoader.LoadString_e( "motion_hw", "", szBuf, MAX_PATH );
	m_szHumanWomanMotion = szBuf;

	int i = 0;
	int iMaxMacro = rkLoader.LoadInt_e( "motion_macro_cnt", 0 );
	for(i = 0;i < iMaxMacro;i++)
	{		
		sprintf_e( szKey, "motion_macro_%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vMotionMacro.push_back( szBuf );
	}

	m_bDisplayMotion = rkLoader.LoadBool_e( "motion_display_set", false );

	m_MotionOption.clear();
	int iMaxOption = rkLoader.LoadInt_e( "motion_option_cnt", 0 );
	for(i = 0;i < iMaxOption;i++)
	{
		MotionOption kMotionOption;

		sprintf_e( szKey, "motion_option%d_type", i + 1 );
		kMotionOption.m_dwType = rkLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "motion_option%d_title", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kMotionOption.m_szTitle = szBuf;

		sprintf_e( szKey, "motion_option%d_ui_title", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kMotionOption.m_szUITitle = szBuf;

		sprintf_e( szKey, "motion_option%d_only_one", i + 1 );
		kMotionOption.m_bOnlyOneMotion = rkLoader.LoadBool( szKey, false );
		
		m_MotionOption.push_back( kMotionOption );
	}

	m_bJoinUserSync = rkLoader.LoadBool_e( "motion_join_sync", false );
}

void ioEtcItemMotion::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	pUserEtcItem->SetEtcItem( kSlot );

	int iMotionType = kSlot.m_iValue1;
	if( IsMotionOptionOnlyOne( iMotionType ) )
	{
		int iCurrentEtcSlot = pUserEtcItem->GetEtcItemCurrentSlot();
		for(int i = 0;i < iCurrentEtcSlot;i++)
		{
			ioUserEtcItem::ETCITEMSLOT kSlot;
			if( pUserEtcItem->GetEtcItemByArray( i, kSlot ) )
			{
				if( kSlot.m_iType == GetType() ) continue;

				if( COMPARE( kSlot.m_iType, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) || 
					COMPARE( kSlot.m_iType, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
				{
					if( kSlot.m_iValue1 == iMotionType )
					{
						kSlot.m_iValue1 = 0;
						pUserEtcItem->SetEtcItem( kSlot );
					}
				}			
			}
		}
	}

	if( iMotionType == MOTION_OPTION_10 )
	{
		g_UserInfoMgr.SetLeaderMotion( g_MyInfo.GetPublicID(), GetType() );
	}
}

void ioEtcItemMotion::GetDisplayMotion( DWORDVec &rkClassList )
{
	if( m_bDisplayMotion )
	{
		UserHeadquartersOption *pHeadquartersOption = g_MyInfo.GetUserHeadquartersOption();
		if( pHeadquartersOption )
		{
			for(int i = 0;i < MAX_DISPLAY_CNT;i++)
			{
				int iClassType = g_MyInfo.GetClassType( pHeadquartersOption->m_dwCharacterIndex[i] );
				if( iClassType == -1 ) continue;

				rkClassList.push_back( iClassType );
			}
		}
	}
}

int ioEtcItemMotion::GetMaxMotionOption()
{ 
	DWORDVec vClassList;
	GetDisplayMotion( vClassList );
	return m_MotionOption.size() + vClassList.size(); 
}

DWORD ioEtcItemMotion::GetMotionOptionArrayToID( int iArray )
{
	if( COMPARE( iArray, 0, (int)m_MotionOption.size() ) )
		return m_MotionOption[iArray].m_dwType;

	// 진열 용병
	DWORDVec vClassList;
	GetDisplayMotion( vClassList );
	int iNewArray = (int)iArray - m_MotionOption.size();
	if( COMPARE( iNewArray, 0, (int)vClassList.size() ) )
		return MOTION_OPTION_100 + vClassList[iNewArray];
	return 0;
}

ioHashString ioEtcItemMotion::GetMotionOptionTitle( DWORD dwType )
{
	int i = 0;
	for(i = 0;i < (int)m_MotionOption.size();i++)
	{
		MotionOption &rkMotionOption = m_MotionOption[i];
		if( rkMotionOption.m_dwType == dwType )
			return rkMotionOption.m_szTitle;
	}

	DWORDVec vClassList;
	GetDisplayMotion( vClassList );
	for(i = 0;i < (int)vClassList.size();i++)
	{
		if( MOTION_OPTION_100 + vClassList[i] == dwType )
		{
			return g_MyInfo.GetClassName( vClassList[i], g_MyInfo.GetCharPowerUpGradeByClassType( vClassList[i] ) );
		}
	}
	return "";
}

ioHashString ioEtcItemMotion::GetMotionOptionUITitle( DWORD dwType )
{
	int i = 0;
	for(i = 0;i < (int)m_MotionOption.size();i++)
	{
		MotionOption &rkMotionOption = m_MotionOption[i];
		if( rkMotionOption.m_dwType == dwType )
			return rkMotionOption.m_szUITitle;
	}

	DWORDVec vClassList;
	GetDisplayMotion( vClassList );
	for(i = 0;i < (int)vClassList.size();i++)
	{
		if( MOTION_OPTION_100 + vClassList[i] == dwType )
		{
			char szBuf[MAX_PATH] = "";
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), g_MyInfo.GetClassName( vClassList[i], g_MyInfo.GetCharPowerUpGradeByClassType( vClassList[i] ) ) );
			return szBuf;
		}
	}
	return "";
}

bool ioEtcItemMotion::IsMotionOptionOnlyOne( DWORD dwType )
{
	int i = 0;
	for(i = 0;i < (int)m_MotionOption.size();i++)
	{
		MotionOption &rkMotionOption = m_MotionOption[i];
		if( rkMotionOption.m_dwType == dwType )
			return rkMotionOption.m_bOnlyOneMotion;
	}

	DWORDVec vClassList;
	GetDisplayMotion( vClassList );
	for(i = 0;i < (int)vClassList.size();i++)
	{
		if( MOTION_OPTION_100 + vClassList[i] == dwType )
		{
			return true;
		}
	}
	return false;
}

bool ioEtcItemMotion::IsMotionMacro( const ioHashString &rkMacro )
{
	int iMaxMacro = m_vMotionMacro.size();
	for(int i = 0;i < iMaxMacro;i++)
	{
		if( m_vMotionMacro[i] == rkMacro )
			return true;
	}
	return false;
}

bool ioEtcItemMotion::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

void ioEtcItemMotion::OnBuy()
{	
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemCustomItemSkin::ioEtcItemCustomItemSkin()
{

}

ioEtcItemCustomItemSkin::~ioEtcItemCustomItemSkin()
{

}

void ioEtcItemCustomItemSkin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	
	TCPNetwork::MouseBusy( false );
	
	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}
	
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );

	// 스킨 처리 마무리
	ItemCustomMakeWnd *pCustomMakeWnd = dynamic_cast< ItemCustomMakeWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_WND ) );
	if( pCustomMakeWnd )
	{
		pCustomMakeWnd->CompleteCustomFileMake();
	}
}

bool ioEtcItemCustomItemSkin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		if( ioPlayMode::GetModeType() == MT_TRAINING || 
			( ioPlayMode::GetModeType() == MT_MYROOM && ioMyRoomMode::GetMyRoomType() == MRT_LOBBY ) )
		{
			ioUserEtcItem::ETCITEMSLOT kItemSlot;
			if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
				return false;

			g_GUIMgr.HideWnd( ITEM_CUSTOM_SELECT_WND );
			g_GUIMgr.ShowWnd( ITEM_CUSTOM_SELECT_WND );
			return true;
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 변경은 광장과 로비에서만 가능합니다." );
	}
	else
	{
		if( ioPlayMode::GetModeType() != MT_TRAINING || ioPlayMode::GetModeType() != MT_HEADQUARTERS || ioPlayMode::GetModeType() != MT_HOUSE )
		{
			ioUserEtcItem::ETCITEMSLOT kItemSlot;
			if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
				return false;

			g_GUIMgr.HideWnd( ITEM_CUSTOM_SELECT_WND );
			g_GUIMgr.ShowWnd( ITEM_CUSTOM_SELECT_WND );
			return true;
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 변경은 광장/개인본부/길드본부/훈련소에서만 가능합니다." );
	}

	return false;	
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemCustomItemSkinTest::ioEtcItemCustomItemSkinTest()
{

}

ioEtcItemCustomItemSkinTest::~ioEtcItemCustomItemSkinTest()
{

}

void ioEtcItemCustomItemSkinTest::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
}

bool ioEtcItemCustomItemSkinTest::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		if( ioPlayMode::GetModeType() == MT_MYROOM )
		{
			if( ioMyRoomMode::GetMyRoomType() == MRT_LOBBY )
			{
				ioUserEtcItem::ETCITEMSLOT kItemSlot;
				if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
					return false;

				g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
				g_GUIMgr.ShowWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
				return true;
			}
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else
	{
		if( ioPlayMode::GetModeType() != MT_TRAINING || ioPlayMode::GetModeType() != MT_HEADQUARTERS || ioPlayMode::GetModeType() != MT_HOUSE )
		{
			ioUserEtcItem::ETCITEMSLOT kItemSlot;
			if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
				return false;

			g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
			g_GUIMgr.ShowWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
			return true;
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 변경은 광장/개인본부/길드본부/훈련소에서만 가능합니다." );
	}

	return false;	
}

///////////////////////////////////////////////////////////////////////////
ioEtcItemBlock::ioEtcItemBlock()
{
}

ioEtcItemBlock::~ioEtcItemBlock()
{

}

void ioEtcItemBlock::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "block_macro", "", szBuf, MAX_PATH );
	m_szCommond = szBuf;

	m_vBlockItemInfoList.reserve( 5 );

	int iMaxCnt = rkLoader.LoadInt_e( "block_cnt", 0 );
	for(int i = 0;i < iMaxCnt;i++)
	{
		BlockItemInfo kInfo;

		wsprintf_e( szKey, "block_%d_struct", i+1 );
		kInfo.m_iBlockNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "block_%d_guid", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_szGuidEffect = szBuf;

		m_vBlockItemInfoList.push_back( kInfo );
	}
}

bool ioEtcItemBlock::IsBlockMacro( const ioHashString &rkMacro )
{
	if( m_szCommond == rkMacro )
		return true;

	return false;
}

void ioEtcItemBlock::OnBuy()
{
}

bool ioEtcItemBlock::GetBlockStructNum( IN int iIndex, OUT int &iStructNum )
{
	int iNewIndex = iIndex - 1;
	if( !COMPARE( iNewIndex, 0, (int)m_vBlockItemInfoList.size() ) )
		return false;
 
	iStructNum = m_vBlockItemInfoList[iNewIndex].m_iBlockNum;
	return true;
}

bool ioEtcItemBlock::GetBlockGuidEffect( IN int iIndex, OUT ioHashString &szEffect )
{
	int iNewIndex = iIndex - 1;
	if( !COMPARE( iNewIndex, 0, (int)m_vBlockItemInfoList.size() ) )
		return false;

	szEffect = m_vBlockItemInfoList[iNewIndex].m_szGuidEffect;
	return true;
}

int ioEtcItemBlock::GetPreBlockIndex( int iIndex )
{
	int iNewIndex = iIndex - 1;

	if( iNewIndex < 1 )
		return (int)m_vBlockItemInfoList.size();

	if( iNewIndex > (int)m_vBlockItemInfoList.size() )
		return 1;

	return iNewIndex;
}

int ioEtcItemBlock::GetNextBlockIndex( int iIndex )
{
	int iNewIndex = iIndex + 1;

	if( iNewIndex < 1 )
		return (int)m_vBlockItemInfoList.size();

	if( iNewIndex > (int)m_vBlockItemInfoList.size() )
		return 1;

	return iNewIndex;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExtraItemGrowthCatalyst::ioEtcItemExtraItemGrowthCatalyst()
{
}

ioEtcItemExtraItemGrowthCatalyst::~ioEtcItemExtraItemGrowthCatalyst()
{
}

void ioEtcItemExtraItemGrowthCatalyst::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExtraItemGrowthCatalyst::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowItemGrowthCatalystWnd( GetType() );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemLuckyCoin::ioEtcItemLuckyCoin()
{

}

ioEtcItemLuckyCoin::~ioEtcItemLuckyCoin()
{

}

void ioEtcItemLuckyCoin::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	int iMaxExchange = rkLoader.LoadInt_e( "max_exchange", 0 );
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	for(int i = 0;i < iMaxExchange;i++)
	{
		ExchangeData kExhcnageData;

		sprintf_e( szKey, "exchange%d_index", i + 1 );
		kExhcnageData.m_iExchangeIndex = rkLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "exchange%d_use_manual", i + 1 );
		kExhcnageData.m_iExchangeUseManual = rkLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "exchange%d_result_manual", i + 1 );
		kExhcnageData.m_iExchangeResultManual = rkLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "exchange%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kExhcnageData.m_szExchangeName = szBuf;

		sprintf_e( szKey, "exchange%d_result_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kExhcnageData.m_szExchangeResultName = szBuf;

		sprintf_e( szKey, "exchange%d_icon", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kExhcnageData.m_szExchangeIcon = szBuf;

		sprintf_e( szKey, "exchange%d_costume", i + 1 );
		kExhcnageData.m_bExchangeCostume = rkLoader.LoadBool( szKey, false );

		sprintf_e( szKey, "exchange%d_grade_type", i + 1 );
		kExhcnageData.m_nGradeType = rkLoader.LoadInt( szKey, 0 );

		//Tab Type
		sprintf_e( szKey, "exchange%d_tab_type", i + 1 );
		kExhcnageData.m_iExchangeTabType = rkLoader.LoadInt( szKey, 0 );
		
		sprintf_e( szKey, "exchange%d_sub_text", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kExhcnageData.m_szExchangeSubText = szBuf;

		sprintf_e( szKey, "exchange%d_sub_text2", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kExhcnageData.m_szExchangeSubText2 = szBuf;

		sprintf_e( szKey, "exchange%d_need_coin", i + 1 );
		kExhcnageData.m_iNeedCoin = rkLoader.LoadInt( szKey, 1 );

		m_vExchangeList.push_back( kExhcnageData );
	}

	enum { MAX_RADIO_NAME = 10 };
	ZeroMemory( szBuf, sizeof( szBuf ) );
	ZeroMemory( szKey, sizeof( szKey ) );
	m_szRadioNameList.clear();
	for ( int i = 0; i< MAX_RADIO_NAME ; ++i )
	{
		ioHashString szRadionName;
		sprintf_e( szKey, "radio_button%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		m_szRadioNameList.push_back( szBuf );
	}
}

void ioEtcItemLuckyCoin::GetExchangeUseData( int iArray, int &rExchangeIndex, int &rExchangeManual, ioHashString &rExchangeName, ioHashString &rExchangeIcon, bool &rExchangeCostume, int &rGradeType )
{
	if( !COMPARE( iArray, 0, (int)m_vExchangeList.size() ) )
		return;

	ExchangeData &rkExchangeData = m_vExchangeList[iArray];
	rExchangeIndex = rkExchangeData.m_iExchangeIndex;
	rExchangeManual= rkExchangeData.m_iExchangeUseManual;
	rExchangeName  = rkExchangeData.m_szExchangeName;
	rExchangeIcon  = rkExchangeData.m_szExchangeIcon;
	rExchangeCostume = rkExchangeData.m_bExchangeCostume;
	rGradeType = rkExchangeData.m_nGradeType;
}

void ioEtcItemLuckyCoin::GetExchangeUseTabData( int iArray, ioHashString &rExchangeSubText, ioHashString &rExchangeSubText2, int &rExchangeTabType, int& rExchangeNeedCoin )
{
	if( !COMPARE( iArray, 0, (int)m_vExchangeList.size() ) )
		return;

	ExchangeData &rkExchangeData = m_vExchangeList[iArray];
	rExchangeSubText = rkExchangeData.m_szExchangeSubText;
	rExchangeSubText2 = rkExchangeData.m_szExchangeSubText2;
	rExchangeTabType = rkExchangeData.m_iExchangeTabType;
	rExchangeNeedCoin = rkExchangeData.m_iNeedCoin;
}

void ioEtcItemLuckyCoin::GetExchangeResultData( int iExchangeIndex, int &rExchangeManual, ioHashString &rExchangeName, ioHashString &rExchangeIcon, bool &rExchangeCostume, int &rGradeType )
{
	for(int i = 0;i < (int)m_vExchangeList.size();i++)
	{
		ExchangeData &rkExchangeData = m_vExchangeList[i];
		if( rkExchangeData.m_iExchangeIndex != iExchangeIndex ) continue;

		rExchangeManual = rkExchangeData.m_iExchangeResultManual;
		rExchangeName   = rkExchangeData.m_szExchangeResultName;
		rExchangeIcon   = rkExchangeData.m_szExchangeIcon;
		rExchangeCostume = rkExchangeData.m_bExchangeCostume;
		rGradeType = rkExchangeData.m_nGradeType;
		break;
	}
}

void ioEtcItemLuckyCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemLuckyCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowItemLuckyCoinWnd( GetType() );
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCompoundEx::ioEtcItemCompoundEx()
{
	m_iCompoundSuccessRand = 0;
}

ioEtcItemCompoundEx::~ioEtcItemCompoundEx()
{
}

void ioEtcItemCompoundEx::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iCompoundSuccessRand = rkLoader.LoadInt_e( "success_rand", 0 );
}

void ioEtcItemCompoundEx::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemCompoundEx::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowItemCompoundExWnd( GetType() );
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRainbowMixer::ioEtcItemRainbowMixer()
{
	m_iSubResultManual = 0;
}

ioEtcItemRainbowMixer::~ioEtcItemRainbowMixer()
{
}

void ioEtcItemRainbowMixer::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_MixerList.clear();
	m_PatternList.clear();
	m_BackgroundList.clear();

	m_iSubResultManual = rkLoader.LoadInt_e( "inventory_sub_result_manual", 0 );

	int i = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	int iMaxMixerItem = rkLoader.LoadInt_e( "max_mixer_item", 0 );
	for(i = 0;i < iMaxMixerItem;i++)
	{
		MixerData kData;

		sprintf_e( szKey, "mixer_etc_item_code%d", i + 1 );
		kData.m_dwEtcItemCode = rkLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "mixer_equip_icon_name%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szEquipIcon = szBuf;

		sprintf_e( szKey, "mixer_release_icon_name%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szReleaseIcon = szBuf;

		m_MixerList.push_back( kData );
	}

	int iMaxPatternList = rkLoader.LoadInt_e( "max_pattern_list", 0 );
	for(i = 0;i < iMaxPatternList;i++)
	{
		PatternData kData;

		sprintf_e( szKey, "pattern_list%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strlen( szBuf ) > 0 )
		{
			StringVector vStringVector = ioStringConverter::Split( szBuf, "." );
			StringVector::iterator iter;
			for( iter = vStringVector.begin() ; iter!=vStringVector.end() ; ++iter )
			{
				int iValue = atoi( iter->c_str() );
				if( iValue == 0 ) continue;

				kData.m_vCodeList.push_back( iValue );
			}
		}

		sprintf_e( szKey, "pattern_present%d", i + 1 );
		kData.m_dwPresentCode = rkLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "pattern_present_icon%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szPresentIcon = szBuf;

		sprintf_e( szKey, "pattern_present_name%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kData.m_szPresentName = szBuf;

		m_PatternList.push_back( kData );
	}

	int iMaxBackground = rkLoader.LoadInt_e( "max_background", 0 );
	for(i = 0;i < iMaxBackground;i++)
	{
		sprintf_e( szKey, "background%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_BackgroundList.push_back( szBuf );
	}
}

void ioEtcItemRainbowMixer::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	int iEtcItemSize;
	rkPacket >> iEtcItemSize;
	for(int i = 0;i < iEtcItemSize;i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		rkPacket >> kSlot.m_iType;
		rkPacket >> kSlot.m_iValue1;
		rkPacket >> kSlot.m_iValue2;

		if( kSlot.m_iValue1 == 0 )
		{
			pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
		}
		else
		{
			pUserEtcItem->SetEtcItem( kSlot );
		}
		g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, kSlot.m_iType );
		g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, kSlot.m_iType );
	}
}

bool ioEtcItemRainbowMixer::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowRainbowMixerWnd( GetType() );
	}
	return true;
}

void ioEtcItemRainbowMixer::GetRandomBackGround( ioHashString &rReturnImage )
{
	int iBackCount = (int)m_BackgroundList.size();
	if( iBackCount > 0 )
	{
		int iIndex = rand()%iBackCount;
		rReturnImage = m_BackgroundList[iIndex];
	}
}

int ioEtcItemRainbowMixer::GetMixerData( int iArray, ioHashString &rReturnIcon, ioHashString &rReleaseIcon )
{
	if( !COMPARE( iArray, 0, (int)m_MixerList.size() ) )
		return 0;

	rReturnIcon = m_MixerList[iArray].m_szEquipIcon;
	rReleaseIcon = m_MixerList[iArray].m_szReleaseIcon;
	return m_MixerList[iArray].m_dwEtcItemCode;
}

void ioEtcItemRainbowMixer::GetPatternResultData( int iPresentIdx, ioHashString &rReturnIcon, ioHashString &rReturnName )
{
	for(int i = 0;i < (int)m_PatternList.size();i++)
	{
		PatternData &rkData = m_PatternList[i];
		if( rkData.m_dwPresentCode == (DWORD)iPresentIdx )
		{
			rReturnIcon = rkData.m_szPresentIcon;
			rReturnName = rkData.m_szPresentName;
			break;
		}
	}
}

int ioEtcItemRainbowMixer::GetPatternPresent( DWORDVec &rEquipList )
{
	for(int i = 0;i < (int)m_PatternList.size();i++)
	{
		PatternData &rkData = m_PatternList[i];
		if( rkData.m_vCodeList.size() == rEquipList.size() )
		{
			for(int a = 0;a < (int)rkData.m_vCodeList.size();a++)
			{
				bool bOK = false;
				for(int b = 0;b < (int)rEquipList.size();b++)
				{
					if( rkData.m_vCodeList[a] == rEquipList[b] )
					{
						bOK = true;
						break;
					}
				}

				if( !bOK ) break;

				// 모두 동일하면
				if( a == (int)rkData.m_vCodeList.size() - 1 )
					return rkData.m_dwPresentCode - 1;
			}
		}
	}
	return -1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemLostSagaMixer::ioEtcItemLostSagaMixer()
{
}

ioEtcItemLostSagaMixer::~ioEtcItemLostSagaMixer()
{
}

void ioEtcItemLostSagaMixer::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	int iEtcItemSize;
	rkPacket >> iEtcItemSize;
	for(int i = 0;i < iEtcItemSize;i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		rkPacket >> kSlot.m_iType;
		rkPacket >> kSlot.m_iValue1;
		rkPacket >> kSlot.m_iValue2;

		if( kSlot.m_iValue1 == 0 )
		{
			pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
		}
		else
		{
			pUserEtcItem->SetEtcItem( kSlot );
		}
		g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, kSlot.m_iType );
		g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, kSlot.m_iType );
	}
}

bool ioEtcItemLostSagaMixer::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowLostSagaMixerWnd( GetType() );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemTimeGashapon::ioEtcItemTimeGashapon()
{
	m_bSequenceOrder = false;
	m_bRealTimeCheck = false;
	m_iRepeatMinute	 = 0;
}

ioEtcItemTimeGashapon::~ioEtcItemTimeGashapon()
{
}

void ioEtcItemTimeGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_bRealTimeCheck = rkLoader.LoadBool_e( "real_time_check", false );
	m_iRepeatMinute  = rkLoader.LoadInt_e( "repeat_minute", 0 );
	m_bSequenceOrder = rkLoader.LoadBool_e( "sequence_order", false );

	m_ExtendDataList.clear();
	char szKey[MAX_PATH] = "";
	int iMaxExtendData = rkLoader.LoadInt_e( "max_check_minute", 0 );
	if( iMaxExtendData == 0 )
		m_bSequenceOrder = false;

	for( int i = 0; i < iMaxExtendData; ++i )
	{
		ExtendData kData;
		sprintf_e( szKey, "check_minute%d", i + 1 );
		kData.m_iUseMinute = rkLoader.LoadInt( szKey, 0 );		

		sprintf_e( szKey, "check_etc_item_type%d", i + 1 );
		kData.m_iMagicCode = rkLoader.LoadInt( szKey, 0 );

		m_ExtendDataList.push_back( kData );
	}
}

DWORD ioEtcItemTimeGashapon::GetExtendGashaponType( ioUserEtcItem *pUserEtcItem )
{
	if( m_ExtendDataList.empty() )
		return GetType();

	if( !pUserEtcItem )
		return GetType();

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( GetType(), kItemSlot ) )
		return GetType();

	int iPresentCode = GetType();

	if( m_bSequenceOrder )
	{
		int iState = kItemSlot.GetDateExcludeValue3State();
		if( COMPARE( iState, 0, (int)m_ExtendDataList.size() ) )
			iPresentCode = m_ExtendDataList[iState].m_iMagicCode;
	}
	else
	{
		int iUseMinuteSize = m_ExtendDataList.size();
		int iKeepMinute = m_iRepeatMinute - kItemSlot.GetDateExcludeValue2();
		for(int i = 0;i < iUseMinuteSize;i++)
		{
			ExtendData &rkData = m_ExtendDataList[i];
			if( iKeepMinute >= rkData.m_iUseMinute )
			{
				iPresentCode = rkData.m_iMagicCode;
			}
		}
	}

	return iPresentCode;
}

bool ioEtcItemTimeGashapon::IsBtnActive( ioUserEtcItem *pUserEtcItem )
{
	if( !pUserEtcItem )
		return false;

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( GetType(), kItemSlot ) )
		return false;
	
	if( m_bSequenceOrder )
	{
		int iState = kItemSlot.GetDateExcludeValue3State();
		if(  kItemSlot.GetDateExcludeValue3Time() == 0 )
		{
			return true;
		}
	}
	else
	{
		if( m_ExtendDataList.empty() )
		{
			if( kItemSlot.GetDateExcludeValue2() == 0 )
				return true;
		}
		else
		{
			int iUseMinuteSize = m_ExtendDataList.size();
			int iKeepMinute    = m_iRepeatMinute - kItemSlot.GetDateExcludeValue2();
			for(int i = 0;i < iUseMinuteSize;i++)
			{
				ExtendData &rkData = m_ExtendDataList[i];
				if( iKeepMinute >= rkData.m_iUseMinute )
				{
					return true;
				}
			}
		}
	}

	return false;
}

int ioEtcItemTimeGashapon::GetSequenceDate( int iIndex )
{
	if( COMPARE( iIndex, 0, (int)m_ExtendDataList.size() ) )
		return m_ExtendDataList[iIndex].m_iUseMinute;
	
	return 0;
}

void ioEtcItemTimeGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( m_bSequenceOrder )
	{
		if( kSlot.GetDateExcludeValue3State() == 0 && kSlot.GetDateExcludeValue3Time() == 0 )
		{
			pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
		}
		else
		{
			pUserEtcItem->SetEtcItem( kSlot );
		}
	}
	else
	{
		if( m_ExtendDataList.empty() )
		{
			if( kSlot.m_iValue1 == 0 )
			{
				pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
			}
			else
			{
				pUserEtcItem->SetEtcItem( kSlot );
			}
		}
		else
		{
			// 사용시 즉시 삭제 아이템
			pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
		}
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlram    = false;
	int   iPresentPeso   = 0;

	rkPacket >> iPresentType;
	rkPacket >> iPresentValue1;
	rkPacket >> iPresentValue2;
	rkPacket >> iPresentValue3;
	rkPacket >> iPresentValue4;
	rkPacket >> bWholeAlram;
	rkPacket >> iPresentPeso;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnGashponResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlram, iPresentPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemTimeGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.m_iValue1 <= 0 )
		return false;

	if( m_bSequenceOrder )
	{
		int iState    = kItemSlot.GetDateExcludeValue3State();
		int iRealTime = kItemSlot.GetDateExcludeValue3Time();
		if( iRealTime > 0 )
		{
			DWORD sInfo1Color;
			DWORD sInfo2Color;
			ioHashString sInfo1;
			ioHashString sInfo2;
			pUserEtcItem->GetUseInfo( iType, sInfo1, sInfo2, sInfo1Color, sInfo2Color );

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), sInfo2.c_str() );

			// UI 갱신
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, false );
			}
			return false;
		}
	}
	else
	{
		if( !IsExtendData() && kItemSlot.GetDateExcludeValue2() > 0 )
		{
			DWORD sInfo1Color;
			DWORD sInfo2Color;
			ioHashString sInfo1;
			ioHashString sInfo2;
			pUserEtcItem->GetUseInfo( iType, sInfo1, sInfo2, sInfo1Color, sInfo2Color );

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), sInfo2.c_str() );

			// UI 갱신
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, false );
			}
			return false;
		}
		else if( IsExtendData() )
		{
			ExtendData &rkData = m_ExtendDataList[0];        // 최소 첫번째 시간이 지나야 가능
			int iKeepMinute = m_iRepeatMinute - kItemSlot.GetDateExcludeValue2();
			if( iKeepMinute < rkData.m_iUseMinute )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), rkData.m_iUseMinute );

				// UI 갱신
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->IsShow() )
				{
					pInvenWnd->UpdateTab( false, false );
				}
				return false;
			}
		}
	}

	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowGashponWnd( GetType(), GetExtendGashaponType( pUserEtcItem ) );
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	kPacket << kItemSlot.GetDateExcludeValue2();       // 서버와 시간 체크
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGoldBox::ioEtcItemGoldBox()
{
}

ioEtcItemGoldBox::~ioEtcItemGoldBox()
{
}

void ioEtcItemGoldBox::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	// 상자 아이템 삭제
	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;
	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;
	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	// 열쇠 아이템 삭제
	int iEtcItemSize;
	rkPacket >> iEtcItemSize;
	for(int i = 0;i < iEtcItemSize;i++)
	{
		rkPacket >> kSlot.m_iType;
		rkPacket >> kSlot.m_iValue1;
		rkPacket >> kSlot.m_iValue2;

		if( kSlot.m_iValue1 == 0 )
		{
			pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
		}
		else
		{
			pUserEtcItem->SetEtcItem( kSlot );
		}
		g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, kSlot.m_iType );
		g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, kSlot.m_iType );
	}
}

bool ioEtcItemGoldBox::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowGoldBoxWnd( GetType() );
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemSoldierSelector::ioEtcItemSoldierSelector()
{
	m_iInventorySubManual2	= 0;
	m_iActiveFilter			= 0;
}

ioEtcItemSoldierSelector::~ioEtcItemSoldierSelector()
{
	m_vSoldierCode.clear();
}

void ioEtcItemSoldierSelector::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	// Soldier
	char szKeyName[MAX_PATH]="";
	int iMax  = rkLoader.LoadInt_e( "max_soldier", 0 );
	m_vSoldierCode.clear();
	m_vSoldierCode.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "soldier%d", i+1 );
		int iSoldierCode  = rkLoader.LoadInt( szKeyName, 0 );
		if( iSoldierCode != 0 )
			m_vSoldierCode.push_back( iSoldierCode );
	}

	// 
	m_iInventorySubManual2	= rkLoader.LoadInt( "inventory_sub_manual2", 0 );
	m_iActiveFilter			= rkLoader.LoadInt( "limit_active_filter", static_cast<int>( AF_ACTIVE ) );

}

void ioEtcItemSoldierSelector::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	// 상황 전개
	int iClassType = 0;
	int iClassTime = 0; 
	rkPacket >> iClassType >> iClassTime;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnSoldierSelectResult( iClassType, iClassTime, iType );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemSoldierSelector::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	bool bError = true;
	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( !g_MyInfo.IsCharMortmain( m_vSoldierCode[i]) )
		{
			bError = false;
			break;
		}
	}

	if( bError )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	// 영구 용병 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ShowSoldierSelectBuyWnd( iType );
	}
	return true;
}

bool ioEtcItemSoldierSelector::IsRightSoldierCode( int iClassType )
{
	if( m_vSoldierCode.empty() )
		return true;

	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( iClassType == m_vSoldierCode[i] )
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemFourExtraCompound::ioEtcItemFourExtraCompound()
{
}

ioEtcItemFourExtraCompound::~ioEtcItemFourExtraCompound()
{
	m_vSoldierCode.clear();
}

void ioEtcItemFourExtraCompound::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	// Soldier
	char szKeyName[MAX_PATH]="";
	int iMax  = rkLoader.LoadInt_e( "max_soldier", 0 );
	m_vSoldierCode.clear();
	m_vSoldierCode.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "soldier%d", i+1 );
		int iSoldierCode  = rkLoader.LoadInt( szKeyName, 0 );
		if( iSoldierCode != 0 )
			m_vSoldierCode.push_back( iSoldierCode );
	}
}

void ioEtcItemFourExtraCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	// delete
	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;
	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;
	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	// 상황 전개
	int  iClassType = 0;
	bool bSuccess   = false;
	int  iClassTime = 0; 
	rkPacket >> iClassType >> bSuccess;
	if( bSuccess )
		rkPacket >> iClassTime;

	// delete extra
	ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
	for (int i = 0; i < MAX_EXTRA_ITEM ; i++)
	{
		int iSlotIdx = 0;
		rkPacket >> iSlotIdx;
		if( !pUserExtraItem )
			break;
		if( iSlotIdx < 0 )
			continue;
		pUserExtraItem->DeleteExtraItemData( iSlotIdx );
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnForExtraCompoundResult( iClassType, iClassTime, iType, bSuccess );
	}
}

bool ioEtcItemFourExtraCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	bool bError = true;
	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( m_vSoldierCode[i] == -1 )
		{
			bError = false;
			break;
		}

		if( !g_MyInfo.IsCharMortmain( m_vSoldierCode[i]) )
		{
			bError = false;
			break;
		}
	}

	if( bError )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	// 영구 용병 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ShowFourExtraCompoundBuyWnd( iType );
	}
	return true;
}

bool ioEtcItemFourExtraCompound::IsRightSoldierCode( int iClassType )
{
	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( m_vSoldierCode[0] == -1 ) // -1이면 모든 용병 지급 가능
			return true;

		if( iClassType == m_vSoldierCode[i] )
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExpandMedalSlot::ioEtcItemExpandMedalSlot()
{
}

ioEtcItemExpandMedalSlot::~ioEtcItemExpandMedalSlot()
{
}

void ioEtcItemExpandMedalSlot::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iUseLevel = rkLoader.LoadInt_e( "use_level", 0 );
	m_dwLimitTime = rkLoader.LoadInt_e( "limit_time", 0 );
	switch( m_iUseLevel )
	{
	case 10:
		m_iSlotNumber = SLOT_NUM2;
		break;
	case 20:
		m_iSlotNumber = SLOT_NUM3;
		break;
	case 30:
		m_iSlotNumber = SLOT_NUM4;
		break;
	case 40:
		m_iSlotNumber = SLOT_NUM5;
		break;
	case 50:
		m_iSlotNumber = SLOT_NUM6;
		break;
	default:
		m_iSlotNumber = SLOT_NONE;
	}
}

void ioEtcItemExpandMedalSlot::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	TCPNetwork::MouseBusy( false );
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExpandMedalSlot::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}
	
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
	if( pInvenWnd )
	{
		ioPlayStage* pStage = pInvenWnd->GetPlayStage();
		if( pStage )
		{
			bool bError = false;
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				if( pStage->GetModeType() != MT_MYROOM )
					bError = true;
			}
			else
			{
				if( pStage->GetModeType() != MT_TRAINING && pStage->GetModeType() != MT_HEADQUARTERS && pStage->GetModeType() != MT_HOUSE )
					bError = true;				
			}

			if( bError )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
						return false;				
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "메달슬롯 오픈은 광장/개인본부/길드본부/훈련소에서만 가능합니다." );
				}
			}
			
			if( pInvenWnd->IsShow() )
				pInvenWnd->ShowExpandMedalSlotOpenWnd( m_dwType );
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemSoldierExpBonus::ioEtcItemSoldierExpBonus()
{
	m_iLimitLevel = 0;
	m_iUseValue   = 0;
}

ioEtcItemSoldierExpBonus::~ioEtcItemSoldierExpBonus()
{
}

void ioEtcItemSoldierExpBonus::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iLimitLevel  = rkLoader.LoadInt_e( "limit_level", 0 );
	m_iUseValue    = rkLoader.LoadInt_e( "use_value", 0 );
}

void ioEtcItemSoldierExpBonus::OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;
	pUserEtcItem->SetEtcItem( kSlot );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnSoldierExpSelectResult( kSlot.GetDateExcludeValue2(), GetType() );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemSoldierExpBonus::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	// 영구 용병 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ShowSoldierExpSelectWnd( GetType() );
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemSelectExtraGashapon::ioEtcItemSelectExtraGashapon()
{
	m_iExtraItemMachineCode = 0;
}

ioEtcItemSelectExtraGashapon::~ioEtcItemSelectExtraGashapon()
{

}

void ioEtcItemSelectExtraGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iExtraItemMachineCode = rkLoader.LoadInt_e( "extraitem_machine_code", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "title", "", szBuf, MAX_PATH );
	m_szTtitle = szBuf;

	rkLoader.LoadString( "ment1", "", szBuf, MAX_PATH );
	m_szMent1 = szBuf;

	rkLoader.LoadString( "ment2", "", szBuf, MAX_PATH );
	m_szMent2 = szBuf;
}

bool ioEtcItemSelectExtraGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	SelectExtraGashaponWnd *pWnd = dynamic_cast<SelectExtraGashaponWnd*>( g_GUIMgr.FindWnd( SELECT_EXTRA_GASHAPON_WND ) );
	if( pWnd )
	{
		if( pWnd->IsShow() )
			pWnd->GoToTop();
		else
			pWnd->ShowSelectExtraGashaponWnd( m_dwType, SelectExtraGashaponWnd::UT_USE_BTN );

		return true;
	}
	return false;
}

void ioEtcItemSelectExtraGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemPreSetPackage::ioEtcItemPreSetPackage()
{
}

ioEtcItemPreSetPackage::~ioEtcItemPreSetPackage()
{
}

void ioEtcItemPreSetPackage::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iLimitTime = rkLoader.LoadInt_e( "preset_period", 0 );
	m_iClassType = rkLoader.LoadInt_e( "preset_class", 0 );

	// 성별
	m_SlotKindred.m_item_type = (m_iClassType * 100000) + (RDT_HUMAN_MAN * 1000) + UID_KINDRED;
	m_SlotKindred.m_item_code = rkLoader.LoadInt_e( "preset_kindred", 0 );

	// 헤어
	m_SlotHair.m_item_type = (m_iClassType * 100000) + (m_SlotKindred.m_item_code * 1000) + UID_HAIR;
	m_SlotHair.m_item_code = rkLoader.LoadInt_e( "preset_hair", 0 );

	// 헤어 컬러
	m_SlotHairColor.m_item_type = (m_iClassType * 100000) + (m_SlotKindred.m_item_code * 1000) + UID_HAIR_COLOR;
	m_SlotHairColor.m_item_code = rkLoader.LoadInt_e( "preset_hair_color", 0 );

	// 얼굴
	m_SlotFace.m_item_type = (m_iClassType * 100000) + (m_SlotKindred.m_item_code * 1000) + UID_FACE;
	m_SlotFace.m_item_code = rkLoader.LoadInt_e( "preset_face", 0 );

	// 피부색
	m_SlotSkin.m_item_type = (m_iClassType * 100000) + (m_SlotKindred.m_item_code * 1000) + UID_SKIN_COLOR;
	m_SlotSkin.m_item_code = rkLoader.LoadInt_e( "preset_skin_color", 0 );

	// 속옷
	m_SlotUnder.m_item_type = (m_iClassType * 100000) + (m_SlotKindred.m_item_code * 1000) + UID_UNDERWEAR;
	m_SlotUnder.m_item_code = rkLoader.LoadInt_e( "preset_underwear", 0 );
}

void ioEtcItemPreSetPackage::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	// 상황 전개
	CHARACTER kCharInfo;
	int iBonusPeso;
	int iLimitTime;
	__int64 iMyMoney;
	rkPacket >> kCharInfo >> iBonusPeso >> iMyMoney;
	rkPacket >> iLimitTime;

	int i;
	ITEMSLOTVec vItemSlot;	
	{   // 기본 치장 세팅 : 기본 치장에서 종족은 제외시킨다.
		g_DecorationPrice.GetPackageBuyDefaultList( kCharInfo, vItemSlot );
		int iDefaultSlotItem = vItemSlot.size();
		for(i = 0;i < iDefaultSlotItem;i++)
		{
			ITEMSLOT kSlot = vItemSlot[i];
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}
		vItemSlot.clear();
	}

	{   // 선택한 치장을 지급한다.		
		enum 
		{
			MAX_DECO_ITEM = 6,
		};
		int iDecoTypeList[MAX_DECO_ITEM]={ UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
		for(i = 0;i < MAX_DECO_ITEM;i++)
		{
			ITEMSLOT kSlot = g_DecorationPrice.GetItemSlot( kCharInfo, iDecoTypeList[i] );
			if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) ) continue;

			g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );
			g_MyInfo.SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );
		}	
	}

	// 페소 적용
	g_MyInfo.SetMoney( iMyMoney );
	DWORD dwCharIndex = g_MyInfo.ApplyCharPackage( kCharInfo );
	if( dwCharIndex > 0 && pPlayStage )
	{
		pPlayStage->ReLoadCharInfo( dwCharIndex, kCharInfo );
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnPreSetPackageItemResult( kCharInfo, iBonusPeso, iLimitTime );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemPreSetPackage::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	if( g_MyInfo.GetMortmainCharCount() + g_MyInfo.GetExerciseCharCount() >= g_MyInfo.GetCurMaxCharSlot() )
	{
		// 전부 영구 용병이거나 무료 용병이다.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "더 이상 용병을 보유할 수 없습니다." );
		return false;
	}

	// 영구 용병 패키지 UI 활성
	MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ShowPreSetPackageItemWnd( m_dwType );
	}

	return true;
}

void ioEtcItemPreSetPackage::SetPreSetPackageInfo( CHARACTER &rkCharInfo )
{
	rkCharInfo.m_class_type = m_iClassType;
	rkCharInfo.m_kindred = 1;
	rkCharInfo.m_sex = m_SlotKindred.m_item_code + 1;
	rkCharInfo.m_hair = m_SlotHair.m_item_code;
	rkCharInfo.m_hair_color = m_SlotHairColor.m_item_code;
	rkCharInfo.m_face = m_SlotFace.m_item_code;
	rkCharInfo.m_skin_color = m_SlotSkin.m_item_code;
	rkCharInfo.m_underwear = m_SlotUnder.m_item_code;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGrowthAllDown::ioEtcItemGrowthAllDown()
{
}

ioEtcItemGrowthAllDown::~ioEtcItemGrowthAllDown()
{
}

void ioEtcItemGrowthAllDown::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemGrowthAllDown::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowGrowthAllDownWnd();
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPrizeItem::ioEtcItemPrizeItem()
{
}

ioEtcItemPrizeItem::~ioEtcItemPrizeItem()
{
}

void ioEtcItemPrizeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH]="";

	m_PrizeDataVec.clear();

	int iPrizeCnt = rkLoader.LoadInt_e( "prize_cnt", 0 );
	for( int i=0; i < iPrizeCnt; ++i )
	{
		PrizeData kData;
		
		wsprintf_e( szKey, "prize%d_type", i+1 );
		kData.m_iPresentType = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "prize%d_ment", i+1 );
		kData.m_iPresentMent = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "prize%d_period", i+1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "prize%d_value1", i+1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "prize%d_value2", i+1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "prize%d_value3", i+1 );
		kData.m_iPresentValue3 = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "prize%d_value4", i+1 );
		kData.m_iPresentValue4 = rkLoader.LoadInt( szKey, 0 );

		m_PrizeDataVec.push_back( kData );
	}
}

void ioEtcItemPrizeItem::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	TCPNetwork::MouseBusy( false );

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->UpdateTab( true, true );
}

bool ioEtcItemPrizeItem::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemTournamentCreate::ioEtcItemTournamentCreate()
{

}

ioEtcItemTournamentCreate::~ioEtcItemTournamentCreate()
{

}

void ioEtcItemTournamentCreate::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );
	
	pUserEtcItem->DeleteEtcItem( iType );
	// 사용 처리는 대회 생성 패킷이 따로 도착하므로 여기서는 사용한것만 처리

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemTournamentCreate::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	// 대회 생성 UI 
	CustomTournamentCreateWnd* pWnd = dynamic_cast<CustomTournamentCreateWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_CREATE_WND ) );
	if( pWnd )
	{		
		pWnd->UseEtcItem( GetType() );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemTournamentPremiumCreate::ioEtcItemTournamentPremiumCreate()
{

}

ioEtcItemTournamentPremiumCreate::~ioEtcItemTournamentPremiumCreate()
{

}

void ioEtcItemTournamentPremiumCreate::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );

	pUserEtcItem->DeleteEtcItem( iType );
	
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemTournamentPremiumCreate::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.IsUse() )
		return false;

	// 대회 생성 UI 
	CustomTournamentCreateWnd* pWnd = dynamic_cast<CustomTournamentCreateWnd*>( g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_CREATE_WND ) );
	if( pWnd )	
	{
		pWnd->UseEtcItem( GetType() );		
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemClover::ioEtcItemClover()
{

}

ioEtcItemClover::~ioEtcItemClover()
{

}

void ioEtcItemClover::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemClover::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemClover::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemAddCash::ioEtcItemAddCash()
{

}

ioEtcItemAddCash::~ioEtcItemAddCash()
{

}

void ioEtcItemAddCash::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemAddCash::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( g_MyInfo.GetChannelingType() != CNT_WEMADEBUY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "채널링 회원은 사용할 수 없는 아이템입니다." );
		return false;
	}

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTournamentCoin::ioEtcItemTournamentCoin()
{

}

ioEtcItemTournamentCoin::~ioEtcItemTournamentCoin()
{

}

void ioEtcItemTournamentCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemTournamentCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemTournamentCoin::OnBuy()
{
}

bool ioEtcItemTournamentCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRouletteCoin::ioEtcItemRouletteCoin()
{

}

ioEtcItemRouletteCoin::~ioEtcItemRouletteCoin()
{

}

void ioEtcItemRouletteCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemRouletteCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemRouletteCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoItem::ioEtcItemBingoItem()
{

}

ioEtcItemBingoItem::~ioEtcItemBingoItem()
{

}

void ioEtcItemBingoItem::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemBingoItem::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	//UI표시를 위한 데이터, 보안관련 문자열 처리를 하지 않아도 되는 부분입니다.

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "title", "", szBuf, MAX_PATH );
	m_szTtitle = szBuf;
}

void ioEtcItemBingoItem::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemBingoItem::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioBingo* pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		BingoWnd *pWnd = dynamic_cast<BingoWnd*>( g_GUIMgr.FindWnd( BINGO_WND ) );
		if( pWnd && pBingo->GetBingoType() == ioBingo::BT_FIXED )
		{
			g_GUIMgr.ChangeWndElementValue( "XML/bingo/bingownd.xml", "XML/bingo/changebingownd.xml", "Fixed", NULL );
			pWnd->ReloadBtnEffect();
		}

		if( !pWnd->IsShow() )		
			 pWnd->ShowBingoWnd( m_szTtitle );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoNumberGashapon::ioEtcItemBingoNumberGashapon()
{

}

ioEtcItemBingoNumberGashapon::~ioEtcItemBingoNumberGashapon()
{

}

void ioEtcItemBingoNumberGashapon::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemBingoNumberGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyGetNumber( rkPacket );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemBingoNumberGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kItemSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BINGO_ITEM, kItemSlot ) )
		{
			if( !g_GUIMgr.IsShow( BINGO_WND ) )
				g_GUIMgr.ShowWnd( BINGO_WND );
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "빙고판이 필요합니다." );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemBingoShuffleNumber::ioEtcItemBingoShuffleNumber()
{

}

ioEtcItemBingoShuffleNumber::~ioEtcItemBingoShuffleNumber()
{

}

void ioEtcItemBingoShuffleNumber::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemBingoShuffleNumber::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyGetAllNumber( rkPacket );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemBingoShuffleNumber::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kItemSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BINGO_ITEM, kItemSlot ) )
		{
			if( !g_GUIMgr.IsShow( BINGO_WND ) )
				g_GUIMgr.ShowWnd( BINGO_WND );
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "빙고판이 필요합니다." );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemBingoShuffleRewardItem::ioEtcItemBingoShuffleRewardItem()
{

}

ioEtcItemBingoShuffleRewardItem::~ioEtcItemBingoShuffleRewardItem()
{

}

void ioEtcItemBingoShuffleRewardItem::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemBingoShuffleRewardItem::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyShuffleReward( rkPacket );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemBingoShuffleRewardItem::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kItemSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BINGO_ITEM, kItemSlot ) )
		{
			if( !g_GUIMgr.IsShow( BINGO_WND ) )
				g_GUIMgr.ShowWnd( BINGO_WND );
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "빙고판이 필요합니다." );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemBingoRandomNumberClear::ioEtcItemBingoRandomNumberClear()
{

}

ioEtcItemBingoRandomNumberClear::~ioEtcItemBingoRandomNumberClear()
{

}

void ioEtcItemBingoRandomNumberClear::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemBingoRandomNumberClear::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyGetNumber( rkPacket );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemBingoRandomNumberClear::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kItemSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BINGO_ITEM, kItemSlot ) )
		{
			if( !g_GUIMgr.IsShow( BINGO_WND ) )
				g_GUIMgr.ShowWnd( BINGO_WND );
		}
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "빙고판이 필요합니다." );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemSuperGashapon::ioEtcItemSuperGashapon()
{
	m_SuperGashponUseType = SUPERGASHPON_USE;
}

ioEtcItemSuperGashapon::~ioEtcItemSuperGashapon()
{

}

void ioEtcItemSuperGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemSuperGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	int iProductType = 0;
	DWORD dwPresentIndex = 0;
	int iUseType;
	rkPacket >> iProductType;
	rkPacket >> dwPresentIndex;	
	rkPacket >> iUseType;


	SuperGashaponWnd *pSuperGashaponWnd = NULL;
	if( iUseType == SUPERGASHPON_BUY )
	{
		NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd )
			pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_SUPERGASHAPON_WND ) );
	}
	else
	{
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd )
			pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*> ( pInvenWnd->FindChildWnd( MyInventoryWnd::ID_SUPER_GASHAPON_WND ) );
	}
	
	if( pSuperGashaponWnd && pSuperGashaponWnd->IsShow() )
	{
		pSuperGashaponWnd->ShowResultSuperGashaponWnd( GetType(), dwPresentIndex, iProductType );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s pSuperGashaponWnd == NULL (%d, %d, %d)", __FUNCTION__, iProductType, dwPresentIndex, iUseType );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
	m_SuperGashponUseType = SUPERGASHPON_USE;
}

bool ioEtcItemSuperGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.m_iValue1 <= 0 )
		return false;
	
	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "선물함이 꽉 찼습니다.#선물함을 비워주시고 사용해 주세요." );
		return false;
	}

	if( m_SuperGashponUseType == SUPERGASHPON_USE )
	{	
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd )
			pInvenWnd->ShowSuperGashponWnd( m_dwType );
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	kPacket << (DWORD)m_SuperGashponUseType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

void ioEtcItemSuperGashapon::OnBuy()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemSendPresent::ioEtcItemSendPresent()
{

}

ioEtcItemSendPresent::~ioEtcItemSendPresent()
{

}

void ioEtcItemSendPresent::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt_e( "present_count", 0 );

	m_vPresentInfo.clear();
	m_vPresentInfo.reserve(( iCnt ));

	for( int i=0; i<iCnt; ++i )
	{
		PresentData kInfo;
		kInfo.Init();

		wsprintf_e( szKey, "present%d_type", i+1 );
		kInfo.m_iPresentType   = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_state", i+1 );
		kInfo.m_iPresentState  = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_ment", i+1 );
		kInfo.m_iPresentMent   = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_period", i+1 );
		kInfo.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_value1", i+1 );
		kInfo.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_value2", i+1 );
		kInfo.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_value3", i+1 );
		kInfo.m_iPresentValue3 = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "present%d_value4", i+1 );
		kInfo.m_iPresentValue4 = rkLoader.LoadInt( szKey, 0 );

		m_vPresentInfo.push_back(kInfo);
	}
}

void ioEtcItemSendPresent::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	TCPNetwork::MouseBusy( false );

	UsePresentEtcItemWnd *pWnd = dynamic_cast<UsePresentEtcItemWnd*>( g_GUIMgr.FindWnd(USE_PRESENT_ETC_ITEM_WND) );
	if( pWnd )
		pWnd->SendPresentComplete();

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemSendPresent::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	// UI 출력
	UsePresentEtcItemWnd *pWnd = dynamic_cast<UsePresentEtcItemWnd*>( g_GUIMgr.FindWnd(USE_PRESENT_ETC_ITEM_WND) );
	if( pWnd )
		pWnd->SetInfo( m_dwType, m_vPresentInfo );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemSoldierExpAdd::ioEtcItemSoldierExpAdd()
{
}

ioEtcItemSoldierExpAdd::~ioEtcItemSoldierExpAdd()
{
}

void ioEtcItemSoldierExpAdd::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	int iClassType = 0;
	int iExp = 0;	
	rkPacket >> iClassType;
	rkPacket >> iExp;

	pUserEtcItem->DeleteEtcItem( iType );
	g_MyInfo.AddClassExp( iClassType, iExp );
	
	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{		
		pInvenWnd->OnSoldierExpSelectResult( iClassType, GetType() );
	}

	// 용병 레벨업
	if( g_MyInfo.IsClassLevelUP( iClassType ) )
	{
		int iPeso = g_MyInfo.GetClassLevelUPPeso( iClassType );
		SoldierLevelUPWnd *pLevelUPWnd = dynamic_cast<SoldierLevelUPWnd*>( g_GUIMgr.FindWnd( SOLDIER_LEVEL_UP_WND ) );
		if( pLevelUPWnd )
		{
			pLevelUPWnd->SetLevelUPData( iClassType, iPeso );
			pLevelUPWnd->GoToTop();
		}

		g_MyInfo.InitLevelUPData();
	}
	
	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemSoldierExpAdd::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}
	
	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemRecharge::ioEtcItemRecharge()
{
}

ioEtcItemRecharge::~ioEtcItemRecharge()
{
}

void ioEtcItemRecharge::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	m_iMaxInfoCnt = rkLoader.LoadInt_e( "recharge_count", 0 );

	m_RechargeInfoList.clear();
	m_RechargeInfoList.reserve( m_iMaxInfoCnt );

	for( int i=0; i<m_iMaxInfoCnt; ++i )
	{
		wsprintf_e( szKey, "recharge_time%d", i+1 );
		int iTime = rkLoader.LoadInt( szKey, -1 );

		wsprintf_e( szKey, "recharge_rate%d", i+1 );
		float fRate = rkLoader.LoadFloat( szKey, 0.0f );

		float fMinRate, fMaxRate;

		if( i == 0 )
		{
			fMinRate = 0.0f;
			fMaxRate = fRate;
		}
		else
		{
			fMinRate = m_RechargeInfoList[i-1].m_fMaxRate;
			fMaxRate = m_RechargeInfoList[i-1].m_fMaxRate + fRate;
		}

		RechargeInfo kInfo( iTime, fMinRate, fMaxRate );
		m_RechargeInfoList.push_back( kInfo );
	}
}

void ioEtcItemRecharge::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemRecharge::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ItemRechargeWnd *pWnd = dynamic_cast<ItemRechargeWnd*>( g_GUIMgr.FindWnd(ITEM_RECHARGE_WND) );
	if( pWnd )
		pWnd->SetInfo( m_dwType );

	return true;
}

int ioEtcItemRecharge::GetRechargeTime( int iArray )
{
	if( !COMPARE( iArray, 0, m_iMaxInfoCnt ) )
		return 0;

	return m_RechargeInfoList[iArray].m_iPeriod;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcAccessoryRecharge::ioEtcAccessoryRecharge()
{
}

ioEtcAccessoryRecharge::~ioEtcAccessoryRecharge()
{
}

void ioEtcAccessoryRecharge::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	m_iMaxInfoCnt = rkLoader.LoadInt_e( "recharge_count", 0 );

	m_RechargeInfoList.clear();
	m_RechargeInfoList.reserve( m_iMaxInfoCnt );

	for( int i=0; i<m_iMaxInfoCnt; ++i )
	{
		wsprintf_e( szKey, "recharge_time%d", i+1 );
		int iTime = rkLoader.LoadInt( szKey, -1 );

		wsprintf_e( szKey, "recharge_rate%d", i+1 );
		float fRate = rkLoader.LoadFloat( szKey, 0.0f );

		float fMinRate, fMaxRate;

		if( i == 0 )
		{
			fMinRate = 0.0f;
			fMaxRate = fRate;
		}
		else
		{
			fMinRate = m_RechargeInfoList[i-1].m_fMaxRate;
			fMaxRate = m_RechargeInfoList[i-1].m_fMaxRate + fRate;
		}

		RechargeInfo kInfo( iTime, fMinRate, fMaxRate );
		m_RechargeInfoList.push_back( kInfo );
	}
}

void ioEtcAccessoryRecharge::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcAccessoryRecharge::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	AccessoryRechargeWnd *pWnd = dynamic_cast<AccessoryRechargeWnd*>( g_GUIMgr.FindWnd(ACCESSORY_RECHARGE_WND) );
	if( pWnd )
		pWnd->SetInfo( m_dwType );

	return true;
}

int ioEtcAccessoryRecharge::GetRechargeTime( int iArray )
{
	if( !COMPARE( iArray, 0, m_iMaxInfoCnt ) )
		return 0;

	return m_RechargeInfoList[iArray].m_iPeriod;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioSelectGashapon::ioSelectGashapon()
{
}

ioSelectGashapon::~ioSelectGashapon()
{

}

void ioSelectGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	//UI표시를 위한 데이터, 보안관련 문자열 처리를 하지 않아도 되는 부분입니다.

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "title", "", szBuf, MAX_PATH );
	m_szTtitle = szBuf;

	rkLoader.LoadString( "ment1", "", szBuf, MAX_PATH );
	m_szMent1 = szBuf;

	rkLoader.LoadString( "ment2", "", szBuf, MAX_PATH );
	m_szMent2 = szBuf;

	char szKey[MAX_PATH];
	m_TabUISettingVec.clear();

	int iCount = min( TAB_MAX, rkLoader.LoadInt( "tab_count", 0 ) );

	for( int i = 0; i < iCount; ++i )
	{
		TabUISetting TabInfo;
		
		StringCbPrintf( szKey, sizeof(szKey), "tab%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		TabInfo.m_szTtitle = szBuf;

		StringCbPrintf( szKey, sizeof(szKey), "tab%d_order", i+1 );
		TabInfo.m_iOrder = rkLoader.LoadInt( szKey, (int)TabUISetting::ORDER_BY_CODE );

		StringCbPrintf( szKey, sizeof(szKey), "tab%d_type_count", i+1 );
		int iTypeCount = rkLoader.LoadInt( szKey, 0 );
		
		for( int j = 0; j < iTypeCount; ++j )
		{
			StringCbPrintf( szKey, sizeof(szKey), "tab%d_type%d", i+1, j+1 );
			int iType = rkLoader.LoadInt( szKey, 0 );
			if( COMPARE( iType, PRESENT_DEFAULT, PRESENT_MAX ) )
			{
				TabFiter Filter;
				Filter.m_iTypeFilter = iType;
				TabInfo.m_FilterVec.push_back( Filter );
			}
		}

		StringCbPrintf( szKey, sizeof(szKey), "tab%d_special_type", i+1 );		
		TabInfo.m_iSpecialAddType = rkLoader.LoadInt( szKey, 0 );
		
		m_TabUISettingVec.push_back( TabInfo );
	}
}

bool ioSelectGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	SelectGashaponWnd *pWnd = dynamic_cast<SelectGashaponWnd*>( g_GUIMgr.FindWnd( SELECT_GASHAPON_WND ) );
	if( pWnd )
	{
		if( pWnd->IsShow() && pWnd->GetSelectGashaponCode() == m_dwType )
		{
			pWnd->GoToTop();
		}
		else
		{							
			pWnd->ShowSelectGashaponWnd( m_dwType, SelectGashaponWnd::UT_USE_BTN );
		}
		return true;
	}

	return false;
}

void ioSelectGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemFixedBingoNumber::ioEtcItemFixedBingoNumber()
{

}

ioEtcItemFixedBingoNumber::~ioEtcItemFixedBingoNumber()
{

}

void ioEtcItemFixedBingoNumber::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyGetNumber( rkPacket );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMaterialCompound::ioEtcItemMaterialCompound()
{
}

ioEtcItemMaterialCompound::~ioEtcItemMaterialCompound()
{
}

void ioEtcItemMaterialCompound::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemMaterialCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemMaterialCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowItemMaterialCompoundWnd( GetType() );

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPetEgg::ioEtcItemPetEgg()
{
}

ioEtcItemPetEgg::~ioEtcItemPetEgg()
{
}

void ioEtcItemPetEgg::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";

}

void ioEtcItemPetEgg::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemPetEgg::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;
	
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if ( !pUserPet )
		return false;

	if ( pUserPet->IsSlotFull() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), pUserPet->GetPetSlotCnt(), ioUserPet::MAX_SLOT_CNT );
		return false;
	}
	
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowPetEggResultWnd( GetType() );	

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << iType;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemSoulStone::ioEtcItemSoulStone()
{

}

ioEtcItemSoulStone::~ioEtcItemSoulStone()
{

}

void ioEtcItemSoulStone::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemSoulStone::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemSoulStone::OnBuy()
{
}

bool ioEtcItemSoulStone::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ReinforceSelectWnd *pReinforceSelectWnd = dynamic_cast<ReinforceSelectWnd*>(g_GUIMgr.FindWnd(REINFORCE_SELECT_WND));
	if( pReinforceSelectWnd )
	{
		pReinforceSelectWnd->SetInfo();
		pReinforceSelectWnd->ShowWnd();
	}
	return true;
}

void ioEtcItemSoulStone::OnUseGradeAdjustment( int iCharIndex, PowerUpTargetType eTargetType )
{
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	kPacket << static_cast<byte>( eTargetType );
	kPacket << iCharIndex;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

//////////////////////////////////////////////////////////////////////////

ioEtcCostumeCustomItemSkin::ioEtcCostumeCustomItemSkin()
{

}

ioEtcCostumeCustomItemSkin::~ioEtcCostumeCustomItemSkin()
{

}

void ioEtcCostumeCustomItemSkin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );

	// 스킨 처리 마무리
	CostumeCustomMakeWnd *pCustomMakeWnd = dynamic_cast< CostumeCustomMakeWnd * >( g_GUIMgr.FindWnd( COSTUME_CUSTOM_MAKE_WND ) );
	if( pCustomMakeWnd )
		pCustomMakeWnd->CompleteCustomFileMake();
}

bool ioEtcCostumeCustomItemSkin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		if( ioPlayMode::GetModeType() == MT_MYROOM )
		{
			if( ioMyRoomMode::GetMyRoomType() == MRT_LOBBY )
			{
				ioUserEtcItem::ETCITEMSLOT kItemSlot;
				if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
					return false;

				g_GUIMgr.HideWnd( COSTUME_CUSTOM_SELECT_WND );
				g_GUIMgr.ShowWnd( COSTUME_CUSTOM_SELECT_WND );
				return true;
			}
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬스킨 변경은 로비에서만 가능합니다." );
	}
	else
	{
		if( ioPlayMode::GetModeType() != MT_TRAINING || ioPlayMode::GetModeType() != MT_HEADQUARTERS || ioPlayMode::GetModeType() != MT_HOUSE )
		{
			ioUserEtcItem::ETCITEMSLOT kItemSlot;
			if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
				return false;

			g_GUIMgr.HideWnd( COSTUME_CUSTOM_SELECT_WND );
			g_GUIMgr.ShowWnd( COSTUME_CUSTOM_SELECT_WND );
			return true;
		}
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬스킨 변경은 광장/개인본부/길드본부/훈련소에서만 가능합니다." );		
	}

	return false;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExtraItemSlotExtend::ioEtcItemExtraItemSlotExtend()
{

}

ioEtcItemExtraItemSlotExtend::~ioEtcItemExtraItemSlotExtend()
{

}

bool ioEtcItemExtraItemSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

bool ioEtcItemExtraItemSlotExtend::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(7), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(2), GetName().c_str() );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(5), GetName().c_str() );

	kPrinter[7].SetTextStyle( TS_NORMAL );
	kPrinter[7].SetBkColor( 0, 0, 0 );	
	kPrinter[7].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[7].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemNewMultipleCompound::ioEtcItemNewMultipleCompound()
{

}

ioEtcItemNewMultipleCompound::~ioEtcItemNewMultipleCompound()
{

}

void ioEtcItemNewMultipleCompound::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	enum { MAX_RADIO_NAME = 10 };
	m_szRadioNameList.clear();
	for ( int i = 0; i< MAX_RADIO_NAME ; ++i )
	{
		ioHashString szRadionName;
		sprintf_e( szKey, "radio_button%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;
		m_szRadioNameList.push_back( szBuf );
	}

	m_dwMaterialType = (DWORD)rkLoader.LoadInt_e( "material_type", 0 );

	m_dwResultGashaponIndex = (DWORD)rkLoader.LoadInt_e( "result_gashapon_code", 0 );
}

void ioEtcItemNewMultipleCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlram    = false;
	int   iPresentPeso   = 0;

	rkPacket >> iPresentType;
	rkPacket >> iPresentValue1;
	rkPacket >> iPresentValue2;
	rkPacket >> iPresentValue3;
	rkPacket >> iPresentValue4;
	rkPacket >> bWholeAlram;
	rkPacket >> iPresentPeso;

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnGashponResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlram, iPresentPeso );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemNewMultipleCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->ShowNewMultipleCompoundWnd( GetType() );
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemHousingBlockItem::ioEtcItemHousingBlockItem()
{

}

ioEtcItemHousingBlockItem::~ioEtcItemHousingBlockItem()
{

}

void ioEtcItemHousingBlockItem::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );		
	m_dwBlockItemCode = (DWORD)rkLoader.LoadInt_e( "block_item_code", 0 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "present_dsec", "", szBuf, MAX_PATH );
	m_BlockPresentDesc = szBuf;
}

void ioEtcItemHousingBlockItem::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;	
	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemHousingBlockItem::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCreateGuildHQCreate::ioEtcItemCreateGuildHQCreate()
{

}

ioEtcItemCreateGuildHQCreate::~ioEtcItemCreateGuildHQCreate()
{

}

void ioEtcItemCreateGuildHQCreate::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemCreateGuildHQCreate::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );
	pUserEtcItem->DeleteEtcItem( iType );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );

	g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드본부가#활성화 되었습니다." );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

bool ioEtcItemCreateGuildHQCreate::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCreateMyHomeCreate::ioEtcItemCreateMyHomeCreate()
{

}

ioEtcItemCreateMyHomeCreate::~ioEtcItemCreateMyHomeCreate()
{

}

void ioEtcItemCreateMyHomeCreate::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemCreateMyHomeCreate::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );

	g_GUIMgr.SetMsgBox( MB_OK, NULL, "개인본부가#활성화 되었습니다." );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

bool ioEtcItemCreateMyHomeCreate::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemNamedTitle::ioEtcItemNamedTitle()
{

}

ioEtcItemNamedTitle::~ioEtcItemNamedTitle()
{

}

void ioEtcItemNamedTitle::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemNamedTitle::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	TCPNetwork::MouseBusy( false );

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemNamedTitle::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	TCPNetwork::SendToServer( kPacket );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemNamedTitlePremium::ioEtcItemNamedTitlePremium()
{

}

ioEtcItemNamedTitlePremium::~ioEtcItemNamedTitlePremium()
{

}

void ioEtcItemNamedTitlePremium::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemNamedTitlePremium::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	TCPNetwork::MouseBusy( false );

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemNamedTitlePremium::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowNamedTitlePremiumSelectWnd( GetType() );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemTimeCash::ioEtcItemTimeCash()
{

}

ioEtcItemTimeCash::~ioEtcItemTimeCash()
{

}

bool ioEtcItemTimeCash::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	/*
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13,STR(6) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	*/

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemOakDrumItem::ioEtcItemOakDrumItem()
{

}

ioEtcItemOakDrumItem::~ioEtcItemOakDrumItem()
{

}

void ioEtcItemOakDrumItem::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
}

void ioEtcItemOakDrumItem::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

bool ioEtcItemOakDrumItem::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /*= true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioPriateRullet *pRullet = g_MyInfo.GetPriateRullet();
	if( pRullet )
		pRullet->SendRequest();

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemOakSwordItem::ioEtcItemOakSwordItem()
{

}

ioEtcItemOakSwordItem::~ioEtcItemOakSwordItem()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemPCRoomFishingRod::ioEtcItemPCRoomFishingRod()
{

}

ioEtcItemPCRoomFishingRod::~ioEtcItemPCRoomFishingRod()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemPCRoomFishingBait::ioEtcItemPCRoomFishingBait()
{

}

ioEtcItemPCRoomFishingBait::~ioEtcItemPCRoomFishingBait()
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRaidCoin::ioEtcItemRaidCoin()
{

}

ioEtcItemRaidCoin::~ioEtcItemRaidCoin()
{

}

void ioEtcItemRaidCoin::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemRaidCoin::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	pUserEtcItem->SetEtcItem( kSlot );          // 0개 이하가 되어도 삭제하지 않는다.

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

void ioEtcItemRaidCoin::OnBuy()
{
	g_BattleRoomMgr.SyncMonsterCoin();
}

bool ioEtcItemRaidCoin::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1), GetName().c_str() );

	kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[1].AddTextPiece( FONT_SIZE_13," " );

	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );	
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );	
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );	
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[4].AddTextPiece( FONT_SIZE_13,STR(5) );

	kPrinter[5].SetTextStyle( TS_NORMAL );
	kPrinter[5].SetBkColor( 0, 0, 0 );	
	kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[5].AddTextPiece( FONT_SIZE_13, " " );

	kPrinter[6].SetTextStyle( TS_NORMAL );
	kPrinter[6].SetBkColor( 0, 0, 0 );	
	kPrinter[6].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 주황
	kPrinter[6].AddTextPiece( FONT_SIZE_13, STR(6) );

	kPrinter[7].SetTextStyle( TS_NORMAL );
	kPrinter[7].SetBkColor( 0, 0, 0 );	
	kPrinter[7].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
	kPrinter[7].AddTextPiece( FONT_SIZE_13,STR(7) );

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemAccessoryCompound::ioEtcItemAccessoryCompound()
{
}

ioEtcItemAccessoryCompound::~ioEtcItemAccessoryCompound()
{
}

void ioEtcItemAccessoryCompound::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemAccessoryCompound::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->ShowAccessoryCompoundWnd( GetType() );

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemExcavatingShovel::ioEtcItemExcavatingShovel()
{
}

ioEtcItemExcavatingShovel::~ioEtcItemExcavatingShovel()
{
}

void ioEtcItemExcavatingShovel::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	if( g_GUIMgr.IsShow( EXCAVATION_SHOVEL_WND ) )
	{
		ExcavationShovelWnd *pExcavationShovelWnd = static_cast<ExcavationShovelWnd*>( g_GUIMgr.FindWnd( EXCAVATION_SHOVEL_WND ) );
		if( pExcavationShovelWnd )
		{
			pExcavationShovelWnd->UpdateUI();
		}
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemExcavatingShovel::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemCardMatching::ioEtcItemCardMatching()
{

}

ioEtcItemCardMatching::~ioEtcItemCardMatching()
{

}

void ioEtcItemCardMatching::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemCardMatching::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioMatchingGame *pMatchingGame = g_MyInfo.GetMatchingGame();
	if( pMatchingGame )
	{
		pMatchingGame->SendOpenGame();
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemCardMatchingTicket::ioEtcItemCardMatchingTicket()
{

}

ioEtcItemCardMatchingTicket::~ioEtcItemCardMatchingTicket()
{

}

void ioEtcItemCardMatchingTicket::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemCardMatchingTicket::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioMatchingGame *pMatchingGame = g_MyInfo.GetMatchingGame();
	if( pMatchingGame )
	{
		pMatchingGame->SendGameStart();
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemCardMatchingPremiumTicket::ioEtcItemCardMatchingPremiumTicket()
{

}

ioEtcItemCardMatchingPremiumTicket::~ioEtcItemCardMatchingPremiumTicket()
{

}

void ioEtcItemCardMatchingPremiumTicket::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemCardMatchingPremiumTicket::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioMatchingGame *pMatchingGame = g_MyInfo.GetMatchingGame();
	if( pMatchingGame )
	{
		pMatchingGame->SendGameStart( true );
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemDiceGame::ioEtcItemDiceGame()
{

}

ioEtcItemDiceGame::~ioEtcItemDiceGame()
{

}

void ioEtcItemDiceGame::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemDiceGame::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioDiceGame *pDiceGame = g_MyInfo.GetDiceGame();
	if( pDiceGame )
	{
		pDiceGame->SendOpenGame();
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemDiceGameDice::ioEtcItemDiceGameDice()
{

}

ioEtcItemDiceGameDice::~ioEtcItemDiceGameDice()
{

}

void ioEtcItemDiceGameDice::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemDiceGameDice::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioDiceGame *pDiceGame = g_MyInfo.GetDiceGame();
	if( pDiceGame )
	{
		pDiceGame->SendDiceRoll();
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemDiceGameRewardChange::ioEtcItemDiceGameRewardChange()
{

}

ioEtcItemDiceGameRewardChange::~ioEtcItemDiceGameRewardChange()
{

}

void ioEtcItemDiceGameRewardChange::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
	if( pWnd )
		pWnd->SetRewardChangeItemCount();

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemDiceGameRewardChange::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioDiceGame *pDiceGame = g_MyInfo.GetDiceGame();
	if( pDiceGame )
	{
		pDiceGame->SendRewardChange();
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////
ioEtcItemDiceGameBoardChange::ioEtcItemDiceGameBoardChange()
{

}

ioEtcItemDiceGameBoardChange::~ioEtcItemDiceGameBoardChange()
{

}

void ioEtcItemDiceGameBoardChange::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
		pUserEtcItem->DeleteEtcItem( GetType() );
	else
		pUserEtcItem->SetEtcItem( kSlot ); 

	DiceGameWnd*	pWnd	= dynamic_cast<DiceGameWnd*>( g_GUIMgr.FindWnd( DICE_GAME_WND ) );
	if( pWnd )
		pWnd->SetBoardChangeItemCount();

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
}

bool ioEtcItemDiceGameBoardChange::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck /* = true */ )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioDiceGame *pDiceGame = g_MyInfo.GetDiceGame();
	if( pDiceGame )
	{
		pDiceGame->SendBoardChange();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemMultiSuperGashapon::ioEtcItemMultiSuperGashapon()
{
	m_MultiSuperGashponUseType = MULTISUPERGASHPON_USE;
}

ioEtcItemMultiSuperGashapon::~ioEtcItemMultiSuperGashapon()
{

}

/*void ioEtcItemNewSuperGashapon::LoadData( DWORD dwEtcItemCode, DWORD dwCategoryIdx, DWORD dwPackageIdx, NewSuperGashaponElement& element )
{
	m_NewSuperGashaponMap[dwCategoryIdx][dwPackageIdx][element.ElementIndex] = element;

	m_dwType = dwEtcItemCode;
}*/

void ioEtcItemMultiSuperGashapon::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
	TCPNetwork::MouseBusy( false );

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	int iProductType = 0;
	DWORD dwPresentIndex = 0;
	DWORDVec vCategory, vPackage;
	vCategory.clear();
	vPackage.clear();
	int iUseType, iCount;
	rkPacket >> iProductType;
	rkPacket >> iCount;	
	for( int i = 0; i < iCount; i++ )
	{
		DWORD dwCategoryIdx, dwPackageIdx;
		rkPacket >> dwCategoryIdx;
		rkPacket >> dwPackageIdx;

		vCategory.push_back( dwCategoryIdx );
		vPackage.push_back( dwPackageIdx );
	}
	rkPacket >> iUseType;

	MultiSuperGashaponWnd *pMultiSuperGashaponWnd = NULL;
	if( iUseType == MULTISUPERGASHPON_BUY )
	{
		NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd )
			pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_MULTI_SUPERGASHAPON_WND ) );
	}
	else
	{
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd )
			pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*> ( pInvenWnd->FindChildWnd( MyInventoryWnd::ID_MULTI_SUPER_GASHAPON_WND ) );
	}

	if( pMultiSuperGashaponWnd && pMultiSuperGashaponWnd->IsShow() )
	{
		pMultiSuperGashaponWnd->InitResultMultiSuperGashaponWnd();

		DWORDVec::iterator iterCategory = vCategory.begin();
		DWORDVec::iterator iterPackage = vPackage.begin();
		for( int i = 0; i < iCount; i++, iterCategory++, iterPackage++ )
		{
			pMultiSuperGashaponWnd->ShowResultMultiSuperGashaponWnd( GetType(), *iterCategory, *iterPackage, iProductType );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s pMultiSuperGashaponWnd == NULL (%d, %d, %d)", __FUNCTION__, iProductType, dwPresentIndex, iUseType );
	}

	g_QuestMgr.QuestOccurTerm( QO_ETCITEM_USE, iType );
	g_QuestMgr.QuestCompleteTerm( QC_ETCITEM_USE, iType );
	m_MultiSuperGashponUseType = MULTISUPERGASHPON_USE;
}

bool ioEtcItemMultiSuperGashapon::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( iType, kItemSlot ) )
		return false;

	if( kItemSlot.m_iValue1 <= 0 )
		return false;

	if( bPresentLimitCheck && g_PresentMgr.GetMaxPresent() >= (int)g_PresentMgr.GetLimitPresentSize() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "선물함이 꽉 찼습니다.#선물함을 비워주시고 사용해 주세요." );
		return false;
	}

	if( m_MultiSuperGashponUseType == MULTISUPERGASHPON_USE )
	{	
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd )
			pInvenWnd->ShowMultiSuperGashaponWnd( m_dwType );
	}

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << m_dwType;
	kPacket << (DWORD)m_MultiSuperGashponUseType;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
	return true;
}

void ioEtcItemMultiSuperGashapon::OnBuy()
{

}