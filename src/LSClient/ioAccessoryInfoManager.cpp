#include "StdAfx.h"
#include "ioAccessoryInfoManager.h"

#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/SoldierSelectWnd.h"
#include "GUI/NewShopBoughtWnd.h"
#include "GUI/MyInventoryInfoWnd.h"

template<> ioAccessoryInfoManager* Singleton< ioAccessoryInfoManager >::ms_Singleton = 0;

ioAccessoryInfoManager::ioAccessoryInfoManager(void)
{
}

ioAccessoryInfoManager::~ioAccessoryInfoManager(void)
{
}

ioAccessoryInfoManager& ioAccessoryInfoManager::GetSingleton()
{
	return Singleton< ioAccessoryInfoManager >::GetSingleton();
}

void ioAccessoryInfoManager::LoadAccessoryInfo()
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	ioINILoader_e kLoader( "config/sp2_accessory.ini" );
	kLoader.SetTitle_e( "common_info" );

	m_fMortmainItemSellPrice = kLoader.LoadFloat_e( "mortmain_item_sell", 0.0f );
	m_fTimeItemSellPrice = kLoader.LoadFloat_e( "time_item_sell", 0.0f );

	kLoader.SetTitle_e( "string" );
	for( int i=0 ; i<3 ; ++i )
	{
		wsprintf_e( szKey, "CompoundErrMsg%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szCompoundErrMsg[i] = szBuf;
	}
	for( int i=0 ; i<2 ; ++i )
	{
		wsprintf_e( szKey, "AccessoryChangeErrMsg%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szAccessoryChangeErrMsg[i] = szBuf;
	}
	kLoader.LoadString_e( "ReinforceErrMsg", "", szKey, MAX_PATH );
	m_szReinforceErrMsg = szKey;
	kLoader.LoadString_e( "DeleteMsg", "", szKey, MAX_PATH );
	m_szDeleteMsg = szKey;

	ioINILoader_e kLoader2( "config/sp2_accessory_skill.ini" );
	kLoader2.SetTitle_e( "compose_common_info" );

	int iCount = kLoader2.LoadInt_e( "count", 0 );
	for( int i = 0; i < iCount; i++ )
	{
		wsprintf_e( szBuf, "compose_info%d", i+1 );
		kLoader2.SetTitle( szBuf );

		AccessoryCompoundInfo cInfo;
		cInfo.m_iCompoundCode = kLoader2.LoadInt_e( "code", 0 );
		cInfo.m_iAbilityMin = kLoader2.LoadInt_e( "ability_min", 0 );
		cInfo.m_iAbilityMax = kLoader2.LoadInt_e( "ability_max", 0 );
		cInfo.m_iAbilityGap = kLoader2.LoadInt_e( "ability_gap", 0 );

		kLoader2.LoadString_e( "name", "", szKey, MAX_PATH );
		cInfo.m_szName = szKey;
		kLoader2.LoadString_e( "skill", "", szKey, MAX_PATH );
		cInfo.m_szSkillName = szKey;

		int iSize = kLoader2.LoadInt_e( "max_tooltip_text_size", 0 );
		for( int i=0 ; i<iSize ; ++i )
		{
			ioHashString szText;
			wsprintf_e( szKey, "tooltip_text%d", i+1 );
			kLoader2.LoadString( szKey, "", szBuf, MAX_PATH );
			szText = szBuf;

			cInfo.m_vToolTipTextList.push_back( szText );
		}
		m_CompoundInfo.insert( AccessoryCompoundInfoMap::value_type( cInfo.m_iCompoundCode, cInfo ) );
	}
}

void ioAccessoryInfoManager::OnAccessoryBuyResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult;

	__int64 iPeso;
	int iCash = 0;
	int nChannelingCash = 0;

	rkPacket >> iResult;

	switch( iResult )
	{
	case ITEM_BUY_OK:
		{
			int iAccessoryCode = 0;
			int iMachineCode = 0;
			int iPeriodTime = 0;

			OnAccessoryAdd( rkPacket, iAccessoryCode );

			rkPacket >> iMachineCode;
			rkPacket >> iPeriodTime >> iPeso >> iCash >> nChannelingCash;

			g_MyInfo.SetMoney( iPeso );
			g_MyInfo.SetCash( iCash );
			g_MyInfo.SetChannelingCash( nChannelingCash );

			// UI 처리	
			NewShopBoughtWnd *pNewShopBoughtWnd = NULL;
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
				pNewShopBoughtWnd = dynamic_cast<NewShopBoughtWnd*>( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BOUGHT_WND ) );
			
			if( pNewShopBoughtWnd && pNewShopBoughtWnd->IsShow() )
				pNewShopBoughtWnd->SetResultInfo( PRESENT_ACCESSORY, iAccessoryCode, iPeriodTime, false, EET_DISABLE );
		}
		return;
	case ITEM_BUY_NON_EXIST_GOODS:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "존재하지 않는 상품입니다." );
		}
		break;
	case ITEM_BUY_PESO_SHORTAGE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "페소가 부족합니다." );
		}
		break;
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->UpdateTab( false, true );
}

void ioAccessoryInfoManager::OnAccessorySellResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	byte eResult;
	rkPacket >> eResult;

	if( eResult == ACCESSORY_SELL_OK )
	{
		int iSlotIndex = 0;
		int iAddPeso = 0;
		__int64 iPeso;
		rkPacket >> iSlotIndex;
		rkPacket >> iAddPeso;
		rkPacket >> iPeso;

		g_MyInfo.SetMoney( iPeso );

		ioHashString szItemName;
		ioHashString szIconName;
		ioHashString szSubIconName;
		bool bCustom = false;
		int iGradeType = 0;

		ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
		if( pAccessory )
		{
			AccessorySlot kSlot;
			if( pAccessory->GetAccessorySlot( iSlotIndex, kSlot ) )
			{
				szItemName = kSlot.GetName();
				szIconName = kSlot.m_IconName;

				if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
					bCustom = true;

				const ioItem* pItem = g_ItemMaker.GetItemConst( kSlot.m_iAccessoryCode, __FUNCTION__ );
				if ( pItem )
					iGradeType = pItem->GetGradeType();
			}
		}

		// 데이터 삭제
		if( !g_MyInfo.DeleteAccessory( iSlotIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 팔기 예외오류" );
			return;
		}

		// UI 처리
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );

			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iAddPeso, szConvertNum, sizeof( szConvertNum ) );

			ComplexStringPrinterVec vTitle;
			ComplexStringPrinterVec vDesc;

			ioComplexStringPrinter kTitle1;
			kTitle1.SetTextStyle( TS_NORMAL );
			kTitle1.SetBkColor( 0, 0, 0 );
			kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szItemName.c_str() );
			vTitle.push_back( kTitle1 );

			if( iAddPeso != 0 )
			{
				ioComplexStringPrinter kTitle2;
				kTitle2.SetTextStyle( TS_NORMAL );
				kTitle2.SetBkColor( 0, 0, 0 );
				kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
				kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
				vTitle.push_back( kTitle2 );

				ioComplexStringPrinter kDesc1;
				kDesc1.SetTextStyle( TS_NORMAL );
				kDesc1.SetBkColor( 0, 0, 0 );	
				kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
				kDesc1.AddTextPiece( FONT_SIZE_12, "★ [%s] 판매 완료.", szItemName.c_str() );
				vDesc.push_back( kDesc1 ); 

				ioComplexStringPrinter kDesc2;
				kDesc2.SetTextStyle( TS_NORMAL );
				kDesc2.SetBkColor( 0, 0, 0 );
				kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
				kDesc2.AddTextPiece( FONT_SIZE_12, "★ %s페소가 지급되었습니다.", szConvertNum );
				vDesc.push_back( kDesc2 );
			}
			else
			{
				ioComplexStringPrinter kDesc1;
				kDesc1.SetTextStyle( TS_NORMAL );
				kDesc1.SetBkColor( 0, 0, 0 );	
				kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
				kDesc1.AddTextPiece( FONT_SIZE_12, m_szDeleteMsg.c_str(), szItemName.c_str() );
				vDesc.push_back( kDesc1 ); 
			}
			pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_SELL, ItemRecvSellInfoWnd::ITEM_ACCESSORY, szIconName, szSubIconName, vTitle, vDesc, -1, 0, bCustom, false, iGradeType );
		}
	}
	else
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 팔기 실패(%d)", eResult );
}

void ioAccessoryInfoManager::OnAccessoryPresentResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iAccessoryCode = 0;
	OnAccessoryAdd( rkPacket, iAccessoryCode );
}

void ioAccessoryInfoManager::OnAccessoryAdd( SP2Packet &rkPacket, int &iCode )
{
	int iAccessoryCode = 0;
	int iSlotIndex = 0;
	byte btPeriodType = 0;
	int iValue1 = 0;
	int iValue2 = 0;
	int iRandomNum=0;
	rkPacket >> iAccessoryCode >> iSlotIndex;
	rkPacket >> btPeriodType;
	rkPacket >> iValue1 >> iValue2;
	rkPacket >> iRandomNum;

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( pAccessory )
	{
		AccessorySlot kSlot;
		kSlot.m_iAccessoryCode = iAccessoryCode;
		kSlot.m_iIndex = iSlotIndex;
		kSlot.m_PeriodType = btPeriodType;
		kSlot.m_iValue1 = iValue1;
		kSlot.m_iValue2 = iValue2;
		kSlot.m_iRandomNumber = iRandomNum;

#ifdef __DEV_QA__
		LOG.PrintTimeAndLog( 0, "Accessory Idx : %d, code : %d , RandomNum : %d ", iSlotIndex, iAccessoryCode, iRandomNum );
#endif
		pAccessory->AddAccessorySlotData( kSlot );
		iCode = iAccessoryCode;
	}
}

void ioAccessoryInfoManager::OnAccessoryChange( SP2Packet &rkPacket, ioPlayStage* pPlayStage )
{
	TCPNetwork::MouseBusy( false );
	
	if ( !pPlayStage )
		return;

	byte eResult;
	rkPacket >> eResult;

	ioHashString szOwner;
	rkPacket >> szOwner;
	
	if ( eResult == ACCESSORY_CHANGE_SUCCESS )
	{
		int iCharIndex, iSlotIndex, iAccessoryCode, iRandomNum, iCompoundCode, iCompoundValue;
		byte iSlot;
		bool bEquip;
		rkPacket >> iCharIndex;
		rkPacket >> iSlot;
		rkPacket >> iSlotIndex;
		rkPacket >> bEquip;
		rkPacket >> iAccessoryCode;
		rkPacket >> iRandomNum;
		rkPacket >> iCompoundCode;
		rkPacket >> iCompoundValue;
		// 교체 대상이 자신일때
		if( szOwner == g_MyInfo.GetPublicID() )
		{
			int iCharArray = g_MyInfo.GetCharArray( iCharIndex );
			g_MyInfo.ApplyChangeAccessory( iCharArray, iSlot, iSlotIndex, bEquip );
			g_MyInfo.SetUserInfoMgr();

			// UI 갱신
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->UpdateTab( false, true );

			SoldierSelectWnd *pSoldierWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
			if( pSoldierWnd && pSoldierWnd->IsShow() )
				pSoldierWnd->SettingSoldierBtn();

			MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ));
			if( pMiniSoldierWnd && pMiniSoldierWnd->IsShow() )
				pMiniSoldierWnd->SettingMiniSoldierBtn();

			if( pPlayStage->GetModeType() == MT_MYROOM )
			{
				ioPlayMode* pPlayMode = pPlayStage->GetPlayMode();
				if( pPlayMode )
					pPlayMode->SetChangeExtraItem( iCharArray, iSlot );
				return;
			}
		}

		// 아이템 다시 장착 (현재 슬롯에 있는 아이템을 복사)
		if( pPlayStage->GetModeType() != MT_MYROOM )
		{
			// 필드에 출전중인 캐릭터면 장비 교체
			ioBaseChar *pOwner = pPlayStage->GetBaseChar( szOwner );
			bool bSelectChar = true;
			if( pOwner && pOwner->IsOwnerChar() && ( g_MyInfo.GetSelectCharArray() != g_MyInfo.GetCharArray( iCharIndex ) ) )
				bSelectChar = false;
			
			if( pOwner && bSelectChar && !pPlayStage->IsModeStateResult() )
			{
				if ( !bEquip )
				{
					int iEquipAccessorySlot = iSlot + ES_RING;
					pOwner->SetEquipAccessoryInfo( iSlot, 0, 0, 0, 0, 0 );
					pOwner->GetEquipSlot()->ReleaseItem( (EquipSlot)iEquipAccessorySlot );
				}
				else
				{
					pOwner->SetEquipAccessoryInfo( iSlot, iSlotIndex, iAccessoryCode, iRandomNum, iCompoundCode, iCompoundValue );
					ioItem *pNewItem = pPlayStage->CreateItem( iAccessoryCode );
					if ( pNewItem )
					{
						pNewItem->SetOwnerName( szOwner );
						pNewItem->SetItemCreateIndex( iSlotIndex );
						pNewItem->InitializeBeforeEquip( pOwner, NULL );
						pNewItem->SetAccessoryRandomNum( iRandomNum );
						ioItem *pPreItem = pOwner->EquipItem( pNewItem, true );
						if ( pPreItem )
							SAFEDELETE( pPreItem );

						pNewItem->Initialize( pOwner, NULL );
					}
				}

				if( pOwner->GetState() == CS_EXTRAITEM_CHANGE )
					pOwner->SetState( CS_DELAY );
				
			}
		}
	}
	else if ( eResult == ACCESSORY_CHANGE_PERIOD )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szAccessoryChangeErrMsg[0].c_str() );
		return;
	}
	else
	{
		ioBaseChar *pOwner = pPlayStage->GetBaseChar( szOwner );
		if( pPlayStage->GetModeType() != MT_MYROOM && !pOwner )
			return;

		if( pOwner && pOwner->IsOwnerChar() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szAccessoryChangeErrMsg[1].c_str(), eResult );

			if( pOwner->GetState() == CS_EXTRAITEM_CHANGE )
				pOwner->SetState( CS_DELAY );
		}
	}
	
}

const float& ioAccessoryInfoManager::GetResellMortmainAccessoryPeso() const
{
	return m_fMortmainItemSellPrice; 
}

const float& ioAccessoryInfoManager::GetResellTimeAccessoryTimePeso() const
{
	return m_fTimeItemSellPrice;
}

AccessoryCompoundInfo& ioAccessoryInfoManager::FindCompoundInfo( int iCompoundCode )
{
	if( m_CompoundInfo.find( iCompoundCode ) != m_CompoundInfo.end() )
		return m_CompoundInfo.find( iCompoundCode )->second;

	static AccessoryCompoundInfo cInfo;
	return cInfo;
}

void ioAccessoryInfoManager::OnAccessoryCompound( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult;
	rkPacket >> iResult;

	switch( iResult )
	{
	case ACCESSORY_COMPOSE_SUCCESS:
		{
			int iBaseAccessoryIndex;
			int iCompoundCode;
			int iCompoundValue;
			rkPacket >> iBaseAccessoryIndex >> iCompoundCode >> iCompoundValue;
			
			ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
			if( pAccessory ) 
				pAccessory->SetAccessoryCompoundInfo( iBaseAccessoryIndex, iCompoundCode, iCompoundValue );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->OnAccessoryCompoundResult( iBaseAccessoryIndex, iCompoundCode, iCompoundValue, ioEtcItem::EIT_ETC_ACCESSORY_COMPOUND );
		}
		return;
	case ACCESSORY_COMPOSE_NOTHAVE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szCompoundErrMsg[0].c_str() );
		return;
	case ACCESSORY_COMPOSE_EXCEPTION:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szCompoundErrMsg[1].c_str() );
		return;
	case ACCESSORY_COMPOSE_ALREADY:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szCompoundErrMsg[2].c_str() );
		return;
	}
}

void ioAccessoryInfoManager::OnAccessoryReinforce( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult;
	rkPacket >> iResult;
	
	switch( iResult )
	{
	case ACCESSORY_REINFORCE_SUCCESS:
		{
			int iBaseAccessoryIndex;
			int iRandomValue;
			int iValue1, iValue2;
			rkPacket >> iBaseAccessoryIndex >> iRandomValue;
			rkPacket >> iValue1 >> iValue2;

			ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
			if( pAccessory ) 
				pAccessory->SetAccessoryReinforceInfo( iBaseAccessoryIndex, iRandomValue, iValue1, iValue2 );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->OnAccessoryReinforceResult( iBaseAccessoryIndex );
		}
		return;
	case ACCESSORY_REINFORCE_NOT_ACCESSORY:
	case ACCESSORY_REINFORCE_NOT_MATERIAL:
	case ACCESSORY_REINFORCE_NOT_ENOUGHTIME:
	case ACCESSORY_REINFORCE_EXCEPTION:
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->ResetAccessoryReinforceTab();

			g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szReinforceErrMsg.c_str() );
		}
		return;
	}
}