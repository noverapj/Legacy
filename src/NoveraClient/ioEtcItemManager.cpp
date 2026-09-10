

#include "stdafx.h"

#include "ioEtcItemManager.h"

#include "ioEtcItem.h"

template<> ioEtcItemManager* Singleton< ioEtcItemManager >::ms_Singleton = 0;

ioEtcItemManager::ioEtcItemManager()
{
}

ioEtcItemManager::~ioEtcItemManager()
{
	ClearEtcItemList();
}

ioEtcItemManager& ioEtcItemManager::GetSingeton()
{
	return Singleton< ioEtcItemManager >::GetSingleton();
}

void ioEtcItemManager::ClearEtcItemList()
{
	EtcItemMap::iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		delete iter->second;
	}
	m_EtcItemMap.clear();
}

void ioEtcItemManager::LoadEtcItem()
{
	ClearEtcItemList();

	ioINILoader_e kLoader( "config/sp2_etcitem_info.ini" ); 

	char szBuf[MAX_PATH];
	int iMaxCount = kLoader.LoadInt( "common", "max", 0 );
	for( int i=0 ; i<iMaxCount ; i++ )
	{
		wsprintf_e( szBuf, "etcitem%d", i+1 );
		kLoader.SetTitle( szBuf );
		
		int iType = kLoader.LoadInt_e( "type", 0 );
		ioEtcItem *pInfo = CreateEtcItem( iType );
		if( !pInfo )	continue;

		pInfo->LoadProperty( kLoader );

		if( !AddEtcItem( pInfo ) )
		{
			LOG.PrintTimeAndLog( 0, "ioEtcItemManager::LoadEtcItem - %s Add Failed", szBuf );
			SAFEDELETE( pInfo );
		}
	}
}

bool ioEtcItemManager::AddEtcItem( ioEtcItem *pInfo )
{
	if( !pInfo->IsValid() )
	{
		LOG.PrintTimeAndLog( 0, "ioEtcItemManager::AddEtcItem - %s Not Valid Info",
								pInfo->GetName().c_str() );
		return false;
	}

	if( FindEtcItem( pInfo->GetType() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEtcItemManager::AddEtcItem - %s(%d) Already Exist",
								pInfo->GetName().c_str(), pInfo->GetType() );
		return false;
	}

	m_EtcItemMap.insert( EtcItemMap::value_type( pInfo->GetType(), pInfo ) );

	return true;
}

ioEtcItem* ioEtcItemManager::CreateEtcItem( DWORD dwType )
{
	if( dwType == ioEtcItem::EIT_ETC_WHOLE_CHAT )
		return new ioEtcItemWholeChat;
	else if( dwType == ioEtcItem::EIT_ETC_FRIEND_SLOT_EXTEND )
		return new ioEtcItemFriendSlotExtend;
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
	else if( dwType == ioEtcItem::EIT_ETC_PESO_BONUS )
		return new ioEtcItemPesoBonus;
	else if( dwType == ioEtcItem::EIT_ETC_EXP_BONUS )
		return new ioEtcItemExpBonus;
	else if( dwType == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND )
		return new ioEtcItemCharSlotExtend;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_BAIT || ( dwType >= ioEtcItem::EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_1 && dwType <= ioEtcItem::EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_100 ) )
		return new ioEtcItemFishingBait;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_ROD || ( dwType >= ioEtcItem::EIT_ETC_PRIVATE_FISHING_ROD_ITEM_1 && dwType <= ioEtcItem::EIT_ETC_PRIVATE_FISHING_ROD_ITEM_100 ) )
		return new ioEtcItemFishingRod;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND )
		return new ioEtcItemFishingSlotExtend;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_MOON_BAIT )
		return new ioEtcItemFishingMoonBait;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_MOON_ROD )
		return new ioEtcItemFishingMoonRod;
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
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_COMPOUND2 )
		return new ioEtcItemExtraItemCompound2;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_COMPOUND3 )
		return new ioEtcItemExtraItemCompound3;
	else if( dwType == ioEtcItem::EIT_ETC_GASHAPON ||
		    COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON2,		ioEtcItem::EIT_ETC_GASHAPON13 + 1)	 ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON14,		ioEtcItem::EIT_ETC_GASHAPON53 + 1)	 ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON54,		ioEtcItem::EIT_ETC_GASHAPON253 + 1 ) ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON254,	ioEtcItem::EIT_ETC_GASHAPON553 + 1 ) ||
			COMPARE( dwType, ioEtcItem::EIT_ETC_GASHAPON554,	ioEtcItem::EIT_ETC_GASHAPON853 + 1 ) )
		return new ioEtcItemGashapon;
	else if( dwType == ioEtcItem::EIT_ETC_FISHING_PACKAGE  ||
		     dwType == ioEtcItem::EIT_ETC_PESO_EXP_PACKAGE ||
			 dwType == ioEtcItem::EIT_ETC_SPECIAL_PACKAGE1 ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE1,	ioEtcItem::EIT_ETC_PACKAGE100 + 1)		||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE101, ioEtcItem::EIT_ETC_PACKAGE300 + 1)		||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_PACKAGE301, ioEtcItem::EIT_ETC_PACKAGE800 + 1)		||
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
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 )
		return new ioEtcItemMultipleCompound4;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND5 )
		return new ioEtcItemMultipleCompound5;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND )
		return new ioEtcItemMultipleEqualCompound;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 )
		return new ioEtcItemMultipleEqualCompound2;
	else if( dwType == ioEtcItem::EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 )
		return new ioEtcItemMultipleEqualCompound3;
	else if( dwType == ioEtcItem::EIT_ETC_EXCAVATING_KIT ||
		     dwType == ioEtcItem::EIT_ETC_NEW_EXCAVATING_KIT)
		return new ioEtcItemExcavatingKit;
	else if( dwType == ioEtcItem::EIT_ETC_NEW_EXCAVATING_SHOVEL )
		return new ioEtcItemExcavatingShovel;
	else if( dwType == ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE )
		return new ioEtcItemTradeStateChange;
	else if( dwType == ioEtcItem::EIT_ETC_QUEST_EVENT )
		return new ioEtcItemQuestEvent;
	else if( dwType == ioEtcItem::EIT_ETC_SILVER_COIN )
		return new ioEtcItemSilverCoin;
	else if( dwType == ioEtcItem::EIT_ETC_MILEAGE_COIN )
		return new ioEtcItemMileageCoin;
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
	else if( IsBlockEtcItem( dwType ) )
		return new ioEtcItemBlock;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_EVENT_CHECK1, ioEtcItem::EIT_ETC_EVENT_CHECK100 + 1 ) ||
		     COMPARE( dwType, ioEtcItem::EIT_ETC_EVENT_CHECK101, ioEtcItem::EIT_ETC_EVENT_CHECK200 + 1 ) )
		return new ioEtcItemEventCheck;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_GROWTH_CATALYST )
		return new ioEtcItemExtraItemGrowthCatalyst;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_1, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_4 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_5, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_205 + 1 ) || 
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_206, ioEtcItem::EIT_ETC_ITEM_LUCKY_COIN_506 + 1 ))
		return new ioEtcItemLuckyCoin;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_1, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_3 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_4, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_10 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_11, ioEtcItem::EIT_ETC_ITEM_COMPOUNDEX_100 + 1 ) )
		return new ioEtcItemCompoundEx;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_RAINBOW_MIXER )
		return new ioEtcItemRainbowMixer;
	else if( dwType == ioEtcItem::EIT_ETC_ITEM_LOSTSAGA_MIXER )
		return new ioEtcItemLostSagaMixer;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) )
		return new ioEtcItemTimeGashapon;
	else if( (dwType == ioEtcItem::EIT_ETC_GOLD_BOX) || 
					(dwType >= ioEtcItem::EIT_ETC_GOLD_BOX01 && dwType <= ioEtcItem::EIT_ETC_GOLD_BOX32))
		return new ioEtcItemGoldBox;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_SELECTOR1, ioEtcItem::EIT_ETC_SOLDIER_SELECTOR101 + 1 ) )
		return new ioEtcItemSoldierSelector;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_FOUR_EXTRA_COMPOUND1, ioEtcItem::EIT_ETC_FOUR_EXTRA_COMPOUND51 + 1 ) )
		return new ioEtcItemFourExtraCompound;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT01, ioEtcItem::EIT_ETC_EXPAND_MEDAL_SLOT20 + 1 ) )
		return new ioEtcItemExpandMedalSlot;
	else if( dwType == ioEtcItem::EIT_ETC_SOLDIER_EXP_BONUS )
		return new ioEtcItemSoldierExpBonus;
	else if( COMPARE(dwType, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01,  ioEtcItem::EIT_ETC_CONSUMPTION_BUFF64 + 1))
		return new ioEtcItemConsumption;
	else if( dwType == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
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
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON1,	ioEtcItem::EIT_ETC_SUPER_GASHAPON200 + 1  ) || 
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON201,	ioEtcItem::EIT_ETC_SUPER_GASHAPON500 + 1  ) || 
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON501,	ioEtcItem::EIT_ETC_SUPER_GASHAPON800 + 1  ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON801,	ioEtcItem::EIT_ETC_SUPER_GASHAPON1000 + 1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SUPER_GASHAPON1001,	ioEtcItem::EIT_ETC_SUPER_GASHAPON2000 + 1 ) )
		return new ioEtcItemSuperGashapon;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_SEND_PRESENT_1, ioEtcItem::EIT_ETC_ITEM_SEND_PRESENT_100 + 1 ) )
		return new ioEtcItemSendPresent;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_001, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_200+1 ) ||
			 COMPARE( dwType, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_201, ioEtcItem::EIT_ETC_SOLDIER_EXP_ADD_2000+1 ) )
		return new ioEtcItemSoldierExpAdd;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_RECHARGE_1, ioEtcItem::EIT_ETC_ITEM_RECHARGE_100 + 1 ) )
		return new ioEtcItemRecharge;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ACCESSORY_RECHARGE_1, ioEtcItem::EIT_ETC_ACCESSORY_RECHARGE_100 + 1 ) )
		return new ioEtcAccessoryRecharge;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) )
		return new ioEtcItemTimeGashapon;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
		return new ioEtcItemTimeGashapon;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON02, ioEtcItem::EIT_ETC_SELECT_EXTRA_GASHAPON51 + 1 ) )
		return new ioEtcItemSelectExtraGashapon;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_SELECT_GASHAPON001, ioEtcItem::EIT_ETC_SELECT_GASHAPON300 + 1 ) )
		return new ioSelectGashapon;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_FIXED_BINGO_NUMBER001, ioEtcItem::EIT_ETC_FIXED_BINGO_NUMBER400 + 1 ) )
		return new ioEtcItemFixedBingoNumber;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_050+1 ) )
		return new ioEtcItemMaterialCompound;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_PET_EGG_001, ioEtcItem::EIT_ETC_PET_EGG_100+1 ) )
		return new ioEtcItemPetEgg;
	else if ( dwType == ioEtcItem::EIT_ETC_RAINBOW_WHOLE_CHAT )
		return new ioEtcItemWholeChat;
	else if( dwType == ioEtcItem::EIT_ETC_SOUL_STONE )
		return new ioEtcItemSoulStone;
	else if( dwType == ioEtcItem::EIT_ETC_CUSTOM_COSTUME_SKIN )
		return new ioEtcCostumeCustomItemSkin;
	else if( dwType == ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND )
		return new ioEtcItemExtraItemSlotExtend;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_NEW_MULTIPLE_COMPOUND001, ioEtcItem::EIT_ETC_NEW_MULTIPLE_COMPOUND010 + 1 ) || 
			 COMPARE( dwType, ioEtcItem::EIT_ETC_NEW_MULTIPLE_COMPOUND011, ioEtcItem::EIT_ETC_NEW_MULTIPLE_COMPOUND060 + 1 ) )
		return new ioEtcItemNewMultipleCompound;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_GUILD_HOUSING_BLOCK_1000 + 1 ) )
		return new ioEtcItemHousingBlockItem;
	else if( dwType == ioEtcItem::EIT_ETC_CREATE_GUILD_HQ )
		return new ioEtcItemCreateGuildHQCreate;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_HOME_HOUSING_BLOCK_0001, ioEtcItem::EIT_ETC_HOME_HOUSING_BLOCK_1000 + 1 ) )
		return new ioEtcItemHousingBlockItem;
	else if( dwType == ioEtcItem::EIT_ETC_CREATE_HOME )
		return new ioEtcItemCreateMyHomeCreate;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_NAMEDTITLE_0001, ioEtcItem::EIT_ETC_NAMEDTITLE_1000+1 ) )
		return new ioEtcItemNamedTitle;
	else if( dwType == ioEtcItem::EIT_ETC_NAMEDTITLE_PREMIUM )
		return new ioEtcItemNamedTitlePremium;
	else if( dwType == ioEtcItem::EIT_ETC_TIME_CASH || dwType == ioEtcItem::EIT_ETC_TIME_CASH2 )
		return new ioEtcItemTimeCash;
	else if( dwType == ioEtcItem::EIT_ETC_OAK_DRUM_ITEM )
		return new ioEtcItemOakDrumItem;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_OAK_WOOD_SWORD, ioEtcItem::EIT_ETC_OAK_GOLD_SWORD + 1 ) )
		return new ioEtcItemOakSwordItem;
	else if( dwType == ioEtcItem::EIT_ETC_PCROOM_FISHING_ROD )
		return new ioEtcItemPCRoomFishingRod;
	else if( dwType == ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT )
		return new ioEtcItemPCRoomFishingBait;
	else if( dwType == ioEtcItem::EIT_ETC_RAID_COIN )
		return new ioEtcItemRaidCoin;
	else if( dwType == ioEtcItem::EIT_ETC_ACCESSORY_COMPOUND )
		return new ioEtcItemAccessoryCompound;
	else if( dwType == ioEtcItem::EIT_ETC_CARD_MATCH_ITEM )
		return new ioEtcItemCardMatching;
	else if( dwType == ioEtcItem::EIT_ETC_CARD_MATCH_TICKET )
		return new ioEtcItemCardMatchingTicket;
	else if( dwType == ioEtcItem::EIT_ETC_CARD_MATCH_PREMIUM_TICKET )
		return new ioEtcItemCardMatchingPremiumTicket;
	else if( dwType == ioEtcItem::EIT_ETC_DICE_GAME_ITEM )
		return new ioEtcItemDiceGame;
	else if( dwType == ioEtcItem::EIT_ETC_DICE_GAME_DICE )
		return new ioEtcItemDiceGameDice;
	else if( dwType == ioEtcItem::EIT_ETC_DICE_GAME_REWARD_CHANGE )
		return new ioEtcItemDiceGameRewardChange;
	else if( dwType == ioEtcItem::EIT_ETC_DICE_GAME_BOARD_CHANGE )
		return new ioEtcItemDiceGameBoardChange;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_PET_FEED_01, ioEtcItem::EIT_ETC_PET_FEED_10 + 1 ) )
		return new ioEtcItem;
	else if( dwType == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
		return new ioEtcItem;
	else if( COMPARE( dwType, ioEtcItem::EIT_ETC_NEW_GASHAPON5000, ioEtcItem::EIT_ETC_NEW_GASHAPON7000 + 1 ) )
		return new ioEtcItemMultiSuperGashapon;
	else if( IsFishingGroundEtcItem( dwType ) )
		return new ioEtcItemBlock;

	LOG.PrintTimeAndLog( 0, "%s UNKOWN ETCITEM TYPE :%d", __FUNCTION__, dwType );
	return NULL;
}

ioEtcItem* ioEtcItemManager::FindEtcItem( DWORD dwType )
{
	dwType = GetEtcItemTypeExceptClass( dwType );
	EtcItemMap::const_iterator iter = m_EtcItemMap.find( dwType );
	if( iter != m_EtcItemMap.end() )
		return iter->second;

	return NULL;
}

ioEtcItem* ioEtcItemManager::GetEtcItemByIdx( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetEtcItemCount() ) )
	{
		EtcItemMap::const_iterator iter = m_EtcItemMap.begin();
		std::advance( iter, iIdx );

		return iter->second;
	}

	LOG.PrintTimeAndLog( 0, "ioEtcItemManager::GetEtcItem - overflow(%d/%d)",
							iIdx, GetEtcItemCount() );

	return NULL;
}

ioEtcItem* ioEtcItemManager::FindEtcItemMacro( const char *szMacro )
{
	EtcItemMap::const_iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		if( COMPARE( iter->first, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
			COMPARE( iter->first, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
		{
			ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * > ( iter->second );
			if( pEtcMotion )
			{
				if( pEtcMotion->IsMotionMacro( szMacro ) )
					return pEtcMotion;
			}
		}
	}

	return NULL;
}

void ioEtcItemManager::FindAllEtcItemMacro( const char *szMacro, IntVec& TypeList )
{
	TypeList.clear();

	EtcItemMap::const_iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		if( COMPARE( iter->first, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
			COMPARE( iter->first, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
		{
			ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * > ( iter->second );
			if( pEtcMotion )
			{
				if( pEtcMotion->IsMotionMacro( szMacro ) )
				{
					TypeList.push_back( pEtcMotion->GetType() );
				}
			}
		}
	}
}

void ioEtcItemManager::GetEtcItemList( int iGroupIdx, EtcItemList &rkList )
{
	DWORD dwStart = iGroupIdx * TYPE_GROUP_GAP;
	DWORD dwEnd = dwStart + TYPE_GROUP_GAP;

	EtcItemMap::const_iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		if( COMPARE( iter->first, dwStart, dwEnd ) )
		{
			rkList.push_back( iter->second );
		}
		else if( iter->first > dwEnd )
		{
			break;
		}
	}
}

DWORD ioEtcItemManager::GetEtcItemTypeExceptClass( DWORD dwType )
{
	// 2100101 -> 2100001 중간에 클래스값을 삭제한다. 
	int iFirstType = (dwType%USE_TYPE_POS)/FIRST_TYPE_POS;
	if( iFirstType != CLASS_ETC_ITEM_TYPE )
		return dwType;

	int iUseType    = dwType / USE_TYPE_POS;
	int iSecondType = dwType % SECOND_TYPE_POS;

	DWORD dwNoneClassType = ( iUseType * USE_TYPE_POS ) + ( iFirstType * FIRST_TYPE_POS ) + ( iSecondType );

	return dwNoneClassType;
}

void ioEtcItemManager::Init( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	EtcItemMap::iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		ioEtcItem *pEtcItem = iter->second;
		if( pEtcItem )
			pEtcItem->OnInit( pUserEtcItem, pPlayStage );
	}
}

bool ioEtcItemManager::IsBlockEtcItem( DWORD dwItem )
{
	if( COMPARE( dwItem, ioEtcItem::EIT_ETC_BLOCK1, ioEtcItem::EIT_ETC_BLOCK10 + 1 ) )
		return true;

	return false;
}

bool ioEtcItemManager::IsFishingGroundEtcItem( DWORD dwItem )
{
	if( COMPARE( dwItem, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_1, ioEtcItem::EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_100 + 1 ) )
		return true;

	return false;
}

bool ioEtcItemManager::IsFishingRodEtcItem( DWORD dwItem )
{
	if( COMPARE( dwItem, ioEtcItem::EIT_ETC_FISHING_ROD, ioEtcItem::EIT_ETC_FISHING_MOON_ROD + 1 ) )
		return true;

	if( COMPARE( dwItem, ioEtcItem::EIT_ETC_PRIVATE_FISHING_ROD_ITEM_1, ioEtcItem::EIT_ETC_PRIVATE_FISHING_ROD_ITEM_100 + 1 ) )
		return true;

	return false;
}

bool ioEtcItemManager::IsFishingBaitEtcItem( DWORD dwItem )
{
	if( COMPARE( dwItem, ioEtcItem::EIT_ETC_FISHING_BAIT, ioEtcItem::EIT_ETC_FISHING_MOON_BAIT + 1 ) )
		return true;

	if( COMPARE( dwItem, ioEtcItem::EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_1, ioEtcItem::EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_100 + 1 ) )
		return true;

	return false;
}

ioEtcItem* ioEtcItemManager::FindEtcItemBlockMacro( const char *szMacro )
{
	EtcItemMap::const_iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		if( IsBlockEtcItem( iter->first ) && !IsFishingGroundEtcItem( iter->first ) )
		{
			ioEtcItemBlock *pEtcBlock = dynamic_cast< ioEtcItemBlock * > ( iter->second );
			if( pEtcBlock )
			{
				if( pEtcBlock->IsBlockMacro( szMacro ) )
					return pEtcBlock;
			}
		}
	}

	return NULL;
}

int ioEtcItemManager::GetEtcItemRecommandedCnt()
{
	int iCount = 0;
	EtcItemMap::const_iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{	
		ioEtcItem* pEtcItem = iter->second;
		for( int i = 0; i < pEtcItem->GetRecommandedShopOrderSize(); i++ )
		{
			if( pEtcItem && pEtcItem->GetRecommandedShopOrder(i) != ORDER_DEFAULT )
			{
				iCount++;
			}
		}
	}

	return iCount;
}

bool ioEtcItemManager::IsFishingGearEtcItem( DWORD dwType )
{
	EtcItemMap::const_iterator iter;
	for( iter=m_EtcItemMap.begin() ; iter!=m_EtcItemMap.end() ; ++iter )
	{
		if( IsFishingRodEtcItem( iter->first ) || IsFishingBaitEtcItem( iter->first ) || IsFishingGroundEtcItem( iter->first ) )
		{
			if( dwType == iter->first )
				return true;
		}
	}
	return false;
}