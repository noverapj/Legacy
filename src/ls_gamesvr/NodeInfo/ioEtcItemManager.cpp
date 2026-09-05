

#include "stdafx.h"

#include "ioEtcItemManager.h"
#include "ioSetItemInfoManager.h"
#include "iosalemanager.h"
#include "../Local/ioLocalManager.h"

#include <strsafe.h>

template<> ioEtcItemManager* Singleton< ioEtcItemManager >::ms_Singleton = 0;

ioEtcItemManager::ioEtcItemManager()
{
}

ioEtcItemManager::~ioEtcItemManager()
{
	Clear();
}

ioEtcItemManager& ioEtcItemManager::GetSingleton()
{
	return Singleton< ioEtcItemManager >::GetSingleton();
}

void ioEtcItemManager::LoadEtcItem( const char *szFileName, bool bCreateLoad /*= true */ )
{
	if( bCreateLoad )
		Clear();

	ioINILoader kLoader;
	if( bCreateLoad )
		kLoader.LoadFile( szFileName );
	else
		kLoader.ReloadFile( szFileName );

	char szBuf[MAX_PATH];
	int iMaxCount = kLoader.LoadInt( "common", "max", 0 );
	for( int i=0 ; i<iMaxCount ; i++ )
	{
		wsprintf( szBuf, "etcitem%d", i+1 );
		kLoader.SetTitle( szBuf );

		ParseEtcItem( kLoader, i+1 , bCreateLoad );
	}

	// test
	if( false )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Start LoadEtcItem :%d", (int) bCreateLoad );
		EtcItemMap::iterator iter;
		for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
		{
			ioEtcItem *pEtcItem = iter->second;
			if( pEtcItem )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "name             = %s", pEtcItem->GetName().c_str() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "type             = %d", pEtcItem->GetType() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "max_use          = %d", pEtcItem->GetMaxUse() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "extra_type       = %d", pEtcItem->GetExtraType() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "can_sell         = %d", (int)pEtcItem->IsCanSell() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "sell_peso        = %d", pEtcItem->GetSellPeso() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "mortmain_sell_peso        = %d", pEtcItem->GetMortmainSellPeso() );
				
				enum { MAX_LOOP = 100, };
				for (int i = 0; i < MAX_LOOP ; i++)
				{
					if( pEtcItem->GetValue( i ) == 0 )
						break;
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "value%d       = %d", i+1, pEtcItem->GetValue( i ) );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "peso%d        = %d", i+1, pEtcItem->GetPeso( i ) );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "cash%d        = %d", i+1, pEtcItem->GetCash( i ) );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "bonus_peso%d  = %d", i+1, pEtcItem->GetBonusPeso( i ) );
				}
			}
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "End LoadEtcItem :%d", (int) bCreateLoad );
	}
	//
}

void ioEtcItemManager::ParseEtcItem( ioINILoader &rkLoader, int iIndex, bool bCreateLoad )
{
	DWORD dwType = rkLoader.LoadInt( "type", 0 );
	if( dwType == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Invalid etc item type : [%d]", iIndex );
		return;
	}

	if( bCreateLoad && FindEtcItem( dwType ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Already exist etc item : [%d] [%d]", dwType, iIndex );
		return;
	}

	ioEtcItem *pEtcItem = NULL;
	if( bCreateLoad )
		pEtcItem = CreateEtcItem( dwType );
	else
		pEtcItem = FindEtcItem( dwType );
	
	if( !pEtcItem )
		return;

	pEtcItem->SetType( dwType ); 
	pEtcItem->SetUseType( ioEtcItem::UseType(dwType/ioEtcItem::USE_TYPE_CUT_VALUE) );

	char szBuf[MAX_PATH];
	rkLoader.LoadString( "name", "", szBuf, MAX_PATH );
	if( bCreateLoad ) // new를 하게되므로 reload시에는 생략
		pEtcItem->SetName( ioHashString(szBuf) );

	pEtcItem->SetUseValue( rkLoader.LoadInt( "use_value", 0 ) );
	pEtcItem->SetMaxUse( rkLoader.LoadInt( "max_use", 0) );
	pEtcItem->SetExtraType( rkLoader.LoadInt( "extra_type", 0 ) );
	pEtcItem->SetSellPeso( rkLoader.LoadInt( "sell_peso", 0 ) );
	pEtcItem->SetMortmainSellPeso( rkLoader.LoadInt( "mortmain_sell_peso", 0 ) );
	pEtcItem->SetCanSell( rkLoader.LoadBool( "can_sell", false ) );
	pEtcItem->SetSpecialGoods( rkLoader.LoadBool( "special_goods", false) );
	pEtcItem->SetSQLUpdateInfo( rkLoader.LoadBool( "SQL_update", true) );
	pEtcItem->SetNeedRealCash( rkLoader.LoadBool( "need_real_cash", false) );

	char szKeyName[MAX_PATH]="";
	int  iKeyValue = -1;
	enum { MAX_LOOP = 100, };
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		StringCbPrintf( szKeyName, sizeof(szKeyName), "value%d", i+1 );
		iKeyValue = rkLoader.LoadInt( szKeyName, -1 );
		if( iKeyValue == -1 )
			break;
		if( bCreateLoad )
			pEtcItem->AddValue( iKeyValue );
		else
			pEtcItem->SetValue( i , iKeyValue );

		StringCbPrintf( szKeyName, sizeof(szKeyName), "peso%d", i+1 );
		if( bCreateLoad )
			pEtcItem->AddPeso( rkLoader.LoadInt( szKeyName, 0 ) );
		else
			pEtcItem->SetPeso( i, rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf( szKeyName, sizeof(szKeyName), "cash%d", i+1 );
		if( bCreateLoad )
			pEtcItem->AddCash( rkLoader.LoadInt( szKeyName, 0 ) );	
		else
			pEtcItem->SetCash( i, rkLoader.LoadInt( szKeyName, 0 ) );	

		StringCbPrintf( szKeyName, sizeof(szKeyName), "bonus_peso%d", i+1 );
		if( bCreateLoad )
			pEtcItem->AddBonusPeso( rkLoader.LoadInt( szKeyName, 0 ) );	
		else
			pEtcItem->SetBonusPeso( i, rkLoader.LoadInt( szKeyName, 0 ) );	

		StringCbPrintf( szKeyName, sizeof(szKeyName), "can_mortmain%d", i+1 );
		if( bCreateLoad )
			pEtcItem->AddCanMortmain( rkLoader.LoadInt( szKeyName, 0 ) );
		else
			pEtcItem->SetCanMortmain( i, rkLoader.LoadInt( szKeyName, 0 ) );

		StringCbPrintf( szKeyName, sizeof(szKeyName), "subscription_type%d", i+1 );
		if( bCreateLoad )
			pEtcItem->AddSubscription( rkLoader.LoadInt( szKeyName, 0 ) );	
		else
			pEtcItem->SetSubscription( i, rkLoader.LoadInt( szKeyName, 0 ) );	

		StringCbPrintf( szKeyName, sizeof(szKeyName), "active%d", i+1 );
		int iActive = rkLoader.LoadInt( szKeyName, 1 );
		if( bCreateLoad )
			pEtcItem->AddActive( iActive == 1 );	
		else
			pEtcItem->SetActive( i, iActive == 1 );	

		g_SaleMgr.LoadINI( bCreateLoad, rkLoader, ioSaleManager::IT_ETC, dwType, i, i );
	}
	pEtcItem->LoadProperty( rkLoader );
	
	if( bCreateLoad )
		m_EtcItemMap.insert( EtcItemMap::value_type( dwType, pEtcItem ) );
}

ioEtcItem* ioEtcItemManager::FindEtcItem( DWORD dwType )
{
	dwType = GetEtcItemTypeExceptClass( dwType );
	EtcItemMap::const_iterator iter = m_EtcItemMap.find( dwType );
	if( iter != m_EtcItemMap.end() )
		return iter->second;

	return NULL;
}

const ioEtcItem* ioEtcItemManager::GetEtcItem( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetEtcItemCount() ) )
	{
		EtcItemMap::const_iterator iter = m_EtcItemMap.begin();
		std::advance( iter, iIdx );

		return iter->second;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEtcItemManager::GetEtcItem - overflow(%d/%d)",
							iIdx, GetEtcItemCount() );

	return NULL;
}

int ioEtcItemManager::GetEtcItemCount() const
{
	return m_EtcItemMap.size();
}

DWORD ioEtcItemManager::GetEtcItemTypeExceptClass( DWORD dwTypeWithClass )
{
	// 2100101 -> 2100001 중간에 클래스값을 삭제한다. 
	int iFirstType = (dwTypeWithClass%USE_TYPE_POS)/FIRST_TYPE_POS;
	if( iFirstType != CLASS_ETC_ITEM_TYPE )
		return dwTypeWithClass;

	int iUseType    = dwTypeWithClass / USE_TYPE_POS;
	int iSecondType = dwTypeWithClass % SECOND_TYPE_POS;

	DWORD dwNoneClassType = ( iUseType * USE_TYPE_POS ) + ( iFirstType * FIRST_TYPE_POS ) + ( iSecondType );

	return dwNoneClassType;
}

bool ioEtcItemManager::IsRightClass( DWORD dwType )
{
	int iFirstType = (dwType%USE_TYPE_POS)/FIRST_TYPE_POS;
	if( iFirstType != CLASS_ETC_ITEM_TYPE )
		return true; // 크래스가 없는 아이템이라면 TRUE

	int iClassType      = ( dwType % FIRST_TYPE_POS ) / SECOND_TYPE_POS ;
	DWORD dwSetItemCode = iClassType + SET_ITEM_CODE;
	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByCode( dwSetItemCode );
	if( !pSetInfo )
	{
		return false;
	}

	return true;
}

ioEtcItem* ioEtcItemManager::CreateEtcItem( DWORD dwType )
{
	if( dwType == ioEtcItem::EIT_ETC_WHOLE_CHAT )
		return new ioEtcItemWholeChat;
	else if( dwType == ioEtcItem::EIT_ETC_FRIEND_SLOT_EXTEND )
		return new ioEtcItemFriendSlotExtend;
	else if( dwType == ioEtcItem::EIT_ETC_PESO_EXP_BONUS )
		return new ioEtcItemPesoExpBonus;
	else if( dwType == ioEtcItem::EIT_ETC_CHANGE_ID )
		return new ioEtcItemChangeID;
	else if( dwType == ioEtcItem::EIT_ETC_CUSTOM_SOUND )
		return new ioEtcItemCustomSound;
	else if( dwType == ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR )
		return new ioEtcItemBuyMortmainChar;
	else if( dwType == ioEtcItem::EIT_ETC_GUILD_CREATE )
		return new ioEtcItemGuildCreate;
	else if( dwType == ioEtcItem::EIT_ETC_GUILD_MARK_CHANGE )
		return new ioEtcItemGuildMarkChange;
	else if( dwType == ioEtcItem::EIT_ETC_GUILD_NAME_CHANGE )
		return new ioEtcItemGuildNameChange;
	else if( dwType == ioEtcItem::EIT_ETC_GROWTH_DOWN )
		return new ioEtcItemGrowthDown;
	else if( dwType == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND )
		return new ioEtcItemCharSlotExtend;
	else if( dwType == ioEtcItem::EIT_ETC_PESO_BONUS)
		return new ioEtcItem;
	else if( dwType == ioEtcItem::EIT_ETC_EXP_BONUS)
		return new ioEtcItem;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_BAIT )
		return new ioEtcItemFishingBait;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_ROD )
		return new ioEtcItemFishingRod;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND )
		return new ioEtcItemFishingSlotExtend;	
#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM	// 2020-02-17 by bckim, 낚시 시스템 확장
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_1, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_100 + 1 ))	// 추가 // 5000314 ~ 5000413  : 낚싯터
		return new ioEtcItemPrivateFishing;	
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_MOON_BAIT || 
		COMPARE( dwType, ioEtcItem::EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_1 , ioEtcItem::EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_100 + 1) )		// 확장 // 1000102 ~ 1000201  :  미끼 
		return new ioEtcItemFishingMoonBait;			
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_MOON_ROD ||
		COMPARE( dwType, ioEtcItem::EIT_ETC_PRIVATE_FISHING_ROD_ITEM_1 , ioEtcItem::EIT_ETC_PRIVATE_FISHING_ROD_ITEM_100 + 1) )			// 확장 // 5000201 ~ 5000300  : 낚싯대 
		return new ioEtcItemFishingMoonRod;
#else
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_MOON_BAIT )
		return new ioEtcItemFishingMoonBait;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_MOON_ROD )
		return new ioEtcItemFishingMoonRod;
#endif	// FISHING_SYSTEM_EX_BY_BCKIM	// End. 2020-02-17 by bckim, 낚시 시스템 확장
	else if( dwType == ioEtcItem::EIT_ETC_SOLDIER_PACKAGE ||
			 dwType == ioEtcItem::EIT_ETC_SOLDIER_PACKAGE2 ||
			 dwType == ioEtcItem::EIT_ETC_PREMIUM_SOLDIER_PACKAGE ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_PACKAGE3, ioEtcItem::EIT_ETC_SOLDIER_PACKAGE10 + 1 ) )
		return new ioEtcItemSoldierPackage;
	else if( dwType == ioEtcItem::EIT_ETC_DECORATION_PACKAGE )
		return new ioEtcItemDecorationPackage;
	else if( dwType == ioEtcItem::EIT_ETC_GOLDMONSTER_COIN )
		return new ioEtcItemGoldMonsterCoin;
	else if( dwType == ioEtcItem::EIT_ETC_MONSTER_COIN )
		return new ioEtcItemMonsterCoin;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_COMPOUND )
		return new ioEtcItemExtraItemCompound;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_ITEM_MATERIAL_COMPOUND001, ioEtcItem::EIT_ETC_ITEM_MATERIAL_COMPOUND050+1 ) )
		return new ioEtcItemMaterialCompound;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_COMPOUND2 )
		return new ioEtcItemExtraItemCompound2;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_COMPOUND3 )
		return new ioEtcItemExtraItemCompound3;
	else if( dwType == ioEtcItem::EIT_ETC_GASHAPON ||
		    COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON2,  ioEtcItem::EIT_ETC_GASHAPON13 + 1) ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON14, ioEtcItem::EIT_ETC_GASHAPON53 + 1) ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON54, ioEtcItem::EIT_ETC_GASHAPON253 + 1 ) ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON254, ioEtcItem::EIT_ETC_GASHAPON553 + 1 )  ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON554, ioEtcItem::EIT_ETC_GASHAPON853 + 1 ) )
		return new ioEtcItemGashapon;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_PACKAGE  ||
		     dwType == ioEtcItem::EIT_ETC_PESO_EXP_PACKAGE ||
			 dwType == ioEtcItem::EIT_ETC_SPECIAL_PACKAGE1 ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE1, ioEtcItem::EIT_ETC_PACKAGE100 + 1) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE101, ioEtcItem::EIT_ETC_PACKAGE300 + 1) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE301, ioEtcItem::EIT_ETC_PACKAGE800 + 1) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE801, ioEtcItem::EIT_ETC_PACKAGE1100 + 1) )
		return new ioEtcItemPackage;
	else if( dwType == ioEtcItem::EIT_ETC_DECO_UNDERWEAR_PACKAGE )
		return new ioEtcItemDecoUnderwearPackage;
	else if( dwType == ioEtcItem::EIT_ETC_RANDOM_DECO_M )
		return new ioEtcItemRandomDecoM;
	else if( dwType == ioEtcItem::EIT_ETC_RANDOM_DECO_W )
		return new ioEtcItemRandomDecoW;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND )
		return new ioEtcItemMultipleCompound;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND2 )
		return new ioEtcItemMultipleCompound2;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND3 )
		return new ioEtcItemMultipleCompound3;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND )
		return new ioEtcItemMultipleEqualCompound;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 )
		return new ioEtcItemMultipleEqualCompound2;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
		return new ioEtcItemMultipleEqualCompound3;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 )
		return new ioEtcItemMultipleCompound4;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND5 )
		return new ioEtcItemMultipleCompound5;
	else if( dwType == ioEtcItem::EIT_ETC_EXCAVATING_KIT ||
		     dwType == ioEtcItem::EIT_ETC_GOLD_EXCAVATING_KIT ||
			 dwType == ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT  ||		// 2018-01-15 by bckim, 탐사 확장
			 dwType == ioEtcItem::EIT_ETC_NEW_EXCAVATING_SHOVEL )		// 2018-01-15 by bckim, 탐사 확장
		return new ioEtcItemExcavatingKit;
	else if( dwType == ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE )
		return new ioEtcItemTradeStateChange;
	else if( dwType == ioEtcItem::EIT_ETC_QUEST_EVENT )
		return new ioEtcItemQuestEvent;
	else if( dwType == ioEtcItem::EIT_ETC_SILVER_COIN )
		return new ioEtcItemSilverCoin;
	else if( dwType == ioEtcItem::EIT_ETC_MILEAGE_COIN )
		return new ioEtcItemMileage;
	else if( dwType == ioEtcItem::EIT_ETC_BATTLE_RECORD_INIT )
		return new ioEtcItemBattleRecordInit;
	else if( dwType == ioEtcItem::EIT_ETC_LADDER_RECORD_INIT )
		return new ioEtcItemLadderRecordInit;
	else if( dwType == ioEtcItem::EIT_ETC_HERO_RECORD_INIT )
		return new ioEtcItemHeroRecordInit;
	else if( dwType == ioEtcItem::EIT_ETC_SKELETON_BIG )
		return new ioEtcItemSkeletonBig;
	else if( dwType == ioEtcItem::EIT_ETC_SKELETON_BIGHEAD )
		return new ioEtcItemSkeletonBigHead;
	else if( dwType == ioEtcItem::EIT_ETC_SKELETON_SMALL )
		return new ioEtcItemSkeletonSmall;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
		return new ioEtcItemMotion;
	else if( dwType == ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN )
		return new ioEtcItemCustomItemSkin;
	else if( dwType == ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN_TEST )
		return new ioEtcItemCustomItemSkinTest;
	else if( dwType == ioEtcItem::EIT_ETC_COSTUM_ITEM_SKIN )
		return new ioEtcItemCostumItemSkin;
	else if( IsBlockEtcItem( dwType ) )
		return new ioEtcItemBlock;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_EVENT_CHECK1, ioEtcItem::EIT_ETC_EVENT_CHECK100 + 1 ) ||
		     COMPARE( dwType, ioEtcItem::EIT_ETC_EVENT_CHECK101, ioEtcItem::EIT_ETC_EVENT_CHECK200 + 1 ) )
		return new ioEtcItemEventCheck;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_GROWTH_CATALYST )
		return new ioEtcItemExtraItemGrowthCatalyst;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_1, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_4 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_5, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_205 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_206, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_506 + 1 ) )
		return new ioEtcItemLuckyCoin;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_1, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_3 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_4, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_10 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_11, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_100 + 1 ) )
		return new ioEtcItemCompoundEx;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_RAINBOW_MIXER )
		return new ioEtcItemRainbowMixer;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_LOSTSAGA_MIXER )
		return new ioEtcItemLostSagaMixer;
	else if( dwType == ioEtcItem::EIT_ETC_RAID_TICKET)
		return new ioEtcItemRaidTicket;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) )
		return new ioEtcItemTimeGashapon;
	else if( (dwType == ioEtcItem::EIT_ETC_GOLD_BOX) || 
			((dwType >= ioEtcItem::EIT_ETC_GOLD_BOX01) && (dwType <= ioEtcItem::EIT_ETC_GOLD_BOX32)))
		return new ioEtcItemGoldBox;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_SELECTOR1, ioEtcItem::EIT_ETC_SOLDIER_SELECTOR101 + 1 ) )
		return new ioEtcItemSoldierSelector;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_FOUR_EXTRA_COMPOUND1, ioEtcItem::EIT_ETC_FOUR_EXTRA_COMPOUND51 + 1 ) )
		return new ioEtcItemFourExtraCompound;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT01, ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT20 + 1 ) )
		return new ioEtcItemExpandMedalSlot;
	else if( dwType == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS )
		return new ioEtcItemSoldierExpBonus;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF64 + 1 ) )
		return new ioEtcItemConsumption;
	else if(dwType == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
		return new ioEtcItemRevive;
	else if( dwType == ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON )
		return new ioEtcItemSelectExtraGashapon;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_PRESET_PACKAGE1, ioEtcItem::EIT_ETC_PRESET_PACKAGE100+1) )
		return new ioEtcItemPreSetPackage;
	else if( dwType == ioEtcItem::EIT_ETC_GROWTH_ALL_DOWN )
		return new ioEtcItemGrowthAllDown;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_PRIZE_ITEM1, ioEtcItem::EIT_ETC_PRIZE_ITEM200+1) )
		return new ioEtcItemPrizeItem;
	else if( dwType == ioEtcItem::EIT_ETC_TOURNAMENT_CREATE )
		return new ioEtcItemTournamentCreate;
	else if( dwType == ioEtcItem::EIT_ETC_TOURNAMENT_PREMIUM_CREATE )
		return new ioEtcItemTournamentPremiumCreate;
	else if( dwType == ioEtcItem::EIT_ETC_CLOVER )
		return new ioEtcItemClover;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_ADD_CASH001, ioEtcItem::EIT_ETC_ADD_CASH100+1) )
		return new ioEtcItemAddCash;
	else if( dwType == ioEtcItem::EIT_ETC_TOURNAMENT_COIN )
		return new ioEtcItemTournamentCoin;
	else if( dwType == ioEtcItem::EIT_ETC_ROULETTE_COIN )
		return new ioEtcItemRouletteCoin;
	else if( dwType == ioEtcItem::EIT_ETC_BINGO_ITEM )
		return new ioEtcItemBingoItem;
	else if( dwType == ioEtcItem::EIT_ETC_BINGO_NUMBER_GASHAPON )
		return new ioEtcItemBingoNumberGashapon;
	else if( dwType == ioEtcItem::EIT_ETC_BINGO_SHUFFLE_NUMBER )
		return new ioEtcItemBingoShuffleNumber;
	else if( dwType == ioEtcItem::EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM )
		return new ioEtcItemBingoShuffleRewardItem;
	else if( dwType == ioEtcItem::EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR )
		return new ioEtcItemBingoRandomNumberClear;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON1, ioEtcItem::EIT_ETC_SUPER_GASHAPON200+1) || 
		COMPARE(dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON201, ioEtcItem::EIT_ETC_SUPER_GASHAPON500+1) ||
		COMPARE(dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON501, ioEtcItem::EIT_ETC_SUPER_GASHAPON800+1) || 
		COMPARE(dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON801, ioEtcItem::EIT_ETC_SUPER_GASHAPON1000+1) ||
		COMPARE(dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON1001, ioEtcItem::EIT_ETC_SUPER_GASHAPON2000+1) )
		return new ioEtcItemSuperGashapon;
#ifdef OHTG_NEW_RANROM_BOX_20201109
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_NEW_CASHAPON5000, ioEtcItem::EIT_ETC_NEW_CASHAPON7000+1) )
		return new ioEtcItemRandomBox;
#endif //OHTG_NEW_RANROM_BOX_20201109
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_SEND_PRESENT_1, ioEtcItem::EIT_ETC_ITEM_SEND_PRESENT_100 + 1 ) )
		return new ioEtcItemSendPresent;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_001, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_200+1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_201, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_2000+1 ) )
		return new ioEtcItemSoldierExpAdd;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_RECHARGE_1, ioEtcItem::EIT_ETC_ITEM_RECHARGE_100 + 1 ) )
		return new ioEtcItemRecharge;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ACCESSORY_RECHARGE_1, ioEtcItem::EIT_ETC_ACCESSORY_RECHARGE_100 + 1 ) )
		return new ioEtcAccessoryRecharge;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
		return new ioEtcItemTimeGashapon;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON02, ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON51 + 1 ) )
		return new ioEtcItemSelectExtraGashapon;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_SELECT_GASHAPON001, ioEtcItem::EIT_ETC_SELECT_GASHAPON300+1) )
		return new ioSelectGashapon;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_FIXED_BINGO_NUMBER001, ioEtcItem::EIT_ETC_FIXED_BINGO_NUMBER400+1) )
		return new ioEtcItemFixedBingoNumber;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_PET_EGG_001, ioEtcItem::EIT_ETC_PET_EGG_100+1 ) )
		return new ioEtcItemPetEgg;
	else if( dwType == ioEtcItem::EIT_ETC_RAINBOW_WHOLE_CHAT )
		return new ioEtcItemRainbowWholeChat;
	else if( dwType == ioEtcItem::EIT_ETC_SOUL_STONE )
		return new ioEtcItemSoulStone;
	else if( dwType == ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND )
		return new ioEtcItemExtraSlotExtend;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_COMPOUND001, ioEtcItem::EIT_ETC_COMPOUND010+1) ||
		COMPARE(dwType, ioEtcItem::EIT_ETC_COMPOUND011, ioEtcItem::EIT_ETC_COMPOUND061+1) )
		return new ioEtcItemCompound;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_1000+1) )
		return new ioEtcItemHousingBlockItem;
	else if( dwType == ioEtcItem::EIT_ETC_CREATE_GUILD_HQ )
		return new ioEtcItemCreateGuildHQCreate;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_HOUSING_BLOCK_1000+1) )
		return new ioEtcItemHousingBlockItem;
	else if( dwType == ioEtcItem::EIT_ETC_CREATE_HOME )
		return new ioEtcItemCreateMyHomeCreate;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_TITLE_0001, ioEtcItem::EIT_ETC_TITLE_1000+1) )
		return new ioEtcItemTitle;
	else if( dwType == ioEtcItem::EIT_ETC_TITLE_PREMIUM )
		return new ioEtcItemTitlePremium;
	else if( dwType == ioEtcItem::EIT_ETC_TIME_CASH1 ||  dwType == ioEtcItem::EIT_ETC_TIME_CASH2 )
		return new ioEtcItemTimeCash;
	else if( dwType == ioEtcItem::EIT_ETC_OAK_DRUM_ITEM || dwType == ioEtcItem::EIT_ETC_OAK_WOOD_SWORD || dwType == ioEtcItem::EIT_ETC_OAK_SILVER_SWORD || dwType == ioEtcItem::EIT_ETC_OAK_GOLD_SWORD )
		return new ioEtcItemOakBarrel( dwType );
	else if( dwType == ioEtcItem::EIT_ETC_PCROOM_FISHING_ROD )
		return new ioEtcItemPCROOMFishingRod;
	else if( dwType == ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT )
		return new ioEtcItemPCROOMFishingBait;
	else if( dwType == ioEtcItem::EIT_ETC_ACCESSORY_COMPOSE )
		return new ioEtcItemAccessoryCompose;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_PET_FEED_01, ioEtcItem::EIT_ETC_PET_FEED_10+1) )
		return new ioEtcItemPetFeed;
	else if( dwType == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
		return new ioEtcItemAddictivePiece;
	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트	// ioEtcItemCardMatching
	else if( dwType == ioEtcItem::EIT_ETC_CARD_MATCH_ITEM || dwType == ioEtcItem::EIT_ETC_CARD_MATCH_TICKET || dwType == ioEtcItem::EIT_ETC_CARD_MATCH_PREMIUM_TICKET)
		return new ioEtcItemCardMatching( dwType );
	// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 
	// 2018-08-30 by bckim, 주사위 이벤트 추가
	else if( dwType == ioEtcItem::EIT_ETC_DICE_GAME_START_ITEM || dwType == ioEtcItem::EIT_ETC_DICE_GAME_DICE_ITEM || dwType == ioEtcItem::EIT_ETC_DICE_GAME_REWARD_ITEM || dwType == ioEtcItem::EIT_ETC_DICE_GAME_BORAD_ITEM)
		return new ioEtcItemDiceGame( dwType );
	// End. 2018-08-30 by bckim, 주사위 이벤트 추가

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s UNKOWN ETCITEM TYPE :%d", __FUNCTION__, dwType );
	return NULL;
}

void ioEtcItemManager::SendJoinUser( User *pExistUser, User *pNewUser )
{
	EtcItemMap::iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		ioEtcItem *pEtcItem = iter->second;
		if( pEtcItem )
			pEtcItem->SendJoinUser( pExistUser, pNewUser );
	}
}

void ioEtcItemManager::Clear()
{
	EtcItemMap::iterator iter, iEnd;
	iEnd = m_EtcItemMap.end();
	for( iter=m_EtcItemMap.begin() ; iter!=iEnd ; ++iter )
	{
		delete iter->second;
	}
	m_EtcItemMap.clear();
}

bool ioEtcItemManager::IsBlockEtcItem( DWORD dwItem )
{
	switch( dwItem )
	{
	case ioEtcItem::EIT_ETC_BLOCK1:
	case ioEtcItem::EIT_ETC_BLOCK2:
	case ioEtcItem::EIT_ETC_BLOCK3:
	case ioEtcItem::EIT_ETC_BLOCK4:
	case ioEtcItem::EIT_ETC_BLOCK5:
	case ioEtcItem::EIT_ETC_BLOCK6:
	case ioEtcItem::EIT_ETC_BLOCK7:
	case ioEtcItem::EIT_ETC_BLOCK8:
	case ioEtcItem::EIT_ETC_BLOCK9:
	case ioEtcItem::EIT_ETC_BLOCK10:
		return true;
	}

	return false;
}

bool ioEtcItemManager::IsUsableClassShopLevel( int UserLevel )
{
	return COMPARE(UserLevel,m_arrClassShopPermitLv[CST_MIN], m_arrClassShopPermitLv[CST_MAX]);
}
