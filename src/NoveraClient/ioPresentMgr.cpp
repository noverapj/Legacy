
#include "stdafx.h"

#include "ioPresentMgr.h"
#include "ioUserSpiritManager.h"

#include "GUI/MyInventoryInfoWnd.h"
#include "GUI/NewShopItemPurchaseLeedWnd.h"
#include "GUI/ioNewMedalRandomBox.h"

template<> ioPresentMgr* Singleton< ioPresentMgr >::ms_Singleton = 0;

ioPresentMgr::ioPresentMgr()
{
	m_dwPresentCurTimer = 0;
	m_dwPresentRequestTime  = 0;
	m_dwPresentRequestBtnTime = 0;
	m_dwPresentRequestCount = 0;
	m_bPresentChange = false;
}

ioPresentMgr::~ioPresentMgr()
{	
	MentMap::iterator iCreate;
	for(iCreate = m_MentList.begin();iCreate != m_MentList.end();++iCreate)
	{
		vMentList &rkMentList = iCreate->second;
		int iMentLine = rkMentList.size();
		for(int i = 0;i < iMentLine;i++)
		{
			MentList &kMentList = rkMentList[i];
			kMentList.m_MentList.clear();
		}
		rkMentList.clear();
	}
	m_MentList.clear();	
}

ioPresentMgr& ioPresentMgr::GetSingleton()
{
	return Singleton<ioPresentMgr>::GetSingleton();
}

void ioPresentMgr::LoadINIInfo()
{
	ioINILoader_e kLoader( "config/sp2_present.ini" );
	kLoader.SetTitle_e( "Info" );
	
	m_dwPresentRequestTime = max( 60000, kLoader.LoadInt_e( "RequestTime", 0 ) );
	m_dwPresentRequestBtnTime = max( 60000, kLoader.LoadInt_e( "RequestBtnTime", 0 ) );
	m_dwPresentRequestCount= max( 1, kLoader.LoadInt_e( "RequestCount", 0 ) );

	m_MentList.clear();
	kLoader.SetTitle_e( "MentInfo" );
	int iMaxMent = kLoader.LoadInt_e( "MaxMent", 0 );
	iMaxMent++; // Ment0이 존재하므로 i+1을 하지 않음.
	for(int i = 0;i < iMaxMent;i++)
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		sprintf_e( szKey, "Ment%d", i );
		kLoader.SetTitle( szKey );

		short iMentType = kLoader.LoadInt_e( "MentType", -1 );
		if( iMentType == -1 )
			continue;
		
		vMentList vList;
		int iMaxLine = kLoader.LoadInt_e( "MaxLine", 0 );
		for(int j = 0;j < iMaxLine;j++)
		{
			MentList kMentList;
			sprintf_e( szKey, "line%d_count", j + 1 );
			int ilineCount = kLoader.LoadInt( szKey, 0 );
			for(int k = 0;k < ilineCount;k++)
			{
				MentData kData;
				sprintf_e( szKey, "line%d_color%d", j + 1, k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				char *pTemp = NULL;
				kData.m_dwMentColor = strtoul( szBuf, &pTemp, 16 );

				sprintf_e( szKey, "line%d_text%d", j + 1, k + 1 );
				kLoader.LoadString( szKey, "", kData.m_szMentText, MAX_PATH );
				kMentList.m_MentList.push_back( kData );
			}
			vList.push_back( kMentList );
		}

		m_MentList.insert( MentMap::value_type( iMentType, vList ) );
	}
}

void ioPresentMgr::_DebugLogTreasureCardItem()
{
	ioINILoader_e kLoader( "config/sp2_monster_treasure_card.ini" );

	int i = 0;
	char szKey[MAX_PATH];
	kLoader.SetTitle_e( "common" );

	// 선물 리스트
	int iMaxTable = kLoader.LoadInt_e( "max_table", 0 );
	for(i = 0;i < iMaxTable;i++)
	{
		sprintf_s_e( szKey, "table%d", i + 1 );
		kLoader.SetTitle( szKey );

		sprintf_s_e( szKey, "\n == TABLE%d == ", i + 1 );
		LOG.PrintTimeAndLog( 0, szKey);

		int nType = 0;
		int nValue1 = 0;
		int nValue2 = 0;
		ioHashString strName = "";

		int iMaxPresent = kLoader.LoadInt_e( "max_present", 0 );
		for(int k = 0;k < iMaxPresent;k++)
		{
			sprintf_s_e( szKey, "present%d_type", k + 1 );
			nType = kLoader.LoadInt( szKey, 0 );

			sprintf_s_e( szKey, "present%d_value1", k + 1 );
			nValue1 = kLoader.LoadInt( szKey, 0 );

			sprintf_s_e( szKey, "present%d_value2", k + 1 );
			nValue2 = kLoader.LoadInt( szKey, 0 );

			strName = g_PresentMgr.GetPresentIconText(nType, nValue1, nValue2);

			LOG.PrintTimeAndLog(0, "[%d] - [TYPE:%d] %s, %s, %s", k+1, nType,  
				g_PresentMgr.GetPresentValue1Text(nType, nValue1, nValue2).c_str() , 
				g_PresentMgr.GetPresentValue2Text(nType, nValue1, nValue2).c_str(), strName.c_str());
		}
	}
}

void ioPresentMgr::DeletePresent( DWORD dwIndex, DWORD dwSlotIndex )
{
	vPresentData::iterator iter = m_vPresentList.begin();
	for(;iter != m_vPresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_dwSlotIndex != dwSlotIndex ) continue;

		m_vPresentList.erase( iter );
		m_bPresentChange = true;
		return;		
	}
}

void ioPresentMgr::SendPresentDataToTime( int iType )
{
	if( m_dwPresentCurTimer == 0 ||
		( iType == REQUEST_CHANGE_MODE && FRAMEGETTIME() - m_dwPresentCurTimer > m_dwPresentRequestTime ) ||
		( iType == REQUEST_BTN && FRAMEGETTIME() - m_dwPresentCurTimer > m_dwPresentRequestBtnTime ) )
	{
		// 선물함 리스트 요청.
		SendPresentData();
	}
}

bool ioPresentMgr::IsEnableSendPresentDataToTime()
{
	if( m_dwPresentCurTimer == 0 ||	FRAMEGETTIME() - m_dwPresentCurTimer > m_dwPresentRequestBtnTime )
		return true;

	return false;
}

void ioPresentMgr::SendPresentData()
{
	m_dwPresentCurTimer = FRAMEGETTIME();

	// 선물함 리스트 요청.
	SP2Packet kPacket( CTPK_PRESENT_REQUEST );
	kPacket << m_dwPresentRequestCount;
	TCPNetwork::SendToServer( kPacket );
}

bool ioPresentMgr::IsAlreadyPresent( DWORD dwIndex, DWORD dwSlotIndex )
{
	int iSize = m_vPresentList.size();
	for(int i = 0;i < iSize;i++)
	{
		PresentData &rkData = m_vPresentList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_dwSlotIndex != dwSlotIndex ) continue;
		
		return true;
	}

	return false;
}

void ioPresentMgr::ApplyPresentData( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iPresentSize, iDeleteSize;
	rkPacket >> iPresentSize >> iDeleteSize;

	ioBaseChar* pMyChar = pStage->GetBaseChar( g_MyInfo.GetPublicID() );

	for(int i = 0;i < iPresentSize;i++)
	{
		PresentData kPresentData;
		rkPacket >> kPresentData.m_dwIndex >> kPresentData.m_dwSlotIndex >> kPresentData.m_szSendID >> kPresentData.m_iPresentType >> kPresentData.m_iPresentMent >> kPresentData.m_iPresentState 
				 >> kPresentData.m_iPresentValue1 >> kPresentData.m_iPresentValue2 >> kPresentData.m_iPresentValue3 >> kPresentData.m_iPresentValue4 >> kPresentData.m_dwLimitDate;

		if( kPresentData.m_iPresentState == PRESENT_STATE_DELETE )
		{
			DeletePresent( kPresentData.m_dwIndex, kPresentData.m_dwSlotIndex );
			continue;   // 이미 삭제된 선물이다. 삭제 알림도 필요없고 매일 새벽에 DB에서 일괄 삭제
		}

		if( kPresentData.m_dwLimitDate == 0 )
		{
			//삭제된 선물 알림 및 처리
			m_vDeletePresentList.push_back( kPresentData );
			DeletePresent( kPresentData.m_dwIndex, kPresentData.m_dwSlotIndex );
		}
		else if( !IsAlreadyPresent( kPresentData.m_dwIndex, kPresentData.m_dwSlotIndex ) )
		{
			if( kPresentData.m_iPresentMent == PRESENT_POPUPITEM_NO_MENT )
				kPresentData.m_iPresentState = PRESENT_STATE_NORMAL;

			if( pMyChar && pMyChar->CheckRecvFishItem( kPresentData.m_iPresentType, kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2 ) )
				kPresentData.m_iPresentState = PRESENT_STATE_NEW;

			m_vPresentList.push_back( kPresentData );
		}
		m_bPresentChange = true;
	}	

	if( iPresentSize + iDeleteSize >= (int)m_dwPresentRequestCount )
	{
		SendPresentData();
	}
}

bool ioPresentMgr::RecvEtcItem( PresentData &rkData, int &iItemType, ComplexStringPrinterVec& vTitle, ComplexStringPrinterVec& vDesc, int &nGradeType )
{
	bool bMyInvenUpdata = true;
	nGradeType = 0;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( rkData.m_iPresentValue1 );
	if( !pEtcItem ) return bMyInvenUpdata;

	nGradeType = pEtcItem->GetGradeType();

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
		kTitle1.AddTextPiece( FONT_SIZE_17, STR(16), GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
		vTitle.push_back( kTitle1 );

		ioComplexStringPrinter kTitle2;
		kTitle2.SetTextStyle( TS_NORMAL );
		kTitle2.SetBkColor( 0, 0, 0 );
		kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kTitle2.AddTextPiece( FONT_SIZE_17, STR(17), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
		vTitle.push_back( kTitle2 );

		ioEtcItemHousingBlockItem* pItem = dynamic_cast<ioEtcItemHousingBlockItem*>( g_EtcItemMgr.FindEtcItem( rkData.m_iPresentValue1 ) );
		if( rkData.m_iPresentType == PRESENT_ETC_ITEM && pItem )
		{
			ioComplexStringPrinter kDesc;
			kDesc.SetTextStyle( TS_NORMAL );
			kDesc.SetBkColor( 0, 0, 0 );
			kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc.AddTextPiece( FONT_SIZE_12, "%s", pItem->GetBlockPresentDesc().c_str() );
			vDesc.push_back( kDesc );
		}
		else
		{
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
	}

	return bMyInvenUpdata;
}

void ioPresentMgr::ApplyPresentRecv( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult;

	bool bPage = false;
	bool bInfo = false;
	bool bExtraItemCustom = false;
	switch( iResult )
	{
	case PRESENT_RECV_OK:
		{
			DWORD dwPresentIndex, dwPresentSlotIndex;
			rkPacket >> dwPresentIndex >> dwPresentSlotIndex;
			vPresentData::iterator iter = m_vPresentList.begin();
			for(;iter != m_vPresentList.end();iter++)
			{
				PresentData &rkData = *iter;
				if( rkData.m_dwIndex == dwPresentIndex && rkData.m_dwSlotIndex == dwPresentSlotIndex )
				{
					ComplexStringPrinterVec vTitle;
					ComplexStringPrinterVec vDesc;

					int iCommandType = 0;
					int iItemInfoType = 0;
					int iReinforce = 0;
					int iClassType = -1;
					bool bIsCostume = false;
					int nGradeType = 0;

					ioHashString szIconName = GetPresentIconText( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
					ioHashString szSubIconName = GetPresentSubIconText( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );

					switch( rkData.m_iPresentType )
					{
					case PRESENT_SOLDIER:
						{
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_SOLDIER;
							iClassType = rkData.m_iPresentValue1;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(8), GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(9), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							if( g_MyInfo.GetClassArray( rkData.m_iPresentValue1 ) == -1 )
							{
								ioComplexStringPrinter kDesc;
								kDesc.SetTextStyle( TS_NORMAL );
								kDesc.SetBkColor( 0, 0, 0 );	
								kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
								kDesc.AddTextPiece( FONT_SIZE_12, STR(10),
													GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str(),
													GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );

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
									kDesc.AddTextPiece( FONT_SIZE_12, STR(11), GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
									vDesc.push_back( kDesc );
								}
								else
								{
									ioComplexStringPrinter kDesc;
									kDesc.SetTextStyle( TS_NORMAL );
									kDesc.SetBkColor( 0, 0, 0 );	
									kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
									kDesc.AddTextPiece( FONT_SIZE_12, STR(12),
														GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str(),
														GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );

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
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(13), GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(14), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
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
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(31), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(32), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
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
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(33),
																  GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str(),
																  ( rkData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2 );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(34), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(35) );
							vDesc.push_back( kDesc );
							
							// MyInven Update
							bInfo = true;
							if( rkData.m_iPresentValue3 > 0 || rkData.m_iPresentValue4 > 0 )
								bExtraItemCustom = true;

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
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(37), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(38), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
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
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(40), GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
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
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

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
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "Lv %d %s", nPetLevel, GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
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
							// UI코스튬 관련 (선물함 받기)
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_COSTUME;
							
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, "★ 코스튬 받기" );
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
					case PRESENT_BONUS_CASH:
						{
							__int64 iNextMoney,iNextMoney2,iNextMoney3,iNextMoney4;
							rkPacket >> iNextMoney >> iNextMoney2 >> iNextMoney3 >> iNextMoney4;
							//g_MyInfo.SetMoney( iNextMoney );

							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NOT_GO;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "보너스 캐쉬" );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(31), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, STR(32), GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
						}
						break;
					case PRESENT_ACCESSORY:
						{
							// UI코스튬 관련 (선물함 받기)
							iCommandType = ItemRecvSellInfoWnd::ITEM_RECV_NORMAL;
							iItemInfoType = ItemRecvSellInfoWnd::ITEM_ACCESSORY;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle2 );

							ioComplexStringPrinter kDesc;
							kDesc.SetTextStyle( TS_NORMAL );
							kDesc.SetBkColor( 0, 0, 0 );	
							kDesc.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kDesc.AddTextPiece( FONT_SIZE_12, "★ 액세서리 받기" );
							vDesc.push_back( kDesc );

							// MyInven Update
							bInfo = true;
							nGradeType = g_CostumeInfoMgr.GetGradeType( rkData.m_iPresentValue1 );
						}
						break;
					}

					m_vPresentList.erase( iter );
					m_bPresentChange = true;

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
	case PRESENT_RECV_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(18) );
		}
		break;
	case PRESENT_RECV_NONE_INDEX:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(19) );
		}
		break;
	case PRESENT_RECV_CHAR_SLOT_FULL:
		{
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
	case PRESENT_RECV_CHAR_CREATE_DELAY:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(21) );
		}
		break;
	case PRESENT_RECV_ALREADY_CHAR:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(22) );
		}
		break;
	case PRESENT_RECV_ALREADY_DECO:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(23) );
		}
		break;
	case PRESENT_RECV_NONE_ETCITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(24) );
		}
		break;
	case PRESENT_RECV_CLASS_ETCITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(25) );
		}
		break;
	case PRESENT_RECV_ETCITEM_VALUE_EXCEPT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(26) );
		}
		break;
	case PRESENT_RECV_ETCITEM_EXCESS_MAX:
	case PRESENT_RECV_ETCITEM_EXCESS_OVER:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(27) );
		}
		break;
	case PRESENT_RECV_EXTRAITEM_NONE_DATE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(28) );
		break;
	case PRESENT_RECV_EXTRAITEM_NONE_SLOT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(29) );
			NewShopItemPurchaseLeedWnd *pPurchaseLeedWnd = dynamic_cast<NewShopItemPurchaseLeedWnd*> ( g_GUIMgr.FindWnd( ITEM_PURCHASE_LEED_WND ) );
			if ( pPurchaseLeedWnd )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND );
				if ( pItem )
				{
					ioHashString sIcon = pItem->GetIconName();
					ioHashString sSubIcon = pItem->GetSubIconName();
					ioHashString sTitle= pItem->GetName();
					ioHashStringVec sDescVec;
					sDescVec.push_back( "[장비슬롯확장]이 필요합니다." );
					sDescVec.push_back( "부족한 장비 슬롯을 위해 구입해주세요." );
					sDescVec.push_back( "상점 - 특별에서 구입하실 수 있습니다." );
					if( pPurchaseLeedWnd->SetInfo( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND, sIcon, sSubIcon, sTitle, sDescVec ) )
						pPurchaseLeedWnd->ShowWnd();
				}
			}
		}
		
		break;
	case PRESENT_RECV_MEDALITEM_NONE_DATE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		break;
	case PRESENT_RECV_MEDALITEM_EXIST:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		break;
	case PRESENT_RECV_MAX_COUNT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "갯수초과" );
		break;
	case PRESENT_RECV_MAX_SLOT:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "빈칸없음" );
		break;
	case PRESENT_RECV_PRESET_FAIL:
		{
			// 아무것도 안하면됨
		}
		break;
	case PRESENT_RECV_NO_GUILD:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드에 가입 되있지 않습니다." );
		break;
	case PRESENT_RECV_GUILD_ROOM_DISABLE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드본부가 활성화 되있지 않습니다." );
		break;
	case PRESENT_RECV_PERSONAL_HQ_DISABLE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "개인본부가 활성화 되있지 않습니다." );
		break;
	case PRESENT_RECV_CUSTOM_MEDAL:
		{
			DWORD dwPresentIndex, dwPresentSlotIndex;
			int iStatus[8];

			rkPacket >> dwPresentIndex >> dwPresentSlotIndex;
			for( int i = 0; i < 8; i++ )
				rkPacket >> iStatus[i];

			ioNewMedalRandomBox* pWnd = dynamic_cast<ioNewMedalRandomBox*>( g_GUIMgr.FindWnd( NEW_MEDAL_RANDOM_WND ) );
			if( pWnd )
				pWnd->SetStatus( dwPresentIndex, dwPresentSlotIndex, iStatus );
		}
		break;
	case PRESENT_DISASSEMBLE_OK:
		{
			DWORD dwPresentIndex, dwPresentSlotIndex;
			rkPacket >> dwPresentIndex >> dwPresentSlotIndex;
			vPresentData::iterator iter = m_vPresentList.begin();
			for(;iter != m_vPresentList.end();iter++)
			{
				PresentData &rkData = *iter;
				if( rkData.m_dwIndex == dwPresentIndex && rkData.m_dwSlotIndex == dwPresentSlotIndex )
				{
					m_vPresentList.erase( iter );
					m_bPresentChange = true;

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

void ioPresentMgr::ApplyPresentSell( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult;
	switch( iResult )
	{
	case PRESENT_SELL_OK:
		{
			DWORD dwPresentIndex, dwPresentSlotIndex;
			rkPacket >> dwPresentIndex >> dwPresentSlotIndex;
			vPresentData::iterator iter = m_vPresentList.begin();
			for(;iter != m_vPresentList.end();iter++)
			{
				PresentData &rkData = *iter;
				if( rkData.m_dwIndex == dwPresentIndex && rkData.m_dwSlotIndex == dwPresentSlotIndex )
				{
					int iResellPeso;
					__int64 iMyMoney;
					rkPacket >> iResellPeso >> iMyMoney;
					g_MyInfo.SetMoney( iMyMoney );

					int iReinforce = 0;
					bool bExtraItemCustom = false;
					bool bIsCostume = false;
					int nGradeType = 0;

					char szConvertNum[MAX_PATH] = "";
					Help::ConvertNumToStrComma( iResellPeso, szConvertNum, sizeof( szConvertNum ) );

					ComplexStringPrinterVec vTitle;
					ComplexStringPrinterVec vDesc;

					ioHashString szIconName = GetPresentIconText( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
					ioHashString szSubIconName  = GetPresentSubIconText( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
					
					switch( rkData.m_iPresentType )
					{
					case PRESENT_SOLDIER:
					case PRESENT_DECORATION:
					case PRESENT_ETC_ITEM:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(14),
												GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str(),
												GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(15), szConvertNum );
							vTitle.push_back( kTitle2 );

							if ( rkData.m_iPresentType == PRESENT_SOLDIER )
								nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( rkData.m_iPresentValue1 );
							else if ( rkData.m_iPresentType == PRESENT_ETC_ITEM )
							{
								ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( rkData.m_iPresentValue1 );
								if( pEtcItem ) 
									nGradeType = pEtcItem->GetGradeType();
							}
						}
						break;
					case PRESENT_PESO:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(16) );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(17), szConvertNum );
							vTitle.push_back( kTitle2 );
						}
						break;
					case PRESENT_EXTRAITEM:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(18),
													GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str(),
													( rkData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2,
													GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(19), szConvertNum );
							vTitle.push_back( kTitle2 );

							iReinforce = ( rkData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;

							if( rkData.m_iPresentValue3 > 0 || rkData.m_iPresentValue4 > 0 )
								bExtraItemCustom = true;

							const ioItem *pItem = g_ItemMaker.GetItemConst( rkData.m_iPresentValue1, __FUNCTION__ );
							if( pItem )
								nGradeType = pItem->GetGradeType();
						}
						break;
					case PRESENT_EXTRAITEM_BOX:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(20),
												GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(21), szConvertNum );
							vTitle.push_back( kTitle2 );

							RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( rkData.m_iPresentValue1 );
							if( pInfo )
								nGradeType = pInfo->GetGradeType();
						}
						break;
					case PRESENT_RANDOM_DECO:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(22),
												GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(23), szConvertNum );
							vTitle.push_back( kTitle2 );
						}
						break;
					case PRESENT_GRADE_EXP:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(24),
												GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(25), szConvertNum );
							vTitle.push_back( kTitle2 );
						}
						break;
					case PRESENT_MEDALITEM:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, STR(26),
												GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str(),
												GetPresentValue2Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, STR(27), szConvertNum );
							vTitle.push_back( kTitle2 );
						}
						break;
					case PRESENT_ALCHEMIC_ITEM:
					case PRESENT_SPIRIT:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
							vTitle.push_back( kTitle2 );
						}
						break;
					case PRESENT_PET:
						{
							int nPetLevel = rkData.m_iPresentValue2 / 10000;

							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "Lv %d %s", nPetLevel, GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
							vTitle.push_back( kTitle2 );
						}
						break;
					case PRESENT_COSTUME:
						{
							// UI코스튬 관련 (선물함 팔기)
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
							vTitle.push_back( kTitle2 );

							bIsCostume = true;
							nGradeType = g_CostumeInfoMgr.GetGradeType( rkData.m_iPresentValue1 );
						}
						break;
					case PRESENT_COSTUME_BOX:
						{
							ioComplexStringPrinter kTitle1;
							kTitle1.SetTextStyle( TS_NORMAL );
							kTitle1.SetBkColor( 0, 0, 0 );
							kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle1.AddTextPiece( FONT_SIZE_17, "%s", GetPresentValue1Text( rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2 ).c_str() );
							vTitle.push_back( kTitle1 );

							ioComplexStringPrinter kTitle2;
							kTitle2.SetTextStyle( TS_NORMAL );
							kTitle2.SetBkColor( 0, 0, 0 );
							kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
							kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
							vTitle.push_back( kTitle2 );

							CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( rkData.m_iPresentValue1 );
							if( pInfo )
								nGradeType = pInfo->GetGradeType();
						}
						break;
					}

					ioComplexStringPrinter kDesc1;
					kDesc1.SetTextStyle( TS_NORMAL );
					kDesc1.SetBkColor( 0, 0, 0 );	
					kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
					kDesc1.AddTextPiece( FONT_SIZE_12, STR(28) );
					vDesc.push_back( kDesc1 );

					ioComplexStringPrinter kDesc2;
					kDesc2.SetTextStyle( TS_NORMAL );
					kDesc2.SetBkColor( 0, 0, 0 );
					kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
					kDesc2.AddTextPiece( FONT_SIZE_12, STR(29), szConvertNum );
					vDesc.push_back( kDesc2 );

					m_vPresentList.erase( iter );
					m_bPresentChange = true;
					g_QuestMgr.QuestCompleteTerm( QC_PRESENT_RECV );

					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
					if( pInvenWnd && pInvenWnd->IsShow() )
					{
						pInvenWnd->UpdateTab( false, true );
						pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_SELL,
															ItemRecvSellInfoWnd::ITEM_PRESENT,
															szIconName, szSubIconName, vTitle, vDesc,
															-1, iReinforce, bExtraItemCustom, bIsCostume, nGradeType );
					}

#if defined( USE_GA )
					char chItemIndex[32]	= {0,};
					char chSlash[16]		= {0,};
					char chPostData[256]	= {0,};

					if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
					{
						sprintf_e( chItemIndex, "%d", rkData.m_dwIndex );
						sprintf_e( chSlash, "%%2F" );
						sprintf_e( chPostData, "%sPESO_GET_GIFT_ITEM_SELL%s%s", chSlash, chSlash, chItemIndex );
					}
					else
					{
						SafeSprintf( chItemIndex, sizeof(chItemIndex), "%1", rkData.m_dwIndex );
						SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
						SafeSprintf( chPostData, sizeof(chPostData), "%1PESO_GET_GIFT_ITEM_SELL%2%3", chSlash, chSlash, chItemIndex );
					}					

					// PESO_GET_GIFT_ITEM_SELL
					g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
						, "Peso"
						, "Gear"
						, ""
						, iResellPeso
						, chPostData );
#endif

					return;
				}
			}
		}
		break;
	case PRESENT_SELL_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		}
		break;
	case PRESENT_SELL_NONE_INDEX:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		}
		break;
	case PRESENT_SELL_NONE_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		}
		break;
	case PRESENT_SELL_NOT_SELL:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "판매 할 수 없는 아이템입니다." );
		}
		break;
	}
}

bool ioPresentMgr::IsAlarmPresentData( bool bSpecialAwardBlock )
{
	vPresentData::iterator iter = m_vPresentList.begin();
	for(;iter != m_vPresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( bSpecialAwardBlock && rkData.m_iPresentMent == PRESENT_SPECIAL_MENT ) continue;

		if( rkData.m_iPresentState == PRESENT_STATE_NEW )
			return true;
	}
	return false;
}

bool ioPresentMgr::IsDeletePresentData()
{
	if( m_vDeletePresentList.empty() )
		return false;
	return true;
}

bool ioPresentMgr::IsPresentChange()
{
	bool bChange = m_bPresentChange;
	m_bPresentChange = false;
	return bChange;
}

void ioPresentMgr::ClearAlarmPresentData( DWORD dwIndex, DWORD dwSlotIndex )
{
	vPresentData::iterator iter = m_vPresentList.begin();
	for(;iter != m_vPresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_dwSlotIndex != dwSlotIndex ) continue;

		rkData.m_iPresentState = PRESENT_STATE_NORMAL;
		return;
	}
}

DWORD ioPresentMgr::FirstAlarmPresentData( bool bSpecialAwardBlock, DWORD &rSlotIndex )
{
	vPresentData::iterator iter = m_vPresentList.begin();
	for(;iter != m_vPresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( bSpecialAwardBlock && rkData.m_iPresentMent == PRESENT_SPECIAL_MENT ) continue;

		if( rkData.m_iPresentState == PRESENT_STATE_NEW )
		{
			rSlotIndex = rkData.m_dwSlotIndex;
			return rkData.m_dwIndex;
		}
	}
	return 0;
}

void ioPresentMgr::AllRemoveDeletePresentData()
{
	m_vDeletePresentList.clear();
}

void ioPresentMgr::AllDeletePresentData()
{
	m_vPresentList.clear();
}

void ioPresentMgr::RemoveDeletePresentData( DWORD dwIndex, DWORD dwSlotIndex )
{
	vPresentData::iterator iter = m_vDeletePresentList.begin();
	for(;iter != m_vDeletePresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_dwSlotIndex != dwSlotIndex ) continue;

		m_vDeletePresentList.erase( iter );
		return;
	}
}

DWORD ioPresentMgr::FirstDeletePresentData( DWORD &rSlotIndex )
{
	if( m_vDeletePresentList.empty() )
		return 0;
	
	PresentData &rkData = m_vDeletePresentList[0];
	rSlotIndex = rkData.m_dwSlotIndex;
	return rkData.m_dwIndex;
}

void ioPresentMgr::GetPresentDataToIndex( DWORD dwIndex, DWORD dwSlotIndex, ioHashString &rSendID, short &rPresentType, short &rPresentMent, 
										  int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, DWORD &rLimitDate )
{
	vPresentData::iterator iter = m_vPresentList.begin();
	for(;iter != m_vPresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( rkData.m_iPresentState == PRESENT_STATE_DELETE ) continue;

		if( rkData.m_dwIndex == dwIndex && rkData.m_dwSlotIndex == dwSlotIndex )
		{
			rSendID			= rkData.m_szSendID;
			rPresentType	= rkData.m_iPresentType;
			rPresentMent	= rkData.m_iPresentMent;
			rPresentValue1	= rkData.m_iPresentValue1;
			rPresentValue2  = rkData.m_iPresentValue2;
			rPresentValue3	= rkData.m_iPresentValue3;
			rPresentValue4  = rkData.m_iPresentValue4;
			rLimitDate      = rkData.m_dwLimitDate;
			return;
		}
	}
}

void ioPresentMgr::GetDeletePresentDataToIndex( DWORD dwIndex, DWORD dwSlotIndex, ioHashString &rSendID, short &rPresentType, short &rPresentMent,
												int &rPresentValue1, int &rPresentValue2, int &rPresentValue3, int &rPresentValue4, DWORD &rLimitDate )
{
	vPresentData::iterator iter = m_vDeletePresentList.begin();
	for(;iter != m_vDeletePresentList.end();iter++)
	{
		PresentData &rkData = *iter;
		if( rkData.m_iPresentState == PRESENT_STATE_DELETE ) continue;

		if( rkData.m_dwIndex == dwIndex && rkData.m_dwSlotIndex == dwSlotIndex )
		{
			rSendID			= rkData.m_szSendID;
			rPresentType	= rkData.m_iPresentType;
			rPresentMent	= rkData.m_iPresentMent;
			rPresentValue1	= rkData.m_iPresentValue1;
			rPresentValue2  = rkData.m_iPresentValue2;
			rPresentValue3	= rkData.m_iPresentValue3;
			rPresentValue4  = rkData.m_iPresentValue4;
			rLimitDate      = rkData.m_dwLimitDate;
			return;
		}
	}
}

DWORD ioPresentMgr::GetPresentIndexToArray( int iArray, DWORD &rSlotIndex )
{
	if( !COMPARE( iArray, 0, (int)m_vPresentList.size() ) ) return 0;
	
	PresentData &rkData = m_vPresentList[iArray];
	if( rkData.m_iPresentState == PRESENT_STATE_DELETE ) return 0;

	rSlotIndex = rkData.m_dwSlotIndex;
	return rkData.m_dwIndex;
}

int ioPresentMgr::GetMentMaxLine( short iPresentMent )
{
	MentMap::iterator iter = m_MentList.find( iPresentMent );
	if( iter != m_MentList.end() )
	{
		vMentList &rkList = iter->second;
		return rkList.size();
	}
	return 0;
}

void ioPresentMgr::GetMentLineText( short iPresentMent, int iLine, ioComplexStringPrinter &rkPrinter )
{
	MentMap::iterator iter = m_MentList.find( iPresentMent );
	if( iter != m_MentList.end() )
	{
		vMentList &rkList = iter->second;
		if( !COMPARE( iLine, 0, (int)rkList.size() ) ) 
			return;

		int iLineSize = rkList[iLine].m_MentList.size();
		for(int i = 0;i < iLineSize;i++)
		{
			MentData &rkMent = rkList[iLine].m_MentList[i];
			rkPrinter.SetTextColor( rkMent.m_dwMentColor );
			rkPrinter.AddTextPiece( FONT_SIZE_13, rkMent.m_szMentText );
		}
	}
}

LSC_PresentReward_info* ioPresentMgr::GetRewardInfo(DWORD dwPresentRewardIndex)
{
/*
	if (m_pPresentRewardInfoMgr)
	{
		return m_pPresentRewardInfoMgr->GetData(dwPresentRewardIndex);
	}*/

	return NULL;
}


const ioHashString& ioPresentMgr::GetPresentValue1Text(DWORD dwPresentRewardIndex)
{
	LSC_PresentReward_info* pInfo = GetRewardInfo(dwPresentRewardIndex);
	if (pInfo)
	{
		return GetPresentValue1Text(pInfo->ItemType, pInfo->ItemCode, pInfo->PresentValue);
	}

	static ioHashString szReturn;
	return szReturn;
}

const ioHashString& ioPresentMgr::GetPresentValue2Text(DWORD dwPresentRewardIndex)
{
	LSC_PresentReward_info* pInfo = GetRewardInfo(dwPresentRewardIndex);
	if (pInfo)
	{
		return GetPresentValue2Text(pInfo->ItemType, pInfo->PresentPeriod, pInfo->PresentValue);
	}

	static ioHashString szReturn;
	return szReturn;
}

const ioHashString& ioPresentMgr::GetPresentIconText(DWORD dwPresentRewardIndex)
{
	LSC_PresentReward_info* pInfo = GetRewardInfo(dwPresentRewardIndex);
	if (pInfo)
	{
		return GetPresentIconText(pInfo->ItemType, pInfo->ItemCode, pInfo->PresentValue);
	}

	static ioHashString szReturn;
	return szReturn;
}

const ioHashString& ioPresentMgr::GetPresentSubIconText(DWORD dwPresentRewardIndex)
{
	LSC_PresentReward_info* pInfo = GetRewardInfo(dwPresentRewardIndex);
	if (pInfo)
	{
		return GetPresentSubIconText(pInfo->ItemType, pInfo->ItemCode, pInfo->PresentValue);
	}

	static ioHashString szReturn;
	return szReturn;
}

const ioHashString &ioPresentMgr::GetPresentValue1Text( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	static ioHashString szReturn;
	switch( iPresentType )
	{
	case PRESENT_SOLDIER:
		{
			szReturn = g_MyInfo.GetClassName( iPresentValue1 );
		}
		break;
	case PRESENT_DECORATION:
		{
			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );

			szReturn = kData.m_szName;
		}
		break;
	case PRESENT_ETC_ITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
			if( !pEtcItem )
				szReturn = "None";
			else
				szReturn = pEtcItem->GetName();
		}
		break;
	case PRESENT_PESO:
		{
			szReturn = STR(1);
		}
		break;
	case PRESENT_EXTRAITEM:
	case PRESENT_ACCESSORY:
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( iPresentValue1, __FUNCTION__ );
			if( pItem )	
				szReturn = pItem->GetName();
			else
				szReturn = "None";
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
			if( pInfo )
				szReturn = pInfo->GetName();
			else
				szReturn = "None";
		}
		break;
	case PRESENT_RANDOM_DECO:
		{
			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );

			char szTemp[MAX_PATH] = "";
			if( iDecoType == UID_KINDRED )
			{
				SafeSprintf( szTemp, sizeof( szTemp), STR(2), kData.m_szName.c_str() );
			}
			else
			{
				if( iSexType == 0 )
					SafeSprintf( szTemp, sizeof( szTemp), STR(3), kData.m_szName.c_str() );
				else
					SafeSprintf( szTemp, sizeof( szTemp), STR(4), kData.m_szName.c_str() );
			}

			szReturn = szTemp;
		}
		break;
	case PRESENT_GRADE_EXP:
		{
			szReturn = STR(5);
		}
		break;
	case PRESENT_MEDALITEM:
		{
			const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( iPresentValue1 );
			if( pItem )	
				szReturn = pItem->m_sName;
			else
				szReturn = "None";
		}
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		{
			int nPetRank = iPresentValue2%10000;
			ioHashString szTemp = g_PetInfoMgr.GetPetName( iPresentValue1, (PetRankType)nPetRank );
			if( !szTemp.IsEmpty() )
				szReturn = szTemp;
			else
				szReturn = "None";
		}
		break;
	case PRESENT_COSTUME:
		{
			// UI코스튬 관련 (선물함 이름)
			ioHashString szTemp = g_CostumeInfoMgr.GetCostumeName( iPresentValue1 );
			if( !szTemp.IsEmpty() )
				szReturn = szTemp;
			else
				szReturn = "None";
		}
		break;
	case PRESENT_COSTUME_BOX:
		{
			CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iPresentValue1 );
			if( pInfo )
				szReturn = pInfo->GetName();
			else
				szReturn = "None";
		}
		break;
	case PRESENT_BONUS_CASH:
		szReturn = "보너스캐쉬";
		break;
	case PRESENT_SPIRIT:
		szReturn = g_SpiritMgr.GetName(iPresentValue1);
		break;
	}
	return szReturn;
}

const ioHashString &ioPresentMgr::GetPresentValue2Text( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	static ioHashString szReturn;
	switch( iPresentType )
	{
	case PRESENT_SOLDIER:
		{
			if( iPresentValue2 == 0 )
			{
				szReturn = STR(1);
			}
			else
			{
				char szTemp[MAX_PATH] = "";
				int iTotalLimitDate = iPresentValue2 / 60;
				int iHour   = iTotalLimitDate / 60;      
				int iMinute = iTotalLimitDate % 60;
				if( iMinute == 0 )
					SafeSprintf( szTemp, sizeof( szTemp ), STR(2), iHour );
				else
					SafeSprintf( szTemp, sizeof( szTemp ), STR(3), iHour, iMinute );
				szReturn = szTemp;
			}
		}
		break;	
	case PRESENT_DECORATION:
		{		
			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );

			char szTemp[MAX_PATH] = "";
			if( iSexType == 0 ) 
				SafeSprintf( szTemp, sizeof( szTemp), STR(4), g_MyInfo.GetClassName( iClassType ) );
			else
				SafeSprintf( szTemp, sizeof( szTemp), STR(5), g_MyInfo.GetClassName( iClassType ) );
			szReturn = szTemp;
		}
		break;
	case PRESENT_ETC_ITEM:
		{
			char szTemp[MAX_PATH] = "";
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
			if( !pEtcItem )
			{						
				sprintf_e( szTemp, "None" );
			}
			else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FRIEND_SLOT_EXTEND )
			{
				SafeSprintf( szTemp, sizeof(szTemp), STR(6), iPresentValue2 );
			}
			else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND ||
					 pEtcItem->GetType() == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND ||
					 pEtcItem->GetType() == ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND )
			{
				SafeSprintf( szTemp, sizeof(szTemp), STR(7), iPresentValue2 );
			}
			else if( pEtcItem->GetUseType() == ioEtcItem::UT_ETERNITY )
			{
				SafeSprintf( szTemp, sizeof(szTemp), STR(8) );
			}
			else if( pEtcItem->GetUseType() == ioEtcItem::UT_TIME )
			{
				if( iPresentValue2 > 3600 )
				{
					SafeSprintf( szTemp, sizeof(szTemp), STR(9), iPresentValue2 / 3600 );
				}
				else
				{
					SafeSprintf( szTemp, sizeof(szTemp), STR(10), iPresentValue2 / 60 );
				}
			}
			else if( pEtcItem->GetUseType() == ioEtcItem::UT_COUNT || 
					 pEtcItem->GetUseType() == ioEtcItem::UT_ONCE )
			{
				char szConvertNum[MAX_PATH] = "";
				Help::ConvertNumToStrComma( iPresentValue2, szConvertNum, sizeof( szConvertNum ) );
				SafeSprintf( szTemp, sizeof(szTemp), STR(11), szConvertNum );
			}
			else if( pEtcItem->GetUseType() == ioEtcItem::UT_DATE )
			{
				if( g_EtcItemMgr.IsBlockEtcItem( iPresentValue1 ) && iPresentValue2 == 0 )
					SafeSprintf( szTemp, sizeof(szTemp), STR(22) );
				else
					SafeSprintf( szTemp, sizeof(szTemp), STR(12), iPresentValue2 );
			}
			szReturn = szTemp;
		}
		break;
	case PRESENT_PESO:
		{
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iPresentValue1, szConvertNum, sizeof( szConvertNum ) );
			
			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), STR(13), szConvertNum );
			szReturn = szTemp;
		}
		break;
	case PRESENT_EXTRAITEM:
	case PRESENT_ACCESSORY:
		{
			char szTemp[MAX_PATH] = "";
			int iLimitDate = iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2;
			if( iLimitDate == 0 )
			{
				SafeSprintf( szTemp, sizeof( szTemp ), STR(14) );
			}
			else if( (iLimitDate/24) > 0 )
			{
				SafeSprintf( szTemp, sizeof( szTemp ), STR(15), iLimitDate/24 );
			}
			else
			{
				SafeSprintf( szTemp, sizeof( szTemp ), STR(17), iLimitDate );
			}
			szReturn = szTemp;
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			szReturn = STR(16);
		}
		break;
	case PRESENT_RANDOM_DECO:
		{
			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), STR(14) );
			szReturn = szTemp;
		}
		break;
	case PRESENT_GRADE_EXP:
		{
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iPresentValue1, szConvertNum, sizeof( szConvertNum ) );

			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), STR(18), szConvertNum );
			szReturn = szTemp;
		}
		break;
	case PRESENT_MEDALITEM:
		{
			char szTemp[MAX_PATH] = "";
			int iLimitTime = iPresentValue2;
			if( iLimitTime == 0 )
			{
				SafeSprintf( szTemp, sizeof( szTemp ), STR(19) );
			}
			else if( (iLimitTime/24) > 0 )
			{
				SafeSprintf( szTemp, sizeof( szTemp ), STR(20), iLimitTime/24 );
			}
			else
			{
				SafeSprintf( szTemp, sizeof( szTemp ), STR(21), iLimitTime );
			}
			szReturn = szTemp;
		}
		break;
	case PRESENT_ALCHEMIC_ITEM:
	case PRESENT_SPIRIT:
		{
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iPresentValue2, szConvertNum, sizeof( szConvertNum ) );
			
			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), "%s개", szConvertNum );
			szReturn = szTemp;
		}
		break;
	case PRESENT_PET:
		{
			int nPetRank = iPresentValue2%10000;
			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), "랭크 %s", g_PetInfoMgr.GetRankName( nPetRank ).c_str() );
			szReturn = szTemp;
		}
		break;
	case PRESENT_COSTUME:
		{
			// UI코스튬 관련 (선물함 영구, 기간제)
			char szTemp[MAX_PATH] = "";
			int iLimitDate = iPresentValue2%10000;
			if( iLimitDate == 0 )
				SafeSprintf( szTemp, sizeof( szTemp ), STR(14) );
			else if( (iLimitDate/24) > 0 )
				SafeSprintf( szTemp, sizeof( szTemp ), STR(15), iLimitDate/24 );
			else
				SafeSprintf( szTemp, sizeof( szTemp ), STR(17), iLimitDate );
			szReturn = szTemp;
		}
		break;
	case PRESENT_COSTUME_BOX:
		{
			szReturn = STR(16);
		}
		break;
	case PRESENT_BONUS_CASH:
		{
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iPresentValue1, szConvertNum, sizeof( szConvertNum ) );

			char szTemp[MAX_PATH] = "";
			SafeSprintf( szTemp, sizeof( szTemp ), "%s 골드", szConvertNum );
			szReturn = szTemp;
		}
		break;
	}
	return szReturn;
}

const ioHashString &ioPresentMgr::GetPresentIconText( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	static ioHashString szReturn;
	switch( iPresentType )
	{
	case PRESENT_SOLDIER:
		{
			szReturn = g_MyInfo.GetMySoldierIconName( iPresentValue1 );
		}
		break;	
	case PRESENT_DECORATION:
		{		
			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );

			szReturn = kData.m_szIcon;
		}
		break;
	case PRESENT_ETC_ITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
			if( pEtcItem )
				szReturn = pEtcItem->GetIconName();
		}
		break;
	case PRESENT_PESO:
		{
			szReturn = "UIIconPack13#quest_002";
		}
		break;
	case PRESENT_EXTRAITEM:
	case PRESENT_ACCESSORY:
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( iPresentValue1, __FUNCTION__ );
			if( pItem )	
				szReturn = pItem->GetItemLargeIconName();
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
			if( pInfo )
				szReturn = pInfo->GetIconName();
		}
		break;
	case PRESENT_RANDOM_DECO:
		{
			int iClassType = iPresentValue1 / 100000;
			int iSexType   = (iPresentValue1 % 100000) / 1000;
			int iDecoType  = iPresentValue1 % 1000;
			const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iPresentValue2 );
			szReturn = kData.m_szIcon;
		}
		break;
	case PRESENT_GRADE_EXP:
		{
			szReturn = "UIIconPack13#quest_001";
		}
		break;
	case PRESENT_MEDALITEM:
		{			
			const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( iPresentValue1 );
			if( pItem )	
				szReturn = pItem->m_sIcon;
		}
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		{
			int nPetRank = iPresentValue2%10000;
			szReturn = g_PetInfoMgr.GetPetIcon( iPresentValue1, (PetRankType)nPetRank );
		}
		break;
	case PRESENT_COSTUME:
		{
			// UI코스튬 관련 (선물함 아이콘)
			szReturn = g_CostumeInfoMgr.GetCostumeIcon( iPresentValue1 );
		}
		break;
	case PRESENT_COSTUME_BOX:
		{
			CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iPresentValue1 );
			if( pInfo )
				szReturn = pInfo->GetIconName();
		}
		break;
	case PRESENT_BONUS_CASH:
		{
			szReturn = "UIIconPack101#Cash";
		}
		break;
	case PRESENT_SPIRIT:
		{
			szReturn = g_SpiritMgr.GetIconName( iPresentValue1 );
		}
		break;
	}
	return szReturn;
}

const ioHashString &ioPresentMgr::GetPresentSubIconText( short iPresentType, int iPresentValue1, int iPresentValue2 )
{
	static ioHashString szReturn;
	switch( iPresentType )
	{
	case PRESENT_SOLDIER:
		{
			szReturn = g_MyInfo.GetSoldierSubIconName( iPresentValue1 );
		}
		break;	
	case PRESENT_ETC_ITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iPresentValue1 );
			if( pEtcItem )
				szReturn = pEtcItem->GetSubIconName();
		}
		break;
	case PRESENT_MEDALITEM:
		{			
			const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( iPresentValue1 );
			if( pItem )	
				szReturn = pItem->m_sSubIcon;
		}
		break;
	}
	return szReturn;
}

int ioPresentMgr::GetPresentItemType(DWORD dwPresentRewardIndex)
{
	LSC_PresentReward_info* pInfo = GetRewardInfo(dwPresentRewardIndex);
	if (pInfo)
	{
		return pInfo->ItemType;
	}

	return -1;
}

const ioHashString &ioPresentMgr::GetLmitDateText( DWORD dwLimitDate )
{
	static ioHashString szReturn;

	if( dwLimitDate == 0 )
	{
		szReturn.Clear();
		return szReturn;
	}

	short iYear   = (short)( 2000 + ( dwLimitDate / 100000000 ) );
	short iMonth  = (short)( ( dwLimitDate % 100000000 ) / 1000000 );
	short iDay    = (short)( ( dwLimitDate % 1000000 ) / 10000 );
	short iHour   = (short)( ( dwLimitDate % 10000 ) / 100 );
	short iMinute = (short)( dwLimitDate % 100 );

	char szLimitDate[MAX_PATH]="";
	SafeSprintf( szLimitDate, sizeof( szLimitDate ), STR(1), iMonth, iDay, iHour, iMinute );
	szReturn = szLimitDate;
	return szReturn;
}

const int ioPresentMgr::GetGradeType( int nPresentType, int nPresentCode )
{
	int nGradeType = 0;
	switch( nPresentType )
	{
	case PRESENT_SOLDIER:
		nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( nPresentCode );
		break;
	case PRESENT_EXTRAITEM:
	case PRESENT_ACCESSORY:
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( nPresentCode, __FUNCTION__ );
			if( pItem )
				nGradeType = pItem->GetGradeType();
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		{
			RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( nPresentCode );
			if( pInfo )
				nGradeType = pInfo->GetGradeType();
		}
		break;
	case PRESENT_COSTUME:
		{
			nGradeType = g_CostumeInfoMgr.GetGradeType( nPresentCode );
		}
		break;
	case PRESENT_COSTUME_BOX:
		{
			CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( nPresentCode );
			if( pInfo )
				nGradeType = pInfo->GetGradeType();
		}
		break;
	case PRESENT_ETC_ITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( nPresentCode );
			if( pEtcItem ) 
				nGradeType = pEtcItem->GetGradeType();
		}
		break;
	}
	return nGradeType;
}