#include "stdafx.h"

#include "ioSubscriptionMgr.h"
#include "ioDecorationPrice.h"
#include "ioDateHelp.h"

#include "GUI/ioMessageBox.h"
#include "GUI/MyInventoryWnd.h"
#include "GUI/MyInventoryInfoWnd.h"
#include "GUI/NewShopItemPurchaseLeedWnd.h"

template<> ioSubscriptionMgr* Singleton< ioSubscriptionMgr >::ms_Singleton = 0;

ioSubscriptionMgr::ioSubscriptionMgr()
{
	m_dwSubscriptionCurTimer = 0;
	m_dwSubscriptionRequestTime  = 0;
	m_dwSubscriptionRequestCount = 0;
}

ioSubscriptionMgr::~ioSubscriptionMgr()
{	
}

ioSubscriptionMgr& ioSubscriptionMgr::GetSingleton()
{
	return Singleton<ioSubscriptionMgr>::GetSingleton();
}

void ioSubscriptionMgr::LoadINIInfo()
{
	ioINILoader_e kLoader( "config/sp2_subscription.ini" );
	kLoader.SetTitle_e( "Info" );
	
	m_iSubscriptionMentType = kLoader.LoadInt_e( "subscription_ment_type", 0 );
	m_iSubscriptionBonusCashMentType = kLoader.LoadInt_e( "subscription_bonus_cash_ment_type", 0 );
	m_iSubscriptionLimitDate = kLoader.LoadInt_e( "subscription_limit_date", 7 );

	m_dwSubscriptionRequestTime = max( 60000, kLoader.LoadInt_e( "RequestTime", 0 ) );
	m_dwSubscriptionRequestCount= max( 1, kLoader.LoadInt_e( "RequestCount", 0 ) );
}

void ioSubscriptionMgr::ProcessSubscription()
{	
}

void ioSubscriptionMgr::DeleteSubscription( DWORD dwIndex, const ioHashString &szSubscriptionID )
{
	vSubscriptionData::iterator iter = m_vSubscriptionList.begin();
	for(;iter != m_vSubscriptionList.end();iter++)
	{
		SubscriptionData &rkData = *iter;
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSubscriptionID ) continue;

		m_vSubscriptionList.erase( iter );
		return;		
	}
}

void ioSubscriptionMgr::SendSubscriptionDataToTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwSubscriptionCurTimer == 0 ||
		dwCurTime - m_dwSubscriptionCurTimer > m_dwSubscriptionRequestTime )
	{
		// 선물함 리스트 요청.
		SendSubscriptionData();
	}
}

void ioSubscriptionMgr::SendSubscriptionData()
{
	m_dwSubscriptionCurTimer = FRAMEGETTIME();

	// 보관함 리스트 요청.
	SP2Packet kPacket( CTPK_SUBSCRIPTION_REQUEST );
	kPacket << m_dwSubscriptionRequestCount;
	TCPNetwork::SendToServer( kPacket );
}

bool ioSubscriptionMgr::IsAlreadySubscription( DWORD dwIndex, const ioHashString &szSubscriptionID )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSubscriptionID ) continue;
		
		return true;
	}

	return false;
}

void ioSubscriptionMgr::ApplySubscriptionData( SP2Packet &rkPacket )
{
	int iPresentSize;
	rkPacket >> iPresentSize;

	for(int i = 0;i < iPresentSize;i++)
	{
		SubscriptionData kPresentData;
		rkPacket >> kPresentData.m_dwIndex >> kPresentData.m_szSubscriptionID >> kPresentData.m_iSubscriptionGold >> kPresentData.m_iSubscriptionBonusGold
				 >> kPresentData.m_iPresentType >> kPresentData.m_iPresentValue1 >> kPresentData.m_iPresentValue2
				 >> kPresentData.m_dwLimitDate;

		if ( kPresentData.m_iSubscriptionBonusGold > 0 )
			kPresentData.m_iMentType = m_iSubscriptionBonusCashMentType;
		else
			kPresentData.m_iMentType = m_iSubscriptionMentType;

		if( !IsAlreadySubscription( kPresentData.m_dwIndex, kPresentData.m_szSubscriptionID ) )
		{
			m_vSubscriptionList.push_back( kPresentData );
		}
	}	

	if( iPresentSize >= (int)m_dwSubscriptionRequestCount )
	{
		SendSubscriptionData();
	}
}

void ioSubscriptionMgr::ApplySubscriptionDataAdd( SP2Packet &rkPacket )
{
	int iPresentSize;
	rkPacket >> iPresentSize;
	for(int i = 0;i < iPresentSize;i++)
	{
		SubscriptionData kPresentData;
		rkPacket >> kPresentData.m_dwIndex >> kPresentData.m_szSubscriptionID >> kPresentData.m_iSubscriptionGold
				 >> kPresentData.m_iPresentType >> kPresentData.m_iPresentValue1 >> kPresentData.m_iPresentValue2
				 >> kPresentData.m_dwLimitDate;

		kPresentData.m_iMentType = m_iSubscriptionMentType;

		if( !IsAlreadySubscription( kPresentData.m_dwIndex, kPresentData.m_szSubscriptionID ) )
		{
			m_vSubscriptionList.push_back( kPresentData );
		}
	}
}

bool ioSubscriptionMgr::RecvEtcItem( SubscriptionData &rkData, int &iItemType, ComplexStringPrinterVec& vTitle, ComplexStringPrinterVec& vDesc, int &nGradeType )
{
	bool bMyInvenUpdata = true;
	nGradeType = 0;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( rkData.m_iPresentValue1 );
	if( !pEtcItem ) return bMyInvenUpdata;

	nGradeType = pEtcItem->GetGradeType();

	ioHashString szValue1Text = g_PresentMgr.GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
	ioHashString szValue2Text = g_PresentMgr.GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );

	// 즉시 사용
	if( pEtcItem->IsCanPresentUse() )
	{
		// MyInven Update
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );
		}
		pEtcItem->OnUseBtnPush( rkData.m_iPresentValue1, g_MyInfo.GetUserEtcItem(), false );
		
		bMyInvenUpdata = false;
	}
	else
	{
		int iShopTabType = SHOP_TAB_TYPE_NONE;
		int iSize = pEtcItem->GetValueSize();
		for (int j = 0; j < iSize ; j++)
		{
			iShopTabType = pEtcItem->GetShopTabType( j );
			if( iShopTabType != SHOP_TAB_TYPE_NONE )
				break;
		}

		ioHashString szMainTab = STR(1);
		ioHashString szSmallTab = "";

		DWORD dwSmallTab = 0;
		switch( iShopTabType )
		{
		case SHOP_TAB_TYPE_SOLDIER:
			{
				szMainTab = STR(2);
				szSmallTab = STR(3);
				iItemType = ItemRecvSellInfoWnd::ITEM_SOLDIER;
			}
			break;
		case SHOP_TAB_TYPE_EXTRA:
			{
				szMainTab = STR(4);
				szSmallTab = STR(5);
				iItemType = ItemRecvSellInfoWnd::ITEM_EXTRAITEM;
			}
			break;
		case SHOP_TAB_TYPE_DECO:
			{
				szMainTab = STR(6);
				szSmallTab = STR(7);
				iItemType = ItemRecvSellInfoWnd::ITEM_DECO;
			}
			break;
		case SHOP_TAB_TYPE_COSTUME:
			{
				szMainTab = "코스튬";
				szSmallTab = "기능";
				iItemType = ItemRecvSellInfoWnd::ITEM_COSTUME;
			}
			break;
		default:
			iItemType = ItemRecvSellInfoWnd::ITEM_ETCITEM;
			break;

		}

		ioComplexStringPrinter kTitle1;
		kTitle1.SetTextStyle( TS_NORMAL );
		kTitle1.SetBkColor( 0, 0, 0 );
		kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kTitle1.AddTextPiece( FONT_SIZE_17, STR(16), szValue1Text.c_str() );
		vTitle.push_back( kTitle1 );

		ioComplexStringPrinter kTitle2;
		kTitle2.SetTextStyle( TS_NORMAL );
		kTitle2.SetBkColor( 0, 0, 0 );
		kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kTitle2.AddTextPiece( FONT_SIZE_17, STR(17), szValue2Text.c_str() );
		vTitle.push_back( kTitle2 );

		ioComplexStringPrinter kDesc;
		kDesc.SetTextStyle( TS_NORMAL );
		kDesc.SetBkColor( 0, 0, 0 );	
		kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );	

		if( szSmallTab.IsEmpty() )
			kDesc.AddTextPiece( FONT_SIZE_12, STR(18), szMainTab.c_str() );
		else
			kDesc.AddTextPiece( FONT_SIZE_12, STR(19), szMainTab.c_str(), szSmallTab.c_str() );

		vDesc.push_back( kDesc );
				
	}

	return bMyInvenUpdata;
}

void ioSubscriptionMgr::ApplySubscriptionRecv( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult;

	bool bPage = false;
	bool bInfo = false;
	bool bExtraItemCustom = false;
	int nGradeType = 0;

	switch( iResult )
	{
	case SUBSCRIPTION_RECV_OK:
		{
			DWORD dwIndex;
			ioHashString szSubscriptionID;
			rkPacket >> dwIndex >> szSubscriptionID;

			vSubscriptionData::iterator iter = m_vSubscriptionList.begin();
			for(;iter != m_vSubscriptionList.end();iter++)
			{
				SubscriptionData &rkData = *iter;

				if( rkData.m_dwIndex == dwIndex && rkData.m_szSubscriptionID == szSubscriptionID )
				{
					ComplexStringPrinterVec vTitle;
					ComplexStringPrinterVec vDesc;

					int iCommandType = 0;
					int iItemInfoType = 0;
					int iReinforce = 0;
					int iClassType = -1;
					bool bIsCostume = false;

					ioHashString szValue1Text = g_PresentMgr.GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
					ioHashString szValue2Text = g_PresentMgr.GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
					ioHashString szIconName = g_PresentMgr.GetPresentIconText( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
					ioHashString szSubIconName = g_PresentMgr.GetPresentSubIconText( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );

					switch( rkData.m_iPresentType )
					{
					case PRESENT_SOLDIER:
						{
							iCommandType	= ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType	= ItemRecvSellInfoWnd::ITEM_SOLDIER;
							iClassType		= rkData.m_iPresentValue1;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(8), szValue1Text.c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(9), szValue2Text.c_str() );
							vTitle.push_back( kTitle2 );

							if( g_MyInfo.GetClassArray( rkData.m_iPresentValue1 ) == -1 )
							{
								ioComplexStringPrinter kDesc;
								kDesc.SetTextStyle( TS_NORMAL );
								kDesc.SetBkColor( 0, 0, 0 );	
								kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
								kDesc.AddTextPiece( FONT_SIZE_12, STR(10), szValue1Text.c_str(), szValue2Text.c_str() );
								vDesc.push_back( kDesc );
							}
							else
							{
								if( rkData.m_iPresentValue2 == 0 )
								{
									ioComplexStringPrinter kDesc;
									kDesc.SetTextStyle( TS_NORMAL );
									kDesc.SetBkColor( 0, 0, 0 );	
									kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
									kDesc.AddTextPiece( FONT_SIZE_12, STR(11), szValue1Text.c_str() );
									vDesc.push_back( kDesc );
								}
								else
								{
									ioComplexStringPrinter kDesc;
									kDesc.SetTextStyle( TS_NORMAL );
									kDesc.SetBkColor( 0, 0, 0 );	
									kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
									kDesc.AddTextPiece( FONT_SIZE_12, STR(12), szValue1Text.c_str(), szValue2Text.c_str() );
									vDesc.push_back( kDesc );
								}
							}

							bInfo = true;

							nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );
						}
						break;	
					case PRESENT_DECORATION:
						{
							ITEMSLOT kSlot;
							rkPacket >> kSlot.m_item_type >> kSlot.m_item_code;
							g_MyInfo.AddSlotItem( kSlot.m_item_type, kSlot.m_item_code );

							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_DECO;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(13), szValue1Text.c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(14), szValue2Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(15) );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_ETC_ITEM:
						{
							// 마이, 유미라 예외처리...
							if( COMPARE(rkData.m_iPresentValue1, ioEtcItem::EIT_ETC_PRESET_PACKAGE1, ioEtcItem::EIT_ETC_PRESET_PACKAGE100+1) )
							{
								bInfo = false;
								iCommandType = 0;
								break;
							}

							bool bNormal;
							bNormal = RecvEtcItem( rkData, iItemInfoType, vTitle, vDesc, nGradeType );

							if( bNormal )
							{
								iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
								bInfo = true;
							}
							else
							{
								bInfo = false;
							}
						}
						break;
					case PRESENT_PESO:
						{
							__int64 iNextMoney;
							rkPacket >> iNextMoney;
							g_MyInfo.SetMoney( iNextMoney );

							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NOT_GO;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(30) );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(31), szValue2Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(32), szValue2Text.c_str() );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_EXTRAITEM:
						{
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_EXTRAITEM;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(33), szValue1Text.c_str(), ( rkData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2 );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(34), szValue2Text.c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(35) );
							vDesc.push_back( kDesc );
							
							// MyInven Update
							bInfo = true;

							const ioItem *pItem = g_ItemMaker.GetItemConst( rkData.m_iPresentValue1, __FUNCTION__ );
							if( pItem )
								nGradeType = pItem->GetGradeType();
						}
						break;
					case PRESENT_EXTRAITEM_BOX:
						{
							// 랜덤박스 처리중일꺼라 따로 메세지 띄울 필요는 없지 않을까...

							// MyInven Update
							bInfo = false;
						}
						break;
					case PRESENT_RANDOM_DECO:
						{
							int iItemType, iItemCode;
							rkPacket >> iItemType >> iItemCode;

							DWORD dwCharIndex;
							CHARACTER kCharInfo;
							rkPacket >> dwCharIndex >> kCharInfo;

							bool bEqual;
							rkPacket >> bEqual;

							if( !bEqual )
							{
								int iSelectClassType = kCharInfo.m_class_type;
								int iItemSexType = (rkData.m_iPresentValue1 % 100000) / 1000;

								int iTempItemType, iTempItemCode;
								if( iItemSexType == 1 )
								{
									iTempItemType = ( iSelectClassType*100000 ) + UID_KINDRED;
									iTempItemCode = RDT_HUMAN_WOMAN;
								}
								else
								{
									iTempItemType = ( iSelectClassType*100000 ) + UID_KINDRED;
									iTempItemCode = RDT_HUMAN_MAN;
								}

								if( g_MyInfo.IsSlotItem( iTempItemType, iTempItemCode ) )
								{
									g_MyInfo.SetEquipItem( iTempItemType, iTempItemCode );
								}
							}

							g_MyInfo.AddSlotItem( iItemType, iItemCode );

							g_MyInfo.SetEquipItem( iItemType, iItemCode );
							g_MyInfo.ApplyCharDecoration( dwCharIndex, kCharInfo );

							if( pStage )
								pStage->ReLoadCharInfo( dwCharIndex, kCharInfo );

							MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
							if( pInvenWnd && pInvenWnd->IsShow() )
							{
								pInvenWnd->OnDecoSelectResult();
							}
						}
						break;
					case PRESENT_GRADE_EXP:
						{
							int iPrevGradeLevel = g_MyInfo.GetGradeLevel();
							int iGradeLevel, iGradeExpert;
							rkPacket >> iGradeLevel >> iGradeExpert;
							g_MyInfo.SetGrade( iGradeLevel, iGradeExpert );

							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NOT_GO;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(36) );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(37), szValue1Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(38), szValue2Text.c_str() );
							vDesc.push_back( kDesc );

							if( iPrevGradeLevel != g_MyInfo.GetGradeLevel() )
							{
								if( pStage )
								{
									ioBaseChar *pOwnerChar = pStage->GetBaseChar( g_MyInfo.GetPublicID() );
									if( pOwnerChar )
										pOwnerChar->SetLevel( iGradeLevel );
								}
							}

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_MEDALITEM:
						{
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_EXTRAITEM;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(39) );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(40), szValue1Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(41) );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_ALCHEMIC_ITEM:
					case PRESENT_SPIRIT:
						{
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_ALCHEMIC;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szValue1Text.c_str() );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s", szValue2Text.c_str() );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, "★ 정기에서 확인 및 사용 가능합니다." );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_PET:
						{
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_PET;

							int nPetLevel = rkData.m_iPresentValue2 / 10000;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szValue1Text.c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "Lv %d, %s", nPetLevel, szValue2Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(42) );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_COSTUME:
						{
							// UI코스튬 관련 (선물함 설명)
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_COSTUME;
							
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szValue1Text.c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s", szValue2Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, "★ 코스튬!" );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
							bIsCostume = true;
							nGradeType = g_CostumeInfoMgr.GetGradeType( rkData.m_iPresentValue1 );
						}
						break;
					case PRESENT_COSTUME_BOX:
						{
							bInfo = false;
						}
						break;
					case PRESENT_ACCESSORY:
						{
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_ACCESSORY;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szValue1Text.c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s", szValue2Text.c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, "★ 액세서리!" );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
							const ioItem *pItem = g_ItemMaker.GetItemConst( rkData.m_iPresentValue1, __FUNCTION__ );
							if( pItem )
								nGradeType = pItem->GetGradeType();
						}
						break;
					}

					m_vSubscriptionList.erase( iter );

					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
					if( pInvenWnd && pInvenWnd->IsShow() )
					{
						pInvenWnd->UpdateTab( bPage, bInfo );

						if( iCommandType != 0 )
							pInvenWnd->ShowItemRecvSellInfoWnd( iCommandType,
																iItemInfoType,
																szIconName,
																szSubIconName,
																vTitle,
																vDesc,
																iClassType,
																iReinforce,
																bExtraItemCustom,
																bIsCostume,
																nGradeType );
					}

					g_QuestMgr.QuestCompleteTerm( QC_PRESENT_RECV );
					return;
				}
			}
		}
		break;
	case SUBSCRIPTION_RECV_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(18) );
		}
		break;
	case SUBSCRIPTION_RECV_NONE_INDEX:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(19) );
		}
		break;
	case SUBSCRIPTION_RECV_CHAR_SLOT_FULL:
		{
			//g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(20) );
			NewShopItemPurchaseLeedWnd *pPurchaseLeedWnd = dynamic_cast<NewShopItemPurchaseLeedWnd*> ( g_GUIMgr.FindWnd( ITEM_PURCHASE_LEED_WND ) );
			if ( pPurchaseLeedWnd )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND );
				if ( pItem )
				{
					ioHashString sIcon = pItem->GetIconName();
					ioHashString sSubIcon = pItem->GetSubIconName();
					ioHashString sTitle= pItem->GetName();
					ioHashStringVec sDescVec;
					sDescVec.push_back( "[용병슬롯확장]이 필요합니다." );
					sDescVec.push_back( "용병 슬롯을 확장하기 위해 구입해주세요." );
					sDescVec.push_back( "상점 - 특별에서 구입하실 수 있습니다." );
					if( pPurchaseLeedWnd->SetInfo( ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND, sIcon, sSubIcon, sTitle, sDescVec ) )
						pPurchaseLeedWnd->ShowWnd();
				}
			}
		}
		break;
	case SUBSCRIPTION_RECV_CHAR_CREATE_DELAY:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(21) );
		}
		break;
	case SUBSCRIPTION_RECV_ALREADY_CHAR:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(22) );
		}
		break;
	case SUBSCRIPTION_RECV_ALREADY_DECO:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(23) );
		}
		break;
	case SUBSCRIPTION_RECV_NONE_ETCITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(24) );
		}
		break;
	case SUBSCRIPTION_RECV_CLASS_ETCITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(25) );
		}
		break;
	case SUBSCRIPTION_RECV_ETCITEM_VALUE_EXCEPT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(26) );
		}
		break;
	case SUBSCRIPTION_RECV_ETCITEM_EXCESS_MAX:
	case SUBSCRIPTION_RECV_ETCITEM_EXCESS_OVER:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(27) );
		}
		break;
	case SUBSCRIPTION_RECV_EXTRAITEM_NONE_DATE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(28) );
		break;
	case SUBSCRIPTION_RECV_EXTRAITEM_NONE_SLOT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(29) );
		break;
	case SUBSCRIPTION_RECV_MEDALITEM_NONE_DATE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		break;
	case SUBSCRIPTION_RECV_MEDALITEM_EXIST:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		break;
	case SUBSCRIPTION_RECV_MAX_COUNT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "갯수초과" );
		break;
	case SUBSCRIPTION_RECV_MAX_SLOT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "빈칸없음" );
		break;
	case SUBSCRIPTION_RECV_PRESET_FAIL:
		{
			// 아무것도 안하면됨
		}
		break;
	case SUBSCRIPTION_DISASSEMBLE_OK:
		{
			DWORD dwIndex;
			ioHashString szSubscriptionID;
			rkPacket >> dwIndex >> szSubscriptionID;

			vSubscriptionData::iterator iter = m_vSubscriptionList.begin();
			for(;iter != m_vSubscriptionList.end();iter++)
			{
				SubscriptionData &rkData = *iter;

				if( rkData.m_dwIndex == dwIndex && rkData.m_szSubscriptionID == szSubscriptionID )
				{
					m_vSubscriptionList.erase( iter );

					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
					if( pInvenWnd && pInvenWnd->IsShow() )
						pInvenWnd->UpdateTab( bPage, bInfo );

					g_QuestMgr.QuestCompleteTerm( QC_PRESENT_RECV );
					return;
				}
			}
		}
		break;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true, false );
	}
}

void ioSubscriptionMgr::ApplySubscriptionRetr( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	
	int iResult;
	rkPacket >> iResult;

	switch( iResult )
	{
	case SUBSCRIPTION_RETR_OK:
		{
			int iIndex;
			ioHashString szSubscriptionID;
			int iRetractGold;

			rkPacket >> iIndex >> szSubscriptionID;
			rkPacket >> iRetractGold;

			// 골드 갱신
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
				pShopWnd->SendGetCash( true );

			// 제거
			DeleteSubscription( iIndex, szSubscriptionID );

			// UI 갱신
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( true, true );
			}

			// 팝업
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iRetractGold, szConvertNum, sizeof( szConvertNum ) );

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "청약철회가 완료되었습니다." );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED ); 
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "환불된 골드 : %s", szConvertNum );

			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );	
			kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, "보너스 골드 등과 같은 경우엔 정책에 따라" );

			kPrinter[3].SetTextStyle( TS_NORMAL );
			kPrinter[3].SetBkColor( 0, 0, 0 );	
			kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[3].AddTextPiece( FONT_SIZE_13, "환불 금액에 포함되지 않을 수도 있습니다." );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		break;
	case SUBSCRIPTION_RETR_EXCEPTION:
		{
			int iErrorNum;
			rkPacket >> iErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, "청약철회 오류:%d", iErrorNum );
		}
		break;
	case SUBSCRIPTION_RETR_NONE_INDEX:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택한 청약상품이 없습니다." );
		}
		break;
	case SUBSCRIPTION_RETR_NONE_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택한 청약상품이 없습니다." );
		}
		break;
	case SUBSCRIPTION_RETR_LIMIT_DATE:
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->ShowSubscriptionRetrCheckLimit();
			}
		}
		break;
	}
}

bool ioSubscriptionMgr::GetSubscriptionDataToArray( IN int iArray, OUT SubscriptionData& rkData )
{
	if( !COMPARE( iArray, 0, (int)m_vSubscriptionList.size() ) )
		return false;
	
	rkData = m_vSubscriptionList[iArray];

	return true;
}

bool ioSubscriptionMgr::GetSubscriptionDataToIndex( DWORD dwIndex, IN const ioHashString& szSubscriptionID, OUT SubscriptionData& rkData )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		if( m_vSubscriptionList[i].m_dwIndex != dwIndex ) continue;
		if( m_vSubscriptionList[i].m_szSubscriptionID != szSubscriptionID ) continue;

		rkData = m_vSubscriptionList[i];
		return true;
	}

	return false;
}

bool ioSubscriptionMgr::CheckSubscriptionRetractLimit( DWORD dwIndex, const ioHashString &szSubscriptionID )
{
	SubscriptionData kData;
	GetSubscriptionDataToIndex( dwIndex, szSubscriptionID, kData );

	if( kData.m_dwLimitDate <= 0 )
		return false;

	short iYear   = (short)( 2000 + ( kData.m_dwLimitDate / 100000000 ) );
	short iMonth  = (short)( ( kData.m_dwLimitDate % 100000000 ) / 1000000 );
	short iDay    = (short)( ( kData.m_dwLimitDate % 1000000 ) / 10000 );
	short iHour   = (short)( ( kData.m_dwLimitDate % 10000 ) / 100 );
	short iMinute = (short)( kData.m_dwLimitDate % 100 );

	time_t kLimitTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMinute, 0 );
	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

	int iGapSec = kLimitTime - kCurServerTime;
	if( iGapSec > 0 )
		return true;

	return false;
}

bool ioSubscriptionMgr::CheckBuyDate( IN DWORD dwLimitDate, OUT time_t &rkBuyTime )
{
	if( dwLimitDate <= 0 )
		return false;

	short iYear   = (short)( 2000 + ( dwLimitDate / 100000000 ) );
	short iMonth  = (short)( ( dwLimitDate % 100000000 ) / 1000000 );
	short iDay    = (short)( ( dwLimitDate % 1000000 ) / 10000 );
	short iHour   = (short)( ( dwLimitDate % 10000 ) / 100 );
	short iMinute = (short)( dwLimitDate % 100 );

	time_t kLimitTime = DateHelp::ConvertSecondTime( iYear, iMonth, iDay, iHour, iMinute, 0 );
	time_t kGapTime = DateHelp::ConvertSecondTime( 0, 0, m_iSubscriptionLimitDate, 0, 0, 0 );
	
	int iLimit = (24 * 60 * 60 * m_iSubscriptionLimitDate);
	kLimitTime -= iLimit;
	rkBuyTime = kLimitTime;

	return true;
}